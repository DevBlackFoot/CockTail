#pragma once
#include "PassBase.h"

namespace GraphicsEngineSpace
{
	// �н��� ����� ���� Ŭ�������� ���漱�� ���ݴϴ�.
	class RenderTargetTexture;
	class DepthStencilResource;
	// ������� ����
	class UnorderedAccessResource;

	class VertexShader;
	class PixelShader;

	// Order Independent Transparency�� ��ƼŬ�� �׸��� ���� �н��Դϴ�.
	class OITPass : public PassBase
	{
	private:
		std::shared_ptr<RenderTargetTexture> OITRTV;
		// ��ǻ� Main DSV
		std::shared_ptr<DepthStencilResource> OITDSV;

		std::shared_ptr<VertexShader> quadVS;
		std::shared_ptr<PixelShader> OITBlendPS;

		std::shared_ptr<DX11GraphicsCore> graphicsCore;

		// Pass���� ����ϴ� �ΰ����� UAV�� ��� ������ ������ �ֵ��� ���ݴϴ�.
		std::shared_ptr<UnorderedAccessResource> pixelLinkUAResource;
		std::shared_ptr<UnorderedAccessResource> firstOffsetUAResource;

		// ������ �ϱ� ���� �ʿ��� �迭��
		ComPtr<ID3D11UnorderedAccessView> uavList[2];
		unsigned int initCount[2] = { 0, 0 };

		// �⺻������ ���� ť�� ���������� �ʽ��ϴ� =>
			// ���⼭ ���� �κ� ���� => ��� ��ƼŬ ������ ���ӿ����� ������(preRender) => UAV ����
			// => ��ƼŬ Render.
			// => ���� ��� ������ ���
			// Draw
			// �ٽ����� uav ������ 1���� �ҷ����մϴ�..
	public:
		virtual void Start(const std::shared_ptr<RenderTargetTexture>& mainRTV, const std::shared_ptr<DepthStencilResource>& mainDSV) override;
		virtual void Release() override;
		virtual void OnResize(int width, int height) override;

		// ���̾� ����, merge �Լ�
		void BeginRender();
		// ��� ��ƼŬ ������ ���� ���� �ش� ������ �������� ȭ�鿡 ������ �׷��ִ� �Լ�.
		void Render(const std::shared_ptr<RenderTargetTexture>& backGroundRTT);

		void EndRender();

		// UAV, SRV ���ε�, unbinding
		void BindingUAV();
		void BindingSRV();
		void UnBindingUAV();

		const std::shared_ptr<RenderTargetTexture>& GetOITRTV() { return OITRTV; }

	private:
		// UAV�� �ʱ� ���·� �����ִ� �Լ�
		void Clear();

		// ���̾ ��� ������� ��, blend PS�� ���� �׷��ִ� �Լ�
		void Draw(const std::shared_ptr<RenderTargetTexture>& backGroundRTT);
	};

}
