#pragma once

#include "ShaderBase.h"

// �ȼ� ���̴��� ���õ� ��ҵ��� ���ִ� Ŭ����
namespace GraphicsEngineSpace
{
	class PixelShader : public ShaderBase
	{
		// �ȼ� ���̴� �ʿ�.
		ComPtr<ID3D11DeviceContext> deviceContext;
		ComPtr<ID3D11PixelShader> shader;
		
	public:
		PixelShader(ComPtr<ID3D11Device> _device, ComPtr<ID3D11DeviceContext> _deviceContext, ComPtr<ID3DBlob> shaderBlob);
		PixelShader(ComPtr<ID3D11Device> _device, ComPtr<ID3D11DeviceContext> _deviceContext, const std::vector<uint8_t>& shaderByteCode);
		~PixelShader() override;

		bool SetUpShader() override;

		// ������ �ּҰ����� �����ϴ� �뵵�̱� ������ ���������͸� ���.
		Shader* GetShader() override;

	private:
		bool CreateShader(ComPtr<ID3D11Device> _device, ComPtr<ID3DBlob> shaderBlob);
		bool CreateShader(ComPtr<ID3D11Device> _device, const std::vector<uint8_t>& shaderByteCode);

	};
}