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
			// ���ҽ� ����
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

		// ���� �ؽ��ĸ� �����ϴ� �Լ��Դϴ�.
		void CreateRandomTexture1D();
		// Noise Texture�� �����ϴ� �Լ��Դϴ�.
		void CreatePerlinNoiseTexture(float frequency, int octaves, float octaveMultiplier, std::uint32_t seed, float width, float height);
	};
}