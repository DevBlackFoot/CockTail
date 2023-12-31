#pragma once

namespace GraphicsEngineSpace
{
	// 샘플러 스테이트를 미리 만들어서 넣어둔 샘플러 스테이트 클래스
	class SamplerManager
	{
	private:
		static std::shared_ptr<SamplerManager> instance;

		ComPtr<ID3D11SamplerState> anisotropicWrapSamplerState;
		ComPtr<ID3D11SamplerState> anisotropicClampSamplerState;
		ComPtr<ID3D11SamplerState> linearWrapSamplerState;
		ComPtr<ID3D11SamplerState> linearClampSamplerState;
		ComPtr<ID3D11SamplerState> pointClampSamplerState;
		ComPtr<ID3D11SamplerState> comparisonLinearSamplerState;
		ComPtr<ID3D11SamplerState> comparisonLinearWrapSamplerState;

	public:
		SamplerManager()
			: anisotropicWrapSamplerState(nullptr)
			, anisotropicClampSamplerState(nullptr)
			, linearWrapSamplerState(nullptr)
			, linearClampSamplerState(nullptr)
			, pointClampSamplerState(nullptr)
			, comparisonLinearSamplerState(nullptr)
			, comparisonLinearWrapSamplerState(nullptr)
		{
			
		}

		void Init(ComPtr<ID3D11Device> device);

		void Release();

		static std::shared_ptr<SamplerManager> GetInstance();

		ComPtr<ID3D11SamplerState> GetAnisotropicWrap() { return anisotropicWrapSamplerState; }
		ComPtr<ID3D11SamplerState> GetAnisotropicClamp() { return anisotropicClampSamplerState; }
		ComPtr<ID3D11SamplerState> GetLinearWrap() { return linearWrapSamplerState; }
		ComPtr<ID3D11SamplerState> GetLinearClamp() { return linearClampSamplerState; }
		ComPtr<ID3D11SamplerState> GetPointClamp() { return pointClampSamplerState; }
		ComPtr<ID3D11SamplerState> GetComparisonLinear() { return comparisonLinearSamplerState; }
		ComPtr<ID3D11SamplerState> GetComparisonLinearWrap() { return comparisonLinearWrapSamplerState; }

	};

}