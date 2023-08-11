#pragma once

#include "PassBase.h"

namespace GraphicsEngineSpace
{
	class VertexShader;
	class PixelShader;
	class RenderTargetTexture;
	class DepthStencilResource;
	class PostProcessTexture;

	// Blur_depth�� ���ȭ => ���� ������ �Ǿ�� Ŭ���� �ȿ� ������.
	constexpr int BLUR_DEPTH = 9;
	// ���� ������ ���
		// �׸��� ����(DSV) => LightPass�� RenderTarget�� Render
		// => �ش� Ÿ���� Texture�� �޾Ƽ� PostProcessing => �� ����� Back���ۿ� �׸�.

	/**
	 * \brief PostProcessing�� ���� �н��Դϴ�.
	 *
	 * Deferred Render ���� ��� ������ ���� Render Target�� �޾ƿ� �ȼ����� ó�����ִ� Pass �Դϴ�.
	 */
	class PostProcess : public PassBase
	{
	private:

		// ����� ����� RTV(main)
		std::shared_ptr<RenderTargetTexture> mainRTV;
		// �׸� �� ����� DSV (��ǻ� main)
		std::shared_ptr<DepthStencilResource> mainDSV;
		std::shared_ptr<PostProcessTexture> reduceTexture;
		// Mip UP �� Mip Down �� ������ ����غ��ô�..
			// MipDown[0] => fliter�� ������ �ٿ� ����..
			// MipDown[1] = MipUp[1] ������ ����
		std::vector<std::shared_ptr<RenderTargetTexture>> BloomMipUpRTV;
		std::vector<std::shared_ptr<RenderTargetTexture>> BloomMipDownRTV;

		// ��ü ���� ���� Ÿ�� �ΰ��� �غ��غ��ϴ�.
		std::vector<std::shared_ptr<RenderTargetTexture>> BlurMipUpRTV;
		std::vector<std::shared_ptr<RenderTargetTexture>> BlurMipDownRTV;

		// ���� ���̴�
		std::shared_ptr<VertexShader> quadVS;
		std::shared_ptr<PixelShader> postProcessPS;
		std::shared_ptr<PixelShader> notPostProcessPS;
		std::shared_ptr<PixelShader> postProcessNotBloomPS;

		std::shared_ptr<PixelShader> blurHPS;
		std::shared_ptr<PixelShader> blurVPS;
		std::shared_ptr<PixelShader> exceptDownPS;
		std::shared_ptr<PixelShader> downPS;
		std::shared_ptr<PixelShader> upSamplePS;


		// Reset�� ���� �׷��Ƚ� �ھ�
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

		// ����Ʈ ���μ��� ��..
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
