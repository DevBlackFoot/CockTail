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
	 * \brief IRenderer를 상속받아 실제로 구현한 클래스.
	 *
	 * 게임 엔진과 붙어 있는 부분을 떨어뜨리려 노력 중..
	 *
	 * 2022.06.14(화) Dev.BlackFoot
	 */
	class Renderer : public IRenderer, public enable_shared_from_this<Renderer>
	{
		// 변수 목록
		// 윈도우 핸들
		HWND hWnd;

		// Device, DC, RS 등이 들어가 있는 코어 클래스
		std::shared_ptr<DX11GraphicsCore> graphicsCore;

		// View들을 가지고 있는 렌더 타겟 클래스
		std::shared_ptr<RenderTargetTexture> mainRenderTarget;
		std::shared_ptr<DepthStencilResource> mainDepthStencil;
		std::shared_ptr<DepthStencilResource> noZTestDepthStencil;

		// depth debug용 렌더 타겟.
		std::shared_ptr<RenderTargetTexture> DMRAORenderTarget;
		std::shared_ptr<RenderTargetTexture> normalRenderTarget;
		std::shared_ptr<RenderTargetTexture> albedoRenderTarget;
		std::shared_ptr<RenderTargetTexture> worldPosRenderTarget;
		std::shared_ptr<RenderTargetTexture> emissiveRenderTarget;

		// lightPass 적용.
		std::unique_ptr<LightPass> lightPass;
		std::vector<std::shared_ptr<RenderTargetTexture>> gBuffer;

		// ShadowPass 적용
		std::unique_ptr<ShadowPass> shadowPass;

		// OIT Pass 적용
		std::unique_ptr<OITPass> oitPass;

		// PostProcess 적용
		std::unique_ptr<PostProcess> postProcessPass;

		// BlendState 세팅
		Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
		Microsoft::WRL::ComPtr<ID3D11BlendState> UIBlendState;

		// 하나만 있어도 되는 실루엣 DepthStencilState(마스킹 용)
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> silhouetteMaskState;
		// 망치 마스킹용
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> silhouetteWeaponMaskState;

		// 그래픽 디버그 헬퍼. Ref by. 지훈이 형
		Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation> annotation;
		unsigned int annotationCount;

		// Sprite
		std::shared_ptr<SpriteBatch> spriteBatch;

		float deltaTime;
		
		// 창 사이즈, 상태 등과 관련된 각종 변수.
			// 이게 렌더러에..?
		bool minimized;
		bool maximized;

		int clientWidth;
		int clientHeight;


	private:
		// 팩토리를 만들면
			// 화면에 그려줄 오브젝트 목록들.
			// 렌더 큐? 렌더 벡터?
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

		// 종횡비 획득
		virtual float GetAspectRatio() const override;

		// 창 사이즈 관련 변수
		virtual bool IsVaildDevice() override;
		virtual void SetClientSize(int _width, int _height) override;
		virtual void SetWinMinMax(bool _isMinimized, bool _isMaximized) override;
		
		// 오브젝트는 일괄적으로 Init을 통해 해줍니다 => RenderVector를 통한 Init은 하지 않음
		virtual void InitObject(std::shared_ptr<IDXObject> obj) override;

		virtual void BeginRender() override;
		// Loading용 Deferred 아닌 메인 Back버퍼 하나 적용
		virtual void BeginMainRender() override;
		// Transparency용 Z-Test 없는 depthbuffer 적용
		virtual void BeginTransparencyRender() override;
		// Particle 용 Z-Test 없는 depthbuffer 적용
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
		// 캔버스 생성
		virtual std::shared_ptr<Canvas> CreateCanvas(const std::string& name, float width, float height) override;

		// 렌더 타겟 세팅 함수들
		virtual void SetNullRenderTarget() override;
		virtual void SetDeferredRenderTarget() override;

		// 뎁스 스테이트 세팅 함수들
		virtual void SetDefaultDSState() override;
		virtual void SetNoZDSState() override;
		virtual void SetSilhouetteMaskDSState(int stencilRef, bool isWeapon) override;

		// 블렌드 스테이트 세팅 함수들
		virtual void SetDefaultBlendState() override;
		virtual void SetUIBlendState() override;

		// depthbuffer 얻기
		virtual std::shared_ptr<DepthStencilResource> GetDepthBuffer() override;
		virtual std::shared_ptr<RenderTargetTexture> GetDepthBufferRTT() override;

		// 디버그 이벤트
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

		// 그림자 텍스쳐로 사용할 pass 얻어오기.
		const std::shared_ptr<DepthStencilResource>& GetShadowMap(int mapIdx);
		const GraphicsEngineSpace::cbLight& GetLight();
		const SimpleMath::Vector3& GetEyePos();
		const SimpleMath::Vector4& GetAmbientSky();

		// 그래픽스 영역에서만 사용할 것이기 때문에.. 렌더러 자체의 함수로 만들어줍니다.
		void OITBindingUAV();
	};
}

// 렌더러 자체를 생성하는 함수를 따로 빼준다.
extern "C" GRAPHICSENGINE_DECLSPEC GraphicsEngineSpace::IRenderer* CreateRenderer();
extern "C" GRAPHICSENGINE_DECLSPEC void DeleteRenderer(GraphicsEngineSpace::IRenderer* obj);