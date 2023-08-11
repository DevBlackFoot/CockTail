#pragma once
#include "Resource.h"
#include "Texture.h"

namespace GraphicsEngineSpace
{
	class RenderingMaterial : public GraphicsEngineSpace::Resource
	{
	public:
		RenderingMaterial()
			: Resource(GraphicsEngineSpace::RESOURCE_TYPE::MATERIAL)
			, materialName("")
			, albedoTex(nullptr)
			, normalTex(nullptr)
			, metallicTex(nullptr)
			, roughnessTex(nullptr)
			, aoTex(nullptr)
			, emissiveTex(nullptr)
			, cubeTex(nullptr)
		{}

		RenderingMaterial(const RenderingMaterial& other)
			: Resource(GraphicsEngineSpace::RESOURCE_TYPE::MATERIAL)
			, materialName(other.materialName)
			, albedoTex(other.albedoTex ? std::make_shared<Texture>(*other.albedoTex) : nullptr)
			, normalTex(other.normalTex ? std::make_shared<Texture>(*other.normalTex) : nullptr)
			, metallicTex(other.metallicTex ? std::make_shared<Texture>(*other.metallicTex) : nullptr)
			, roughnessTex(other.roughnessTex ? std::make_shared<Texture>(*other.roughnessTex) : nullptr)
			, aoTex(other.aoTex ? std::make_shared<Texture>(*other.aoTex) : nullptr)
			, emissiveTex(other.emissiveTex ? std::make_shared<Texture>(*other.emissiveTex) : nullptr)
			, cubeTex(other.cubeTex ? std::make_shared<Texture>(*other.cubeTex) : nullptr)
			, material_Ambient(other.material_Ambient)
			, material_Diffuse(other.material_Diffuse)
			, material_Specular(other.material_Specular)
			, material_Emissive(other.material_Emissive)
			, emissionFactor(other.emissionFactor)
			, material_Transparency(other.material_Transparency)
			, material_Reflectivity(other.material_Reflectivity)
			, metallic(other.metallic)
			, roughness(other.roughness)
		{}

		~RenderingMaterial() {};

	public:
		std::string materialName;

		std::shared_ptr<Texture> albedoTex;
		std::shared_ptr<Texture> normalTex;
		std::shared_ptr<Texture> metallicTex;
		std::shared_ptr<Texture> roughnessTex;
		std::shared_ptr<Texture> aoTex;
		std::shared_ptr<Texture> emissiveTex;
		std::shared_ptr<Texture> cubeTex;

		DirectX::SimpleMath::Vector4 material_Ambient = { 0.f, 0.f, 0.f, 0.f };
		DirectX::SimpleMath::Vector4 material_Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
		DirectX::SimpleMath::Vector4 material_Specular = { 0.f, 0.f, 0.f, 0.f };
		DirectX::SimpleMath::Vector4 material_Emissive = { 0.f, 0.f, 0.f, 0.f };

		float emissionFactor = 0.f;			// 방출 크기

		float material_Transparency = 0.f;	// 투명도

		float material_Reflectivity = 0.f;	// 반사율

		float metallic = 0.0f;

		float roughness = 0.f;
	};
}

