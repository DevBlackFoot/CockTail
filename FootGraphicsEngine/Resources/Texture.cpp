#include "GraphicsPch.h"
#include "Texture.h"
#include "GraphicsCore/DX11GraphicsCore.h"
#include "PerlinNoise.hpp"

namespace GraphicsEngineSpace
{
	void Texture::Load(const std::wstring& path)
	{
		std::wstring ext = fs::path(path).extension();

		if (ext == L".dds" || ext == L".DDS")
			::LoadFromDDSFile(path.c_str(), DDS_FLAGS_NONE, nullptr, image);
		else if (ext == L".tga" || ext == L".TGA")
			::LoadFromTGAFile(path.c_str(), nullptr, image);
		else if (ext == L".hdr" || ext == L".HDR")
			::LoadFromHDRFile(path.c_str(), nullptr, image);
		else // png, jpg, jpeg, bmp
			::LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, nullptr, image);

		HRESULT hr = ::CreateTexture(DX11GraphicsCore::GetInstance()->GetDevice().Get(), image.GetImages(), image.GetImageCount(), image.GetMetadata(),
			reinterpret_cast<ID3D11Resource**>(texResource.GetAddressOf()));

		if (FAILED(hr))
			MessageBox(0, TEXT("Texture Failed."), 0, 0);

		bool isCubeMap = image.GetMetadata().IsCubemap();

		D3D11_TEXTURE2D_DESC textureDesc;

		texResource->GetDesc(&textureDesc);

		width = textureDesc.Width;

		height = textureDesc.Height;

		levels = textureDesc.MipLevels;

		format = textureDesc.Format;

		// 레벨 부여
		//if (_levels <= 0)
			//_levels = Texture::NumMipmapLevels(_width, _height);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

		ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

		srvDesc.Format = textureDesc.Format;
		srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;

		if (!isCubeMap)
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		else
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;

		hr = DX11GraphicsCore::GetInstance()->GetDevice()->CreateShaderResourceView(texResource.Get(), &srvDesc, mapSRV.GetAddressOf());

		if (FAILED(hr))
			MessageBox(0, TEXT("Texture Failed."), 0, 0);

		// Mip Level이 만약 0이면 .. Mip Map 형성해주자.
		/*if (textureDesc.MipLevels == 0)
			Graphics_Interface::Get()->GetDeviceClass()->GetDeviceContext()->GenerateMips(this->mapSRV.Get());*/
	}

	void Texture::Release()
	{
		texResource.ReleaseAndGetAddressOf();

		mapSRV.ReleaseAndGetAddressOf();
	}

	// 1차원 랜덤 텍스쳐를 생성해서 this의 SRV와 2D Texture에 넣어줍니다.
	void Texture::CreateRandomTexture1D()
	{
		auto device = DX11GraphicsCore::GetInstance()->GetDevice();

		// 랜덤 Value
		XMFLOAT4 randomValue[1024];
		int count = 0;
		for (int i = 0; i < 1024; i++)
		{
			randomValue[i].x = MathHelper::RandF(-1.f, 1.f);
			randomValue[i].y = MathHelper::RandF(-1.f, 1.f);
			randomValue[i].z = MathHelper::RandF(-1.f, 1.f);
			randomValue[i].w = MathHelper::RandF(-1.f, 1.f);

			// random값이 0.9와 1.0 사이일 때 count를 올려줍니다.
			if (0.9f <= randomValue[i].x && randomValue[i].x <= 1.0f)
				count++;
		}

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = randomValue;
		data.SysMemPitch = 1024 * sizeof(XMFLOAT4);
		data.SysMemSlicePitch = 0;

		// Texture를 생성합니다.
		D3D11_TEXTURE2D_DESC texDesc;

		texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.Usage = D3D11_USAGE_IMMUTABLE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.ArraySize = 1;
		texDesc.MipLevels = 1;
		// 1차원 텍스쳐이기 때문에..
		texDesc.Width = 1024;
		texDesc.Height = 1;

		// 각각의 정보들을 가져와 줍니다.
		width = texDesc.Width;
		height = texDesc.Height;
		levels = texDesc.MipLevels;
		format = texDesc.Format;

		// 2D texture를 만들어줍니다.
		HRESULT hr = device->CreateTexture2D(&texDesc, &data, texResource.GetAddressOf());

		if (FAILED(hr))
			MessageBox(0, TEXT("Texture Failed."), 0, 0);

		// SRV를 만들어줍니다.
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;

		hr = device->CreateShaderResourceView(texResource.Get(), &srvDesc, mapSRV.GetAddressOf());

		if (FAILED(hr))
			MessageBox(0, TEXT("TextureSRV Failed."), 0, 0);
	}

	// 노이즈 텍스쳐를 생성해서 this의 SRV와 texture에 넣어줍니다.
	void Texture::CreatePerlinNoiseTexture(float frequency, int octaves, float octaveMultiplier, std::uint32_t seed,
		float width, float height)
	{
		auto device = DX11GraphicsCore::GetInstance()->GetDevice();

		const siv::PerlinNoise perlin0 { seed };
		const siv::PerlinNoise perlin1 { seed + 1 };
		const siv::PerlinNoise perlin2 { seed + 2 };
		const double fx = frequency / width;
		const double fy = frequency / height;

		std::vector<XMFLOAT4> colors(width * height);

		for (std::int32_t y = 0; y < static_cast<std::int32_t>(height); y++)
		{
			for (std::int32_t x = 0; x < static_cast<std::int32_t>(width); x++)
			{
				int index = static_cast<int>(width) * y + x;

				colors[index].x = perlin0.octave2D_01((x * fx), (y * fy), octaves, octaveMultiplier);
				colors[index].y = perlin1.octave2D_01((x * fx), (y * fy), octaves, octaveMultiplier);
				colors[index].z = perlin2.octave2D_01((x * fx), (y * fy), octaves, octaveMultiplier);
				colors[index].w = 1.0f;
			}
		}

		D3D11_SUBRESOURCE_DATA data;

		data.pSysMem = colors.data();
		data.SysMemPitch = static_cast<UINT>(width) * sizeof(XMFLOAT4);
		data.SysMemSlicePitch = 0;

		// Create the texture.
		D3D11_TEXTURE2D_DESC texDesc;

		ZeroMemory(&texDesc, sizeof(texDesc));

		texDesc.Width = static_cast<UINT>(width);
		texDesc.Height = static_cast<UINT>(height);
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		texDesc.Usage = D3D11_USAGE_DEFAULT;

		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		texDesc.SampleDesc.Count = 1;

		// 각각의 정보들을 가져와 줍니다.
		this->width = texDesc.Width;
		this->height = texDesc.Height;
		levels = texDesc.MipLevels;
		format = texDesc.Format;

		// 2D texture를 만들어줍니다.
		HRESULT hr = device->CreateTexture2D(&texDesc, &data, texResource.GetAddressOf());

		if (FAILED(hr))
			MessageBox(0, TEXT("Texture Failed."), 0, 0);

		// SRV를 만들어줍니다.
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;

		hr = device->CreateShaderResourceView(texResource.Get(), &srvDesc, mapSRV.GetAddressOf());

		if (FAILED(hr))
			MessageBox(0, TEXT("TextureSRV Failed."), 0, 0);
	}
}
