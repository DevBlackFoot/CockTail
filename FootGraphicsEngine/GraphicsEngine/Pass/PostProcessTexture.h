#pragma once

namespace GraphicsEngineSpace
{
	class DX11GraphicsCore;

	/**
	 * \brief Blur�� ����ϱ� ���� ��� ���۵�(Texture2D Array)�� ������ִ� PostProcessTextureŬ�����Դϴ�.
	 *
	 * ��ҹ��۴� RTT�� �����ϰ� �ش� Ŭ������ �� ���۸� UpSacling �� �ؽ��ĸ� �����ص� 2DArray�� ������ �ֵ��� �մϴ�.
	 * 2023.03.14 Dev.BlackFoot
	 */
	class PostProcessTexture
	{
		// �⺻������ Texture�� SRV�� ������ �� �� �����ϴ�.
	private:
		ComPtr<ID3D11Texture2D> reductionBuffer;
		std::vector<ComPtr<ID3D11RenderTargetView>> renderTargetArray;
		ComPtr<ID3D11ShaderResourceView> bufferSRV;

	public:
		PostProcessTexture();
		~PostProcessTexture();

		bool Init(ComPtr<ID3D11Device> device, int width, int height, int arraySize);

		void Finalize();

		// �ۿ��� �������� ���� ���̱� ������..
		const ComPtr<ID3D11ShaderResourceView>& GetShaderResourceView() { return bufferSRV; }
		const ComPtr<ID3D11Texture2D>& GetTexture() { return reductionBuffer; }
		const std::vector<ComPtr<ID3D11RenderTargetView>>& GetRenderTargetArray() { return renderTargetArray; }

		friend DX11GraphicsCore;

	private:
		//bool CreateReductionBuffer(ComPtr<ID3D11Device> device, int width, int height, int reduceIdx);
	};

}
