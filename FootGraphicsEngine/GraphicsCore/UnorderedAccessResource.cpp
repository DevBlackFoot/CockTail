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
		// ����̽� �˻�
		assert(device);

		// �⺻������ ���ۺ��� ������ݴϴ�.
		D3D11_BUFFER_DESC bufferDesc;
		// Byte Width�� Structured Byte Stride * numelements �Դϴ�.
		bufferDesc.ByteWidth = structuredByteStride * numElements;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		// Counter ���ο� ���� ���ε� �÷��װ� �޶����ϴ�.
		if(isCounter != true)
			bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		else
			// Counter���۸� uav�� ���ε� ���ݴϴ�.
			bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bufferDesc.StructureByteStride = structuredByteStride;

		// �ȿ� �� ���긮�ҽ� �����͸� �������ݴϴ�.
		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));

		data.pSysMem = pSysMem;
		// ���� ByteWidth�� �����մϴ�.
		data.SysMemPitch = bufferDesc.ByteWidth;
		data.SysMemSlicePitch = 0;

		// ���۸� �������ݴϴ�.
		if(FAILED(device->CreateBuffer(&bufferDesc, &data, uaBuffer.GetAddressOf())))
		{
			::MessageBoxA(nullptr, "Buffer Create Failed!", nullptr, MB_OK);
		}

		// �������� SRV�� ������ݴϴ�. �� �κ��� counter Buffer�� �ƴ� ���� ������ݴϴ�.
		if(isCounter != true)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			ZeroMemory(&srvDesc, sizeof(srvDesc));
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			srvDesc.Format = DXGI_FORMAT_UNKNOWN; //Must be UNKOWN when creating structured Buffer
			srvDesc.Buffer.FirstElement = 0;
			srvDesc.Buffer.NumElements = numElements;

			// SRV�� �������ݴϴ�.
			if (FAILED(device->CreateShaderResourceView(uaBuffer.Get(), &srvDesc, shaderResource.GetAddressOf())))
			{
				::MessageBoxA(nullptr, "SRV Create Failed!", nullptr, MB_OK);
			}
		}

		// ���������� UAV�� ������ݴϴ�.
			// �� �� pixel Link�� ���� flag�� �ٸ��� ������ ���ڷ� �޾��ݴϴ�.
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		ZeroMemory(&uavDesc, sizeof(uavDesc));
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.Buffer.Flags = UAVFlag;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = numElements;

		// uav�� �������ݴϴ�.
		if (FAILED(device->CreateUnorderedAccessView(uaBuffer.Get(), &uavDesc, unorderedAccessView.GetAddressOf())))
		{
			::MessageBoxA(nullptr, "UAV Create Failed!", nullptr, MB_OK);
		}
	}

	void UnorderedAccessResource::CreateFirstOffsetBuffer(ComPtr<ID3D11Device> device, UINT screenWidth,
		UINT screenHeight, UINT layerCount)
	{
		// ����̽� �˻�
		assert(device);

		// ���⵵ ���ۺ��� ������ݴϴ�.
		D3D11_BUFFER_DESC rawBufferDesc;
		rawBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		// OIT�� First Offset�̱� ������ ��ũ��, ���̾��� ���� + Uint size�� ���ϴ�.
		rawBufferDesc.ByteWidth =  screenWidth * screenHeight * layerCount * sizeof(unsigned int);
		rawBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		rawBufferDesc.CPUAccessFlags = 0;
		rawBufferDesc.StructureByteStride = 0;
		rawBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

		// ���۸� �������ݴϴ�.
			// Structured Buffer�� �ƴϱ� ������ �����ʹ� nullptr�Դϴ�.
		if (FAILED(device->CreateBuffer(&rawBufferDesc, nullptr, uaBuffer.GetAddressOf())))
		{
			::MessageBoxA(nullptr, "Buffer Create Failed!", nullptr, MB_OK);
		}

		// SRV�� ������ݴϴ�.
			// ������ ������ ���ۿ� ����, View Dimension���� �ٸ��� ������ �ٸ� �Լ��� ��������ϴ�.
		D3D11_SHADER_RESOURCE_VIEW_DESC rawSrvDesc;
		rawSrvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		rawSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		rawSrvDesc.BufferEx.FirstElement = 0;
		rawSrvDesc.BufferEx.NumElements = screenWidth * screenHeight;
		rawSrvDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;

		// SRV�� �������ݴϴ�.
		if (FAILED(device->CreateShaderResourceView(uaBuffer.Get(), &rawSrvDesc, shaderResource.GetAddressOf())))
		{
			::MessageBoxA(nullptr, "SRV Create Failed!", nullptr, MB_OK);
		}

		// ���������� UAV�� ������ݴϴ�.
		D3D11_UNORDERED_ACCESS_VIEW_DESC rawUavDesc;
		rawUavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		rawUavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		rawUavDesc.Buffer.FirstElement = 0;
		rawUavDesc.Buffer.NumElements = screenWidth * screenHeight;
		rawUavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;

		// uav�� �������ݴϴ�.
		if (FAILED(device->CreateUnorderedAccessView(uaBuffer.Get(), &rawUavDesc, unorderedAccessView.GetAddressOf())))
		{
			::MessageBoxA(nullptr, "UAV Create Failed!", nullptr, MB_OK);
		}
	}
}
