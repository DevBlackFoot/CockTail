#pragma once
#include "PassBase.h"

namespace GraphicsEngineSpace
{
	// 패스에 사용할 각종 클래스들을 전방선언 해줍니다.
	class RenderTargetTexture;
	class DepthStencilResource;
	// 사용해줄 버퍼
	class UnorderedAccessResource;

	class VertexShader;
	class PixelShader;

	// Order Independent Transparency로 파티클을 그리기 위한 패스입니다.
	class OITPass : public PassBase
	{
	private:
		std::shared_ptr<RenderTargetTexture> OITRTV;
		// 사실상 Main DSV
		std::shared_ptr<DepthStencilResource> OITDSV;

		std::shared_ptr<VertexShader> quadVS;
		std::shared_ptr<PixelShader> OITBlendPS;

		std::shared_ptr<DX11GraphicsCore> graphicsCore;

		// Pass에서 사용하는 두가지의 UAV를 멤버 변수로 가지고 있도록 해줍니다.
		std::shared_ptr<UnorderedAccessResource> pixelLinkUAResource;
		std::shared_ptr<UnorderedAccessResource> firstOffsetUAResource;

		// 세팅을 하기 위해 필요한 배열들
		ComPtr<ID3D11UnorderedAccessView> uavList[2];
		unsigned int initCount[2] = { 0, 0 };

		// 기본적으로 렌더 큐를 돌려주지는 않습니다 =>
			// 여기서 시작 부분 세팅 => 모든 파티클 렌더를 게임엔진의 씬에서(preRender) => UAV 세팅
			// => 파티클 Render.
			// => 이후 모든 정보를 모아
			// Draw
			// 핵심적인 uav 세팅은 1번만 불러야합니다..
	public:
		virtual void Start(const std::shared_ptr<RenderTargetTexture>& mainRTV, const std::shared_ptr<DepthStencilResource>& mainDSV) override;
		virtual void Release() override;
		virtual void OnResize(int width, int height) override;

		// 레이어 생성, merge 함수
		void BeginRender();
		// 모든 파티클 렌더가 끝난 이후 해당 정보를 바탕으로 화면에 실제로 그려주는 함수.
		void Render(const std::shared_ptr<RenderTargetTexture>& backGroundRTT);

		void EndRender();

		// UAV, SRV 바인딩, unbinding
		void BindingUAV();
		void BindingSRV();
		void UnBindingUAV();

		const std::shared_ptr<RenderTargetTexture>& GetOITRTV() { return OITRTV; }

	private:
		// UAV를 초기 상태로 돌려주는 함수
		void Clear();

		// 레이어가 모두 만들어진 뒤, blend PS를 통해 그려주는 함수
		void Draw(const std::shared_ptr<RenderTargetTexture>& backGroundRTT);
	};

}
