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
		// device와 dc를 받아와줍니다.
		D3DDevice = DX11GraphicsCore::GetInstance()->GetDevice();
		D3DDeviceContext = DX11GraphicsCore::GetInstance()->GetImmediateDC();

		// 우선 UAResource 두개를 만들어줍니다.
		particleUAResource = std::make_shared<UnorderedAccessResource>();
		counterUAResource = std::make_shared<UnorderedAccessResource>();

		SetMaxParticleSize(1000);

		// Counter 버퍼를 만들어서 uav에 넣어줍니다.
		Structure::CounterBuffer initData;
		counterUAResource->CreateStructuredBuffer(D3DDevice, sizeof(Structure::CounterBuffer), 1, &initData, 0, true);

		// 카운터 버퍼를 카피해줍니다.
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		counterUAResource->GetUABuffer()->GetDesc(&bufferDesc);

		// 읽기 전용 버퍼로 만들어줍니다..
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

		// 기본적으로 토폴로지와 스테이트를 세팅해줍니다.
		rasterState = RasterizerState::GetSolidRS();
		topology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

		// Texture를 만들어줍시다.
		randomTexture = std::make_shared<Texture>();
		randomTexture->CreateRandomTexture1D();
	}

	ParticleMesh::~ParticleMesh()
	{
		// 각종 ComPtr을 해제해줍니다.
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
		// 사이즈가 현재 맥스 값과 같으면 리턴 해줍니다.
		if(size == maxParticles)
			return;

		maxParticles = size;

		// 기존 리소스를 지워줍니다.
		particleUAResource->Finalize();

		if(indexVB != nullptr)
			indexVB = nullptr;

		// index 버텍스 버퍼 부터 만들어줍니다.
		vbd.ByteWidth = sizeof(unsigned int) * maxParticles;
		// Immutable은 subResource를 nullptr로 가질 수 없기 때문에 Default로 만들어봅니다.
		vbd.Usage = D3D11_USAGE_DEFAULT;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;

		HR(D3DDevice->CreateBuffer(&vbd, nullptr, indexVB.GetAddressOf()));

		// UAV에 SubResource로 활용될 데이터를 만들어줍니다.
		std::vector<VertexStruct::Particle> initVertex(maxParticles);

		// 파티클 UAV를 생성합니다.
		particleUAResource->CreateStructuredBuffer(D3DDevice, sizeof(VertexStruct::Particle), maxParticles, initVertex.data(), 0);
	}

	// 사용할 셰이더에 필요한 리소스들을 Binding(Slot에 추가)해줍니다.
	void ParticleMesh::ParticleUpdate()
	{
		// 해당 함수가 실행되기 전에 특정 부분에서 각각의 셰이더를 SetUp해주어야합니다.

		// 지오메트리 셰이더에 있는 리소스를 빼줍니다.
			// 지오메트리 셰이더에 리소스를 추가하는 것은 다른 부분 (파티클 오브젝트)에서 해줍니다..
		ID3D11ShaderResourceView* nullSRV = NULL;

		D3DDeviceContext->GSSetShaderResources(2, 1, &nullSRV);

		// 컴퓨트 셰이더에 UAV를 세팅해줍니다.
		D3DDeviceContext->CSSetUnorderedAccessViews(0, 1, particleUAResource->GetUAV().GetAddressOf(), NULL);
		D3DDeviceContext->CSSetUnorderedAccessViews(1, 1, counterUAResource->GetUAV().GetAddressOf(), NULL);

		// 컴퓨트 셰이더를 실행해줍니다.
		D3DDeviceContext->Dispatch(dim, dim, dim);

		// 실행이 끝난 slot을 초기화 시켜줍니다.
		ID3D11UnorderedAccessView* nullUAV = NULL;

		D3DDeviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, NULL);
		D3DDeviceContext->CSSetUnorderedAccessViews(1, 1, &nullUAV, NULL);

		// copy Counter에 버퍼를 옮겨 줍니다.
		/*
		D3DDeviceContext->CopyResource(copyCounterBuffer.Get(), counterUAResource->GetUABuffer().Get());

		// map해줍니다..
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
		// inputlayouy을 세팅해줍니다.
			// ID기 때문에 UINT와 사이즈가 같습니다.
		UINT stride = sizeof(unsigned int);
		UINT offset = 0;

		// GS의 2번슬롯을 세팅해줍니다.
		D3DDeviceContext->GSSetShaderResources(2, 1, particleUAResource->GetSRV().GetAddressOf());

		// IA를 세팅해줍니다.
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
		// 파일명만 가져와 줍니다.
		this->particleTexture = ResourceManager::GetInstance()->LoadTexture(L"Resources/Particle/" + fileName);
	}

	void ParticleMesh::SetParticleTexture(const std::shared_ptr<Texture>& texture)
	{
		// 텍스쳐를 바꿔줍니다.
		this->particleTexture = texture;
	}

	void ParticleMesh::SetTrailTexture(std::wstring fileName)
	{
		this->trailTexture = ResourceManager::GetInstance()->LoadTexture(L"Resources/Particle/" + fileName);
	}

	void ParticleMesh::CreateNoiseTexture()
	{
		// 렌더링 데이터의 noise를 만들어줍니다.
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
		// 라디안 각을 넣어줍니다.
		if(renderingData->commonInfo.startRotationOption == ParticleCommonInfo::OptionParticle::CONSTANT)
		{
			renderingData->commonInfo.startRotation[0] = radAngle;
			renderingData->commonInfo.startRotation[1] = radAngle;
		}
	}
}
