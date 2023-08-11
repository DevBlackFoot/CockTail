#include "GraphicsPch.h"
#include "ParticleMesh.h"

#include "GraphicsCore/DX11GraphicsCore.h"
#include "GraphicsCore/UnorderedAccessResource.h"
#include "GraphicsCore/RasterizerState.h"
#include "Manager/ResourceManager.h"
#include "Util/VertexStruct.h"

namespace GraphicsEngineSpace
{
	ParticleMesh::ParticleMesh()
		: Resource(RESOURCE_TYPE::PARTICLE_MESH)
		, maxParticles(0)
		, emitterCount(0)
		, particleCount(0)
		, dim(0)
		, renderingData(nullptr)
		, playTime(0.f)
		, delayTime(0.f)
		, isPlay(false)
		, isDelayStart(false)
		, isFirstRun(false)
		, prevMatrix(SimpleMath::Matrix::Identity)
		, baseScale(SimpleMath::Vector3{1.0f, 1.0f, 1.0f})
		, baseRotation(SimpleMath::Quaternion::Identity)
	{
		// device�� dc�� �޾ƿ��ݴϴ�.
		D3DDevice = DX11GraphicsCore::GetInstance()->GetDevice();
		D3DDeviceContext = DX11GraphicsCore::GetInstance()->GetImmediateDC();

		// �켱 UAResource �ΰ��� ������ݴϴ�.
		particleUAResource = std::make_shared<UnorderedAccessResource>();
		counterUAResource = std::make_shared<UnorderedAccessResource>();

		SetMaxParticleSize(1000);

		// Counter ���۸� ���� uav�� �־��ݴϴ�.
		Structure::CounterBuffer initData;
		counterUAResource->CreateStructuredBuffer(D3DDevice, sizeof(Structure::CounterBuffer), 1, &initData, 0, true);

		// ī���� ���۸� ī�����ݴϴ�.
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		counterUAResource->GetUABuffer()->GetDesc(&bufferDesc);

		// �б� ���� ���۷� ������ݴϴ�..
		bufferDesc.BindFlags = 0;
		bufferDesc.ByteWidth = sizeof(Structure::CounterBuffer);
		bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		bufferDesc.StructureByteStride = sizeof(Structure::CounterBuffer);
		bufferDesc.Usage = D3D11_USAGE_STAGING;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = &initData;

		if(FAILED(D3DDevice->CreateBuffer(&bufferDesc, &data, copyCounterBuffer.GetAddressOf())))
		{
			::MessageBoxA(nullptr, "Buffer Create Failed!", nullptr, MB_OK);
		}

		// �⺻������ ���������� ������Ʈ�� �������ݴϴ�.
		rasterState = RasterizerState::GetSolidRS();
		topology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

		// Texture�� ������ݽô�.
		randomTexture = std::make_shared<Texture>();
		randomTexture->CreateRandomTexture1D();
	}

	ParticleMesh::~ParticleMesh()
	{
		// ���� ComPtr�� �������ݴϴ�.
		if(copyCounterBuffer != nullptr)
			copyCounterBuffer = nullptr;

		if(indexVB != nullptr)
			indexVB = nullptr;

		particleUAResource->Finalize();
		counterUAResource->Finalize();

		SafeReset(particleUAResource)
		SafeReset(counterUAResource)
		
		SafeReset(renderingData)
		
	}

	void ParticleMesh::SetMaxParticleSize(unsigned size)
	{
		// ����� ���� �ƽ� ���� ������ ���� ���ݴϴ�.
		if(size == maxParticles)
			return;

		maxParticles = size;

		// ���� ���ҽ��� �����ݴϴ�.
		particleUAResource->Finalize();

		if(indexVB != nullptr)
			indexVB = nullptr;

		// index ���ؽ� ���� ���� ������ݴϴ�.
		vbd.ByteWidth = sizeof(unsigned int) * maxParticles;
		// Immutable�� subResource�� nullptr�� ���� �� ���� ������ Default�� �����ϴ�.
		vbd.Usage = D3D11_USAGE_DEFAULT;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;

		HR(D3DDevice->CreateBuffer(&vbd, nullptr, indexVB.GetAddressOf()));

		// UAV�� SubResource�� Ȱ��� �����͸� ������ݴϴ�.
		std::vector<VertexStruct::Particle> initVertex(maxParticles);

		// ��ƼŬ UAV�� �����մϴ�.
		particleUAResource->CreateStructuredBuffer(D3DDevice, sizeof(VertexStruct::Particle), maxParticles, initVertex.data(), 0);
	}

	// ����� ���̴��� �ʿ��� ���ҽ����� Binding(Slot�� �߰�)���ݴϴ�.
	void ParticleMesh::ParticleUpdate()
	{
		// �ش� �Լ��� ����Ǳ� ���� Ư�� �κп��� ������ ���̴��� SetUp���־���մϴ�.

		// ������Ʈ�� ���̴��� �ִ� ���ҽ��� ���ݴϴ�.
			// ������Ʈ�� ���̴��� ���ҽ��� �߰��ϴ� ���� �ٸ� �κ� (��ƼŬ ������Ʈ)���� ���ݴϴ�..
		ID3D11ShaderResourceView* nullSRV = NULL;

		D3DDeviceContext->GSSetShaderResources(2, 1, &nullSRV);

		// ��ǻƮ ���̴��� UAV�� �������ݴϴ�.
		D3DDeviceContext->CSSetUnorderedAccessViews(0, 1, particleUAResource->GetUAV().GetAddressOf(), NULL);
		D3DDeviceContext->CSSetUnorderedAccessViews(1, 1, counterUAResource->GetUAV().GetAddressOf(), NULL);

		// ��ǻƮ ���̴��� �������ݴϴ�.
		D3DDeviceContext->Dispatch(dim, dim, dim);

		// ������ ���� slot�� �ʱ�ȭ �����ݴϴ�.
		ID3D11UnorderedAccessView* nullUAV = NULL;

		D3DDeviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, NULL);
		D3DDeviceContext->CSSetUnorderedAccessViews(1, 1, &nullUAV, NULL);

		// copy Counter�� ���۸� �Ű� �ݴϴ�.
		/*
		D3DDeviceContext->CopyResource(copyCounterBuffer.Get(), counterUAResource->GetUABuffer().Get());

		// map���ݴϴ�..
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

		D3DDeviceContext->Map(copyCounterBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedResource);

		auto temp = reinterpret_cast<Structure::CounterBuffer*>(mappedResource.pData);

		particleCount = temp->particleCounter;

		D3DDeviceContext->Unmap(copyCounterBuffer.Get(), 0);
		*/
	}

	void ParticleMesh::VSSetResource()
	{
		// inputlayouy�� �������ݴϴ�.
			// ID�� ������ UINT�� ����� �����ϴ�.
		UINT stride = sizeof(unsigned int);
		UINT offset = 0;

		// GS�� 2�������� �������ݴϴ�.
		D3DDeviceContext->GSSetShaderResources(2, 1, particleUAResource->GetSRV().GetAddressOf());

		// IA�� �������ݴϴ�.
		D3DDeviceContext->IASetVertexBuffers(0, 1, indexVB.GetAddressOf(), &stride, &offset);
	}

	void ParticleMesh::ResetParticleBuffer()
	{
		UINT clearNum = 0;
		
		D3DDeviceContext->ClearUnorderedAccessViewUint(particleUAResource->GetUAV().Get(), &clearNum);
	}

	void ParticleMesh::ResetCounter()
	{
		UINT clearNum = 0;

		D3DDeviceContext->ClearUnorderedAccessViewUint(counterUAResource->GetUAV().Get(), &clearNum);
	}

	int ParticleMesh::GetDim()
	{
		if (maxParticles % 1024 == 0)
			dim = maxParticles / 1024;
		else
			dim = (maxParticles / 1024) + 1;

		dim = static_cast<int>(ceil(pow(static_cast<double>(dim), (1.0 / 3.0))));

		return dim;
	}

	unsigned ParticleMesh::GetParticleCount()
	{
		return particleCount;
	}

	void ParticleMesh::Play()
	{
		Stop();

		isPlay = true;
		isFirstRun = false;
		renderingData->commonInfo.firstRun = true;
		playTime = 0.f;
		delayTime = 0.f;
		isDelayStart = false;
	}

	void ParticleMesh::Stop()
	{
		isPlay = false;

		renderingData->particleCount = 0;
	}

	void ParticleMesh::PlayTimeSetting(float tick, const SimpleMath::Matrix& world)
	{
		if (renderingData->emission.emissiveTimer >= renderingData->emission.emissiveTime)
			renderingData->emission.emissiveTimer = 0;

		renderingData->emission.emissiveTimer += tick * renderingData->commonInfo.simulationSpeed;

		delayTime += tick;

		if (delayTime <= renderingData->commonInfo.startDelay[0])
			return;

		isDelayStart = true;
		playTime += delayTime;

		if (renderingData->commonInfo.transformMatrix != prevMatrix)
			renderingData->commonInfo.deltaMatrix = prevMatrix.Invert() * world;
		else
			renderingData->commonInfo.deltaMatrix = SimpleMath::Matrix::Identity;

		renderingData->commonInfo.playTime = this->playTime;

		prevMatrix = world;
	}

	void ParticleMesh::SetParticleTexture(std::wstring fileName)
	{
		// ���ϸ� ������ �ݴϴ�.
		this->particleTexture = ResourceManager::GetInstance()->LoadTexture(L"Resources/Particle/" + fileName);
	}

	void ParticleMesh::SetParticleTexture(const std::shared_ptr<Texture>& texture)
	{
		// �ؽ��ĸ� �ٲ��ݴϴ�.
		this->particleTexture = texture;
	}

	void ParticleMesh::SetTrailTexture(std::wstring fileName)
	{
		this->trailTexture = ResourceManager::GetInstance()->LoadTexture(L"Resources/Particle/" + fileName);
	}

	void ParticleMesh::CreateNoiseTexture()
	{
		// ������ �������� noise�� ������ݴϴ�.
		this->noiseTexture = ResourceManager::GetInstance()->LoadNoiseTexture(renderingData->noise.frequency, renderingData->noise.octaves, renderingData->noise.octaveMultiplier, 0, 100.f, 100.f);
	}

	void ParticleMesh::SetBaseScaleRotation()
	{
		auto baseTM = renderingData->commonInfo.transformMatrix;

		baseTM.Decompose(baseScale, baseRotation, basePosition);
	}

	void ParticleMesh::SetWorldTM(const SimpleMath::Matrix& translateMatrix)
	{
		renderingData->commonInfo.transformMatrix = SimpleMath::Matrix::CreateScale(baseScale) * SimpleMath::Matrix::CreateFromQuaternion(baseRotation) * translateMatrix * SimpleMath::Matrix::CreateTranslation(basePosition);
	}

	void ParticleMesh::SetStartRocation(float radAngle)
	{
		// ���� ���� �־��ݴϴ�.
		if(renderingData->commonInfo.startRotationOption == ParticleCommonInfo::OptionParticle::CONSTANT)
		{
			renderingData->commonInfo.startRotation[0] = radAngle;
			renderingData->commonInfo.startRotation[1] = radAngle;
		}
	}
}
