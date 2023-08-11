#include "GraphicsPch.h"
#include "SpriteEffectBuilder.h"

#include "Interface/IDXObject.h"
#include "Manager/ResourceManager.h"
#include "Object/SpriteEffect.h"
#include "Resources/RenderingMaterial.h"

using namespace DirectX::SimpleMath;

namespace GraphicsEngineSpace
{
	std::shared_ptr<IDXObject> SpriteEffectBuilder::BuildDXObject(std::shared_ptr<IDXObject> pDXObj, std::string objectName)
	{
		if (std::dynamic_pointer_cast<SpriteEffect>(pDXObj) == nullptr)
			return nullptr;

		BuildGeometry(pDXObj, objectName);

		return pDXObj;
	}

	std::shared_ptr<MeshResources> SpriteEffectBuilder::BuildGeometry(std::shared_ptr<IDXObject> DXObject,
		std::string objectPath)
	{
		std::shared_ptr<MeshResources> tmpRes = std::make_shared<MeshResources>();

		tmpRes->ObjName = objectPath;

		// 오브젝트 이름을 판별합니다.
		std::string extension = objectPath;
		int _extensionIdx = objectPath.rfind(".");
		if (_extensionIdx != -1)
		{
			extension = objectPath.substr(_extensionIdx);
		}

		if (extension == ".model")
		{
			// 모델을 불러오자 일단 path.. 임시
			std::ifstream ifs("BinaryFile/Model/" + objectPath, std::ios_base::binary);
			boost::iostreams::filtering_streambuf<boost::iostreams::input> buffer;
			buffer.push(boost::iostreams::zlib_decompressor());
			buffer.push(ifs);
			boost::archive::binary_iarchive ia(buffer);

			FBXBinaryData::ModelData modelData;

			ia >> modelData;

			tmpRes.reset();

			BuildBinaryEffect(DXObject, modelData);
		}
		else
		{
			// path가 model이 아니라면..!
			BuildDefaultEffect(DXObject, tmpRes, objectPath);
		}

		return tmpRes;
	}

	void SpriteEffectBuilder::InitBuilder(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDC)
	{
		D3DDevice = pDevice;
		D3DDeviceContext = pDC;

		resourceManager = ResourceManager::GetInstance();
	}

	void SpriteEffectBuilder::BuildDefaultEffect(std::shared_ptr<IDXObject> DXObject, std::shared_ptr<MeshResources> tmpResource, std::string texPath)
	{
		// Plane Effect 생성 Basic Mesh 가져오기

		tmpResource->mesh = resourceManager->LoadMesh("ScreenMesh", false);
		tmpResource->setMesh = true;

		// 텍스쳐 하나 생성.
		auto mat = std::make_shared<RenderingMaterial>();

		if (texPath == "SpawnEffect")
		{
			auto albedoTex = resourceManager->GetSpawnEffectTexture();
			if(albedoTex != nullptr)
				mat->albedoTex = albedoTex;
		}
		else if(texPath == "Alert")
		{
			auto albedoTex = resourceManager->GetAlertEffectTexture();
			if (albedoTex != nullptr)
				mat->albedoTex = albedoTex;
		}
		else
		{
			std::wstring wPath;
			wPath.assign(texPath.begin(), texPath.end());

			auto albedoTex = resourceManager->LoadTexture(wPath);
			mat->albedoTex = albedoTex;
		}

		tmpResource->material.emplace_back(mat);

		tmpResource->setMaterial = true;

		DXObject->SetMeshResources(tmpResource);
	}

	void SpriteEffectBuilder::BuildBinaryEffect(std::shared_ptr<IDXObject> DXObject, FBXBinaryData::ModelData modelData)
	{
		for (auto& mesh : modelData.meshInfoList)
		{
			std::shared_ptr<MeshResources> objRes = std::make_shared<MeshResources>();

			objRes->isSkinnedMesh = true;

			objRes->setMesh = true;

			objRes->setMaterial = true;

			objRes->ObjName = mesh;

			DXObject->SetMeshResources(objRes);
		}
	}
}
