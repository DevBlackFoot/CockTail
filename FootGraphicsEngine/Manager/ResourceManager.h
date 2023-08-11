#pragma once
#include "GraphicsCore/RasterizerState.h"
#include "Resources/Mesh.h"
#include "Resources/Texture.h"
#include "YAMLBinaryLayout.h"
#include "FBXBinaryLayout.h"

#ifdef FOOTGRAPHICSENGINE_EXPORTS
#define GRAPHICSENGINE_DECLSPEC __declspec(dllexport)
#else
#define GRAPHICSENGINE_DECLSPEC __declspec(dllimport)
#endif

namespace GraphicsEngineSpace
{
	class IDXObject;

	class PBRObj;

	class ParticleEffect;

	class UnityScene;

	// 게임 엔진에서 사용 가능하도록 DLL Export를 해준다.
	class GRAPHICSENGINE_DECLSPEC ResourceManager
	{
	private:
		static std::shared_ptr<ResourceManager> instance;

	public:
		ResourceManager()
		{}
		~ResourceManager() {};

		static std::shared_ptr<ResourceManager> GetInstance();

	private:

	private:
		std::string meshPath;
		std::string materialPath;
		std::string AnimationPath;
		std::string modelPath;
		std::string prefabPath;

	public:
		// 내부에서 들고 있을 기본 메시들(도형, navmesh)
		std::unordered_map<std::string, std::shared_ptr<Mesh>> basicMeshes;

		// prefab!
		std::unordered_map<std::string, std::shared_ptr<IDXObject>> prefab;

		std::unordered_map<std::string, std::shared_ptr<IDXObject>> particlePrefab;

		std::unordered_map<std::string, std::shared_ptr<Texture>> randomTexture;

		// unorderd_map은 키로 튜플이 안되네..??
		std::map<std::tuple<float, float, float>, std::shared_ptr<Texture>> noiseTextures;

	public:
		std::shared_ptr<Mesh> LoadMesh(std::string name, bool isSkinned = false, bool hasSilhouette = false);

		std::shared_ptr<Texture> LoadTexture(std::wstring path);
		const std::shared_ptr<Texture>& LoadPrefabTexture(std::wstring path);


		std::shared_ptr<Texture> LoadNoiseTexture(float frequency, int octaves, float octaveMultiplier, std::uint32_t seed,
			float width, float height);

		std::shared_ptr<Texture> Load2DNoiseTexture();

		std::shared_ptr<Texture> GetSpawnEffectTexture();
		std::shared_ptr<Texture> GetAlertEffectTexture();

	public:
		// 메시를 만들어서 넘겨준다.
		template <typename T>
		const std::shared_ptr<Mesh> CreateMesh(std::vector<T> vertices, std::vector<std::vector<unsigned int>> indices, std::string name, D3D_PRIMITIVE_TOPOLOGY topology, ComPtr<ID3D11RasterizerState> rasterizerState);
		
		void Initialize();

		void Release();

		void LoadCubeMesh();			

		void LoadSphereMesh();			

		void LoadAxisMesh();			

		void LoadGridMesh();			

		void LoadScreenMesh();			

		void LoadDebugScreenMesh();		

		void LoadWireCubeMesh();		

		void LoadWireSphereMesh();

		void LoadCapsuleMesh();

		void LoadPlaneMesh();			

		void LoadPrefab();

		void LoadNoiseTexture();

		void LoadEffectTexture();

		std::shared_ptr<PBRObj> GetPrefab(std::string objName);

		std::shared_ptr<ParticleEffect> GetParticlePrefab(std::string particleName);

		// 게임엔진 쪽에서 mesh를 만들어서 넣을 수 있도록 해주는 함수
		void MakeMesh(std::string name, std::vector<FBXBinaryData::VertexData> vertex, std::vector<std::vector<unsigned int>> indices);

		std::shared_ptr<YAMLBinaryData::Scene> LoadUnityScene(std::string path);
	};

	template<typename T>
	inline const std::shared_ptr<Mesh> ResourceManager::CreateMesh(std::vector<T> vertices, std::vector<std::vector<unsigned int>> indices, std::string name, D3D_PRIMITIVE_TOPOLOGY topology, ComPtr<ID3D11RasterizerState> rasterizerState)
	{
		// TODO : unique가 맞는가?
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

		mesh->SetName(name);

		mesh->SetPrimitiveTopology(topology);
		
		mesh->SetRenderState(rasterizerState);

		if (!vertices.empty())
		{
			mesh->CreateVertexBuffer(vertices);
			for (int i = 0; i < indices.size(); i++)
			{
				mesh->CreateIndexBuffer(indices[i]);
			}
			mesh->SetStride(sizeof(T));
		}

		return mesh;
	}
}