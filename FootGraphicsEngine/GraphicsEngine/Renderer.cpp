#include "GraphicsPch.h"
#include "Renderer.h"

#include <cassert>
#include <d3dcompiler.h>
#include "Interface/IDXObject.h"
#include "GraphicsCore/DX11GraphicsCore.h"
#include "GraphicsCore/DepthStencilResource.h"
#include "GraphicsCore/RenderTargetTexture.h"
#include "GraphicsCore/RasterizerState.h"

#include "Resources/InputLayout.h"
#include "Manager/BuilderManager.h"
#include "Factory/Factory.h"

#include "Manager/ShaderManager.h"
#include "Manager/BufferManager.h"
#include "Manager/SamplerManager.h"
#include "Manager/UIUtilsManager.h"
#include "Manager/ResourceManager.h"
#include "Manager/ImGUIManager.h"

#include "Object/UI/Canvas.h"
#include "Object/UI/TextUI.h"
#include "Object/UI/SpriteUI.h"
#include "Object/UI/ButtonUI.h"

#include "Pass/LightPass.h"
#include "Pass/ShadowPass.h"
#include "Pass/PostProcess.h"
#include "Pass/OITPass.h"
#include "Object/PBRObj.h"

namespace GraphicsEngineSpace
{
	// 각종 오브젝트..

	Renderer::Renderer()
		: hWnd(0)
		, graphicsCore(nullptr)
		, mainRenderTarget(nullptr)
		, mainDepthStencil(nullptr)
		, noZTestDepthStencil(nullptr)
		, DMRAORenderTarget(nullptr)
		, normalRenderTarget(nullptr)
		, albedoRenderTarget(nullptr)
		, worldPosRenderTarget(nullptr)
		, emissiveRenderTarget(nullptr)
		, lightPass(nullptr)
		, shadowPass(nullptr)
		, oitPass(nullptr)
		, postProcessPass(nullptr)
		, blendState(nullptr)
		, UIBlendState(nullptr)
		, annotation(nullptr)
		, annotationCount(0)
		, spriteBatch(nullptr)
		, deltaTime(0.0f)
		, minimized(false)
		, maximized(false)
		, clientWidth(0)
		, clientHeight(0)
	{

	}

	Renderer::~Renderer()
	{
	}

	bool Renderer::Initialize(HWND _hWnd, int _clientWidth, int _clientHeight)
	{
		// 일단 멤버 변수 변경
		hWnd = _hWnd;
		clientWidth = _clientWidth;
		clientHeight = _clientHeight;

		graphicsCore = DX11GraphicsCore::GetInstance();
		// 각종 디바이스 등 초기화.
		graphicsCore->Initialize(hWnd, clientWidth, clientHeight);

		mainRenderTarget = std::make_shared<RenderTargetTexture>();
		mainDepthStencil = std::make_shared<DepthStencilResource>();
		noZTestDepthStencil = std::make_shared<DepthStencilResource>();
		DMRAORenderTarget = std::make_shared<RenderTargetTexture>();
		normalRenderTarget = std::make_shared<RenderTargetTexture>();
		albedoRenderTarget = std::make_shared<RenderTargetTexture>();
		worldPosRenderTarget = std::make_shared<RenderTargetTexture>();
		emissiveRenderTarget = std::make_shared<RenderTargetTexture>();

		ComPtr<ID3D11Device> device = graphicsCore->GetDevice();
		ComPtr<ID3D11DeviceContext> deviceContext = graphicsCore->GetImmediateDC();

		// 각종 Effect 등 Static 클래스들 모두 초기화.
		RasterizerState::InitAllRS(device);
		ShaderManager::GetInstance()->Init(device, deviceContext);
		// 모든 셰이더가 컴파일 되었는지에 대한 점검
		if(ShaderManager::GetInstance()->ReadAllShader() != true)
		{
			MessageBox(hWnd, L"Shader Compile ERROR", 0, 0);
		}
		
		BufferManager::GetInstance()->Init(device, deviceContext);
		SamplerManager::GetInstance()->Init(device);
		// 팩토리 인스턴스 생성 => dll 외부에서 생성하기 전에 내부에서 생성해준다.
		Factory::GetInstance();

		// 빌더 매니저 생성 및 Init => 디바이스를 받기 때문에 렌더러에서 Init을 해주어야한다.
		BuilderManager::GetInstance()->InitBuilderAll(graphicsCore->GetDevice(), graphicsCore->GetImmediateDC());

		lightPass = std::make_unique<LightPass>();
		lightPass->Init();
		lightPass->Start(mainRenderTarget, mainDepthStencil);

		shadowPass = std::make_unique<ShadowPass>();
		shadowPass->Init();
		shadowPass->Start(mainRenderTarget, mainDepthStencil);

		oitPass = std::make_unique<OITPass>();
		oitPass->Init();
		oitPass->Start(mainRenderTarget, mainDepthStencil);

		postProcessPass = std::make_unique<PostProcess>();
		postProcessPass->Init();
		postProcessPass->Start(mainRenderTarget, mainDepthStencil);

		OnResize();

		gBuffer.push_back(albedoRenderTarget);
		gBuffer.push_back(DMRAORenderTarget);
		gBuffer.push_back(normalRenderTarget);
		gBuffer.push_back(worldPosRenderTarget);
		gBuffer.push_back(emissiveRenderTarget);

		// BlendState 세팅
		D3D11_BLEND_DESC blendDesc = {};
		ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

		blendDesc.IndependentBlendEnable = true;

		// Main + albedo
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

		// Depth + Metal + roughness + AO
		blendDesc.RenderTarget[1].BlendEnable = true;
		blendDesc.RenderTarget[1].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[1].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[1].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[1].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[1].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[1].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[1].BlendOpAlpha = D3D11_BLEND_OP_ADD;

		// Normal
			// 노말 값을 어떻게.. blend해야할까요..
		blendDesc.RenderTarget[2].BlendEnable = true;
		blendDesc.RenderTarget[2].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[2].SrcBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[2].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[2].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[2].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[2].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[2].BlendOpAlpha = D3D11_BLEND_OP_ADD;

		// WorldPos를 나타내는 3번 rendertarget의 alpha블렌딩을 세팅해줍니다.
			// 반드시 SRC의 알파를 적용시켜주기 위해서 Dest의 alpha를 0으로 만들어줍니다
		blendDesc.RenderTarget[3].BlendEnable = true;
		blendDesc.RenderTarget[3].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[3].SrcBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[3].DestBlend = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[3].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[3].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[3].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[3].BlendOpAlpha = D3D11_BLEND_OP_ADD;

		// Emissive
		blendDesc.RenderTarget[4].BlendEnable = true;
		blendDesc.RenderTarget[4].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[4].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[4].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[4].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[4].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[4].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[4].BlendOpAlpha = D3D11_BLEND_OP_ADD;

		HRESULT hr = graphicsCore->GetDevice()->CreateBlendState(&blendDesc, blendState.GetAddressOf());
		if (FAILED(hr))
			return false;

		// UI를 위한 알파블렌딩을 적용시켜 봅니다.
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

		hr = graphicsCore->GetDevice()->CreateBlendState(&blendDesc, UIBlendState.GetAddressOf());

		if (FAILED(hr))
			return false;

		// 그래픽스 디버거 얻어오기
		graphicsCore->GetImmediateDC()->QueryInterface<ID3DUserDefinedAnnotation>(annotation.GetAddressOf());

		// 스프라이트 생성
		spriteBatch = std::make_shared<DirectX::SpriteBatch>(deviceContext.Get());

		// imgui 초기화
		ImGUIManager::GetInstance()->InitImplDX11(device.Get(), deviceContext.Get());

		// 실루엣을 위한 뎁스 스텐실 스테이트 새로 생성
		D3D11_DEPTH_STENCIL_DESC descDSS = {};
		ZeroMemory(&descDSS, sizeof(D3D11_DEPTH_STENCIL_DESC));

		descDSS.DepthEnable = true;
		// 뎁스 버퍼에 뎁스를 그리지는 않습니다..
		descDSS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		descDSS.DepthFunc = D3D11_COMPARISON_GREATER;

		descDSS.StencilEnable = true;
		descDSS.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		descDSS.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

		descDSS.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		// 뎁스 판정 실패한 친구를 마스킹
		descDSS.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		descDSS.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		// 망치를 그리지 않기 위해서.. 클 때만
		descDSS.FrontFace.StencilFunc = D3D11_COMPARISON_GREATER;

		descDSS.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		descDSS.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		descDSS.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		descDSS.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// 얘를 마스킹에 넣어줍니다.
		HR(device->CreateDepthStencilState(&descDSS, silhouetteMaskState.GetAddressOf()));

		descDSS.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		descDSS.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		HR(device->CreateDepthStencilState(&descDSS, silhouetteWeaponMaskState.GetAddressOf()));

		// 여기까지 하면 성공
		return true;
	}

	bool Renderer::UIUtilsInitialize()
	{
		auto device = graphicsCore->GetDevice();

		UIUtilsManager::GetInstance()->Init(device, spriteBatch, mainDepthStencil->GetDepthStencilState(), blendState);

		// 스크린 메쉬는 미리 불러준다.(로딩스크린 띄우려고)
		ResourceManager::GetInstance()->LoadScreenMesh();

		return true;
	}

	bool Renderer::LateInitialize()
	{
		/*auto device = graphicsCore->GetDevice();

		UIUtilsManager::GetInstance()->Init(device, spriteBatch, mainDepthStencil->GetDepthStencilState(), blendState);*/

		ResourceManager::GetInstance()->Initialize();

		// ResourceManager renderObj 생성
		for (auto& renderObj : ResourceManager::GetInstance()->prefab)
		{
			InitObject(renderObj.second);
		}

		for (auto& renderObj : ResourceManager::GetInstance()->particlePrefab)
		{
			InitObject(renderObj.second);
		}

		return true;
	}

	void Renderer::Finalize()
	{
		Factory::GetInstance()->DeleteFactory();
		ImGUIManager::GetInstance()->ShutDownImplDX11();

		BuilderManager::GetInstance()->DeleteBuildManager();

		RasterizerState::DestroyAll();

		// 각종 매니저 Finalize
		ShaderManager::GetInstance()->Finalize();
		BufferManager::GetInstance()->Finalize();
		SamplerManager::GetInstance()->Release();
		UIUtilsManager::GetInstance()->Finalize();
		ResourceManager::GetInstance()->Release();

		// 각종 COM 포인터를 Release 한다.
		mainRenderTarget->Finalize();

		graphicsCore->Finalize();

		mainRenderTarget->Finalize();
		mainDepthStencil->Finalize();
		DMRAORenderTarget->Finalize();
		normalRenderTarget->Finalize();
		albedoRenderTarget->Finalize();
		worldPosRenderTarget->Finalize();
		emissiveRenderTarget->Finalize();

		lightPass->Release();
		shadowPass->Release();
		postProcessPass->Release();

		// 그리고포인터 변수를 지워준다.
		SafeReset(mainRenderTarget);
		SafeReset(mainDepthStencil);
		SafeReset(DMRAORenderTarget);
		SafeReset(normalRenderTarget);
		SafeReset(albedoRenderTarget);
		SafeReset(worldPosRenderTarget);
		SafeReset(emissiveRenderTarget);

		SafeReset(lightPass);
	}

	void Renderer::OnResize()
	{
		ComPtr<ID3D11Device> device = graphicsCore->GetDevice();
		ComPtr<ID3D11DeviceContext> deviceContext = graphicsCore->GetImmediateDC();

		// 기존의 것을 지워주고..
		mainRenderTarget->Finalize();
		// 새로 만들어준다.
		graphicsCore->CreateMainRenderTarget(mainRenderTarget, clientWidth, clientHeight, DXGI_FORMAT_R8G8B8A8_UNORM);

		DMRAORenderTarget->Finalize();
		DMRAORenderTarget->Init(device, clientWidth, clientHeight, graphicsCore->GetMSAAQuality());

		normalRenderTarget->Finalize();
		normalRenderTarget->Init(device, clientWidth, clientHeight, graphicsCore->GetMSAAQuality());

		albedoRenderTarget->Finalize();
		albedoRenderTarget->Init(device, clientWidth, clientHeight, graphicsCore->GetMSAAQuality());

		worldPosRenderTarget->Finalize();
		worldPosRenderTarget->Init(device, clientWidth, clientHeight, graphicsCore->GetMSAAQuality());

		emissiveRenderTarget->Finalize();
		emissiveRenderTarget->Init(device, clientWidth, clientHeight, graphicsCore->GetMSAAQuality());

		mainDepthStencil->DeleteImmediateDepthStencil();
		mainDepthStencil->CreateDepthStencil(device, deviceContext, clientWidth, clientHeight, true);

		noZTestDepthStencil->DeleteImmediateDepthStencil();
		// 투명 오브젝트 테스트용.
		noZTestDepthStencil->CreateDepthStencil(device, deviceContext, clientWidth, clientHeight, true, D3D11_COMPARISON_ALWAYS);
		//noZTestDepthStencil->CreateDepthStencil(device, deviceContext, clientWidth, clientHeight, true, D3D11_COMPARISON_LESS_EQUAL);

		// core의 Width Height 최신화.
		graphicsCore->SetScreenWidth(clientWidth);
		graphicsCore->SetScreenHeight(clientHeight);

		lightPass->OnResize(clientWidth, clientHeight);
		shadowPass->OnResize(clientWidth, clientHeight);
		oitPass->OnResize(clientWidth, clientHeight);
		postProcessPass->OnResize(clientWidth, clientHeight);

		// 뷰포트 설정.
		deviceContext->RSSetViewports(1, mainDepthStencil->GetViewport().get());
	}

	float Renderer::GetAspectRatio() const
	{
		return static_cast<float>(clientWidth) / clientHeight;
	}

	bool Renderer::IsVaildDevice()
	{
		return (graphicsCore->GetDevice() != nullptr);
	}

	void Renderer::SetClientSize(int _width, int _height)
	{
		clientWidth = _width;
		clientHeight = _height;
	}

	void Renderer::SetWinMinMax(bool _isMinimized, bool _isMaximized)
	{
		minimized = _isMinimized;
		maximized = _isMaximized;
	}
	/*void Renderer::AddRenderObj(std::shared_ptr<IDXObject> obj)
	{
		renderVector.push_back(obj);
	}*/

	/*void Renderer::InitObject()
	{
		for (auto obj : renderVector)
		{
			obj->Init(graphicsCore->GetDevice(), graphicsCore->GetImmediateDC());
		}
	}*/

	void Renderer::InitObject(std::shared_ptr<IDXObject> obj)
	{
		obj->Init(graphicsCore->GetDevice(), graphicsCore->GetImmediateDC());
	}

	std::shared_ptr<Canvas> Renderer::CreateCanvas(const std::string& name, float width, float height)
	{
		std::shared_ptr<Canvas> newCanvas = std::make_shared<Canvas>(width, height);

		if (newCanvas == nullptr)
			return nullptr;

		newCanvas->SetName(name);

		return newCanvas;
	}

	// 기본 DepthStencil을 사용하면서 렌더 타겟에 아무것도 그리고 싶지 않을 때 호출하는 함수입니다.
	void Renderer::SetNullRenderTarget()
	{
		// 렌더 타겟을 널로 세팅해줍니다.
		ID3D11RenderTargetView* renderTargets[1] = { 0 };
		graphicsCore->GetImmediateDC()->OMSetRenderTargets(ARRAYSIZE(renderTargets), renderTargets, mainDepthStencil->GetDepthStencilView().Get());
	}

	void Renderer::SetDeferredRenderTarget()
	{
		// 리셋을 해주지는 않습니다.. 그리던 렌더타겟에 이어서 그려줍니다..
		// 기존에 사용하던 렌더 타겟을 다시 세팅해줍니다.
		ComPtr<ID3D11RenderTargetView> renderTargets[] =
		{
			//mainRenderTarget->GetRenderTargetView(),
			albedoRenderTarget->GetRenderTargetView(),
			DMRAORenderTarget->GetRenderTargetView(),
			normalRenderTarget->GetRenderTargetView(),
			worldPosRenderTarget->GetRenderTargetView(),
			emissiveRenderTarget->GetRenderTargetView()
		};
		graphicsCore->GetImmediateDC()->OMSetRenderTargets(ARRAYSIZE(renderTargets), renderTargets->GetAddressOf(), mainDepthStencil->GetDepthStencilView().Get());

	}

	void Renderer::SetDefaultDSState()
	{
		graphicsCore->GetImmediateDC()->OMSetDepthStencilState(mainDepthStencil->GetDepthStencilState().Get(), 0);
	}

	void Renderer::SetNoZDSState()
	{
		graphicsCore->GetImmediateDC()->OMSetDepthStencilState(noZTestDepthStencil->GetDepthStencilState().Get(), 0);
	}

	void Renderer::SetSilhouetteMaskDSState(int stencilRef, bool isWeapon)
	{
		// 스텐실 레퍼런스 1
		if(isWeapon == true)
			graphicsCore->GetImmediateDC()->OMSetDepthStencilState(silhouetteWeaponMaskState.Get(), stencilRef);
		else
			graphicsCore->GetImmediateDC()->OMSetDepthStencilState(silhouetteMaskState.Get(), stencilRef);
	}

	void Renderer::SetDefaultBlendState()
	{
		float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		graphicsCore->GetImmediateDC()->OMSetBlendState(blendState.Get(), blendFactor, 0xffffffff);
	}

	void Renderer::SetUIBlendState()
	{
		float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		graphicsCore->GetImmediateDC()->OMSetBlendState(UIBlendState.Get(), blendFactor, 0xffffffff);
	}

	std::shared_ptr<DepthStencilResource> Renderer::GetDepthBuffer()
	{
		// 불투명 오브젝트들이 모두 그려져있는 depthStencil Buffer
		return mainDepthStencil;
	}

	std::shared_ptr<RenderTargetTexture> Renderer::GetDepthBufferRTT()
	{
		return DMRAORenderTarget;
	}

	void Renderer::GraphicsDebugBeginEvent(const std::string& name)
	{
		if (annotation == nullptr)
			return;

		std::wstring wstr;

		wstr.assign(name.begin(), name.end());

		annotation->BeginEvent(wstr.c_str());

		annotationCount++;
	}

	void Renderer::GraphicsDebugEndEvent()
	{
		if (annotation == nullptr || annotationCount <= 0)
			return;

		annotation->EndEvent();
		annotationCount--;
	}

	void Renderer::PassDirectionalLight(SimpleMath::Vector3 color, SimpleMath::Vector3 direction, float power, bool isShadow, SimpleMath::Matrix lightViewProj)
	{
		lightPass->SetDirectionalLight(color, direction, power, isShadow, lightViewProj);
		shadowPass->SetLightViewProj(lightViewProj);
	}

	void Renderer::PassPointLight(SimpleMath::Vector3 color, SimpleMath::Vector3 position, float power, float range, bool isShadow, std::vector<SimpleMath::Matrix> lightViewProj)
	{
		lightPass->SetPointLight(color, position, power, range, isShadow, lightViewProj);
	}

	void Renderer::PassSpotLight(SimpleMath::Vector3 color, SimpleMath::Vector3 position, SimpleMath::Vector3 direction, float power, float innerSpotAngle, float outerSpotAngle, float range, bool
		isShadow, SimpleMath::Matrix lightViewProj)
	{
		lightPass->SetSpotLight(color, position, direction, power, innerSpotAngle, outerSpotAngle, range, isShadow, lightViewProj);
	}

	void Renderer::PassAmbientSkyColor(SimpleMath::Vector4 color)
	{
		lightPass->SetAmbientSkyColor(color);
	}

	void Renderer::PassEyePos(SimpleMath::Vector3 camPos)
	{
		lightPass->SetEyePos(camPos);
	}

	void Renderer::PassIndoor(bool isIndoor)
	{
		lightPass->SetIndoor(isIndoor);
	}

	void Renderer::ResetShadowPass()
	{
		shadowPass->ResetIsSetPointLightCnt();
		shadowPass->ResetIsSetSpotLightCnt();
	}

	void Renderer::FadeIn(float speed)
	{
		postProcessPass->StartFadeIn(speed);
	}

	void Renderer::FadeOut(float speed)
	{
		postProcessPass->StartFadeOut(speed);
	}

	const std::shared_ptr<DepthStencilResource>& Renderer::GetShadowMap(int mapIdx)
	{
		// 0 - point, 1 - spot, 2 - bake point, 3 - bake spot
		switch (mapIdx)
		{
		case 0:
			return shadowPass->GetPointShadowDSV();
			break;

		case 1:
			return shadowPass->GetSpotShadowDSV();
			break;

		case 2:
			return shadowPass->GetBakedPointShadowDSV();
			break;

		case 3:
			return shadowPass->GetBakedSpotShadowDSV();
			break;

		default:
			return nullptr;
		}
	}

	const cbLight& Renderer::GetLight()
	{
		return lightPass->GetcbLight();
	}

	const SimpleMath::Vector3& Renderer::GetEyePos()
	{
		return lightPass->GetEyePos();
	}

	const SimpleMath::Vector4& Renderer::GetAmbientSky()
	{
		return lightPass->GetAmbientSkyColor();
	}

	void Renderer::OITBindingUAV()
	{
		oitPass->BindingUAV();
	}

	void Renderer::BeginRender()
	{
		graphicsCore->ResetRenderTargetView(mainRenderTarget->GetRenderTargetView(), Colors::Gray);
		graphicsCore->ResetRenderTargetView(DMRAORenderTarget->GetRenderTargetView(), Colors::Black);
		graphicsCore->ResetRenderTargetView(normalRenderTarget->GetRenderTargetView(), Colors::Black);
		graphicsCore->ResetRenderTargetView(albedoRenderTarget->GetRenderTargetView(), Colors::Black);
		// 빛 정보를 확실히 하기 위해 여기서 alpha 값도 0으로 세팅해봅니다..
		graphicsCore->ResetRenderTargetView(worldPosRenderTarget->GetRenderTargetView(), XMVECTORF32{ 0.f, 0.f, 0.f, 0.f });
		graphicsCore->ResetRenderTargetView(emissiveRenderTarget->GetRenderTargetView(), Colors::Black);

		graphicsCore->ResetDepthStencilView(mainDepthStencil->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		graphicsCore->GetImmediateDC()->OMSetDepthStencilState(mainDepthStencil->GetDepthStencilState().Get(), 0);

		// Deferred
		ComPtr<ID3D11RenderTargetView> renderTargets[] =
		{
			//mainRenderTarget->GetRenderTargetView(),
			albedoRenderTarget->GetRenderTargetView(),
			DMRAORenderTarget->GetRenderTargetView(),
			normalRenderTarget->GetRenderTargetView(),
			worldPosRenderTarget->GetRenderTargetView(),
			emissiveRenderTarget->GetRenderTargetView()
		};
		graphicsCore->GetImmediateDC()->OMSetRenderTargets(ARRAYSIZE(renderTargets), renderTargets->GetAddressOf(), mainDepthStencil->GetDepthStencilView().Get());

		ImGUIManager::GetInstance()->Frame();
	}

	void Renderer::BeginMainRender()
	{
		// 렌더 타겟 세팅을 main 렌더타겟 하나로 세팅해줍니다.
		graphicsCore->ResetRenderTargetView(mainRenderTarget->GetRenderTargetView(), Colors::Gray);
		graphicsCore->ResetDepthStencilView(mainDepthStencil->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		graphicsCore->GetImmediateDC()->OMSetRenderTargets(1, mainRenderTarget->GetRenderTargetView().GetAddressOf(), mainDepthStencil->GetDepthStencilView().Get());

		graphicsCore->GetImmediateDC()->OMSetDepthStencilState(mainDepthStencil->GetDepthStencilState().Get(), 0);

		ImGUIManager::GetInstance()->Frame();
	}

	void Renderer::BeginTransparencyRender()
	{
		graphicsCore->ResetDepthStencilView(noZTestDepthStencil->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		/*
		// BlendState를 변경해봅시다..
		float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		graphicsCore->GetImmediateDC()->OMSetBlendState(alphaBlendState.Get(), blendFactor, 0xffffffff);
		*/
		// Deferred
		ComPtr<ID3D11RenderTargetView> renderTargets[] =
		{
			//mainRenderTarget->GetRenderTargetView(),
			albedoRenderTarget->GetRenderTargetView(),
			DMRAORenderTarget->GetRenderTargetView(),
			normalRenderTarget->GetRenderTargetView(),
			worldPosRenderTarget->GetRenderTargetView(),
			emissiveRenderTarget->GetRenderTargetView()
		};
		graphicsCore->GetImmediateDC()->OMSetRenderTargets(ARRAYSIZE(renderTargets), renderTargets->GetAddressOf(), noZTestDepthStencil->GetDepthStencilView().Get());

		// ** 적용해본 결과 차이가 없었습니다 => 오히려 더 안좋게 그려졌습니다.
		// 해당 부분은 포워드로 그려져야하기 때문에.. main 렌더 타겟만 받습니다.
			// 새로운 셰이더 코드를 타봅시다.
			// 디퍼드가 모두 끝난 lightRTV의 렌더타겟(텍스쳐)를 받아 거기에 이어 그립니다.
		//graphicsCore->GetImmediateDC()->OMSetRenderTargets(1, lightPass->GetLightingRTV()->GetRenderTargetView().GetAddressOf(), noZTestDepthStencil->GetDepthStencilView().Get());

		// 핵심은 Depth State 이기 때문에 해당 부분만 바꿔준다..
		graphicsCore->GetImmediateDC()->OMSetDepthStencilState(noZTestDepthStencil->GetDepthStencilState().Get(), 0);
	}

	void Renderer::BeginParticleRender()
	{
		// 핵심은 Depth State 이기 때문에 해당 부분+blend State 부분을 바꿔줍니다.
		oitPass->BeginRender();

		graphicsCore->GetImmediateDC()->OMSetDepthStencilState(noZTestDepthStencil->GetDepthStencilState().Get(), 0);
		graphicsCore->GetImmediateDC()->OMSetBlendState(nullptr, nullptr, 0xffffffff);
	}

	void Renderer::Render()
	{
		/*GraphicsDebugBeginEvent("Object Render");
		// 각종 렌더.
		for (auto obj : renderVector)
		{
			// Debug 이벤트를 오브젝트 별로 관리
			GraphicsDebugBeginEvent(obj->GetObjName());
			obj->Render();
			GraphicsDebugEndEvent();
		}
		GraphicsDebugEndEvent();*/

		//lightPass->IMGuiRender();
		//postProcessPass->ImGuiRender();

		// BlendState를 원상 복귀 시킵니다.
		SetDefaultBlendState();

		// lightPass Seting
		std::vector<std::shared_ptr<DepthStencilResource>> shadowMaps;
		shadowMaps.push_back(shadowPass->GetShadowDSV());
		shadowMaps.push_back(shadowPass->GetPointShadowDSV());
		shadowMaps.push_back(shadowPass->GetSpotShadowDSV());
		shadowMaps.push_back(shadowPass->GetBakedPointShadowDSV());
		shadowMaps.push_back(shadowPass->GetBakedSpotShadowDSV());

		GraphicsDebugBeginEvent("Light Pass");
		//lightPass->Render(gBuffer, shadowPass->GetPointShadowDSV(), shadowPass->GetSpotShadowDSV(), shadowPass->GetBakedPointShadowDSV(), shadowPass->GetBakedSpotShadowDSV());
		lightPass->Render(gBuffer, shadowMaps);
		GraphicsDebugEndEvent();

		graphicsCore->ResetRS();
	}

	void Renderer::OITParticleMerge()
	{
		oitPass->Render(lightPass->GetLightingRTV());
		oitPass->EndRender();
	}

	void Renderer::PostProcessing()
	{
		// 모든 빛연산이 끝났기 때문에.. cbLight 초기화
		lightPass->resetcbLight();

		graphicsCore->GetImmediateDC()->OMSetDepthStencilState(mainDepthStencil->GetDepthStencilState().Get(), 0);

		GraphicsDebugBeginEvent("PostProcess Pass");
		postProcessPass->Render(oitPass->GetOITRTV());
		GraphicsDebugEndEvent();

		graphicsCore->ResetRS();
	}

	void Renderer::DebugRender()
	{
		/*ID3D11RenderTargetView* backbufferRTV = mainRenderTarget->GetRenderTargetView();
		// 다 그리고 백버퍼에 그려주자.
		graphicsCore->GetImmediateDC()->OMSetRenderTargets(1, &backbufferRTV, mainRenderTarget->GetDepthStencilView());*/

		GraphicsDebugBeginEvent("Debug RTV");

		ID3D11ShaderResourceView* null[] = { nullptr };

		auto depthState = mainDepthStencil->GetDepthStencilState();

		auto depthSRV = DMRAORenderTarget->GetShaderResourceView();

		spriteBatch->Begin(DirectX::SpriteSortMode_Deferred, nullptr, nullptr, depthState.Get());
		spriteBatch->Draw(depthSRV.Get(), RECT{ 0, 0, static_cast<long>(clientWidth * 0.1f), static_cast<long>(clientHeight * 0.1f) });
		spriteBatch->End();

		auto normalSRV = normalRenderTarget->GetShaderResourceView();

		spriteBatch->Begin(DirectX::SpriteSortMode_Deferred, nullptr, nullptr, depthState.Get());
		spriteBatch->Draw(normalSRV.Get(), RECT{ static_cast<long>(clientWidth * 0.1f), 0, static_cast<long>(clientWidth * 0.2f), static_cast<long>(clientHeight * 0.1f) });
		spriteBatch->End();

		auto albedoSRV = albedoRenderTarget->GetShaderResourceView();

		spriteBatch->Begin(DirectX::SpriteSortMode_Deferred, nullptr, nullptr, depthState.Get());
		spriteBatch->Draw(albedoSRV.Get(), RECT{ static_cast<long>(clientWidth * 0.2f), 0, static_cast<long>(clientWidth * 0.3f), static_cast<long>(clientHeight * 0.1f) });
		spriteBatch->End();

		auto worldPosSRV = worldPosRenderTarget->GetShaderResourceView();

		spriteBatch->Begin(DirectX::SpriteSortMode_Deferred, nullptr, nullptr, depthState.Get());
		spriteBatch->Draw(worldPosSRV.Get(), RECT{ static_cast<long>(clientWidth * 0.3f), 0, static_cast<long>(clientWidth * 0.4f), static_cast<long>(clientHeight * 0.1f) });
		spriteBatch->End();

		auto shadowSRV = shadowPass->GetShadowDSV()->GetDepthShaderResource();

		spriteBatch->Begin(DirectX::SpriteSortMode_Deferred, nullptr, nullptr, depthState.Get());
		spriteBatch->Draw(shadowSRV.Get(), RECT{ static_cast<long>(clientWidth * 0.4f), 0, static_cast<long>(clientWidth * 0.5f), static_cast<long>(clientHeight * 0.1f) });
		spriteBatch->End();

		graphicsCore->GetImmediateDC()->PSSetShaderResources(0, 1, null);

		GraphicsDebugEndEvent();
	}

	void Renderer::ShadowRenderStart()
	{
		GraphicsDebugBeginEvent("Shadow Render");

		auto pointLights = lightPass->GetPointLights();
		if (shadowPass->GetIsSetPointLightCnt() != true)
			shadowPass->SetPointLightArray(pointLights.size());

		// 여기는 무조건 directional이 간다 가정..
		shadowPass->SetLightViewProj(lightPass->GetDirectionalLight().lightViewProj);
		shadowPass->RenderStart();
		
		//shadowPass->PointRenderStart();
	}

	void Renderer::ShadowRenderStart(int idx, bool isPointLight)
	{
		GraphicsDebugBeginEvent("Shadow Render");

		// pointLight면 true
		if (isPointLight == true)
		{
			auto pointLights = lightPass->GetPointLights();
			if (shadowPass->GetIsSetPointLightCnt() != true)
				shadowPass->SetPointLightArray(pointLights.size());

			//shadowPass->RenderStart();
			shadowPass->PointRenderStart(idx);
			shadowPass->SetLightMatrix(pointLights[idx]);
		}
		else
		{
			// 아니면 false (== SpotLight)
			auto spotLights = lightPass->GetSpotLights();
			if (shadowPass->GetIsSetSpotLightCnt() != true)
				shadowPass->SetSpotLightArray(spotLights.size());

			shadowPass->SpotRenderStart(idx);
			shadowPass->SetLightViewProj(spotLights[idx].lightViewProj);
		}
	}

	void Renderer::ShadowBakeStart(int idx, bool isPointLight)
	{
		GraphicsDebugBeginEvent("Shadow Bake");

		// pointLight면 true
		if (isPointLight == true)
		{
			auto pointLights = lightPass->GetPointLights();
			if (shadowPass->GetIsSetPointLightCnt() != true)
				shadowPass->SetPointLightArray(pointLights.size());

			//shadowPass->RenderStart();
			shadowPass->PointBakeStart(idx);
			shadowPass->SetLightMatrix(pointLights[idx]);
		}
		else
		{
			// 아니면 false (== SpotLight)
			auto spotLights = lightPass->GetSpotLights();
			if (shadowPass->GetIsSetSpotLightCnt() != true)
				shadowPass->SetSpotLightArray(spotLights.size());

			shadowPass->SpotBakeStart(idx);
			shadowPass->SetLightViewProj(spotLights[idx].lightViewProj);
		}
	}

	void Renderer::ShadowRender(std::shared_ptr<IDXObject> obj, bool isPointLight)
	{
		// BeginRender 전에 해줘야함.
		if (isPointLight == true)
			shadowPass->PointRender(obj);
		else
			shadowPass->Render(obj);
	}

	void Renderer::ShadowBake(std::shared_ptr<IDXObject> obj, bool isPointLight)
	{
		if (isPointLight == true)
			shadowPass->PointBake(obj);
		else
			shadowPass->Bake(obj);
	}

	void Renderer::ShadowRenderEnd()
	{
		shadowPass->EndRender();
		//shadowPass->PointEndRender();
		GraphicsDebugEndEvent();
	}

	void Renderer::EndRender()
	{
		GraphicsDebugBeginEvent("ImGUI");
		ImGUIManager::GetInstance()->Render();
		GraphicsDebugEndEvent();

		graphicsCore->PresentSwapChain();
	}
}

// 렌더러 생성 함수
GraphicsEngineSpace::IRenderer* CreateRenderer()
{
	return new GraphicsEngineSpace::Renderer;
}

void DeleteRenderer(GraphicsEngineSpace::IRenderer* obj)
{
	if (obj != nullptr)
	{
		delete obj;
	}
}
