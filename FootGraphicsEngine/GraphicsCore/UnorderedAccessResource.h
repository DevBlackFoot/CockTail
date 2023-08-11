#pragma once

namespace GraphicsEngineSpace
{
	/**
	 * \brief 해당 클래스는 파티클 렌더에 필요한 UAV와 SRV에 해당하는 버퍼를 래핑한 클래스이며,
	 *		각각의 역할(Pixel Link, First Offset, Counter, Particle Buffer)에 맞춰서 초기화를 진행합니다.
	 *
	 *		공통적인 desc의 포맷 등을 공유하는 함수와 아닌 함수를 분류하여 만들어봅니다.
	 */
	class UnorderedAccessResource
	{

	private:
		// 기본적으로 Structed로 사용할 버퍼
		ComPtr<ID3D11Buffer> uaBuffer;
		// SRV => 셰이더에 넣어줄 수 있는 것으로 Counter Buffer는 사용하지 않습니다.
		ComPtr<ID3D11ShaderResourceView> shaderResource;
		// UAV 를 가집니다.
		ComPtr<ID3D11UnorderedAccessView> unorderedAccessView;

	public:
		UnorderedAccessResource();
		~UnorderedAccessResource();

		void Finalize();

		// 각종 게터들
		ComPtr<ID3D11Buffer> GetUABuffer() { return uaBuffer; }
		ComPtr<ID3D11ShaderResourceView> GetSRV() { return shaderResource; }
		ComPtr<ID3D11UnorderedAccessView> GetUAV() { return unorderedAccessView; }

		// 각각의 상황에 맞게 만들어줍니다.
		// 변경되는 것. ByteWidth(structured * numElement) / StructuredByte / NumElement / uav flag
			// SRV 여부
		// 핵심이 되는 것은 Structured Buffer라는 것 + UAV는 버퍼에 상태를 쓸 수 있는가여부..
			// 기본적으로 파티클 셰이더에 사용되는 버퍼는 UAV를 사용합니다..
		// 서브리소스 데이터로 사용할 pSysMem이 들어간다.
		void CreateStructuredBuffer(ComPtr<ID3D11Device> device, UINT structuredByteStride, UINT numElements, const void* pSysMem, UINT UAVFlag, bool isCounter = false);

		// FirstOffset 버퍼는 요소들이 완전히 다르기 때문에 따로 함수를 만들어줍니다.
		void CreateFirstOffsetBuffer(ComPtr<ID3D11Device> device, UINT screenWidth, UINT screenHeight, UINT layerCount);

	};

}
