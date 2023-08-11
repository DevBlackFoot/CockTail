#include "GraphicsPch.h"
#include "UnorderedAccessResource.h"

namespace GraphicsEngineSpace
{
	UnorderedAccessResource::UnorderedAccessResource()
		: uaBuffer(nullptr)
		, shaderResource(nullptr)
		, unorderedAccessView(nullptr)
	{
	}

	UnorderedAccessResource::~UnorderedAccessResource()
	{

	}

	void UnorderedAccessResource::Finalize()
	{
		if(uaBuffer != nullptr)
			uaBuffer = nullptr;

		if(shaderResource != nullptr)
			shaderResource = nullptr;

		if(unorderedAccessView != nullptr)
			unorderedAccessView = nullptr;
	}

	void UnorderedAccessResource::CreateStructuredBuffer(ComPtr<ID3D11Device> device, UINT structuredByteStride,
	                                                     UINT numElements, const void* pSysMem, UINT UAVFlag, bool isCounter)
	{
		// 디바이스 검사
		assert(device);

		// 기본적으로 버퍼부터 만들어줍니다.
		D3D11_BUFFER_DESC bufferDesc;
		// Byte Width는 Structured Byte Stride * numelements 입니다.
		bufferDesc.ByteWidth = structuredByteStride * numElements;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		// Counter 여부에 따라서 바인드 플래그가 달라집니다.
		if(isCounter != true)
			bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		else
			// Counter버퍼면 uav만 바인드 해줍니다.
			bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bufferDesc.StructureByteStride = structuredByteStride;

		// 안에 들어갈 서브리소스 데이터를 설정해줍니다.
		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));

		data.pSysMem = pSysMem;
		// 위의 ByteWidth와 동일합니다.
		data.SysMemPitch = bufferDesc.ByteWidth;
		data.SysMemSlicePitch = 0;

		// 버퍼를 생성해줍니다.
		if(FAILED(device->CreateBuffer(&bufferDesc, &data, uaBuffer.GetAddressOf())))
		{
			::MessageBoxA(nullptr, "Buffer Create Failed!", nullptr, MB_OK);
		}

		// 다음으로 SRV를 만들어줍니다. 이 부분은 counter Buffer가 아닐 때만 만들어줍니다.
		if(isCounter != true)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			ZeroMemory(&srvDesc, sizeof(srvDesc));
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			srvDesc.Format = DXGI_FORMAT_UNKNOWN; //Must be UNKOWN when creating structured Buffer
			srvDesc.Buffer.FirstElement = 0;
			srvDesc.Buffer.NumElements = numElements;

			// SRV를 생성해줍니다.
			if (FAILED(device->CreateShaderResourceView(uaBuffer.Get(), &srvDesc, shaderResource.GetAddressOf())))
			{
				::MessageBoxA(nullptr, "SRV Create Failed!", nullptr, MB_OK);
			}
		}

		// 마지막으로 UAV를 만들어줍니다.
			// 이 때 pixel Link의 버퍼 flag가 다르기 때문에 인자로 받아줍니다.
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		ZeroMemory(&uavDesc, sizeof(uavDesc));
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.Buffer.Flags = UAVFlag;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = numElements;

		// uav를 생성해줍니다.
		if (FAILED(device->CreateUnorderedAccessView(uaBuffer.Get(), &uavDesc, unorderedAccessView.GetAddressOf())))
		{
			::MessageBoxA(nullptr, "UAV Create Failed!", nullptr, MB_OK);
		}
	}

	void UnorderedAccessResource::CreateFirstOffsetBuffer(ComPtr<ID3D11Device> device, UINT screenWidth,
		UINT screenHeight, UINT layerCount)
	{
		// 디바이스 검사
		assert(device);

		// 여기도 버퍼부터 만들어줍니다.
		D3D11_BUFFER_DESC rawBufferDesc;
		rawBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		// OIT의 First Offset이기 때문에 스크린, 레이어의 정보 + Uint size가 들어갑니다.
		rawBufferDesc.ByteWidth =  screenWidth * screenHeight * layerCount * sizeof(unsigned int);
		rawBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		rawBufferDesc.CPUAccessFlags = 0;
		rawBufferDesc.StructureByteStride = 0;
		rawBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

		// 버퍼를 생성해줍니다.
			// Structured Buffer가 아니기 때문에 데이터는 nullptr입니다.
		if (FAILED(device->CreateBuffer(&rawBufferDesc, nullptr, uaBuffer.GetAddressOf())))
		{
			::MessageBoxA(nullptr, "Buffer Create Failed!", nullptr, MB_OK);
		}

		// SRV를 만들어줍니다.
			// 위에서 생성한 버퍼와 포맷, View Dimension등이 다르기 때문에 다른 함수로 만들었습니다.
		D3D11_SHADER_RESOURCE_VIEW_DESC rawSrvDesc;
		rawSrvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		rawSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		rawSrvDesc.BufferEx.FirstElement = 0;
		rawSrvDesc.BufferEx.NumElements = screenWidth * screenHeight;
		rawSrvDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;

		// SRV를 생성해줍니다.
		if (FAILED(device->CreateShaderResourceView(uaBuffer.Get(), &rawSrvDesc, shaderResource.GetAddressOf())))
		{
			::MessageBoxA(nullptr, "SRV Create Failed!", nullptr, MB_OK);
		}

		// 마지막으로 UAV를 만들어줍니다.
		D3D11_UNORDERED_ACCESS_VIEW_DESC rawUavDesc;
		rawUavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		rawUavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		rawUavDesc.Buffer.FirstElement = 0;
		rawUavDesc.Buffer.NumElements = screenWidth * screenHeight;
		rawUavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;

		// uav를 생성해줍니다.
		if (FAILED(device->CreateUnorderedAccessView(uaBuffer.Get(), &rawUavDesc, unorderedAccessView.GetAddressOf())))
		{
			::MessageBoxA(nullptr, "UAV Create Failed!", nullptr, MB_OK);
		}
	}
}
