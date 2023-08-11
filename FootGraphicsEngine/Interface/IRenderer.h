#pragma once
/// Dll 매크로
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
	* \brief Renderer의 기능만 정리해둔 인터페이스 클래스
	*
	* 2022.06.14(화) Dev.BlackFoot
	*/
	class GRAPHICSENGINE_DECLSPEC IRenderer
	{
	public:
		IRenderer() = default;
		virtual ~IRenderer() {}

		// 반드시 구현해줘야하는 목록
		// 초기화 => Device, DC, SwapChain 생성
		// 초기화 할 때 받아서 Render에게 넘겨준다.
		virtual bool Initialize(HWND _hWnd, int _clientWidth, int _clientHeight) abstract;

		virtual bool UIUtilsInitialize() abstract;

		// WIC Factory 이슈로 인한 Late Init
		virtual bool LateInitialize() abstract;

		// 렌더러 자체의 초기화가 끝난 후, 오브젝트 들을 초기화 해주는 함수 추가
		virtual void InitObject(std::shared_ptr<IDXObject> obj) abstract;

		// 엔진 종료
		virtual void Finalize() abstract;
		// 화면 창이 바뀌었을 때 해주는 onResize(렌더 타겟 생성)
		virtual void OnResize() abstract;

		// 그리기 함수
		virtual void BeginRender() abstract;
		// 로딩을 위한 백버퍼 Begin 함수
		virtual void BeginMainRender() abstract;
		virtual void BeginTransparencyRender() abstract;
		// 파티클 관련 렌더..
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

		// 깊이 판정을 하는 depth Stencil View를 사용해줍니다.
		// 렌더 타겟 세팅함수 (기록된 렌더타겟을 지우지 않습니다.)
		virtual void SetNullRenderTarget() abstract;
		virtual void SetDeferredRenderTarget() abstract;

		// depthState 세팅 함수들 생성
		virtual void SetDefaultDSState() abstract;
		virtual void SetNoZDSState() abstract;
		virtual void SetSilhouetteMaskDSState(int stencilRef, bool isWeapon) abstract;

		virtual void SetUIBlendState() abstract;
		virtual void SetDefaultBlendState() abstract;

		// 창 사이즈와 관련된 함수 목록 => 외부에서 사용 가능해야하기 때문에 인터페이스에 추가
		virtual bool IsVaildDevice() abstract;
		virtual void SetClientSize(int _width, int _height) abstract;
		virtual void SetWinMinMax(bool _isMinimized, bool _isMaximized) abstract;

		// 화면비 얻어오기
		virtual float GetAspectRatio() const abstract;

		// UI 관련 생성함수
		virtual std::shared_ptr<Canvas> CreateCanvas(const std::string& name, float width, float height) abstract;

		// 뎁스버퍼 얻기
		virtual std::shared_ptr<DepthStencilResource> GetDepthBuffer() abstract;
		virtual std::shared_ptr<RenderTargetTexture> GetDepthBufferRTT() abstract;

		// 그래픽스 디버그 이벤트
		virtual void GraphicsDebugBeginEvent(const std::string& name) abstract;
		virtual void GraphicsDebugEndEvent() abstract;

		// 빛 던지기
		virtual void PassDirectionalLight(SimpleMath::Vector3 color, SimpleMath::Vector3 direction, float power, bool isShadow, SimpleMath::Matrix lightViewProj) abstract;
		virtual void PassPointLight(SimpleMath::Vector3 color, SimpleMath::Vector3 position, float power, float range, bool isShadow, std::vector<SimpleMath::Matrix> lightViewProj) abstract;
		virtual void PassSpotLight(SimpleMath::Vector3 color, SimpleMath::Vector3 position, SimpleMath::Vector3 direction, float power, float innerSpotAngle, float outerSpotAngle, float range, bool
		                           isShadow, SimpleMath::Matrix lightViewProj) abstract;
		virtual void PassAmbientSkyColor(SimpleMath::Vector4 color) abstract;

		// eyePos 던지기
		virtual void PassEyePos(SimpleMath::Vector3 camPos) abstract;

		// 실내여부 던지기
		virtual void PassIndoor(bool isIndoor) abstract;

		// Scene이 바뀔 때 초기화 해주는 부분(특히 shadow Map을 초기화)
		virtual void ResetShadowPass() abstract;

		virtual void FadeIn(float speed) abstract;
		virtual void FadeOut(float speed) abstract;
	};

}