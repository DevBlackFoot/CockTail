#pragma once

#include "Interface/IRenderer.h"
#include <mutex>
#include <vector>
#include <mutex>

using namespace std;
using namespace DirectX::SimpleMath;

namespace GraphicsEngineSpace
{
	class DX11GraphicsCore;
	class DepthStencilResource;
	class RenderTargetTexture;
	class LightPass;
	class ShadowPass;
	class OITPass;
	class PostProcess;
	struct cbLight;

	/**
	 * \brief IRenderer�� ��ӹ޾� ������ ������ Ŭ����.
	 *
	 * ���� ������ �پ� �ִ� �κ��� ����߸��� ��� ��..
	 *
	 * 2022.06.14(ȭ) Dev.BlackFoot
	 */
	class Renderer : public IRenderer, public enable_shared_from_this<Renderer>
	{
		// ���� ���
		// ������ �ڵ�
		HWND hWnd;

		// Device, DC, RS ���� �� �ִ� �ھ� Ŭ����
		std::shared_ptr<DX11GraphicsCore> graphicsCore;

		// View���� ������ �ִ� ���� Ÿ�� Ŭ����
		std::shared_ptr<RenderTargetTexture> mainRenderTarget;
		std::shared_ptr<DepthStencilResource> mainDepthStencil;
		std::shared_ptr<DepthStencilResource> noZTestDepthStencil;

		// depth debug�� ���� Ÿ��.
		std::shared_ptr<RenderTargetTexture> DMRAORenderTarget;
		std::shared_ptr<RenderTargetTexture> normalRenderTarget;
		std::shared_ptr<RenderTargetTexture> albedoRenderTarget;
		std::shared_ptr<RenderTargetTexture> worldPosRenderTarget;
		std::shared_ptr<RenderTargetTexture> emissiveRenderTarget;

		// lightPass ����.
		std::unique_ptr<LightPass> lightPass;
		std::vector<std::shared_ptr<RenderTargetTexture>> gBuffer;

		// ShadowPass ����
		std::unique_ptr<ShadowPass> shadowPass;

		// OIT Pass ����
		std::unique_ptr<OITPass> oitPass;

		// PostProcess ����
		std::unique_ptr<PostProcess> postProcessPass;

		// BlendState ����
		Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
		Microsoft::WRL::ComPtr<ID3D11BlendState> UIBlendState;

		// �ϳ��� �־ �Ǵ� �Ƿ翧 DepthStencilState(����ŷ ��)
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> silhouetteMaskState;
		// ��ġ ����ŷ��
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> silhouetteWeaponMaskState;

		// �׷��� ����� ����. Ref by. ������ ��
		Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation> annotation;
		unsigned int annotationCount;

		// Sprite
		std::shared_ptr<SpriteBatch> spriteBatch;

		float deltaTime;
		
		// â ������, ���� ��� ���õ� ���� ����.
			// �̰� ��������..?
		bool minimized;
		bool maximized;

		int clientWidth;
		int clientHeight;


	private:
		// ���丮�� �����
			// ȭ�鿡 �׷��� ������Ʈ ��ϵ�.
			// ���� ť? ���� ����?
		vector<std::shared_ptr<IDXObject>> transparencyRenderVector;
		
		std::mutex renderMutex;

	public:
		Renderer();
		virtual ~Renderer();

		virtual bool Initialize(HWND _hWnd, int _clientWidth, int _clientHeight) override;
		virtual bool LateInitialize() override;
		virtual bool UIUtilsInitialize() override;
		virtual void Finalize() override;
		virtual void OnResize() override;

		// ��Ⱦ�� ȹ��
		virtual float GetAspectRatio() const override;

		// â ������ ���� ����
		virtual bool IsVaildDevice() override;
		virtual void SetClientSize(int _width, int _height) override;
		virtual void SetWinMinMax(bool _isMinimized, bool _isMaximized) override;
		
		// ������Ʈ�� �ϰ������� Init�� ���� ���ݴϴ� => RenderVector�� ���� Init�� ���� ����
		virtual void InitObject(std::shared_ptr<IDXObject> obj) override;

		virtual void BeginRender() override;
		// Loading�� Deferred �ƴ� ���� Back���� �ϳ� ����
		virtual void BeginMainRender() override;
		// Transparency�� Z-Test ���� depthbuffer ����
		virtual void BeginTransparencyRender() override;
		// Particle �� Z-Test ���� depthbuffer ����
		virtual void BeginParticleRender() override;

		virtual void Render() override;
		virtual void OITParticleMerge() override;
		virtual void PostProcessing() override;
		virtual void DebugRender() override;
		virtual void ShadowRenderStart() override;
		virtual void ShadowRenderStart(int idx, bool isPointLight) override;
		virtual void ShadowBakeStart(int idx, bool isPointLight) override;
		virtual void ShadowRender(std::shared_ptr<IDXObject> obj, bool isPointLight) override;
		virtual void ShadowBake(std::shared_ptr<IDXObject> obj, bool isPointLight) override;
		virtual void ShadowRenderEnd() override;
		virtual void EndRender() override;
		// ĵ���� ����
		virtual std::shared_ptr<Canvas> CreateCanvas(const std::string& name, float width, float height) override;

		// ���� Ÿ�� ���� �Լ���
		virtual void SetNullRenderTarget() override;
		virtual void SetDeferredRenderTarget() override;

		// ���� ������Ʈ ���� �Լ���
		virtual void SetDefaultDSState() override;
		virtual void SetNoZDSState() override;
		virtual void SetSilhouetteMaskDSState(int stencilRef, bool isWeapon) override;

		// ���� ������Ʈ ���� �Լ���
		virtual void SetDefaultBlendState() override;
		virtual void SetUIBlendState() override;

		// depthbuffer ���
		virtual std::shared_ptr<DepthStencilResource> GetDepthBuffer() override;
		virtual std::shared_ptr<RenderTargetTexture> GetDepthBufferRTT() override;

		// ����� �̺�Ʈ
		virtual void GraphicsDebugBeginEvent(const std::string& name) override;
		virtual void GraphicsDebugEndEvent() override;

		virtual void PassDirectionalLight(SimpleMath::Vector3 color, SimpleMath::Vector3 direction, float power, bool isShadow, SimpleMath::Matrix lightViewProj) override;
		virtual void PassPointLight(SimpleMath::Vector3 color, SimpleMath::Vector3 position, float power, float range, bool isShadow, std::vector<SimpleMath::Matrix> lightViewProj) override;
		virtual void PassSpotLight(SimpleMath::Vector3 color, SimpleMath::Vector3 position, SimpleMath::Vector3 direction, float power, float innerSpotAngle, float outerSpotAngle, float range, bool
		                           isShadow, SimpleMath::Matrix lightViewProj) override;
		virtual void PassAmbientSkyColor(SimpleMath::Vector4 color) override;
		virtual void PassEyePos(SimpleMath::Vector3 camPos) override;

		virtual void PassIndoor(bool isIndoor) override;

		virtual void ResetShadowPass() override;

		virtual void FadeIn(float speed) override;
		virtual void FadeOut(float speed) override;

		// �׸��� �ؽ��ķ� ����� pass ������.
		const std::shared_ptr<DepthStencilResource>& GetShadowMap(int mapIdx);
		const GraphicsEngineSpace::cbLight& GetLight();
		const SimpleMath::Vector3& GetEyePos();
		const SimpleMath::Vector4& GetAmbientSky();

		// �׷��Ƚ� ���������� ����� ���̱� ������.. ������ ��ü�� �Լ��� ������ݴϴ�.
		void OITBindingUAV();
	};
}

// ������ ��ü�� �����ϴ� �Լ��� ���� ���ش�.
extern "C" GRAPHICSENGINE_DECLSPEC GraphicsEngineSpace::IRenderer* CreateRenderer();
extern "C" GRAPHICSENGINE_DECLSPEC void DeleteRenderer(GraphicsEngineSpace::IRenderer* obj);