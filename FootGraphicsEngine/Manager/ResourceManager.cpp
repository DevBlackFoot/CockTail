#include "GraphicsPch.h"
#include "Factory/Factory.h"
#include "ResourceManager.h"
#include "BuilderManager.h"
#include "Object/PBRObj.h"
#include "Object/ParticleEffect.h"

namespace GraphicsEngineSpace
{
	std::shared_ptr<ResourceManager> ResourceManager::instance = nullptr;

	std::shared_ptr<ResourceManager> ResourceManager::GetInstance()
	{
		if (instance == nullptr)
			instance = std::make_shared<ResourceManager>();

		return instance;
	}

	std::shared_ptr<Mesh> ResourceManager::LoadMesh(std::string name, bool isSkinned, bool hasSilhouette)
	{
		// 내부에 저장된 기본 메시를 찾는지 확인한다.
		auto findIt = basicMeshes.find(name);

		if (findIt != basicMeshes.end())
			return basicMeshes[name];

		// 외부에서 불러올 메시(FBX에서 불러온 메시..)
		FBXBinaryData::MeshData binaryMesh;

		std::ifstream ifs(meshPath + name + ".mesh", std::ios_base::binary);
		boost::iostreams::filtering_streambuf<boost::iostreams::input> buffer;
		buffer.push(boost::iostreams::zlib_decompressor());
		buffer.push(ifs);
		boost::archive::binary_iarchive iaMesh(buffer);
		iaMesh >> binaryMesh;

		if (isSkinned)
		{
			// 버텍스 정보 입력
			std::vector<VertexStruct::PBRSkinnedVertex> vertices;

			// 버텍스 정보 입력
			for (auto& vertex : binaryMesh.meshVertexList)
			{
				auto tempVertex = std::make_unique<VertexStruct::PBRSkinnedVertex>();

				tempVertex->Pos.x = vertex.position.x;
				tempVertex->Pos.y = vertex.position.y;
				tempVertex->Pos.z = vertex.position.z;

				tempVertex->Tex.x = vertex.uv.x;
				tempVertex->Tex.y = vertex.uv.y;

				tempVertex->Normal.x = vertex.normal.x;
				tempVertex->Normal.y = vertex.normal.y;
				tempVertex->Normal.z = vertex.normal.z;

				tempVertex->TangentU.x = vertex.tangent.x;
				tempVertex->TangentU.y = vertex.tangent.y;
				tempVertex->TangentU.z = vertex.tangent.z;

				tempVertex->Color.x = vertex.color.x;
				tempVertex->Color.y = vertex.color.y;
				tempVertex->Color.z = vertex.color.z;
				tempVertex->Color.w = vertex.color.w;

				tempVertex->Weight.x = vertex.weights[0];
				tempVertex->Weight.y = vertex.weights[1];
				tempVertex->Weight.z = vertex.weights[2];
				tempVertex->Weight.w = vertex.weights[3];

				tempVertex->Weights.x = vertex.weights[4];
				tempVertex->Weights.y = vertex.weights[5];
				tempVertex->Weights.z = vertex.weights[6];
				tempVertex->Weights.w = vertex.weights[7];

				tempVertex->BoneIndex.x = vertex.boneIndices[0];
				tempVertex->BoneIndex.y = vertex.boneIndices[1];
				tempVertex->BoneIndex.z = vertex.boneIndices[2];
				tempVertex->BoneIndex.w = vertex.boneIndices[3];

				tempVertex->BoneIndices.x = vertex.boneIndices[4];
				tempVertex->BoneIndices.y = vertex.boneIndices[5];
				tempVertex->BoneIndices.z = vertex.boneIndices[6];
				tempVertex->BoneIndices.w = vertex.boneIndices[7];

				vertices.push_back(*tempVertex);
			}

			std::vector<std::vector<unsigned int>> indices = binaryMesh.indices;

			std::vector<unsigned int> tmpIdx;

			// SkinMat 만들기
			for (int i = 0; i < indices.size(); i++)
			{
				tmpIdx.insert(tmpIdx.end(), indices[i].begin(), indices[i].end());
			}

			indices.push_back(std::move(tmpIdx));

			if (hasSilhouette == true)
			{
				// 실루엣을 위해서 하나 더 만들어 줍니다.
				tmpIdx.clear();
				for (int i = 0; i < indices.size(); i++)
				{
					tmpIdx.insert(tmpIdx.end(), indices[i].begin(), indices[i].end());
				}
				indices.push_back(std::move(tmpIdx));
			}

			auto mesh = CreateMesh(
				vertices,
				indices,
				name,
				D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
				RasterizerState::GetSolidRS()
			);

			// Mesh에 참조하는 머터리얼 이름들을 넣어준다.
			for (auto& mat : binaryMesh.materials)
			{
				mesh->materials.emplace_back(mat);
			}

			// SkinMaterial 이름을 넣어준다.
			mesh->materials.emplace_back("SkinMaterial");

			if (hasSilhouette == true)
			{
				// 실루엣 이름을 넣어줍니다.
				mesh->materials.emplace_back("SilhouetteMaterial");

				// 실루엣은 마지막에 들어간다고 가정합니다.
				mesh->silhouetteIdx = mesh->materials.size() - 1;
			}

			mesh->SetName(binaryMesh.meshName);

			mesh->isSkinnedMesh = true;


			return mesh;
		}
		else
		{
			// 버텍스 정보 입력
			std::vector<VertexStruct::PBRStaticVertex> vertices;

			// 버텍스 정보 입력
			for (auto& vertex : binaryMesh.meshVertexList)
			{
				auto tempVertex = std::make_unique<VertexStruct::PBRStaticVertex>();

				tempVertex->Pos.x = vertex.position.x;
				tempVertex->Pos.y = vertex.position.y;
				tempVertex->Pos.z = vertex.position.z;

				tempVertex->Tex.x = vertex.uv.x;
				tempVertex->Tex.y = vertex.uv.y;

				tempVertex->Normal.x = vertex.normal.x;
				tempVertex->Normal.y = vertex.normal.y;
				tempVertex->Normal.z = vertex.normal.z;

				tempVertex->TangentU.x = vertex.tangent.x;
				tempVertex->TangentU.y = vertex.tangent.y;
				tempVertex->TangentU.z = vertex.tangent.z;

				tempVertex->Color.x = vertex.color.x;
				tempVertex->Color.y = vertex.color.y;
				tempVertex->Color.z = vertex.color.z;
				tempVertex->Color.w = vertex.color.w;

				vertices.push_back(*tempVertex);
			}

			std::vector<std::vector<unsigned int>> indices = binaryMesh.indices;

			if (hasSilhouette == true)
			{
				std::vector<unsigned int> tmpIdx;

				// SkinMat 만들기
				for (int i = 0; i < indices.size(); i++)
				{
					tmpIdx.insert(tmpIdx.end(), indices[i].begin(), indices[i].end());
				}

				indices.push_back(std::move(tmpIdx));

				// 실루엣을 위해서 하나 더 만들어 줍니다.
				tmpIdx.clear();
				for (int i = 0; i < indices.size(); i++)
				{
					tmpIdx.insert(tmpIdx.end(), indices[i].begin(), indices[i].end());
				}
				indices.push_back(std::move(tmpIdx));
			}

			auto mesh = CreateMesh(
				vertices,
				indices,
				name,
				D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
				RasterizerState::GetSolidRS()
			);

			// Mesh에 참조하는 머터리얼 이름들을 넣어준다.
			for (auto& mat : binaryMesh.materials)
			{
				mesh->materials.emplace_back(mat);
			}

			if (hasSilhouette == true)
			{
				mesh->materials.emplace_back("SkinMaterial");

				// 실루엣 이름을 넣어줍니다.
				mesh->materials.emplace_back("SilhouetteMaterial");

				// 실루엣은 마지막에 들어간다고 가정합니다.
				mesh->silhouetteIdx = mesh->materials.size() - 1;
			}

			mesh->SetName(binaryMesh.meshName);

			return mesh;
		}

		return nullptr;
	}

	std::shared_ptr<Texture> ResourceManager::LoadTexture(std::wstring path)
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->Load(path);

		return texture;
	}

	const std::shared_ptr<Texture>& ResourceManager::LoadPrefabTexture(std::wstring path)
	{
		std::string str(path.begin(), path.end());

		auto findIt = randomTexture.find(str);

		if (findIt != randomTexture.end())
			return findIt->second;

		return nullptr;
	}

	std::shared_ptr<Texture> ResourceManager::LoadNoiseTexture(float frequency, int octaves, float octaveMultiplier, std::uint32_t seed, float width, float height)
	{
		// 내부에 저장된 노이저 텍스쳐를 찾는다.
		auto findIt = noiseTextures.find({ frequency, octaves, octaveMultiplier });

		if (findIt != noiseTextures.end())
			return findIt->second;

		// 못찾았으면 만들어준다.
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();

		texture->CreatePerlinNoiseTexture(frequency, octaveMultiplier, octaveMultiplier, seed, width, height);

		noiseTextures[{frequency, octaves, octaveMultiplier}] = texture;

		return texture;
	}

	std::shared_ptr<Texture> ResourceManager::Load2DNoiseTexture()
	{
		auto findIt = randomTexture.find("NoiseTexture");

		if (findIt != randomTexture.end())
			return findIt->second;

		return nullptr;
	}

	std::shared_ptr<Texture> ResourceManager::GetSpawnEffectTexture()
	{
		auto findIt = randomTexture.find("SpawnEffect");

		if (findIt != randomTexture.end())
			return findIt->second;

		return nullptr;
	}

	std::shared_ptr<Texture> ResourceManager::GetAlertEffectTexture()
	{
		auto findIt = randomTexture.find("AlertEffect");

		if (findIt != randomTexture.end())
			return findIt->second;

		return nullptr;
	}


	void ResourceManager::Initialize()
	{
		meshPath = "BinaryFile/Mesh/";
		materialPath = "BinaryFile/Material/";
		AnimationPath = "BinaryFile/Animation/";
		modelPath = "BinaryFile/Model/";
		prefabPath = "BinaryFile/Prefab/";

		LoadCubeMesh();

		LoadSphereMesh();

		LoadAxisMesh();

		LoadGridMesh();

		// Renderer::UIUtilsInitialize() 에서 미리불렀다.
		// LoadScreenMesh();

		LoadDebugScreenMesh();

		LoadWireCubeMesh();

		LoadWireSphereMesh();

		LoadCapsuleMesh();

		LoadPlaneMesh();

		LoadPrefab();

		LoadNoiseTexture();

		LoadEffectTexture();
	}

	void ResourceManager::Release()
	{
		for (auto& mesh : basicMeshes)
		{
			mesh.second.reset();
		}
	}

	void ResourceManager::LoadCubeMesh()
	{
		float w2 = 0.5f;
		float h2 = 0.5f;
		float d2 = 0.5f;

		std::vector<VertexStruct::PBRStaticVertex> vertices(24);

		// 앞면
		vertices[0] = VertexStruct::PBRStaticVertex(SimpleMath::Vector3(-w2, -h2, -d2), SimpleMath::Vector3(0.0f, 0.0f, -1.0f), SimpleMath::Vector3(1.0f, 0.0f, 0.0f), SimpleMath::Vector2(0.0f, 1.0f), SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		vertices[1] = VertexStruct::PBRStaticVertex(SimpleMath::Vector3(-w2, +h2, -d2), SimpleMath::Vector3(0.0f, 0.0f, -1.0f), SimpleMath::Vector3(1.0f, 0.0f, 0.0f), SimpleMath::Vector2(0.0f, 0.0f), SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		vertices[2] = VertexStruct::PBRStaticVertex(SimpleMath::Vector3(+w2, +h2, -d2), SimpleMath::Vector3(0.0f, 0.0f, -1.0f), SimpleMath::Vector3(1.0f, 0.0f, 0.0f), SimpleMath::Vector2(1.0f, 0.0f), SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		vertices[3] = VertexStruct::PBRStaticVertex(SimpleMath::Vector3(+w2, -h2, -d2), SimpleMath::Vector3(0.0f, 0.0f, -1.0f), SimpleMath::Vector3(1.0f, 0.0f, 0.0f), SimpleMath::Vector2(1.0f, 1.0f), SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		// 뒷면
		vertices[4] = VertexStruct::PBRStaticVertex(SimpleMath::Vector3(-w2, -h2, +d2), SimpleMath::Vector3(0.0f, 0.0f, 1.0f), SimpleMath::Vector3(-1.0f, 0.0f, 0.0f), SimpleMath::Vector2(1.0f, 1.0f), SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		vertices[5] = VertexStruct::PBRStaticVertex(SimpleMath::Vector3(+w2, -h2, +d2), SimpleMath::Vector3(0.0f, 0.0f, 1.0f), SimpleMath::Vector3(-1.0f, 0.0f, 0.0f), SimpleMath::Vector2(0.0f, 1.0f), SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		vertices[6] = VertexStruct::PBRStaticVertex(SimpleMath::Vector3(+w2, +h2, +d2), SimpleMath::Vector3(0.0f, 0.0f, 1.0f), SimpleMath::Vector3(-1.0f, 0.0f, 0.0f), SimpleMath::Vector2(0.0f, 0.0f), SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		vertices[7] = VertexStruct::PBRStaticVertex(SimpleMath::Vector3(-w2, +h2, +d2), SimpleMath::Vector3(0.0f, 0.0f, 1.0f), SimpleMath::Vector3(-1.0f, 0.0f, 0.0f), SimpleMath::Vector2(1.0f, 0.0f), SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		// 윗면
		vertices[8] = VertexStruct::PBRStaticVertex(SimpleMath::Vector3(-w2, +h2, -d2), SimpleMath::Vector3(0.0f, 1.0f, 0.0f), SimpleMath::Vector3(1.0f, 0.0f, 0.0f), SimpleMath::Vector2(0.0f, 1.0f), SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		vertices[9] = VertexStruct::PBRStaticVertex(SimpleMath::Vector3(-w2, +h2, +d2), SimpleMath::Vector3(0.0f, 1.0f, 0.0f), SimpleMath::Vector3(1.0f, 0.0f, 0.0f), SimpleMath::Vector2(0.0f, 0.0f), SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		vertices[10] = VertexStruct::PBRStaticVertex(SimpleMath::Vector3(+w2, +h2, +d2), SimpleMath::Vector3(0.0f, 1.0f, 0.0f), SimpleMath::Vector3(1.0f, 0.0f, 0.0f), SimpleMath::Vector2(1.0f, 0.0f), SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		vertices[11] = VertexStruct::PBRStaticVertex(SimpleMath::Vector3(+w2, +h2, -d2), SimpleMath::Vector3(0.0f, 1.0f, 0.0f), SimpleMath::Vector3(1.0f, 0.0f, 0.0f), SimpleMath::Vector2(1.0f, 1.0f), SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		// 아랫면
		vertices[12] = VertexStruct::PBRStaticVertex(SimpleMath::Vector3(-w2, -h2, -d2), SimpleMath::Vector3(0.0f, -1.0f, 0.0f), SimpleMath::Vector3(-1.0f, 0.0f, 0.0f), SimpleMath::Vector2(1.0f, 1.0f), SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		vertices[13] = VertexStruct::PBRStaticVertex(SimpleMath::Vector3(+w2, -h2, -d2), SimpleMath::Vector3(0.0f, -1.0f, 0.0f), SimpleMath::Vector3(-1.0f, 0.0f, 0.0f), SimpleMath::Vector2(0.0f, 1.0f), SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		vertices[14] = VertexStruct::PBRStaticVertex(SimpleMath::Vector3(+w2, -h2, +d2), SimpleMath::Vector3(0.0f, -1.0f, 0.0f), SimpleMath::Vector3(-1.0f, 0.0f, 0.0f), SimpleMath::Vector2(0.0f, 0.0f), SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		vertices[15] = VertexStruct::PBRStaticVertex(SimpleMath::Vector3(-w2, -h2, +d2), SimpleMath::Vector3(0.0f, -1.0f, 0.0f), SimpleMath::Vector3(-1.0f, 0.0f, 0.0f), SimpleMath::Vector2(1.0f, 0.0f), SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		// 왼쪽면
		vertices[16] = VertexStruct::PBRStaticVertex(SimpleMath::Vector3(-w2, -h2, +d2), SimpleMath::Vector3(-1.0f, 0.0f, 0.0f), SimpleMath::Vector3(0.0f, 0.0f, -1.0f), SimpleMath::Vector2(0.0f, 1.0f), SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		vertices[17] = VertexStruct::PBRStaticVertex(SimpleMath::Vector3(-w2, +h2, +d2), SimpleMath::Vector3(-1.0f, 0.0f, 0.0f), SimpleMath::Vector3(0.0f, 0.0f, -1.0f), SimpleMath::Vector2(0.0f, 0.0f), SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		vertices[18] = VertexStruct::PBRStaticVertex(SimpleMath::Vector3(-w2, +h2, -d2), SimpleMath::Vector3(-1.0f, 0.0f, 0.0f), SimpleMath::Vector3(0.0f, 0.0f, -1.0f), SimpleMath::Vector2(1.0f, 0.0f), SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		vertices[19] = VertexStruct::PBRStaticVertex(SimpleMath::Vector3(-w2, -h2, -d2), SimpleMath::Vector3(-1.0f, 0.0f, 0.0f), SimpleMath::Vector3(0.0f, 0.0f, -1.0f), SimpleMath::Vector2(1.0f, 1.0f), SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		// 오른쪽면
		vertices[20] = VertexStruct::PBRStaticVertex(SimpleMath::Vector3(+w2, -h2, -d2), SimpleMath::Vector3(1.0f, 0.0f, 0.0f), SimpleMath::Vector3(0.0f, 0.0f, 1.0f), SimpleMath::Vector2(0.0f, 1.0f), SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		vertices[21] = VertexStruct::PBRStaticVertex(SimpleMath::Vector3(+w2, +h2, -d2), SimpleMath::Vector3(1.0f, 0.0f, 0.0f), SimpleMath::Vector3(0.0f, 0.0f, 1.0f), SimpleMath::Vector2(0.0f, 0.0f), SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		vertices[22] = VertexStruct::PBRStaticVertex(SimpleMath::Vector3(+w2, +h2, +d2), SimpleMath::Vector3(1.0f, 0.0f, 0.0f), SimpleMath::Vector3(0.0f, 0.0f, 1.0f), SimpleMath::Vector2(1.0f, 0.0f), SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		vertices[23] = VertexStruct::PBRStaticVertex(SimpleMath::Vector3(+w2, -h2, +d2), SimpleMath::Vector3(1.0f, 0.0f, 0.0f), SimpleMath::Vector3(0.0f, 0.0f, 1.0f), SimpleMath::Vector2(1.0f, 1.0f), SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f));

		std::vector<std::vector<unsigned int>> idx(1, std::vector<unsigned int>(36));

		// 앞면
		idx[0][0] = 0; idx[0][1] = 1; idx[0][2] = 2;
		idx[0][3] = 0; idx[0][4] = 2; idx[0][5] = 3;
		// 뒷면
		idx[0][6] = 4; idx[0][7] = 5; idx[0][8] = 6;
		idx[0][9] = 4; idx[0][10] = 6; idx[0][11] = 7;
		// 윗면
		idx[0][12] = 8; idx[0][13] = 9; idx[0][14] = 10;
		idx[0][15] = 8; idx[0][16] = 10; idx[0][17] = 11;
		// 아랫면
		idx[0][18] = 12; idx[0][19] = 13; idx[0][20] = 14;
		idx[0][21] = 12; idx[0][22] = 14; idx[0][23] = 15;
		// 왼쪽면
		idx[0][24] = 16; idx[0][25] = 17; idx[0][26] = 18;
		idx[0][27] = 16; idx[0][28] = 18; idx[0][29] = 19;
		// 오른쪽면
		idx[0][30] = 20; idx[0][31] = 21; idx[0][32] = 22;
		idx[0][33] = 20; idx[0][34] = 22; idx[0][35] = 23;

		auto mesh = CreateMesh(
			vertices,
			idx,
			"CubeMesh",
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			RasterizerState::GetSolidRS()
		);

		basicMeshes.insert(std::make_pair("CubeMesh", mesh));
	}

	void ResourceManager::LoadSphereMesh()
	{
		float radius = 0.5f;	// 구의 반지름
		uint32 stackCount = 20; // 가로 분할
		uint32 sliceCount = 20; // 세로 분할

		std::vector<VertexStruct::PBRStaticVertex> vertices;

		VertexStruct::PBRStaticVertex v;

		// 북극
		v.Pos = SimpleMath::Vector3(0.0f, radius, 0.0f);
		v.Tex = SimpleMath::Vector2(0.5f, 0.0f);
		v.Normal = v.Pos;
		XMVector3Normalize(v.Normal);
		v.Normal.Normalize();
		v.TangentU = SimpleMath::Vector3(1.0f, 0.0f, 1.0f);
		vertices.push_back(v);

		float stackAngle = XM_PI / stackCount;
		float sliceAngle = XM_2PI / sliceCount;

		float deltaU = 1.f / static_cast<float>(sliceCount);
		float deltaV = 1.f / static_cast<float>(stackCount);

		// 고리마다 돌면서 정점을 계산한다 (북극/남극 단일점은 고리가 X)
		for (uint32 y = 1; y <= stackCount - 1; ++y)
		{
			float phi = y * stackAngle;

			// 고리에 위치한 정점
			for (uint32 x = 0; x <= sliceCount; ++x)
			{
				float theta = x * sliceAngle;

				v.Pos.x = radius * sinf(phi) * cosf(theta);
				v.Pos.y = radius * cosf(phi);
				v.Pos.z = radius * sinf(phi) * sinf(theta);

				v.Tex = SimpleMath::Vector2(deltaU * x, deltaV * y);

				v.Normal = v.Pos;
				v.Normal.Normalize();

				v.TangentU.x = -radius * sinf(phi) * sinf(theta);
				v.TangentU.y = 0.0f;
				v.TangentU.z = radius * sinf(phi) * cosf(theta);
				v.TangentU.Normalize();

				vertices.push_back(v);
			}
		}

		// 남극
		v.Pos = SimpleMath::Vector3(0.0f, -radius, 0.0f);
		v.Tex = SimpleMath::Vector2(0.5f, 1.0f);
		v.Normal = v.Pos;
		v.Normal.Normalize();
		v.TangentU = SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
		vertices.push_back(v);

		std::vector<std::vector<unsigned int>> idx(1);

		// 북극 인덱스
		for (uint32 i = 0; i <= sliceCount; ++i)
		{
			//  [0]
			//   |  \
			//  [i+1]-[i+2]
			idx[0].push_back(0);
			idx[0].push_back(i + 2);
			idx[0].push_back(i + 1);
		}

		// 몸통 인덱스
		uint32 ringVertexCount = sliceCount + 1;
		for (uint32 y = 0; y < stackCount - 2; ++y)
		{
			for (uint32 x = 0; x < sliceCount; ++x)
			{
				//  [y, x]-[y, x+1]
				//  |		/
				//  [y+1, x]
				idx[0].push_back(1 + (y)*ringVertexCount + (x));
				idx[0].push_back(1 + (y)*ringVertexCount + (x + 1));
				idx[0].push_back(1 + (y + 1) * ringVertexCount + (x));
				//		 [y, x+1]
				//		 /	  |
				//  [y+1, x]-[y+1, x+1]
				idx[0].push_back(1 + (y + 1) * ringVertexCount + (x));
				idx[0].push_back(1 + (y)*ringVertexCount + (x + 1));
				idx[0].push_back(1 + (y + 1) * ringVertexCount + (x + 1));
			}
		}

		// 남극 인덱스
		uint32 bottomIndex = static_cast<uint32>(vertices.size()) - 1;
		uint32 lastRingStartIndex = bottomIndex - ringVertexCount;
		for (uint32 i = 0; i < sliceCount; ++i)
		{
			//  [last+i]-[last+i+1]
			//  |      /
			//  [bottom]
			idx[0].push_back(bottomIndex);
			idx[0].push_back(lastRingStartIndex + i);
			idx[0].push_back(lastRingStartIndex + i + 1);
		}

		auto mesh = CreateMesh(
			vertices,
			idx,
			"SphereMesh",
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			RasterizerState::GetSolidRS()
		);

		mesh->materials.emplace_back("PBR_Mat");

		basicMeshes.insert(std::make_pair("SphereMesh", mesh));
	}

	void ResourceManager::LoadAxisMesh()
	{
		std::vector<VertexStruct::ColorVertex> vertices =
		{
			// x축 => 빨간색
			{SimpleMath::Vector3(0.0f, 0.0f, 0.0f), SimpleMath::Vector4((const float*)&Colors::Red)},
			{SimpleMath::Vector3(50.0f, 0.0f, 0.0f), SimpleMath::Vector4((const float*)&Colors::Red)},

			// y축 초록색
			{SimpleMath::Vector3(0.0f, 0.0f, 0.0f), SimpleMath::Vector4((const float*)&Colors::GreenYellow)},
			{SimpleMath::Vector3(0.0f, 50.0f, 0.0f), SimpleMath::Vector4((const float*)&Colors::GreenYellow)},

			// z축 파란색
			{SimpleMath::Vector3(0.0f, 0.0f, 0.0f), SimpleMath::Vector4((const float*)&Colors::Blue)},
			{SimpleMath::Vector3(0.0f, 0.0f, 50.0f), SimpleMath::Vector4((const float*)&Colors::Blue)},
		};

		// 인덱스 버퍼 생성
		std::vector<std::vector<unsigned int>> indices(1);
		indices[0].emplace_back(0);
		indices[0].emplace_back(1);
		indices[0].emplace_back(2);
		indices[0].emplace_back(3);
		indices[0].emplace_back(4);
		indices[0].emplace_back(5);

		auto mesh = CreateMesh(
			vertices,
			indices,
			"AxisMesh",
			D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
			RasterizerState::GetWireFrameRS()
		);

		basicMeshes.insert(std::make_pair("AxisMesh", mesh));
	}

	void ResourceManager::LoadGridMesh()
	{
		std::vector<VertexStruct::ColorVertex> vertices(84);

		float z_val = -10.0f;
		// for문을 돌면서 버텍스 생성.
		// 세로 버텍스
		for (int i = 0; i < 42; i += 2)
		{
			vertices[i] = { SimpleMath::Vector3(-10.0f, 0.0f, z_val), SimpleMath::Vector4((const float*)&Colors::White) };
			vertices[i + 1] = { SimpleMath::Vector3(10.0f, 0.0f, z_val), SimpleMath::Vector4((const float*)&Colors::White) };
			z_val++;
		}
		float x_val = -10.0f;
		for (int i = 42; i < 84; i += 2)
		{
			vertices[i] = { SimpleMath::Vector3(x_val, 0.0f, -10.0f), SimpleMath::Vector4((const float*)&Colors::White) };
			vertices[i + 1] = { SimpleMath::Vector3(x_val, 0.0f, 10.0f), SimpleMath::Vector4((const float*)&Colors::White) };
			x_val++;
		}

		// 인덱스 버퍼 생성
		// 총 라인의 개수 40개 => 인덱스의 개수 40 * 2개
		std::vector<std::vector<unsigned int>> indices(1, std::vector<unsigned int>(84));

		for (int i = 0; i < 84; i++)
		{
			indices[0][i] = i;
		}

		auto mesh = CreateMesh(
			vertices,
			indices,
			"GridMesh",
			D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
			RasterizerState::GetWireFrameRS()
		);

		basicMeshes.insert(std::make_pair("GridMesh", mesh));
	}

	void ResourceManager::LoadScreenMesh()
	{
		// Screen Mesh 생성
		std::vector<VertexStruct::TexVertex> vertices(6);

		// 좌 하단
		vertices[0].Pos = SimpleMath::Vector3(-1.f, -1.f, 0.f);
		vertices[0].Tex = SimpleMath::Vector2(0.f, 1.f);

		// 좌 상단
		vertices[1].Pos = SimpleMath::Vector3(-1.f, 1.f, 0.f);
		vertices[1].Tex = SimpleMath::Vector2(0.f, 0.f);

		// 우 상단
		vertices[2].Pos = SimpleMath::Vector3(1.f, 1.f, 0.f);
		vertices[2].Tex = SimpleMath::Vector2(1.f, 0.f);

		// 좌 하단
		vertices[3].Pos = SimpleMath::Vector3(-1.f, -1.f, 0.f);
		vertices[3].Tex = SimpleMath::Vector2(0.f, 1.f);

		// 우 상단
		vertices[4].Pos = SimpleMath::Vector3(1.f, 1.f, 0.f);
		vertices[4].Tex = SimpleMath::Vector2(1.f, 0.f);

		// 우 하단
		vertices[5].Pos = SimpleMath::Vector3(1.f, -1.f, 0.f);
		vertices[5].Tex = SimpleMath::Vector2(1.f, 1.f);

		std::vector<std::vector<unsigned int>> indices(1, std::vector<unsigned int>(6));

		for (int i = 0; i < 6; i++)
			indices[0][i] = i;

		auto mesh = CreateMesh(
			vertices,
			indices,
			"ScreenMesh",
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			RasterizerState::GetSolidRS()
		);

		basicMeshes.insert(std::make_pair("ScreenMesh", mesh));
	}

	void ResourceManager::LoadDebugScreenMesh()
	{
		// Debug Screen Mesh 생성
		std::vector<VertexStruct::PBRStaticVertex> vertices(6);

		// 좌 하단
		vertices[0].Pos = SimpleMath::Vector3(-1.f, -0.6f, 0.f);
		vertices[0].Tex = SimpleMath::Vector2(0.f, 1.f);

		// 좌 상단
		vertices[1].Pos = SimpleMath::Vector3(-1.f, 1.f, 0.f);
		vertices[1].Tex = SimpleMath::Vector2(0.f, 0.f);

		// 우 상단
		vertices[2].Pos = SimpleMath::Vector3(1.f, 1.f, 0.f);
		vertices[2].Tex = SimpleMath::Vector2(1.f, 0.f);

		// 좌 하단
		vertices[3].Pos = SimpleMath::Vector3(-1.f, -0.6f, 0.f);
		vertices[3].Tex = SimpleMath::Vector2(0.f, 1.f);

		// 우 상단
		vertices[4].Pos = SimpleMath::Vector3(1.f, 1.f, 0.f);
		vertices[4].Tex = SimpleMath::Vector2(1.f, 0.f);

		// 우 하단
		vertices[5].Pos = SimpleMath::Vector3(1.f, -0.6f, 0.f);
		vertices[5].Tex = SimpleMath::Vector2(1.f, 1.f);

		std::vector<std::vector<unsigned int>> indices(1, std::vector<unsigned int>(6));

		for (int i = 0; i < 6; i++)
			indices[0][i] = i;

		auto mesh = CreateMesh(
			vertices,
			indices,
			"DebugScreenMesh",
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			RasterizerState::GetSolidRS()
		);

		basicMeshes.insert(std::make_pair("DebugScreenMesh", mesh));
	}

	void ResourceManager::LoadWireCubeMesh()
	{
		float w2 = 0.5f;
		float h2 = 0.5f;
		float d2 = 0.5f;

		std::vector<VertexStruct::PosVertex> vertices(24);

		// 앞면
		vertices[0] = VertexStruct::PosVertex(SimpleMath::Vector3(-w2, -h2, -d2));
		vertices[1] = VertexStruct::PosVertex(SimpleMath::Vector3(-w2, +h2, -d2));
		vertices[2] = VertexStruct::PosVertex(SimpleMath::Vector3(+w2, +h2, -d2));
		vertices[3] = VertexStruct::PosVertex(SimpleMath::Vector3(+w2, -h2, -d2));
		// 뒷면
		vertices[4] = VertexStruct::PosVertex(SimpleMath::Vector3(-w2, -h2, +d2));
		vertices[5] = VertexStruct::PosVertex(SimpleMath::Vector3(+w2, -h2, +d2));
		vertices[6] = VertexStruct::PosVertex(SimpleMath::Vector3(+w2, +h2, +d2));
		vertices[7] = VertexStruct::PosVertex(SimpleMath::Vector3(-w2, +h2, +d2));
		// 윗면
		vertices[8] = VertexStruct::PosVertex(SimpleMath::Vector3(-w2, +h2, -d2));
		vertices[9] = VertexStruct::PosVertex(SimpleMath::Vector3(-w2, +h2, +d2));
		vertices[10] = VertexStruct::PosVertex(SimpleMath::Vector3(+w2, +h2, +d2));
		vertices[11] = VertexStruct::PosVertex(SimpleMath::Vector3(+w2, +h2, -d2));
		// 아랫면
		vertices[12] = VertexStruct::PosVertex(SimpleMath::Vector3(-w2, -h2, -d2));
		vertices[13] = VertexStruct::PosVertex(SimpleMath::Vector3(+w2, -h2, -d2));
		vertices[14] = VertexStruct::PosVertex(SimpleMath::Vector3(+w2, -h2, +d2));
		vertices[15] = VertexStruct::PosVertex(SimpleMath::Vector3(-w2, -h2, +d2));
		// 왼쪽면
		vertices[16] = VertexStruct::PosVertex(SimpleMath::Vector3(-w2, -h2, +d2));
		vertices[17] = VertexStruct::PosVertex(SimpleMath::Vector3(-w2, +h2, +d2));
		vertices[18] = VertexStruct::PosVertex(SimpleMath::Vector3(-w2, +h2, -d2));
		vertices[19] = VertexStruct::PosVertex(SimpleMath::Vector3(-w2, -h2, -d2));
		// 오른쪽면						
		vertices[20] = VertexStruct::PosVertex(SimpleMath::Vector3(+w2, -h2, -d2));
		vertices[21] = VertexStruct::PosVertex(SimpleMath::Vector3(+w2, +h2, -d2));
		vertices[22] = VertexStruct::PosVertex(SimpleMath::Vector3(+w2, +h2, +d2));
		vertices[23] = VertexStruct::PosVertex(SimpleMath::Vector3(+w2, -h2, +d2));

		std::vector<std::vector<unsigned int>> idx(1, std::vector<unsigned int>(36));

		// 앞면
		idx[0][0] = 0; idx[0][1] = 1; idx[0][2] = 2;
		idx[0][3] = 0; idx[0][4] = 2; idx[0][5] = 3;
		// 뒷면
		idx[0][6] = 4; idx[0][7] = 5; idx[0][8] = 6;
		idx[0][9] = 4; idx[0][10] = 6; idx[0][11] = 7;
		// 윗면
		idx[0][12] = 8; idx[0][13] = 9; idx[0][14] = 10;
		idx[0][15] = 8; idx[0][16] = 10; idx[0][17] = 11;
		// 아랫면
		idx[0][18] = 12; idx[0][19] = 13; idx[0][20] = 14;
		idx[0][21] = 12; idx[0][22] = 14; idx[0][23] = 15;
		// 왼쪽면
		idx[0][24] = 16; idx[0][25] = 17; idx[0][26] = 18;
		idx[0][27] = 16; idx[0][28] = 18; idx[0][29] = 19;
		// 오른쪽면
		idx[0][30] = 20; idx[0][31] = 21; idx[0][32] = 22;
		idx[0][33] = 20; idx[0][34] = 22; idx[0][35] = 23;

		auto wireCubemesh = CreateMesh(
			vertices,
			idx,
			"WireCubeMesh",
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			RasterizerState::GetWireFrameRS()
		);

		basicMeshes.insert(std::make_pair("WireCubeMesh", wireCubemesh));

		auto skyMesh = CreateMesh(
			vertices,
			idx,
			"SkyBoxMesh",
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			RasterizerState::GetSkyBoxRS()
		);

		basicMeshes.insert(std::make_pair("SkyBoxMesh", skyMesh));
	}

	void ResourceManager::LoadWireSphereMesh()
	{
		float radius = 1.0f;	// 구의 반지름
		uint32 stackCount = 20; // 가로 분할
		uint32 sliceCount = 20; // 세로 분할

		std::vector<VertexStruct::PosVertex> vertices;

		VertexStruct::PosVertex v;

		// 북극
		v.Pos = SimpleMath::Vector3(0.0f, radius, 0.0f);
		vertices.push_back(v);

		float stackAngle = XM_PI / stackCount;
		float sliceAngle = XM_2PI / sliceCount;

		float deltaU = 1.f / static_cast<float>(sliceCount);
		float deltaV = 1.f / static_cast<float>(stackCount);

		// 고리마다 돌면서 정점을 계산한다 (북극/남극 단일점은 고리가 X)
		for (uint32 y = 1; y <= stackCount - 1; ++y)
		{
			float phi = y * stackAngle;

			// 고리에 위치한 정점
			for (uint32 x = 0; x <= sliceCount; ++x)
			{
				float theta = x * sliceAngle;

				v.Pos.x = radius * sinf(phi) * cosf(theta);
				v.Pos.y = radius * cosf(phi);
				v.Pos.z = radius * sinf(phi) * sinf(theta);

				vertices.push_back(v);
			}
		}

		// 남극
		v.Pos = SimpleMath::Vector3(0.0f, -radius, 0.0f);
		vertices.push_back(v);

		std::vector<std::vector<unsigned int>> idx(1);

		// 북극 인덱스
		for (uint32 i = 0; i <= sliceCount; ++i)
		{
			//  [0]
			//   |  \
			//  [i+1]-[i+2]
			idx[0].push_back(0);
			idx[0].push_back(i + 2);
			idx[0].push_back(i + 1);
		}

		// 몸통 인덱스
		uint32 ringVertexCount = sliceCount + 1;
		for (uint32 y = 0; y < stackCount - 2; ++y)
		{
			for (uint32 x = 0; x < sliceCount; ++x)
			{
				//  [y, x]-[y, x+1]
				//  |		/
				//  [y+1, x]
				idx[0].push_back(1 + (y)*ringVertexCount + (x));
				idx[0].push_back(1 + (y)*ringVertexCount + (x + 1));
				idx[0].push_back(1 + (y + 1) * ringVertexCount + (x));
				//		 [y, x+1]
				//		 /	  |
				//  [y+1, x]-[y+1, x+1]
				idx[0].push_back(1 + (y + 1) * ringVertexCount + (x));
				idx[0].push_back(1 + (y)*ringVertexCount + (x + 1));
				idx[0].push_back(1 + (y + 1) * ringVertexCount + (x + 1));
			}
		}

		// 남극 인덱스
		uint32 bottomIndex = static_cast<uint32>(vertices.size()) - 1;
		uint32 lastRingStartIndex = bottomIndex - ringVertexCount;
		for (uint32 i = 0; i < sliceCount; ++i)
		{
			//  [last+i]-[last+i+1]
			//  |      /
			//  [bottom]
			idx[0].push_back(bottomIndex);
			idx[0].push_back(lastRingStartIndex + i);
			idx[0].push_back(lastRingStartIndex + i + 1);
		}

		auto mesh = CreateMesh(
			vertices,
			idx,
			"WireSphereMesh",
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			RasterizerState::GetWireFrameRS()
		);

		basicMeshes.insert(std::make_pair("WireSphereMesh", mesh));
	}

	void ResourceManager::LoadCapsuleMesh()
	{
		float radius = 0.5f;	// 구의 반지름
		float height = 0.5f;	// 캡슐의 높이 => 해당 높이 + 지름 => 캡슐 전체의 높이
		uint32 stackCount = 20; // 가로 분할
		uint32 sliceCount = 20; // 세로 분할

		std::vector<VertexStruct::PosVertex> vertices;

		VertexStruct::PosVertex v;

		// 북극
		v.Pos = SimpleMath::Vector3(0.0f, radius + height, 0.0f);
		vertices.push_back(v);

		float stackAngle = XM_PI / stackCount;
		float sliceAngle = XM_2PI / sliceCount;

		float deltaU = 1.f / static_cast<float>(sliceCount);
		float deltaV = 1.f / static_cast<float>(stackCount);

		// 고리마다 돌면서 정점을 계산한다 (북극/남극 단일점은 고리가 X)
		for (uint32 y = 1; y <= stackCount - 1; ++y)
		{
			float phi = y * stackAngle;

			// 고리에 위치한 정점
			for (uint32 x = 0; x <= sliceCount; ++x)
			{
				float theta = x * sliceAngle;

				v.Pos.x = radius * sinf(phi) * cosf(theta);
				if (y < 10)
					v.Pos.y = radius * cosf(phi) + height;
				else
					v.Pos.y = radius * cosf(phi) - height;
				v.Pos.z = radius * sinf(phi) * sinf(theta);

				vertices.push_back(v);
			}
		}

		// 남극
		v.Pos = SimpleMath::Vector3(0.0f, -(radius + height), 0.0f);
		vertices.push_back(v);

		std::vector<std::vector<unsigned int>> idx(1);

		// 북극 인덱스
		for (uint32 i = 0; i <= sliceCount; ++i)
		{
			//  [0]
			//   |  \
			//  [i+1]-[i+2]
			idx[0].push_back(0);
			idx[0].push_back(i + 2);
			idx[0].push_back(i + 1);
		}

		// 몸통 인덱스
		uint32 ringVertexCount = sliceCount + 1;
		for (uint32 y = 0; y < stackCount - 2; ++y)
		{
			for (uint32 x = 0; x < sliceCount; ++x)
			{
				//  [y, x]-[y, x+1]
				//  |		/
				//  [y+1, x]
				idx[0].push_back(1 + (y)*ringVertexCount + (x));
				idx[0].push_back(1 + (y)*ringVertexCount + (x + 1));
				idx[0].push_back(1 + (y + 1) * ringVertexCount + (x));
				//		 [y, x+1]
				//		 /	  |
				//  [y+1, x]-[y+1, x+1]
				idx[0].push_back(1 + (y + 1) * ringVertexCount + (x));
				idx[0].push_back(1 + (y)*ringVertexCount + (x + 1));
				idx[0].push_back(1 + (y + 1) * ringVertexCount + (x + 1));
			}
		}

		// 남극 인덱스
		uint32 bottomIndex = static_cast<uint32>(vertices.size()) - 1;
		uint32 lastRingStartIndex = bottomIndex - ringVertexCount;
		for (uint32 i = 0; i < sliceCount; ++i)
		{
			//  [last+i]-[last+i+1]
			//  |      /
			//  [bottom]
			idx[0].push_back(bottomIndex);
			idx[0].push_back(lastRingStartIndex + i);
			idx[0].push_back(lastRingStartIndex + i + 1);
		}

		auto mesh = CreateMesh(
			vertices,
			idx,
			"WireCapsuleMesh",
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			RasterizerState::GetWireFrameRS()
		);

		basicMeshes.insert(std::make_pair("WireCapsuleMesh", mesh));
	}

	void ResourceManager::LoadPlaneMesh()
	{
		// Plane Mesh 생성
		std::vector<VertexStruct::PBRStaticVertex> vertices(6);

		// 좌 하단
		vertices[0].Pos = SimpleMath::Vector3(-5.f, 0.f, -5.f);
		vertices[0].Normal = SimpleMath::Vector3(0.f, 1.f, 0.f);
		vertices[0].Tex = SimpleMath::Vector2(0.f, 1.f);
		vertices[0].Color = SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f);

		// 좌 상단
		vertices[1].Pos = SimpleMath::Vector3(-5.f, 0.f, 5.f);
		vertices[1].Normal = SimpleMath::Vector3(0.f, 1.f, 0.f);
		vertices[1].Tex = SimpleMath::Vector2(0.f, 0.f);
		vertices[1].Color = SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f);

		// 우 상단
		vertices[2].Pos = SimpleMath::Vector3(5.f, 0.f, 5.f);
		vertices[2].Normal = SimpleMath::Vector3(0.f, 1.f, 0.f);
		vertices[2].Tex = SimpleMath::Vector2(1.f, 0.f);
		vertices[2].Color = SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f);

		// 좌 하단
		vertices[3].Pos = SimpleMath::Vector3(-5.f, 0.f, -5.f);
		vertices[3].Normal = SimpleMath::Vector3(0.f, 1.f, 0.f);
		vertices[3].Tex = SimpleMath::Vector2(0.f, 1.f);
		vertices[3].Color = SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f);

		// 우 상단
		vertices[4].Pos = SimpleMath::Vector3(5.f, 0.f, 5.f);
		vertices[4].Normal = SimpleMath::Vector3(0.f, 1.f, 0.f);
		vertices[4].Tex = SimpleMath::Vector2(1.f, 0.f);
		vertices[4].Color = SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f);

		// 우 하단
		vertices[5].Pos = SimpleMath::Vector3(5.f, 0.f, -5.f);
		vertices[5].Normal = SimpleMath::Vector3(0.f, 1.f, 0.f);
		vertices[5].Tex = SimpleMath::Vector2(1.f, 1.f);
		vertices[5].Color = SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f);

		std::vector<std::vector<unsigned int>> indices(1, std::vector<unsigned int>(6));

		for (int i = 0; i < 6; i++)
			indices[0][i] = i;

		auto mesh = CreateMesh(
			vertices,
			indices,
			"PlaneMesh",
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			RasterizerState::GetSolidRS()
		);

		basicMeshes.insert(std::make_pair("PlaneMesh", mesh));
	}

	void ResourceManager::LoadPrefab()
	{
		auto factory = Factory::GetInstance();
		auto PBRBuilder = BuilderManager::GetInstance()->GetBuilder("PBRBuilder");
		auto ParticleBuilder = BuilderManager::GetInstance()->GetBuilder("ParticleEffectBuilder");

		prefab.insert({ "tailani.model", factory->CreateDXObject<PBRObj>(PBRBuilder, "tailani.model") });
		prefab.insert({ "toadani.model", factory->CreateDXObject<PBRObj>(PBRBuilder, "toadani.model") });
		prefab.insert({ "redani.model", factory->CreateDXObject<PBRObj>(PBRBuilder, "redani.model") });
		prefab.insert({ "Warning.model", factory->CreateDXObject<PBRObj>(PBRBuilder, "Warning.model") });
		prefab.insert({ "dog.model", factory->CreateDXObject<PBRObj>(PBRBuilder, "dog.model") });
		prefab.insert({ "dogani.model", factory->CreateDXObject<PBRObj>(PBRBuilder, "dogani.model") });
		prefab.insert({ "dogani_throw.model", factory->CreateDXObject<PBRObj>(PBRBuilder, "dogani_throw.model") });
		prefab.insert({ "cockani.model", factory->CreateDXObject<PBRObj>(PBRBuilder, "cockani.model") });
		prefab.insert({ "MiniTurret.model", factory->CreateDXObject<PBRObj>(PBRBuilder, "MiniTurret.model") });
		prefab.insert({ "Turret.model", factory->CreateDXObject<PBRObj>(PBRBuilder, "Turret.model") });
		prefab.insert({ "Room1Door.model", factory->CreateDXObject<PBRObj>(PBRBuilder, "Room1Door.model") });
		prefab.insert({ "Room4Door.model", factory->CreateDXObject<PBRObj>(PBRBuilder, "Room4Door.model") });
		prefab.insert({ "RedStatue.model", factory->CreateDXObject<PBRObj>(PBRBuilder, "RedStatue.model") });

		prefab.insert({ "Boss.model", factory->CreateDXObject<PBRObj>(PBRBuilder, "Boss.model") });
		prefab.insert({ "Elevator.model", factory->CreateDXObject<PBRObj>(PBRBuilder, "Elevator.model") });
		prefab.insert({ "Elevator2.model", factory->CreateDXObject<PBRObj>(PBRBuilder, "Elevator2.model") });
		prefab.insert({ "LeftPassage1.model", factory->CreateDXObject<PBRObj>(PBRBuilder, "LeftPassage1.model") });
		prefab.insert({ "LeftPassage2.model", factory->CreateDXObject<PBRObj>(PBRBuilder, "LeftPassage2.model") });
		prefab.insert({ "lobby.model", factory->CreateDXObject<PBRObj>(PBRBuilder, "lobby.model") });
		prefab.insert({ "Room1.model", factory->CreateDXObject<PBRObj>(PBRBuilder, "Room1.model") });
		prefab.insert({ "Room2.model", factory->CreateDXObject<PBRObj>(PBRBuilder, "Room2.model") });
		prefab.insert({ "Room3.model", factory->CreateDXObject<PBRObj>(PBRBuilder, "Room3.model") });
		prefab.insert({ "Room4.model", factory->CreateDXObject<PBRObj>(PBRBuilder, "Room4.model") });
		prefab.insert({ "StraightPassage.model", factory->CreateDXObject<PBRObj>(PBRBuilder, "StraightPassage.model") });
		prefab.insert({ "tutorial.model", factory->CreateDXObject<PBRObj>(PBRBuilder, "tutorial.model") });
		prefab.insert({ "ElevatorButton.model", factory->CreateDXObject<PBRObj>(PBRBuilder, "ElevatorButton.model") });
		prefab.insert({ "CannonBall.model", factory->CreateDXObject<PBRObj>(PBRBuilder, "CannonBall.model") });

		particlePrefab.insert({ "Item", factory->CreateDXObject<ParticleEffect>(ParticleBuilder, "ItemDropIngredient") });
		particlePrefab.insert({ "ItemTrail", factory->CreateDXObject<ParticleEffect>(ParticleBuilder, "ItemTrail") });
		particlePrefab.insert({ "ItemDropRed", factory->CreateDXObject<ParticleEffect>(ParticleBuilder, "ItemDropRed") });
		particlePrefab.insert({ "ItemDropYellow", factory->CreateDXObject<ParticleEffect>(ParticleBuilder, "ItemDropYellow") });
		particlePrefab.insert({ "Hit", factory->CreateDXObject<ParticleEffect>(ParticleBuilder, "Hit") });
		particlePrefab.insert({ "Hit3", factory->CreateDXObject<ParticleEffect>(ParticleBuilder, "Hit3") });
		particlePrefab.insert({ "ArcAttack", factory->CreateDXObject<ParticleEffect>(ParticleBuilder, "ArcAttack") });
		particlePrefab.insert({ "Whirlwind", factory->CreateDXObject<ParticleEffect>(ParticleBuilder, "Whirlwind") });
		particlePrefab.insert({ "Attack1Shadow", factory->CreateDXObject<ParticleEffect>(ParticleBuilder, "Attack1Shadow") });
		particlePrefab.insert({ "Attack2Shadow", factory->CreateDXObject<ParticleEffect>(ParticleBuilder, "Attack2Shadow") });
		particlePrefab.insert({ "Attack3Effect", factory->CreateDXObject<ParticleEffect>(ParticleBuilder, "Attack3Effect") });
		particlePrefab.insert({ "TailWalkEffect", factory->CreateDXObject<ParticleEffect>(ParticleBuilder, "TailWalkEffect") });
		particlePrefab.insert({ "Portal", factory->CreateDXObject<ParticleEffect>(ParticleBuilder, "PortalA") });
		particlePrefab.insert({ "TurretBeam", factory->CreateDXObject<ParticleEffect>(ParticleBuilder, "TurretBeam") });
		particlePrefab.insert({ "TurretBomb", factory->CreateDXObject<ParticleEffect>(ParticleBuilder, "TurretBomb") });
		particlePrefab.insert({ "TurretGuide", factory->CreateDXObject<ParticleEffect>(ParticleBuilder, "TurretGuide") });
		particlePrefab.insert({ "Turretlaser", factory->CreateDXObject<ParticleEffect>(ParticleBuilder, "Turretlaser") });
		particlePrefab.insert({ "Interaction", factory->CreateDXObject<ParticleEffect>(ParticleBuilder, "Interaction") });
		particlePrefab.insert({ "Warning", factory->CreateDXObject<ParticleEffect>(ParticleBuilder, "Warning") });
		particlePrefab.insert({ "Bomb1", factory->CreateDXObject<ParticleEffect>(ParticleBuilder, "Bomb1") });
		particlePrefab.insert({ "Missile", factory->CreateDXObject<ParticleEffect>(ParticleBuilder, "Missile") });
	}

	void ResourceManager::LoadNoiseTexture()
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->Load(L"Resources/Texture/Noise.png");

		randomTexture.insert({ "NoiseTexture", texture });
	}

	void ResourceManager::LoadEffectTexture()
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->Load(L"Resources/Effect/SpawnEffect.png");

		randomTexture.insert({ "SpawnEffect", texture });

		std::shared_ptr<Texture> texture2 = std::make_shared<Texture>();
		texture2->Load(L"Resources/UI/Alert.png");

		randomTexture.insert({ "AlertEffect", texture2 });

		std::shared_ptr<Texture> texture3 = std::make_shared<Texture>();
		texture3->Load(L"Resources/UI/Hud/HP_BG.png");

		randomTexture.insert({ "HPBlank", texture3 });

		std::shared_ptr<Texture> texture4 = std::make_shared<Texture>();
		texture4->Load(L"Resources/UI/Hud/HP.png");

		randomTexture.insert({ "HP", texture4 });

		std::shared_ptr<Texture> cursor = std::make_shared<Texture>();
		cursor->Load(L"Resources/UI/Icon/cursor.png");
		randomTexture.insert({ "Cursor", cursor });

		// 파티클 + 스프라이트용 텍스쳐 세팅
		std::shared_ptr<Texture> ingredient0 = std::make_shared<Texture>();
		ingredient0->Load(L"Resources/UI/Ingredient/EmptyBottle.png");
		std::shared_ptr<Texture> ingredient1 = std::make_shared<Texture>();
		ingredient1->Load(L"Resources/UI/Ingredient/EmptyBottleRed.png");
		std::shared_ptr<Texture> ingredient2 = std::make_shared<Texture>();
		ingredient2->Load(L"Resources/UI/Ingredient/EmptyBottleBlue.png");
		std::shared_ptr<Texture> ingredient3 = std::make_shared<Texture>();
		ingredient3->Load(L"Resources/UI/Ingredient/SparklingWater.png");
		std::shared_ptr<Texture> ingredient4 = std::make_shared<Texture>();
		ingredient4->Load(L"Resources/UI/Ingredient/Pigment.png");
		std::shared_ptr<Texture> ingredient5 = std::make_shared<Texture>();
		ingredient5->Load(L"Resources/UI/Ingredient/Sugar.png");
		std::shared_ptr<Texture> ingredient6 = std::make_shared<Texture>();
		ingredient6->Load(L"Resources/UI/Ingredient/Herb.png");
		std::shared_ptr<Texture> ingredient7 = std::make_shared<Texture>();
		ingredient7->Load(L"Resources/UI/Ingredient/Punch.png");
		std::shared_ptr<Texture> ingredient8 = std::make_shared<Texture>();
		ingredient8->Load(L"Resources/UI/Ingredient/Liquor.png");
		std::shared_ptr<Texture> ingredient9 = std::make_shared<Texture>();
		ingredient9->Load(L"Resources/UI/Ingredient/Seed.png");
		std::shared_ptr<Texture> ingredient10 = std::make_shared<Texture>();
		ingredient10->Load(L"Resources/UI/Ingredient/Spice.png");
		std::shared_ptr<Texture> ingredient11 = std::make_shared<Texture>();
		ingredient11->Load(L"Resources/UI/Ingredient/Sugarcane.png");
		std::shared_ptr<Texture> ingredient12 = std::make_shared<Texture>();
		ingredient12->Load(L"Resources/UI/Ingredient/Potato.png");
		std::shared_ptr<Texture> ingredient13 = std::make_shared<Texture>();
		ingredient13->Load(L"Resources/UI/Ingredient/Grain.png");

		randomTexture.insert({ "Ingredient0", ingredient0 });
		randomTexture.insert({ "Ingredient1", ingredient1 });
		randomTexture.insert({ "Ingredient2", ingredient2 });
		randomTexture.insert({ "Ingredient3", ingredient3 });
		randomTexture.insert({ "Ingredient4", ingredient4 });
		randomTexture.insert({ "Ingredient5", ingredient5 });
		randomTexture.insert({ "Ingredient6", ingredient6 });
		randomTexture.insert({ "Ingredient7", ingredient7 });
		randomTexture.insert({ "Ingredient8", ingredient8 });
		randomTexture.insert({ "Ingredient9", ingredient9 });
		randomTexture.insert({ "Ingredient10", ingredient10 });
		randomTexture.insert({ "Ingredient11", ingredient11 });
		randomTexture.insert({ "Ingredient12", ingredient12 });
		randomTexture.insert({ "Ingredient13", ingredient13 });

		std::shared_ptr<Texture> popup = std::make_shared<Texture>();
		popup->Load(L"Resources/UI/CockPopup.png");
		randomTexture.insert({ "PopUp", popup });

		popup = std::make_shared<Texture>();
		popup->Load(L"Resources/UI/CockPopupClose.png");
		randomTexture.insert({ "PopUpClose", popup });

		for (int i = 1; i <= 3; i++)
		{
			for (int j = 1; j <= 5; j++)
			{
				std::shared_ptr<Texture> tmpTexture = std::make_shared<Texture>();
				std::string tmpStr = "npc" + std::to_string(i) + "_" + std::to_string(j);
				std::wstring tmpWstr(tmpStr.begin(), tmpStr.end());
				std::wstring tmpPath = L"Resources/UI/Character/" + tmpWstr + L".png";
				tmpTexture->Load(tmpPath);

				randomTexture.insert({ tmpStr, tmpTexture });
			}
		}

		for (int i = 1; i <= 8; i++)
		{
			std::shared_ptr<Texture> tmpTexture = std::make_shared<Texture>();
			std::string tmpstr = "BarTutorial" + std::to_string(i);
			std::wstring tmpPath = L"Resources/UI/slide" + std::to_wstring(i) + L".PNG";
			tmpTexture->Load(tmpPath);

			randomTexture.insert({ tmpstr, tmpTexture });
		}

		// 각종 원료들의 텍스쳐를 가지고 있습니다. (미니 게임 + 선택 요소 확인 전용)
		std::shared_ptr<Texture> cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/I_rum.png");
		randomTexture.insert({ "I_rum", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/I_vodka.png");
		randomTexture.insert({ "I_vodka", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/I_gin.png");
		randomTexture.insert({ "I_gin", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/I_liqueur.png");
		randomTexture.insert({ "I_liqueur", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/I_juice.png");
		randomTexture.insert({ "I_juice", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/I_soda.png");
		randomTexture.insert({ "I_soda", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/I_fruit.png");
		randomTexture.insert({ "I_fruit", cockTail });

		// 칵테일을 띄우기 위한 텍스쳐 로드
		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/RUM.png");
		randomTexture.insert({ "Rum", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/Rumcoke.png");
		randomTexture.insert({ "Rum & Coke", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/CUBALIBRE.png");
		randomTexture.insert({ "Cuba Libre", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/Daiquiri.png");
		randomTexture.insert({ "Daiquiri", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/PAINKILLER.png");
		randomTexture.insert({ "Pain Killer", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/MAITAI.png");
		randomTexture.insert({ "MaiTai", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/Planter's punch.png");
		randomTexture.insert({ "Planter's Punch", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/BUSHWACKER.png");
		randomTexture.insert({ "Bushwacker", cockTail });

		// vodka mix texture
		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/Vodka.png");
		randomTexture.insert({ "Vodka", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/ESPRESSOTINI.png");
		randomTexture.insert({ "Espressotini", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/MOSCOWMULE.png");
		randomTexture.insert({ "Moscow Mule", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/BLACKRUSSIAN.png");
		randomTexture.insert({ "Black Russian", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/COSMOPOLITAN.png");
		randomTexture.insert({ "Cosmopolitan", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/BLOODYMARY.png");
		randomTexture.insert({ "Bloody Mary", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/POETKILLER.png");
		randomTexture.insert({ "PoetKiller", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/SUNRISE.png");
		randomTexture.insert({ "Sunrise", cockTail });

		// gin Mix texture
		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/Gin.png");
		randomTexture.insert({ "Gin", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/GINFIZZ.png");
		randomTexture.insert({ "GinFizz", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/GINTONIC.png");
		randomTexture.insert({ "GinTonic", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/Aviation.png");
		randomTexture.insert({ "Aviation", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/LADYKILLER.png");
		randomTexture.insert({ "LadyKiller", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/BRAMBLE.png");
		randomTexture.insert({ "Bramble", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/LASTWORD.png");
		randomTexture.insert({ "LastWord", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Cocktail/Snapper.png");
		randomTexture.insert({ "Snapper", cockTail });

		// 칵테일 이름
		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Text/Name_rum.png");
		randomTexture.insert({ "Name_Rum", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Text/Name_rumcoke.png");
		randomTexture.insert({ "Name_Rum & Coke", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Text/Name_cubalibre.png");
		randomTexture.insert({ "Name_Cuba Libre", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Text/Name_daiquiri.png");
		randomTexture.insert({ "Name_Daiquiri", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Text/Name_painkiller.png");
		randomTexture.insert({ "Name_Pain Killer", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Text/Name_maitai.png");
		randomTexture.insert({ "Name_MaiTai", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Text/Name_planter.png");
		randomTexture.insert({ "Name_Planter's Punch", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Text/Name_bushwacker.png");
		randomTexture.insert({ "Name_Bushwacker", cockTail });

		// vodka mix texture
		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Text/Name_vodka.png");
		randomTexture.insert({ "Name_Vodka", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Text/Name_espressotini.png");
		randomTexture.insert({ "Name_Espressotini", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Text/Name_moscowmule.png");
		randomTexture.insert({ "Name_Moscow Mule", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Text/Name_blackrussian.png");
		randomTexture.insert({ "Name_Black Russian", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Text/Name_cosmopolitan.png");
		randomTexture.insert({ "Name_Cosmopolitan", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Text/Name_bloodymary.png");
		randomTexture.insert({ "Name_Bloody Mary", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Text/Name_poetkiller.png");
		randomTexture.insert({ "Name_PoetKiller", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Text/Name_sunrise.png");
		randomTexture.insert({ "Name_Sunrise", cockTail });

		// gin Mix texture
		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Text/Name_gin.png");
		randomTexture.insert({ "Name_Gin", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Text/Name_ginfizz.png");
		randomTexture.insert({ "Name_GinFizz", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Text/Name_gintonic.png");
		randomTexture.insert({ "Name_GinTonic", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Text/Name_aviation.png");
		randomTexture.insert({ "Name_Aviation", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Text/Name_ladykiller.png");
		randomTexture.insert({ "Name_LadyKiller", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Text/Name_bramble.png");
		randomTexture.insert({ "Name_Bramble", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Text/Name_lastword.png");
		randomTexture.insert({ "Name_LastWord", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Text/Name_snapper.png");
		randomTexture.insert({ "Name_Snapper", cockTail });

		cockTail = std::make_shared<Texture>();
		cockTail->Load(L"Resources/UI/Text/Name_fail.png");
		randomTexture.insert({ "Name_Fail", cockTail });

		std::shared_ptr<Texture> fButton = std::make_shared<Texture>();
		fButton->Load(L"Resources/UI/FButton.png");
		randomTexture.insert({"FButton", fButton});

		fButton = std::make_shared<Texture>();
		fButton->Load(L"Resources/UI/FButtonPress.png");
		randomTexture.insert({ "FButtonPress", fButton });
	}

	std::shared_ptr<PBRObj> ResourceManager::GetPrefab(std::string objName)
	{
		return std::make_shared<PBRObj>(*std::dynamic_pointer_cast<PBRObj>(prefab[objName]));
	}

	std::shared_ptr<ParticleEffect> ResourceManager::GetParticlePrefab(std::string particleName)
	{
		return std::make_shared<ParticleEffect>(*std::dynamic_pointer_cast<ParticleEffect>(particlePrefab[particleName]));
	}

	void ResourceManager::MakeMesh(std::string name, std::vector<FBXBinaryData::VertexData> vertex, std::vector<std::vector<unsigned int>> indices)
	{
		// 내부에 저장된 기본 메시를 찾는지 확인한다.
		auto findIt = basicMeshes.find(name);

		// 있으면 만들지마!
		if (findIt != basicMeshes.end())
			return;

		std::vector<VertexStruct::PBRStaticVertex> vertices(vertex.size());

		for (int i = 0; i < vertices.size(); i++)
		{
			vertices[i].Pos = SimpleMath::Vector3{ vertex[i].position.x, vertex[i].position.y, vertex[i].position.z };
			vertices[i].Color = SimpleMath::Vector4{ vertex[i].color.x, vertex[i].color.y, vertex[i].color.z, vertex[i].color.w };
		}

		auto mesh = CreateMesh(
			vertices,
			indices,
			name,
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			RasterizerState::GetSolidRS()
		);

		basicMeshes.insert(std::make_pair(name, mesh));
	}

	std::shared_ptr<YAMLBinaryData::Scene> ResourceManager::LoadUnityScene(std::string path)
	{
		std::ifstream ifs(path, std::ios_base::binary);
		boost::iostreams::filtering_streambuf<boost::iostreams::input> buffer;
		buffer.push(boost::iostreams::zlib_decompressor());
		buffer.push(ifs);
		boost::archive::binary_iarchive ia(buffer);

		std::shared_ptr<YAMLBinaryData::Scene> sceneData;

		ia >> sceneData;

		return sceneData;
	}
}