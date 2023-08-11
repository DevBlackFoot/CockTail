#pragma once

#include "PassBase.h"

namespace GraphicsEngineSpace
{
	class VertexShader;
	class PixelShader;
	class RenderTargetTexture;
	class DepthStencilResource;
	class PostProcessTexture;

	// Blur_depth의 상수화 => 전역 변수가 되어야 클래스 안에 들어가진다.
	constexpr int BLUR_DEPTH = 9;
	// 렌더 순서의 경우
		// 그림자 렌더(DSV) => LightPass의 RenderTarget에 Render
		// => 해당 타겟의 Texture를 받아서 PostProcessing => 이 결과를 Back버퍼에 그림.

	/**
	 * \brief PostProcessing을 위한 패스입니다.
	 *
	 * Deferred Render 이후 모든 렌더가 끝난 Render Target을 받아와 픽셀별로 처리해주는 Pass 입니다.
	 */
	class PostProcess : public PassBase
	{
	private:

		// 결과를 출력할 RTV(main)
		std::shared_ptr<RenderTargetTexture> mainRTV;
		// 그릴 때 사용할 DSV (사실상 main)
		std::shared_ptr<DepthStencilResource> mainDSV;
		std::shared_ptr<PostProcessTexture> reduceTexture;
		// Mip UP 과 Mip Down 두 가지만 사용해봅시다..
			// MipDown[0] => fliter들어간 최초의 다운 샘플..
			// MipDown[1] = MipUp[1] 사이즈 동일
		std::vector<std::shared_ptr<RenderTargetTexture>> BloomMipUpRTV;
		std::vector<std::shared_ptr<RenderTargetTexture>> BloomMipDownRTV;

		// 전체 블러를 위한 타겟 두개를 준비해봅니다.
		std::vector<std::shared_ptr<RenderTargetTexture>> BlurMipUpRTV;
		std::vector<std::shared_ptr<RenderTargetTexture>> BlurMipDownRTV;

		// 각종 셰이더
		std::shared_ptr<VertexShader> quadVS;
		std::shared_ptr<PixelShader> postProcessPS;
		std::shared_ptr<PixelShader> notPostProcessPS;
		std::shared_ptr<PixelShader> postProcessNotBloomPS;

		std::shared_ptr<PixelShader> blurHPS;
		std::shared_ptr<PixelShader> blurVPS;
		std::shared_ptr<PixelShader> exceptDownPS;
		std::shared_ptr<PixelShader> downPS;
		std::shared_ptr<PixelShader> upSamplePS;


		// Reset을 위한 그래픽스 코어
		std::shared_ptr<DX11GraphicsCore> graphicsCore;

		SimpleMath::Vector4 vignetteBuffer;
		SimpleMath::Vector4 vignetteColorBuffer;
		SimpleMath::Vector4 bloomThresholdBuffer;
		SimpleMath::Vector4 texInfoBuffer;
		SimpleMath::Vector4 fadeOffset;

		int downSampleSize = 1;
		int blurDownSize = 1;

		ComPtr<ID3D11BlendState> postBlendState;
		float fadeSpeed = 0.f;

		// 포스트 프로세싱 용..
		bool bloom = true;
		bool allPostProcessing = true;

	public:
		virtual void Start(const std::shared_ptr<RenderTargetTexture>& mainRTV, const std::shared_ptr<DepthStencilResource>& mainDSV) override;
		virtual void Release() override;

		virtual void OnResize(int width, int height) override;

		void RenderStart();

		//void DownSample(const std::shared_ptr<RenderTargetTexture>& renderTarget);
		void Blur(const std::shared_ptr<RenderTargetTexture>& sourceTarget);
		void Bloom(const std::shared_ptr<RenderTargetTexture>& sourceTarget);

		void Render(const std::shared_ptr<RenderTargetTexture>& renderTarget);

		void EndRender();

		void ImGuiRender();

		void StartFadeIn(float speed);
		void StartFadeOut(float speed);
	};

}
