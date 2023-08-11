#pragma once

namespace GraphicsEngineSpace
{
	/**
	 * \brief �ش� Ŭ������ ��ƼŬ ������ �ʿ��� UAV�� SRV�� �ش��ϴ� ���۸� ������ Ŭ�����̸�,
	 *		������ ����(Pixel Link, First Offset, Counter, Particle Buffer)�� ���缭 �ʱ�ȭ�� �����մϴ�.
	 *
	 *		�������� desc�� ���� ���� �����ϴ� �Լ��� �ƴ� �Լ��� �з��Ͽ� �����ϴ�.
	 */
	class UnorderedAccessResource
	{

	private:
		// �⺻������ Structed�� ����� ����
		ComPtr<ID3D11Buffer> uaBuffer;
		// SRV => ���̴��� �־��� �� �ִ� ������ Counter Buffer�� ������� �ʽ��ϴ�.
		ComPtr<ID3D11ShaderResourceView> shaderResource;
		// UAV �� �����ϴ�.
		ComPtr<ID3D11UnorderedAccessView> unorderedAccessView;

	public:
		UnorderedAccessResource();
		~UnorderedAccessResource();

		void Finalize();

		// ���� ���͵�
		ComPtr<ID3D11Buffer> GetUABuffer() { return uaBuffer; }
		ComPtr<ID3D11ShaderResourceView> GetSRV() { return shaderResource; }
		ComPtr<ID3D11UnorderedAccessView> GetUAV() { return unorderedAccessView; }

		// ������ ��Ȳ�� �°� ������ݴϴ�.
		// ����Ǵ� ��. ByteWidth(structured * numElement) / StructuredByte / NumElement / uav flag
			// SRV ����
		// �ٽ��� �Ǵ� ���� Structured Buffer��� �� + UAV�� ���ۿ� ���¸� �� �� �ִ°�����..
			// �⺻������ ��ƼŬ ���̴��� ���Ǵ� ���۴� UAV�� ����մϴ�..
		// ���긮�ҽ� �����ͷ� ����� pSysMem�� ����.
		void CreateStructuredBuffer(ComPtr<ID3D11Device> device, UINT structuredByteStride, UINT numElements, const void* pSysMem, UINT UAVFlag, bool isCounter = false);

		// FirstOffset ���۴� ��ҵ��� ������ �ٸ��� ������ ���� �Լ��� ������ݴϴ�.
		void CreateFirstOffsetBuffer(ComPtr<ID3D11Device> device, UINT screenWidth, UINT screenHeight, UINT layerCount);

	};

}
