#pragma once
/// Dll ��ũ��
#ifdef FOOTGRAPHICSENGINE_EXPORTS
#define GRAPHICSENGINE_DECLSPEC __declspec(dllexport)
#else
#define GRAPHICSENGINE_DECLSPEC __declspec(dllimport)
#endif

namespace GraphicsEngineSpace
{
	class IDXObject;
	class Canvas;
	class DepthStencilResource;
	class RenderTargetTexture;

	/**
	* \brief Renderer�� ��ɸ� �����ص� �������̽� Ŭ����
	*
	* 2022.06.14(ȭ) Dev.BlackFoot
	*/
	class GRAPHICSENGINE_DECLSPEC IRenderer
	{
	public:
		IRenderer() = default;
		virtual ~IRenderer() {}

		// �ݵ�� ����������ϴ� ���
		// �ʱ�ȭ => Device, DC, SwapChain ����
		// �ʱ�ȭ �� �� �޾Ƽ� Render���� �Ѱ��ش�.
		virtual bool Initialize(HWND _hWnd, int _clientWidth, int _clientHeight) abstract;

		virtual bool UIUtilsInitialize() abstract;

		// WIC Factory �̽��� ���� Late Init
		virtual bool LateInitialize() abstract;

		// ������ ��ü�� �ʱ�ȭ�� ���� ��, ������Ʈ ���� �ʱ�ȭ ���ִ� �Լ� �߰�
		virtual void InitObject(std::shared_ptr<IDXObject> obj) abstract;

		// ���� ����
		virtual void Finalize() abstract;
		// ȭ�� â�� �ٲ���� �� ���ִ� onResize(���� Ÿ�� ����)
		virtual void OnResize() abstract;

		// �׸��� �Լ�
		virtual void BeginRender() abstract;
		// �ε��� ���� ����� Begin �Լ�
		virtual void BeginMainRender() abstract;
		virtual void BeginTransparencyRender() abstract;
		// ��ƼŬ ���� ����..
		virtual void BeginParticleRender() abstract;
		virtual void Render() abstract;
		virtual void OITParticleMerge() abstract;
		virtual void PostProcessing() abstract;
		virtual void DebugRender() abstract;
		virtual void ShadowRenderStart() abstract;
		virtual void ShadowRenderStart(int idx, bool isPointLight) abstract;
		virtual void ShadowBakeStart(int idx, bool isPointLight) abstract;
		virtual void ShadowRender(std::shared_ptr<IDXObject> obj, bool isPointLight) abstract;
		virtual void ShadowBake(std::shared_ptr<IDXObject> obj, bool isPointLight) abstract;
		virtual void ShadowRenderEnd() abstract;
		virtual void EndRender() abstract;

		// ���� ������ �ϴ� depth Stencil View�� ������ݴϴ�.
		// ���� Ÿ�� �����Լ� (��ϵ� ����Ÿ���� ������ �ʽ��ϴ�.)
		virtual void SetNullRenderTarget() abstract;
		virtual void SetDeferredRenderTarget() abstract;

		// depthState ���� �Լ��� ����
		virtual void SetDefaultDSState() abstract;
		virtual void SetNoZDSState() abstract;
		virtual void SetSilhouetteMaskDSState(int stencilRef, bool isWeapon) abstract;

		virtual void SetUIBlendState() abstract;
		virtual void SetDefaultBlendState() abstract;

		// â ������� ���õ� �Լ� ��� => �ܺο��� ��� �����ؾ��ϱ� ������ �������̽��� �߰�
		virtual bool IsVaildDevice() abstract;
		virtual void SetClientSize(int _width, int _height) abstract;
		virtual void SetWinMinMax(bool _isMinimized, bool _isMaximized) abstract;

		// ȭ��� ������
		virtual float GetAspectRatio() const abstract;

		// UI ���� �����Լ�
		virtual std::shared_ptr<Canvas> CreateCanvas(const std::string& name, float width, float height) abstract;

		// �������� ���
		virtual std::shared_ptr<DepthStencilResource> GetDepthBuffer() abstract;
		virtual std::shared_ptr<RenderTargetTexture> GetDepthBufferRTT() abstract;

		// �׷��Ƚ� ����� �̺�Ʈ
		virtual void GraphicsDebugBeginEvent(const std::string& name) abstract;
		virtual void GraphicsDebugEndEvent() abstract;

		// �� ������
		virtual void PassDirectionalLight(SimpleMath::Vector3 color, SimpleMath::Vector3 direction, float power, bool isShadow, SimpleMath::Matrix lightViewProj) abstract;
		virtual void PassPointLight(SimpleMath::Vector3 color, SimpleMath::Vector3 position, float power, float range, bool isShadow, std::vector<SimpleMath::Matrix> lightViewProj) abstract;
		virtual void PassSpotLight(SimpleMath::Vector3 color, SimpleMath::Vector3 position, SimpleMath::Vector3 direction, float power, float innerSpotAngle, float outerSpotAngle, float range, bool
		                           isShadow, SimpleMath::Matrix lightViewProj) abstract;
		virtual void PassAmbientSkyColor(SimpleMath::Vector4 color) abstract;

		// eyePos ������
		virtual void PassEyePos(SimpleMath::Vector3 camPos) abstract;

		// �ǳ����� ������
		virtual void PassIndoor(bool isIndoor) abstract;

		// Scene�� �ٲ� �� �ʱ�ȭ ���ִ� �κ�(Ư�� shadow Map�� �ʱ�ȭ)
		virtual void ResetShadowPass() abstract;

		virtual void FadeIn(float speed) abstract;
		virtual void FadeOut(float speed) abstract;
	};

}