#pragma once
#include "Resource.h"

namespace GraphicsEngineSpace
{
	class Texture : public Resource
	{
	private:
		ComPtr<ID3D11ShaderResourceView> mapSRV;

		ScratchImage image;

		ComPtr<ID3D11Texture2D> texResource;

		DXGI_FORMAT format;

		unsigned int levels;

		unsigned int width;

		unsigned int height;

	public:
		GRAPHICSENGINE_DECLSPEC Texture()
			: Resource(RESOURCE_TYPE::TEXTURE)
			, mapSRV(nullptr)
			, image(ScratchImage())
			, texResource(nullptr)
		{};

		Texture(const Texture& other)
			: Resource(RESOURCE_TYPE::TEXTURE)
		{
			// 리소스 복사
			texResource = other.texResource;
			mapSRV = other.mapSRV;
			width = other.width;
			height = other.height;
			levels = other.levels;
			format = other.format;
		}

		~Texture() {};

	public:
		ComPtr<ID3D11ShaderResourceView> GetMapSRV() { return mapSRV; }

		virtual void Load(const std::wstring& path) override;

		void Release();

		unsigned int GetWidth() { return width; }
		unsigned int GetHeight() { return height; }

		// 랜덤 텍스쳐를 생성하는 함수입니다.
		void CreateRandomTexture1D();
		// Noise Texture를 생성하는 함수입니다.
		void CreatePerlinNoiseTexture(float frequency, int octaves, float octaveMultiplier, std::uint32_t seed, float width, float height);
	};
}