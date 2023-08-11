#include "GraphicsPch.h"
#include "ComputeShader.h"

namespace GraphicsEngineSpace
{
	ComputeShader::ComputeShader(ComPtr<ID3D11Device> _device, ComPtr<ID3D11DeviceContext> _deviceContext,
		ComPtr<ID3DBlob> shaderBlob)
		: ShaderBase(ShaderType::COMPUTE)
		, deviceContext(_deviceContext)
		, shader(nullptr)
	{
		if(CreateShader(_device, shaderBlob) != true)
		{
			assert(0);
		}
	}

	ComputeShader::ComputeShader(ComPtr<ID3D11Device> _device, ComPtr<ID3D11DeviceContext> _deviceContext,
		const std::vector<uint8_t>& shaderByteCode)
		: ShaderBase(ShaderType::COMPUTE)
		, deviceContext(_deviceContext)
		, shader(nullptr)
	{
		if (CreateShader(_device, shaderByteCode) != true)
		{
			assert(0);
		}
	}

	ComputeShader::~ComputeShader()
	{
	}

	bool ComputeShader::SetUpShader()
	{
		// ���̴� �ν��Ͻ�..�� ���ַ��� ���ڸ� �޾Ƽ� ���⼭ ���� �� ���� �� �ϴ�.
		deviceContext->CSSetShader(shader.Get(), nullptr, 0);

		return true;
	}

	Shader* ComputeShader::GetShader()
	{
		return reinterpret_cast<Shader*>(shader.Get());
	}

	bool ComputeShader::CreateShader(ComPtr<ID3D11Device> _device, ComPtr<ID3DBlob> shaderBlob)
	{
		// ���̴� ���÷����� �� �� �ִٸ� �̰����� ���� �� ���� �� �ϴ�.
		HRESULT hr = _device->CreateComputeShader
		(
			shaderBlob->GetBufferPointer(),
			shaderBlob->GetBufferSize(),
			nullptr,
			shader.GetAddressOf()
		);

		return SUCCEEDED(hr);
	}

	bool ComputeShader::CreateShader(ComPtr<ID3D11Device> _device, const std::vector<uint8_t>& shaderByteCode)
	{
		HRESULT hr = _device->CreateComputeShader
		(
			shaderByteCode.data(),
			shaderByteCode.size(),
			nullptr,
			shader.GetAddressOf()
		);

		return SUCCEEDED(hr);
	}
}
