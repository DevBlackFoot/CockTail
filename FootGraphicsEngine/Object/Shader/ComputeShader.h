#pragma once

#include "ShaderBase.h"

// 컴퓨트 셰이더와 관련된 요소들이 모여있는 클래스
	// => 사실상 픽셀, 지오메트리와 다른 것 없이 만들어줍니다.

namespace GraphicsEngineSpace
{

	class ComputeShader : public ShaderBase
	{
		// 캐싱용.
		ComPtr<ID3D11DeviceContext> deviceContext;
		// 컴퓨트 셰이더 본체
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
