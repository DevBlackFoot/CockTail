#pragma once

#include "ShaderBase.h"

// ��ǻƮ ���̴��� ���õ� ��ҵ��� ���ִ� Ŭ����
	// => ��ǻ� �ȼ�, ������Ʈ���� �ٸ� �� ���� ������ݴϴ�.

namespace GraphicsEngineSpace
{

	class ComputeShader : public ShaderBase
	{
		// ĳ�̿�.
		ComPtr<ID3D11DeviceContext> deviceContext;
		// ��ǻƮ ���̴� ��ü
		ComPtr<ID3D11ComputeShader> shader;

	public:
		ComputeShader(ComPtr<ID3D11Device> _device, ComPtr<ID3D11DeviceContext> _deviceContext, ComPtr<ID3DBlob> shaderBlob);
		ComputeShader(ComPtr<ID3D11Device> _device, ComPtr<ID3D11DeviceContext> _deviceContext, const std::vector<uint8_t>& shaderByteCode);

		~ComputeShader() override;

		bool SetUpShader() override;

		Shader* GetShader() override;

	private:
		bool CreateShader(ComPtr<ID3D11Device> _device, ComPtr<ID3DBlob> shaderBlob);
		bool CreateShader(ComPtr<ID3D11Device> _device, const std::vector<uint8_t>& shaderByteCode);

	};

}
