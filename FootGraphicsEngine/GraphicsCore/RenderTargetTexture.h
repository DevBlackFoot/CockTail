#pragma once

namespace GraphicsEngineSpace
{
	class DX11GraphicsCore;

	/**
	 * \brief Deferred Rendering, ���� ����� ������ Ȯ���ϱ� ���� ���� Ÿ�� �ؽ��� Ŭ�����̴�.
	 *
	 * 2D Texture�� �ϳ� �����ؼ� ���� Ÿ��ȭ ��Ű��, �ش� �ؽ��ĸ� Shader Resource View�� �����ؼ�
	 * ȭ�鿡 �ؽ��ķμ� �׷��� �� �ֵ��� �ϴ� ������ ������.
	 *
	 * 2022.10.24 Dev.BlackFoot
	 */
	class RenderTargetTexture
	{
		// �ʿ��� ��� ����
	private:
		ComPtr<ID3D11Texture2D> renderTexture;
		ComPtr<ID3D11RenderTargetView> renderTarget;
		ComPtr<ID3D11ShaderResourceView> shaderResource;

		int texWidth;
		int texHeight;

	public:
		RenderTargetTexture();
		~RenderTargetTexture();

		// init. �ؽ����� ũ�⸦ �޾Ƽ� �������ش�.
		bool Init(ComPtr<ID3D11Device> device, int width, int height, UINT MSAAQuality);
		bool Init(ComPtr<ID3D11Device> device, int width, int height, DXGI_FORMAT textureFormat, UINT MSAAQuality);

		
		// COM ������ ���� (�������� �� �� �ҷ��ش�)
		void Finalize();

		int GetTextureWidth() { return texWidth; }
		int GetTextureHeight() { return texHeight; }

		// ���� Ÿ�� �並 ���� �ݴϴ�.
			// GraphicsCore�� ResetView�� �����ֱ� ���ؼ� �ʿ��մϴ�.
		ComPtr<ID3D11RenderTargetView> GetRenderTargetView() { return renderTarget; }

		// ���̴� ���ҽ� �並 �����ݴϴ�.
			// �̸� ���̴��� �������ν� �ٸ� ���̴� ���� �ش� �ؽ��� �����Ϳ� ���� ���������ϴ�.
		ComPtr<ID3D11ShaderResourceView> GetShaderResourceView() { return shaderResource; }

		// ���� �ؽ��ĸ� �޾ƿɴϴ�.
		ComPtr<ID3D11Texture2D> GetRenderTexture() { return renderTexture; }

		friend DX11GraphicsCore;
	};

}