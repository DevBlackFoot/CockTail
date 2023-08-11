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
		// 예외 확인
		assert(pDevice);
		assert(pDeviceContext);

		D3DDevice = pDevice;
		D3DDeviceContext = pDeviceContext;

		resourceManager = ResourceManager::GetInstance();

		// 셰이더 매니저에서 각각의 셰이더를 가져와 줍니다.
		auto shaderManager = ShaderManager::GetInstance();
		particleVS = std::dynamic_pointer_cast<VertexShader>(shaderManager->GetShader("ParticleVS"));
		particleCS = std::dynamic_pointer_cast<ComputeShader>(shaderManager->GetShader("ParticleCS"));
		// 기본적으로는 Particle => 이후에 Trail로 변경..
		particleGS = std::dynamic_pointer_cast<GeometryShader>(shaderManager->GetShader("ParticleGS"));
		particlePS = std::dynamic_pointer_cast<PixelShader>(shaderManager->GetShader("ParticlePS"));

		// 빌더에서 파티클 메시를 만들어서 넣어주고
			// 만들어줄 때 Rendering data를 로드해서 넣어줍니다.
		// 렌더링 데이터가 있다고 가정해주고..

		// 파일명을 뽑아서.. 파일명만 가져와 줍니다.
		if (particleMeshes.size() < 0)
			return false;

		for (auto& particleMesh : particleMeshes)
		{
			auto& renderingData = particleMesh->GetRenderingData();

			std::wstring path = renderingData->renderer.texturePath;

			// 뒤에서 첫 번째 \를 찾습니다..
			size_t filePos = path.rfind(L"\\");
			if (filePos != path.npos && filePos != -1)
			{
				path = path.substr(filePos);
				particleMesh->SetParticleTexture(path);
			}

			path = renderingData->renderer.trailTexturePath;
			// 뒤에서 첫 번째 \를 찾습니다..
			filePos = path.rfind(L"\\");
			if (filePos != path.npos && filePos != -1)
			{
				path = path.substr(filePos);
				particleMesh->SetTrailTexture(path);
			}

			// 노이즈맵 생성
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

			// 여기 들어가는 것은 Position 정보만 담긴 Matrix여야 합니다.
			particleMesh->SetWorldTM(_world);

			this->world = _world;
			this->view = _view;
			this->proj = _proj;
		}
	}

	void ParticleEffect::PreRender(float tick)
	{
		// tick은 타임 스텝에 들어갈 수 있습니다..
		timeBuffer.timeStep = tick;

		for (auto& particleMesh : particleMeshes)
		{
			// 여기서 틱 사용.
				// 참고한 엔진의 Particle Renderer 컴포넌트에 들어가 있던 내용...
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
		// 렌더에 들어가는 CB는 총 세 가지.
			// CB_PerFrame-> Time으로 대체
			// CB_PerObject_Particle => rendering 데이터에서 획득
			// CB_DynamicBuffer -> 멤버 변수로 존재합니다.
		for (auto& particleMesh : particleMeshes)
		{
			auto& renderingData = particleMesh->GetRenderingData();

			// 플래그를 확인해서 없다면.. 바로 리턴합시다.
			unsigned int flag = renderingData->GetFlag();

			// 해당 부분의 비트가 둘 다 1이 아니면 return
				// 여기서 나오는 결과는 True / False 값이 아니라 해당 flag의 unsigned int 값이 나옵니다..
				// != 1로 해두면 제대로 안되는 것이 정상입니다.. => 값이 0이면 리턴으로 바꿔줘야합니다.
			if ((flag & static_cast<unsigned int>(BasicParticle::Flags::PARTICLE_SYSTEM_COMMON_INFO)) == 0)
				return;
			if ((flag & static_cast<unsigned int>(BasicParticle::Flags::RENDERER)) == 0)
				return;
			if ((flag & static_cast<unsigned int>(BasicParticle::Flags::EMISSION)) == 0)
				return;
			// 렌더러에서 깊이 버퍼를 가져 와야하기 때문에 여기서 렌더를 작업해줍니다.

			// 렌더러를 DXRenderer로 바꿔줍니다.
			auto dxRenderer = std::dynamic_pointer_cast<Renderer>(renderer);
			assert(dxRenderer != nullptr);

			// 여기서 검사
			if (particleMesh->GetIsFirstRun() == true)
				renderingData->commonInfo.firstRun = false;
			else
				renderingData->emission.emissiveTimer = (std::numeric_limits<float>::max)();

			// 밑의 부분이 렌더의 핵심 부분이기 때문에..
			if (particleMesh->GetIsPlay() && particleMesh->GetIsDelayStart())
			{
				// 성현이 구조의 Update에 있던 것으로 => 실제로는 CS를 디스패치 해주는 부분입니다.
					// 우리 구조에서는 프리 렌더에서 돌려줍니다,.
				DispatchParticleCS(particleMesh);

				// 해당 작업을 진행하기 전에 OIT Pass를 통해 UAV를 바인딩 해주어야합니다.
				dxRenderer->OITBindingUAV();

				// 여기선 CS이후의 결과를 GS 등을 통해서 화면에 그려줍니다.
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
		// 렌더링 데이터를 많이 쓰기 때문에 auto로 뽑아 둡시다
		auto& renderingData = mesh->GetRenderingData();

		// 메시의 카운터 초기화
		mesh->ResetCounter();

		// 구조상 위의 업데이트에서 업데이트를 한 것이 해당 오브젝트의 렌더에 반영되지 않을 가능성이 높기 때문에
		// 여기서 모든 업데이트 및 C Buffer를 세팅해줍니다. (컴퓨트 셰이더에 들어가는 부분)

		// cs 세팅
		particleCS->SetUpShader();
		D3DDeviceContext->CSSetSamplers(0, 1, SamplerManager::GetInstance()->GetAnisotropicWrap().GetAddressOf());

		// Constant Buffer 생성 후, 전달해주기.
			// 이 전에 TotalTime Setting이 끝났다고 가정합니다.
		auto bufferManager = BufferManager::GetInstance();


		// dynamic Buffer 업데이트.
		dynamicBuffer.dim = mesh->GetDim();
		dynamicBuffer.EmissionTime = renderingData->emission.emissiveTimer;

		// DynamicBuffer
		bufferManager->GetBuffer("DynamicBufferCB")->SetUpBuffer(0, &dynamicBuffer, ShaderType::COMPUTE);

		ConstantBufferDesc::cbPerObjectParticle data(renderingData);

		bufferManager->GetBuffer("PerObjectParticleCB")->SetUpBuffer(1, &data, ShaderType::COMPUTE);

		// Time 버퍼의 경우 Vector4형태로 들어가집니다.
			// ColorCB를 얻어와서 넣어봅시다.
		bufferManager->GetBuffer("ColorCB")->SetUpBuffer(2, &timeBuffer, ShaderType::COMPUTE);

		mesh->SetMaxParticleSize(renderingData->commonInfo.maxParticles);

		if (renderingData->commonInfo.firstRun != false)
		{
			renderingData->commonInfo.firstRun = false;
			mesh->ResetParticleBuffer();
		}

		mesh->VSSetResource();

		// Texture 세팅

		D3DDeviceContext->CSSetShaderResources(0, 1, mesh->GetRandomTexture()->GetMapSRV().GetAddressOf());
		D3DDeviceContext->CSSetShaderResources(1, 1, mesh->GetNoiseTexture()->GetMapSRV().GetAddressOf());

		D3DDeviceContext->RSSetState(mesh->GetRasterizer().Get());

		// off Depth의 경우 Renderer에서 함수를 만들어서 돌려줍시다..
		// blend State 포함

		// Dispatch -> 해당 함수안에서 CS Dispatch가 호출 됩니다.
		mesh->ParticleUpdate();

		renderingData->particleCount = mesh->GetParticleCount();

		// 모두 끝난 뒤 Shader Resource를 초기화 시켜 줍니다..
		ID3D11ShaderResourceView* null[] = { nullptr, nullptr };
		D3DDeviceContext->CSSetShaderResources(0, 2, null);
	}

	void ParticleEffect::DrawParticle(std::shared_ptr<IRenderer> renderer, const std::shared_ptr<ParticleMesh>& mesh)
	{

		if (mesh->GetRenderingData()->renderer.renderMode == ParticleRenderer::RenderMode::NONE)
			return;

		// 렌더러를 DXRenderer로 바꿔줍니다.
		auto dxRenderer = std::dynamic_pointer_cast<Renderer>(renderer);
		if (dxRenderer == nullptr)
			return;

		auto& renderingData = mesh->GetRenderingData();

		// 셰이더 세팅
		// VS, GS, PS를 세팅해줍니다.
		particleVS->SetUpShader();
		// GS의 경우 Trail로 변경 되었을 수도 있으므로 기본 파티클 GS를 가져와 줍니다.
		particleGS = std::dynamic_pointer_cast<GeometryShader>(ShaderManager::GetInstance()->GetShader("ParticleGS"));
		particleGS->SetUpShader();
		particlePS->SetUpShader();

		// PrimitiveTopology 세팅
		D3DDeviceContext->IASetPrimitiveTopology(mesh->GetTopology());

		// Constant Buffer를 세팅해줍니다..
		// VS에는 세팅할 것이 없습니다..
		auto bufferManager = BufferManager::GetInstance();

		/// GS 세팅
		// Time Buffer ViewProj
		bufferManager->GetBuffer("ColorCB")->SetUpBuffer(0, &timeBuffer, ShaderType::GEOMETRY);

		// PerobjParticle
		ConstantBufferDesc::cbPerObjectParticle data(renderingData);
		bufferManager->GetBuffer("PerObjectParticleCB")->SetUpBuffer(1, &data, ShaderType::GEOMETRY);

		// ViewProj만 넘겨줍니다.
		SimpleMath::Matrix viewProj = view * proj;
		bufferManager->GetBuffer("WorldViewProjCB")->SetUpBuffer(2, &viewProj, ShaderType::GEOMETRY);

		// eyepos
		SimpleMath::Vector3 eyePos = dxRenderer->GetEyePos();
		SimpleMath::Vector4 eyePosV4 = SimpleMath::Vector4(eyePos.x, eyePos.y, eyePos.z, 1.0f);
		bufferManager->GetBuffer("ViewPosCB")->SetUpBuffer(3, &eyePosV4, ShaderType::GEOMETRY);

		/// PS 세팅
		// 스크린 Info를 세팅해줍니다.
		SimpleMath::Vector4 screenInfo;
		screenInfo.x = DX11GraphicsCore::GetInstance()->GetScreenWidth();
		screenInfo.y = DX11GraphicsCore::GetInstance()->GetScreenHeight();
		bufferManager->GetBuffer("TextureInfoCB")->SetUpBuffer(0, &screenInfo, ShaderType::PIXEL);

		// perobj Particle
		bufferManager->GetBuffer("PerObjectParticleCB")->SetUpBuffer(1, &data, ShaderType::PIXEL);

		// 각 셰이더 샘플러 업데이트
		D3DDeviceContext->GSSetSamplers(0, 1, SamplerManager::GetInstance()->GetAnisotropicWrap().GetAddressOf());
		D3DDeviceContext->PSSetSamplers(0, 1, SamplerManager::GetInstance()->GetAnisotropicWrap().GetAddressOf());

		mesh->SetMaxParticleSize(renderingData->commonInfo.maxParticles);

		if (renderingData->commonInfo.firstRun != false)
		{
			renderingData->commonInfo.firstRun = false;
			mesh->ResetParticleBuffer();
		}

		mesh->VSSetResource();

		// 텍스쳐 세팅.
		// 기존 불투명 오브젝트들의 depth 버퍼를 가져옵니다.
		auto depthBuffer = renderer->GetDepthBufferRTT()->GetShaderResourceView();

		D3DDeviceContext->PSSetShaderResources(0, 1, mesh->GetParticleTexture()->GetMapSRV().GetAddressOf());
		D3DDeviceContext->PSSetShaderResources(1, 1, depthBuffer.GetAddressOf());

		D3DDeviceContext->RSSetState(mesh->GetRasterizer().Get());

		D3DDeviceContext->Draw(renderingData->commonInfo.maxParticles, 0);

		// SRV를 초기화 시켜줍니다..
		// 모두 끝난 뒤 Shader Resource를 초기화 시켜 줍니다..
		ID3D11ShaderResourceView* null[] = { nullptr, nullptr, nullptr };
		D3DDeviceContext->PSSetShaderResources(0, 3, null);

		// 사용이 끝난 지오메트리 쉐이더는 nullptr로 설정해줍니다.
		D3DDeviceContext->GSSetShader(nullptr, nullptr, 0);

	}

	void ParticleEffect::DrawTrail(std::shared_ptr<IRenderer> renderer, const std::shared_ptr<ParticleMesh>& mesh)
	{
		// 트레일 플래그 확인합니다..

		auto& renderingData = mesh->GetRenderingData();
		if ((renderingData->GetFlag() & static_cast<unsigned int>(BasicParticle::Flags::TRAILS)) == 0)
			return;

		// 렌더러를 DXRenderer로 바꿔줍니다.
		auto dxRenderer = std::dynamic_pointer_cast<Renderer>(renderer);
		if (dxRenderer == nullptr)
			return;

		// 셰이더 세팅
		// VS, GS, PS를 세팅해줍니다.
		particleVS->SetUpShader();
		// GS의 경우 Trail GS를 가져와 줍니다.
		particleGS = std::dynamic_pointer_cast<GeometryShader>(ShaderManager::GetInstance()->GetShader("TrailGS"));
		particleGS->SetUpShader();
		particlePS->SetUpShader();

		// PrimitiveTopology 세팅
		D3DDeviceContext->IASetPrimitiveTopology(mesh->GetTopology());

		// Constant Buffer를 세팅해줍니다..
		// VS에는 세팅할 것이 없습니다..
		auto bufferManager = BufferManager::GetInstance();

		/// GS 세팅
		// Time Buffer ViewProj
		bufferManager->GetBuffer("ColorCB")->SetUpBuffer(0, &timeBuffer, ShaderType::GEOMETRY);

		// PerobjParticle
		ConstantBufferDesc::cbPerObjectParticle data(renderingData);
		bufferManager->GetBuffer("PerObjectParticleCB")->SetUpBuffer(1, &data, ShaderType::GEOMETRY);

		// ViewProj만 넘겨줍니다.
		SimpleMath::Matrix viewProj = view * proj;
		bufferManager->GetBuffer("WorldViewProjCB")->SetUpBuffer(2, &viewProj, ShaderType::GEOMETRY);

		// eyepos
		SimpleMath::Vector3 eyePos = dxRenderer->GetEyePos();
		SimpleMath::Vector4 eyePosV4 = SimpleMath::Vector4(eyePos.x, eyePos.y, eyePos.z, 1.0f);
		bufferManager->GetBuffer("ViewPosCB")->SetUpBuffer(3, &eyePosV4, ShaderType::GEOMETRY);

		/// PS 세팅
		// 스크린 Info를 세팅해줍니다.
		SimpleMath::Vector4 screenInfo;
		screenInfo.x = DX11GraphicsCore::GetInstance()->GetScreenWidth();
		screenInfo.y = DX11GraphicsCore::GetInstance()->GetScreenHeight();
		bufferManager->GetBuffer("TextureInfoCB")->SetUpBuffer(0, &screenInfo, ShaderType::PIXEL);

		// perobj Particle
		bufferManager->GetBuffer("PerObjectParticleCB")->SetUpBuffer(1, &data, ShaderType::PIXEL);

		// 각 셰이더 샘플러 업데이트
		D3DDeviceContext->GSSetSamplers(0, 1, SamplerManager::GetInstance()->GetAnisotropicWrap().GetAddressOf());
		D3DDeviceContext->PSSetSamplers(0, 1, SamplerManager::GetInstance()->GetAnisotropicWrap().GetAddressOf());

		mesh->SetMaxParticleSize(renderingData->commonInfo.maxParticles);

		if (renderingData->commonInfo.firstRun != false)
		{
			renderingData->commonInfo.firstRun = false;
			mesh->ResetParticleBuffer();
		}

		mesh->VSSetResource();

		// 텍스쳐 세팅.
		// 기존 불투명 오브젝트들의 depth 버퍼를 가져옵니다.
		auto depthBuffer = renderer->GetDepthBufferRTT()->GetShaderResourceView();

		D3DDeviceContext->PSSetShaderResources(0, 1, mesh->GetParticleTexture()->GetMapSRV().GetAddressOf());
		D3DDeviceContext->PSSetShaderResources(0, 1, mesh->GetTrailTexture()->GetMapSRV().GetAddressOf());
		D3DDeviceContext->PSSetShaderResources(1, 1, depthBuffer.GetAddressOf());

		D3DDeviceContext->RSSetState(mesh->GetRasterizer().Get());

		D3DDeviceContext->Draw(renderingData->commonInfo.maxParticles, 0);

		// SRV를 초기화 시켜줍니다..
		// 모두 끝난 뒤 Shader Resource를 초기화 시켜 줍니다..
		ID3D11ShaderResourceView* null[] = { nullptr, nullptr, nullptr };
		D3DDeviceContext->PSSetShaderResources(0, 3, null);

		// 사용이 끝난 지오메트리 쉐이더는 nullptr로 설정해줍니다.
		D3DDeviceContext->GSSetShader(nullptr, nullptr, 0);
	}
}
