#include "GraphicsPch.h"
#include "ParticleEffect.h"

#include "Buffer/BufferBase.h"
#include "GraphicsEngine/Renderer.h"

#include "GraphicsCore/DepthStencilResource.h"
#include "GraphicsCore/RenderTargetTexture.h"

#include "Resources/ParticleMesh.h"
#include "Shader/VertexShader.h"
#include "Shader/ComputeShader.h"
#include "Shader/GeometryShader.h"
#include "Shader/PixelShader.h"

#include "Define/GraphicsRenderingData.h"
#include "Manager/BufferManager.h"
#include "Manager/SamplerManager.h"
#include "Manager/ResourceManager.h"
#include "Manager/ShaderManager.h"

#include "Util/StringHelper.h"

namespace GraphicsEngineSpace
{
	ParticleEffect::ParticleEffect()
		: D3DDevice(nullptr)
		, D3DDeviceContext(nullptr)
		, particleMeshes()
		, resourceManager(nullptr)
		, dynamicBuffer()
		, particleVS(nullptr)
		, particleCS(nullptr)
		, particleGS(nullptr)
		, particlePS(nullptr)

	{
	}

	ParticleEffect::~ParticleEffect()
	{
		for (auto& particleMesh : particleMeshes)
			SafeReset(particleMesh)

			particleMeshes.clear();
	}

	bool ParticleEffect::Init(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	{
		// ���� Ȯ��
		assert(pDevice);
		assert(pDeviceContext);

		D3DDevice = pDevice;
		D3DDeviceContext = pDeviceContext;

		resourceManager = ResourceManager::GetInstance();

		// ���̴� �Ŵ������� ������ ���̴��� ������ �ݴϴ�.
		auto shaderManager = ShaderManager::GetInstance();
		particleVS = std::dynamic_pointer_cast<VertexShader>(shaderManager->GetShader("ParticleVS"));
		particleCS = std::dynamic_pointer_cast<ComputeShader>(shaderManager->GetShader("ParticleCS"));
		// �⺻�����δ� Particle => ���Ŀ� Trail�� ����..
		particleGS = std::dynamic_pointer_cast<GeometryShader>(shaderManager->GetShader("ParticleGS"));
		particlePS = std::dynamic_pointer_cast<PixelShader>(shaderManager->GetShader("ParticlePS"));

		// �������� ��ƼŬ �޽ø� ���� �־��ְ�
			// ������� �� Rendering data�� �ε��ؼ� �־��ݴϴ�.
		// ������ �����Ͱ� �ִٰ� �������ְ�..

		// ���ϸ��� �̾Ƽ�.. ���ϸ� ������ �ݴϴ�.
		if (particleMeshes.size() < 0)
			return false;

		for (auto& particleMesh : particleMeshes)
		{
			auto& renderingData = particleMesh->GetRenderingData();

			std::wstring path = renderingData->renderer.texturePath;

			// �ڿ��� ù ��° \�� ã���ϴ�..
			size_t filePos = path.rfind(L"\\");
			if (filePos != path.npos && filePos != -1)
			{
				path = path.substr(filePos);
				particleMesh->SetParticleTexture(path);
			}

			path = renderingData->renderer.trailTexturePath;
			// �ڿ��� ù ��° \�� ã���ϴ�..
			filePos = path.rfind(L"\\");
			if (filePos != path.npos && filePos != -1)
			{
				path = path.substr(filePos);
				particleMesh->SetTrailTexture(path);
			}

			// ������� ����
			particleMesh->CreateNoiseTexture();

			particleMesh->SetBaseScaleRotation();
		}

		return true;
	}

	void ParticleEffect::Update(const XMMATRIX& _world, const XMMATRIX& _view, const XMMATRIX& _proj)
	{
		for (auto& particleMesh : particleMeshes)
		{
			//particleMesh->GetRenderingData()->commonInfo.transformMatrix = _world;

			// ���� ���� ���� Position ������ ��� Matrix���� �մϴ�.
			particleMesh->SetWorldTM(_world);

			this->world = _world;
			this->view = _view;
			this->proj = _proj;
		}
	}

	void ParticleEffect::PreRender(float tick)
	{
		// tick�� Ÿ�� ���ܿ� �� �� �ֽ��ϴ�..
		timeBuffer.timeStep = tick;

		for (auto& particleMesh : particleMeshes)
		{
			// ���⼭ ƽ ���.
				// ������ ������ Particle Renderer ������Ʈ�� �� �ִ� ����...
			if (particleMesh->GetIsPlay() == true)
			{
				particleMesh->PlayTimeSetting(tick, this->world);
			}
		}
	}

	void ParticleEffect::Render()
	{

	}

	void ParticleEffect::Render(std::shared_ptr<IRenderer> renderer)
	{
		// ������ ���� CB�� �� �� ����.
			// CB_PerFrame-> Time���� ��ü
			// CB_PerObject_Particle => rendering �����Ϳ��� ȹ��
			// CB_DynamicBuffer -> ��� ������ �����մϴ�.
		for (auto& particleMesh : particleMeshes)
		{
			auto& renderingData = particleMesh->GetRenderingData();

			// �÷��׸� Ȯ���ؼ� ���ٸ�.. �ٷ� �����սô�.
			unsigned int flag = renderingData->GetFlag();

			// �ش� �κ��� ��Ʈ�� �� �� 1�� �ƴϸ� return
				// ���⼭ ������ ����� True / False ���� �ƴ϶� �ش� flag�� unsigned int ���� ���ɴϴ�..
				// != 1�� �صθ� ����� �ȵǴ� ���� �����Դϴ�.. => ���� 0�̸� �������� �ٲ�����մϴ�.
			if ((flag & static_cast<unsigned int>(BasicParticle::Flags::PARTICLE_SYSTEM_COMMON_INFO)) == 0)
				return;
			if ((flag & static_cast<unsigned int>(BasicParticle::Flags::RENDERER)) == 0)
				return;
			if ((flag & static_cast<unsigned int>(BasicParticle::Flags::EMISSION)) == 0)
				return;
			// ���������� ���� ���۸� ���� �;��ϱ� ������ ���⼭ ������ �۾����ݴϴ�.

			// �������� DXRenderer�� �ٲ��ݴϴ�.
			auto dxRenderer = std::dynamic_pointer_cast<Renderer>(renderer);
			assert(dxRenderer != nullptr);

			// ���⼭ �˻�
			if (particleMesh->GetIsFirstRun() == true)
				renderingData->commonInfo.firstRun = false;
			else
				renderingData->emission.emissiveTimer = (std::numeric_limits<float>::max)();

			// ���� �κ��� ������ �ٽ� �κ��̱� ������..
			if (particleMesh->GetIsPlay() && particleMesh->GetIsDelayStart())
			{
				// ������ ������ Update�� �ִ� ������ => �����δ� CS�� ����ġ ���ִ� �κ��Դϴ�.
					// �츮 ���������� ���� �������� �����ݴϴ�,.
				DispatchParticleCS(particleMesh);

				// �ش� �۾��� �����ϱ� ���� OIT Pass�� ���� UAV�� ���ε� ���־���մϴ�.
				dxRenderer->OITBindingUAV();

				// ���⼱ CS������ ����� GS ���� ���ؼ� ȭ�鿡 �׷��ݴϴ�.
				DrawParticle(renderer, particleMesh);

				DrawTrail(renderer, particleMesh);

				particleMesh->SetIsFirstRun(true);
			}

		}
	}

	void ParticleEffect::YieldRender(std::shared_ptr<IRenderer> renderer)
	{

	}

	const std::string& ParticleEffect::GetObjName()
	{
		return particleMeshes[0]->GetName();
	}

	std::vector<std::shared_ptr<MeshResources>> ParticleEffect::GetMeshResources()
	{
		std::vector<std::shared_ptr<MeshResources>> nullVec = std::vector<std::shared_ptr<MeshResources>>();
		return nullVec;
	}

	void ParticleEffect::Play()
	{
		for (auto& particleMesh : particleMeshes)
			particleMesh->Play();
	}

	void ParticleEffect::Stop()
	{
		for (auto& particleMesh : particleMeshes)
			particleMesh->Stop();
	}

	float ParticleEffect::GetPlayTime(int meshIdx)
	{
		return particleMeshes[meshIdx]->GetPlayTime();
	}

	void ParticleEffect::SetTexRotation(float radian)
	{
		for(auto& particleMesh : particleMeshes)
			particleMesh->SetStartRocation(radian);
	}

	void ParticleEffect::SetParticleTexture(int meshIdx, const std::shared_ptr<Texture>& texture)
	{
		if(meshIdx >= particleMeshes.size())
			return;

		if(texture == nullptr)
			return;

		particleMeshes[meshIdx]->SetParticleTexture(texture);
	}

	void ParticleEffect::SetParticleMesh(const std::shared_ptr<ParticleMesh>& particleMesh)
	{
		particleMeshes.emplace_back(particleMesh);
	}

	void ParticleEffect::DispatchParticleCS(const std::shared_ptr<ParticleMesh>& mesh)
	{
		// ������ �����͸� ���� ���� ������ auto�� �̾� �ӽô�
		auto& renderingData = mesh->GetRenderingData();

		// �޽��� ī���� �ʱ�ȭ
		mesh->ResetCounter();

		// ������ ���� ������Ʈ���� ������Ʈ�� �� ���� �ش� ������Ʈ�� ������ �ݿ����� ���� ���ɼ��� ���� ������
		// ���⼭ ��� ������Ʈ �� C Buffer�� �������ݴϴ�. (��ǻƮ ���̴��� ���� �κ�)

		// cs ����
		particleCS->SetUpShader();
		D3DDeviceContext->CSSetSamplers(0, 1, SamplerManager::GetInstance()->GetAnisotropicWrap().GetAddressOf());

		// Constant Buffer ���� ��, �������ֱ�.
			// �� ���� TotalTime Setting�� �����ٰ� �����մϴ�.
		auto bufferManager = BufferManager::GetInstance();


		// dynamic Buffer ������Ʈ.
		dynamicBuffer.dim = mesh->GetDim();
		dynamicBuffer.EmissionTime = renderingData->emission.emissiveTimer;

		// DynamicBuffer
		bufferManager->GetBuffer("DynamicBufferCB")->SetUpBuffer(0, &dynamicBuffer, ShaderType::COMPUTE);

		ConstantBufferDesc::cbPerObjectParticle data(renderingData);

		bufferManager->GetBuffer("PerObjectParticleCB")->SetUpBuffer(1, &data, ShaderType::COMPUTE);

		// Time ������ ��� Vector4���·� �����ϴ�.
			// ColorCB�� ���ͼ� �־�ô�.
		bufferManager->GetBuffer("ColorCB")->SetUpBuffer(2, &timeBuffer, ShaderType::COMPUTE);

		mesh->SetMaxParticleSize(renderingData->commonInfo.maxParticles);

		if (renderingData->commonInfo.firstRun != false)
		{
			renderingData->commonInfo.firstRun = false;
			mesh->ResetParticleBuffer();
		}

		mesh->VSSetResource();

		// Texture ����

		D3DDeviceContext->CSSetShaderResources(0, 1, mesh->GetRandomTexture()->GetMapSRV().GetAddressOf());
		D3DDeviceContext->CSSetShaderResources(1, 1, mesh->GetNoiseTexture()->GetMapSRV().GetAddressOf());

		D3DDeviceContext->RSSetState(mesh->GetRasterizer().Get());

		// off Depth�� ��� Renderer���� �Լ��� ���� �����ݽô�..
		// blend State ����

		// Dispatch -> �ش� �Լ��ȿ��� CS Dispatch�� ȣ�� �˴ϴ�.
		mesh->ParticleUpdate();

		renderingData->particleCount = mesh->GetParticleCount();

		// ��� ���� �� Shader Resource�� �ʱ�ȭ ���� �ݴϴ�..
		ID3D11ShaderResourceView* null[] = { nullptr, nullptr };
		D3DDeviceContext->CSSetShaderResources(0, 2, null);
	}

	void ParticleEffect::DrawParticle(std::shared_ptr<IRenderer> renderer, const std::shared_ptr<ParticleMesh>& mesh)
	{

		if (mesh->GetRenderingData()->renderer.renderMode == ParticleRenderer::RenderMode::NONE)
			return;

		// �������� DXRenderer�� �ٲ��ݴϴ�.
		auto dxRenderer = std::dynamic_pointer_cast<Renderer>(renderer);
		if (dxRenderer == nullptr)
			return;

		auto& renderingData = mesh->GetRenderingData();

		// ���̴� ����
		// VS, GS, PS�� �������ݴϴ�.
		particleVS->SetUpShader();
		// GS�� ��� Trail�� ���� �Ǿ��� ���� �����Ƿ� �⺻ ��ƼŬ GS�� ������ �ݴϴ�.
		particleGS = std::dynamic_pointer_cast<GeometryShader>(ShaderManager::GetInstance()->GetShader("ParticleGS"));
		particleGS->SetUpShader();
		particlePS->SetUpShader();

		// PrimitiveTopology ����
		D3DDeviceContext->IASetPrimitiveTopology(mesh->GetTopology());

		// Constant Buffer�� �������ݴϴ�..
		// VS���� ������ ���� �����ϴ�..
		auto bufferManager = BufferManager::GetInstance();

		/// GS ����
		// Time Buffer ViewProj
		bufferManager->GetBuffer("ColorCB")->SetUpBuffer(0, &timeBuffer, ShaderType::GEOMETRY);

		// PerobjParticle
		ConstantBufferDesc::cbPerObjectParticle data(renderingData);
		bufferManager->GetBuffer("PerObjectParticleCB")->SetUpBuffer(1, &data, ShaderType::GEOMETRY);

		// ViewProj�� �Ѱ��ݴϴ�.
		SimpleMath::Matrix viewProj = view * proj;
		bufferManager->GetBuffer("WorldViewProjCB")->SetUpBuffer(2, &viewProj, ShaderType::GEOMETRY);

		// eyepos
		SimpleMath::Vector3 eyePos = dxRenderer->GetEyePos();
		SimpleMath::Vector4 eyePosV4 = SimpleMath::Vector4(eyePos.x, eyePos.y, eyePos.z, 1.0f);
		bufferManager->GetBuffer("ViewPosCB")->SetUpBuffer(3, &eyePosV4, ShaderType::GEOMETRY);

		/// PS ����
		// ��ũ�� Info�� �������ݴϴ�.
		SimpleMath::Vector4 screenInfo;
		screenInfo.x = DX11GraphicsCore::GetInstance()->GetScreenWidth();
		screenInfo.y = DX11GraphicsCore::GetInstance()->GetScreenHeight();
		bufferManager->GetBuffer("TextureInfoCB")->SetUpBuffer(0, &screenInfo, ShaderType::PIXEL);

		// perobj Particle
		bufferManager->GetBuffer("PerObjectParticleCB")->SetUpBuffer(1, &data, ShaderType::PIXEL);

		// �� ���̴� ���÷� ������Ʈ
		D3DDeviceContext->GSSetSamplers(0, 1, SamplerManager::GetInstance()->GetAnisotropicWrap().GetAddressOf());
		D3DDeviceContext->PSSetSamplers(0, 1, SamplerManager::GetInstance()->GetAnisotropicWrap().GetAddressOf());

		mesh->SetMaxParticleSize(renderingData->commonInfo.maxParticles);

		if (renderingData->commonInfo.firstRun != false)
		{
			renderingData->commonInfo.firstRun = false;
			mesh->ResetParticleBuffer();
		}

		mesh->VSSetResource();

		// �ؽ��� ����.
		// ���� ������ ������Ʈ���� depth ���۸� �����ɴϴ�.
		auto depthBuffer = renderer->GetDepthBufferRTT()->GetShaderResourceView();

		D3DDeviceContext->PSSetShaderResources(0, 1, mesh->GetParticleTexture()->GetMapSRV().GetAddressOf());
		D3DDeviceContext->PSSetShaderResources(1, 1, depthBuffer.GetAddressOf());

		D3DDeviceContext->RSSetState(mesh->GetRasterizer().Get());

		D3DDeviceContext->Draw(renderingData->commonInfo.maxParticles, 0);

		// SRV�� �ʱ�ȭ �����ݴϴ�..
		// ��� ���� �� Shader Resource�� �ʱ�ȭ ���� �ݴϴ�..
		ID3D11ShaderResourceView* null[] = { nullptr, nullptr, nullptr };
		D3DDeviceContext->PSSetShaderResources(0, 3, null);

		// ����� ���� ������Ʈ�� ���̴��� nullptr�� �������ݴϴ�.
		D3DDeviceContext->GSSetShader(nullptr, nullptr, 0);

	}

	void ParticleEffect::DrawTrail(std::shared_ptr<IRenderer> renderer, const std::shared_ptr<ParticleMesh>& mesh)
	{
		// Ʈ���� �÷��� Ȯ���մϴ�..

		auto& renderingData = mesh->GetRenderingData();
		if ((renderingData->GetFlag() & static_cast<unsigned int>(BasicParticle::Flags::TRAILS)) == 0)
			return;

		// �������� DXRenderer�� �ٲ��ݴϴ�.
		auto dxRenderer = std::dynamic_pointer_cast<Renderer>(renderer);
		if (dxRenderer == nullptr)
			return;

		// ���̴� ����
		// VS, GS, PS�� �������ݴϴ�.
		particleVS->SetUpShader();
		// GS�� ��� Trail GS�� ������ �ݴϴ�.
		particleGS = std::dynamic_pointer_cast<GeometryShader>(ShaderManager::GetInstance()->GetShader("TrailGS"));
		particleGS->SetUpShader();
		particlePS->SetUpShader();

		// PrimitiveTopology ����
		D3DDeviceContext->IASetPrimitiveTopology(mesh->GetTopology());

		// Constant Buffer�� �������ݴϴ�..
		// VS���� ������ ���� �����ϴ�..
		auto bufferManager = BufferManager::GetInstance();

		/// GS ����
		// Time Buffer ViewProj
		bufferManager->GetBuffer("ColorCB")->SetUpBuffer(0, &timeBuffer, ShaderType::GEOMETRY);

		// PerobjParticle
		ConstantBufferDesc::cbPerObjectParticle data(renderingData);
		bufferManager->GetBuffer("PerObjectParticleCB")->SetUpBuffer(1, &data, ShaderType::GEOMETRY);

		// ViewProj�� �Ѱ��ݴϴ�.
		SimpleMath::Matrix viewProj = view * proj;
		bufferManager->GetBuffer("WorldViewProjCB")->SetUpBuffer(2, &viewProj, ShaderType::GEOMETRY);

		// eyepos
		SimpleMath::Vector3 eyePos = dxRenderer->GetEyePos();
		SimpleMath::Vector4 eyePosV4 = SimpleMath::Vector4(eyePos.x, eyePos.y, eyePos.z, 1.0f);
		bufferManager->GetBuffer("ViewPosCB")->SetUpBuffer(3, &eyePosV4, ShaderType::GEOMETRY);

		/// PS ����
		// ��ũ�� Info�� �������ݴϴ�.
		SimpleMath::Vector4 screenInfo;
		screenInfo.x = DX11GraphicsCore::GetInstance()->GetScreenWidth();
		screenInfo.y = DX11GraphicsCore::GetInstance()->GetScreenHeight();
		bufferManager->GetBuffer("TextureInfoCB")->SetUpBuffer(0, &screenInfo, ShaderType::PIXEL);

		// perobj Particle
		bufferManager->GetBuffer("PerObjectParticleCB")->SetUpBuffer(1, &data, ShaderType::PIXEL);

		// �� ���̴� ���÷� ������Ʈ
		D3DDeviceContext->GSSetSamplers(0, 1, SamplerManager::GetInstance()->GetAnisotropicWrap().GetAddressOf());
		D3DDeviceContext->PSSetSamplers(0, 1, SamplerManager::GetInstance()->GetAnisotropicWrap().GetAddressOf());

		mesh->SetMaxParticleSize(renderingData->commonInfo.maxParticles);

		if (renderingData->commonInfo.firstRun != false)
		{
			renderingData->commonInfo.firstRun = false;
			mesh->ResetParticleBuffer();
		}

		mesh->VSSetResource();

		// �ؽ��� ����.
		// ���� ������ ������Ʈ���� depth ���۸� �����ɴϴ�.
		auto depthBuffer = renderer->GetDepthBufferRTT()->GetShaderResourceView();

		D3DDeviceContext->PSSetShaderResources(0, 1, mesh->GetParticleTexture()->GetMapSRV().GetAddressOf());
		D3DDeviceContext->PSSetShaderResources(0, 1, mesh->GetTrailTexture()->GetMapSRV().GetAddressOf());
		D3DDeviceContext->PSSetShaderResources(1, 1, depthBuffer.GetAddressOf());

		D3DDeviceContext->RSSetState(mesh->GetRasterizer().Get());

		D3DDeviceContext->Draw(renderingData->commonInfo.maxParticles, 0);

		// SRV�� �ʱ�ȭ �����ݴϴ�..
		// ��� ���� �� Shader Resource�� �ʱ�ȭ ���� �ݴϴ�..
		ID3D11ShaderResourceView* null[] = { nullptr, nullptr, nullptr };
		D3DDeviceContext->PSSetShaderResources(0, 3, null);

		// ����� ���� ������Ʈ�� ���̴��� nullptr�� �������ݴϴ�.
		D3DDeviceContext->GSSetShader(nullptr, nullptr, 0);
	}
}
