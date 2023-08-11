#include "GraphicsPch.h"
#include "PostProcessTexture.h"

namespace GraphicsEngineSpace
{
	PostProcessTexture::PostProcessTexture()
		: reductionBuffer(nullptr)
		, bufferSRV(nullptr)
	{
	}

	PostProcessTexture::~PostProcessTexture()
	{
	}

	bool PostProcessTexture::Init(ComPtr<ID3D11Device> device, int width, int height, int arraySize)
	{
		D3D11_TEXTURE2D_DESC textureDesc;
		HRESULT hr;
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;

		ZeroMemory(&textureDesc, sizeof(textureDesc));

		renderTargetArray.resize(arraySize);

		// �ش�κп��� index + 1��ŭ�� 2�� pow ���ش�.
			// 2�� ������ŭ width�� height�� ������ش�.		
		
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		// Array�� ������ 0 �̻��̾�� �մϴ�.
		textureDesc.ArraySize = arraySize;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;

		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		hr = device->CreateTexture2D(&textureDesc, 0, reductionBuffer.GetAddressOf());

		if (FAILED(hr) == true)
		{
			::MessageBoxA(nullptr, "Texture Create Failed!", nullptr, MB_OK);
			return false;
		}

		rtvDesc.Format = textureDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		rtvDesc.Texture2DArray.MipSlice = 0;
		rtvDesc.Texture2DArray.ArraySize = 1;

		for(int i = 0; i < arraySize; i++)
		{
			rtvDesc.Texture2DArray.FirstArraySlice = i;
			ComPtr<ID3D11RenderTargetView> tmpRTV;

			HR(device->CreateRenderTargetView(reductionBuffer.Get(), &rtvDesc, tmpRTV.GetAddressOf()));

			renderTargetArray[i] = tmpRTV;
		}

		// ���⼭ �׳� SRV�� ������ݴϴ�.
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		// �Ӹ� ������ ��� �ٸ��� �ٲ���� ���� �� �ϴ�..
		srvDesc.Texture2DArray.MostDetailedMip = 0;
		srvDesc.Texture2DArray.MipLevels = 1;
		srvDesc.Texture2DArray.ArraySize = arraySize;
		srvDesc.Texture2DArray.FirstArraySlice = 0;

		hr = device->CreateShaderResourceView(reductionBuffer.Get(), &srvDesc, bufferSRV.GetAddressOf());
		if (FAILED(hr) == true)
		{
			::MessageBoxA(nullptr, "SRV Create Failed!", nullptr, MB_OK);
			return false;
		}

		return true;
	}

	void PostProcessTexture::Finalize()
	{
		// ������ �͵� reset => nullptr�� ����
		if(reductionBuffer)
			reductionBuffer = nullptr;
		if(bufferSRV)
			bufferSRV = nullptr;

		for(auto& rtv : renderTargetArray)
		{
			rtv = nullptr;
		}

		renderTargetArray.clear();
	}

	// Texture �ϳ��ϳ��� �����ؼ� vector�� �־��ִ� Create Ŭ����.
	/*bool PostProcessTexture::CreateReductionBuffer(ComPtr<ID3D11Device> device, int width, int height, int reduceIdx)
	{
		D3D11_TEXTURE2D_DESC textureDesc;
		HRESULT hr;
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		
		ZeroMemory(&textureDesc, sizeof(textureDesc));

		// �ش�κп��� index + 1��ŭ�� 2�� pow ���ش�.
			// 2�� ������ŭ width�� height�� ������ش�.		
		int reduceExponent = static_cast<int>(roundf(powf(2.0f, static_cast<float>(reduceIdx + 1))));

		textureDesc.Width = width / reduceExponent;
		textureDesc.Height = height / reduceExponent;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;

		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		// �ܼ��� Shader Resource�θ� ����� ���̱� ������..
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		ComPtr<ID3D11Texture2D> tmpTexture2D;

		hr = device->CreateTexture2D(&textureDesc, 0, tmpTexture2D.GetAddressOf());

		if (FAILED(hr) == true)
		{
			::MessageBoxA(nullptr, "Texture Create Failed!", nullptr, MB_OK);
			return false;
		}
		// vector�� �ش� index�� �־��ݴϴ� => ���� �Լ����� resize�� ���־��� ������ �����մϴ�.
		reductionBuffer[reduceIdx] = tmpTexture2D;

		// ���⼭ �׳� SRV�� ������ݴϴ�.
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		// �Ӹ� ������ ��� �ٸ��� �ٲ���� ���� �� �ϴ�..
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		ComPtr<ID3D11ShaderResourceView> tmpSRV;

		hr = device->CreateShaderResourceView(tmpTexture2D.Get(), &srvDesc, tmpSRV.GetAddressOf());
		if (FAILED(hr) == true)
		{
			::MessageBoxA(nullptr, "SRV Create Failed!", nullptr, MB_OK);
			return false;
		}

		bufferSRV[reduceIdx] = tmpSRV;

		return true;
	}*/
}
