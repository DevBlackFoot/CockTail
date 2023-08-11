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
	// ���� ������Ʈ..

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
		// �ϴ� ��� ���� ����
		hWnd = _hWnd;
		clientWidth = _clientWidth;
		clientHeight = _clientHeight;

		graphicsCore = DX11GraphicsCore::GetInstance();
		// ���� ����̽� �� �ʱ�ȭ.
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

		// ���� Effect �� Static Ŭ������ ��� �ʱ�ȭ.
		RasterizerState::InitAllRS(device);
		ShaderManager::GetInstance()->Init(device, deviceContext);
		// ��� ���̴��� ������ �Ǿ������� ���� ����
		if(ShaderManager::GetInstance()->ReadAllShader() != true)
		{
			MessageBox(hWnd, L"Shader Compile ERROR", 0, 0);
		}
		
		BufferManager::GetInstance()->Init(device, deviceContext);
		SamplerManager::GetInstance()->Init(device);
		// ���丮 �ν��Ͻ� ���� => dll �ܺο��� �����ϱ� ���� ���ο��� �������ش�.
		Factory::GetInstance();

		// ���� �Ŵ��� ���� �� Init => ����̽��� �ޱ� ������ ���������� Init�� ���־���Ѵ�.
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

		// BlendState ����
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
			// �븻 ���� ���.. blend�ؾ��ұ��..
		blendDesc.RenderTarget[2].BlendEnable = true;
		blendDesc.RenderTarget[2].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[2].SrcBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[2].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[2].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[2].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[2].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[2].BlendOpAlpha = D3D11_BLEND_OP_ADD;

		// WorldPos�� ��Ÿ���� 3�� rendertarget�� alpha������ �������ݴϴ�.
			// �ݵ�� SRC�� ���ĸ� ��������ֱ� ���ؼ� Dest�� alpha�� 0���� ������ݴϴ�
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

		// UI�� ���� ���ĺ����� ������� ���ϴ�.
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

		hr = graphicsCore->GetDevice()->CreateBlendState(&blendDesc, UIBlendState.GetAddressOf());

		if (FAILED(hr))
			return false;

		// �׷��Ƚ� ����� ������
		graphicsCore->GetImmediateDC()->QueryInterface<ID3DUserDefinedAnnotation>(annotation.GetAddressOf());

		// ��������Ʈ ����
		spriteBatch = std::make_shared<DirectX::SpriteBatch>(deviceContext.Get());

		// imgui �ʱ�ȭ
		ImGUIManager::GetInstance()->InitImplDX11(device.Get(), deviceContext.Get());

		// �Ƿ翧�� ���� ���� ���ٽ� ������Ʈ ���� ����
		D3D11_DEPTH_STENCIL_DESC descDSS = {};
		ZeroMemory(&descDSS, sizeof(D3D11_DEPTH_STENCIL_DESC));

		descDSS.DepthEnable = true;
		// ���� ���ۿ� ������ �׸����� �ʽ��ϴ�..
		descDSS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		descDSS.DepthFunc = D3D11_COMPARISON_GREATER;

		descDSS.StencilEnable = true;
		descDSS.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		descDSS.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

		descDSS.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		// ���� ���� ������ ģ���� ����ŷ
		descDSS.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		descDSS.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		// ��ġ�� �׸��� �ʱ� ���ؼ�.. Ŭ ����
		descDSS.FrontFace.StencilFunc = D3D11_COMPARISON_GREATER;

		descDSS.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		descDSS.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		descDSS.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		descDSS.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// �긦 ����ŷ�� �־��ݴϴ�.
		HR(device->CreateDepthStencilState(&descDSS, silhouetteMaskState.GetAddressOf()));

		descDSS.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		descDSS.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		HR(device->CreateDepthStencilState(&descDSS, silhouetteWeaponMaskState.GetAddressOf()));

		// ������� �ϸ� ����
		return true;
	}

	bool Renderer::UIUtilsInitialize()
	{
		auto device = graphicsCore->GetDevice();

		UIUtilsManager::GetInstance()->Init(device, spriteBatch, mainDepthStencil->GetDepthStencilState(), blendState);

		// ��ũ�� �޽��� �̸� �ҷ��ش�.(�ε���ũ�� ������)
		ResourceManager::GetInstance()->LoadScreenMesh();

		return true;
	}

	bool Renderer::LateInitialize()
	{
		/*auto device = graphicsCore->GetDevice();

		UIUtilsManager::GetInstance()->Init(device, spriteBatch, mainDepthStencil->GetDepthStencilState(), blendState);*/

		ResourceManager::GetInstance()->Initialize();

		// ResourceManager renderObj ����
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

		// ���� �Ŵ��� Finalize
		ShaderManager::GetInstance()->Finalize();
		BufferManager::GetInstance()->Finalize();
		SamplerManager::GetInstance()->Release();
		UIUtilsManager::GetInstance()->Finalize();
		ResourceManager::GetInstance()->Release();

		// ���� COM �����͸� Release �Ѵ�.
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

		// �׸��������� ������ �����ش�.
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

		// ������ ���� �����ְ�..
		mainRenderTarget->Finalize();
		// ���� ������ش�.
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
		// ���� ������Ʈ �׽�Ʈ��.
		noZTestDepthStencil->CreateDepthStencil(device, deviceContext, clientWidth, clientHeight, true, D3D11_COMPARISON_ALWAYS);
		//noZTestDepthStencil->CreateDepthStencil(device, deviceContext, clientWidth, clientHeight, true, D3D11_COMPARISON_LESS_EQUAL);

		// core�� Width Height �ֽ�ȭ.
		graphicsCore->SetScreenWidth(clientWidth);
		graphicsCore->SetScreenHeight(clientHeight);

		lightPass->OnResize(clientWidth, clientHeight);
		shadowPass->OnResize(clientWidth, clientHeight);
		oitPass->OnResize(clientWidth, clientHeight);
		postProcessPass->OnResize(clientWidth, clientHeight);

		// ����Ʈ ����.
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

	// �⺻ DepthStencil�� ����ϸ鼭 ���� Ÿ�ٿ� �ƹ��͵� �׸��� ���� ���� �� ȣ���ϴ� �Լ��Դϴ�.
	void Renderer::SetNullRenderTarget()
	{
		// ���� Ÿ���� �η� �������ݴϴ�.
		ID3D11RenderTargetView* renderTargets[1] = { 0 };
		graphicsCore->GetImmediateDC()->OMSetRenderTargets(ARRAYSIZE(renderTargets), renderTargets, mainDepthStencil->GetDepthStencilView().Get());
	}

	void Renderer::SetDeferredRenderTarget()
	{
		// ������ �������� �ʽ��ϴ�.. �׸��� ����Ÿ�ٿ� �̾ �׷��ݴϴ�..
		// ������ ����ϴ� ���� Ÿ���� �ٽ� �������ݴϴ�.
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
		// ���ٽ� ���۷��� 1
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
		// ������ ������Ʈ���� ��� �׷����ִ� depthStencil Buffer
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
		// �� ������ Ȯ���� �ϱ� ���� ���⼭ alpha ���� 0���� �����غ��ϴ�..
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
		// ���� Ÿ�� ������ main ����Ÿ�� �ϳ��� �������ݴϴ�.
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
		// BlendState�� �����غ��ô�..
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

		// ** �����غ� ��� ���̰� �������ϴ� => ������ �� ������ �׷������ϴ�.
		// �ش� �κ��� ������� �׷������ϱ� ������.. main ���� Ÿ�ٸ� �޽��ϴ�.
			// ���ο� ���̴� �ڵ带 Ÿ���ô�.
			// ���۵尡 ��� ���� lightRTV�� ����Ÿ��(�ؽ���)�� �޾� �ű⿡ �̾� �׸��ϴ�.
		//graphicsCore->GetImmediateDC()->OMSetRenderTargets(1, lightPass->GetLightingRTV()->GetRenderTargetView().GetAddressOf(), noZTestDepthStencil->GetDepthStencilView().Get());

		// �ٽ��� Depth State �̱� ������ �ش� �κи� �ٲ��ش�..
		graphicsCore->GetImmediateDC()->OMSetDepthStencilState(noZTestDepthStencil->GetDepthStencilState().Get(), 0);
	}

	void Renderer::BeginParticleRender()
	{
		// �ٽ��� Depth State �̱� ������ �ش� �κ�+blend State �κ��� �ٲ��ݴϴ�.
		oitPass->BeginRender();

		graphicsCore->GetImmediateDC()->OMSetDepthStencilState(noZTestDepthStencil->GetDepthStencilState().Get(), 0);
		graphicsCore->GetImmediateDC()->OMSetBlendState(nullptr, nullptr, 0xffffffff);
	}

	void Renderer::Render()
	{
		/*GraphicsDebugBeginEvent("Object Render");
		// ���� ����.
		for (auto obj : renderVector)
		{
			// Debug �̺�Ʈ�� ������Ʈ ���� ����
			GraphicsDebugBeginEvent(obj->GetObjName());
			obj->Render();
			GraphicsDebugEndEvent();
		}
		GraphicsDebugEndEvent();*/

		//lightPass->IMGuiRender();
		//postProcessPass->ImGuiRender();

		// BlendState�� ���� ���� ��ŵ�ϴ�.
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
		// ��� �������� ������ ������.. cbLight �ʱ�ȭ
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
		// �� �׸��� ����ۿ� �׷�����.
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

		// ����� ������ directional�� ���� ����..
		shadowPass->SetLightViewProj(lightPass->GetDirectionalLight().lightViewProj);
		shadowPass->RenderStart();
		
		//shadowPass->PointRenderStart();
	}

	void Renderer::ShadowRenderStart(int idx, bool isPointLight)
	{
		GraphicsDebugBeginEvent("Shadow Render");

		// pointLight�� true
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
			// �ƴϸ� false (== SpotLight)
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

		// pointLight�� true
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
			// �ƴϸ� false (== SpotLight)
			auto spotLights = lightPass->GetSpotLights();
			if (shadowPass->GetIsSetSpotLightCnt() != true)
				shadowPass->SetSpotLightArray(spotLights.size());

			shadowPass->SpotBakeStart(idx);
			shadowPass->SetLightViewProj(spotLights[idx].lightViewProj);
		}
	}

	void Renderer::ShadowRender(std::shared_ptr<IDXObject> obj, bool isPointLight)
	{
		// BeginRender ���� �������.
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

// ������ ���� �Լ�
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
