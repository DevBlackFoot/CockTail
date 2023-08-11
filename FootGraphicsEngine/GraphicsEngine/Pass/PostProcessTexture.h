#pragma once

namespace GraphicsEngineSpace
{
	class DX11GraphicsCore;

	/**
	 * \brief Blur에 사용하기 위한 축소 버퍼들(Texture2D Array)를 만들어주는 PostProcessTexture클래스입니다.
	 *
	 * 축소버퍼는 RTT에 저장하고 해당 클래스는 그 버퍼를 UpSacling 한 텍스쳐를 저장해둔 2DArray를 가지고 있도록 합니다.
	 * 2023.03.14 Dev.BlackFoot
	 */
	class PostProcessTexture
	{
		// 기본적으로 Texture와 SRV만 있으면 될 것 같습니다.
	private:
		ComPtr<ID3D11Texture2D> reductionBuffer;
		std::vector<ComPtr<ID3D11RenderTargetView>> renderTargetArray;
		ComPtr<ID3D11ShaderResourceView> bufferSRV;

	public:
		PostProcessTexture();
		~PostProcessTexture();

		bool Init(ComPtr<ID3D11Device> device, int width, int height, int arraySize);

		void Finalize();

		// 밖에서 변경하지 않을 것이기 때문에..
		const ComPtr<ID3D11ShaderResourceView>& GetShaderResourceView() { return bufferSRV; }
		const ComPtr<ID3D11Texture2D>& GetTexture() { return reductionBuffer; }
		const std::vector<ComPtr<ID3D11RenderTargetView>>& GetRenderTargetArray() { return renderTargetArray; }

		friend DX11GraphicsCore;

	private:
		//bool CreateReductionBuffer(ComPtr<ID3D11Device> device, int width, int height, int reduceIdx);
	};

}
