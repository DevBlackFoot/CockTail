#include "GraphicsPch.h"
#include "PBRObj.h"

#include "Manager/BufferManager.h"
#include "Manager/ShaderManager.h"
#include "Manager/SamplerManager.h"
#include "Manager/ResourceManager.h"

#include "Shader/ShaderBase.h"
#include "Buffer/BufferBase.h"
#include "GraphicsCore/DepthStencilResource.h"
#include "Resources/Texture.h"
#include "Resources/RenderingMaterial.h"
#include "Util/AnimatorTransition.h"
#include "Util/AnimatorState.h"

#include "GraphicsEngine/Renderer.h"
#include "GraphicsEngine/Pass/ShadowPass.h"
#include "Shader/VertexShader.h"


namespace GraphicsEngineSpace
{
	PBRObj::PBRObj()
		: D3DDevice(nullptr)
		, D3DDeviceContext(nullptr)
		, resourceManager(nullptr)
		, boneResources(nullptr)
		, objAnimator(nullptr)
		, world()
		, view()
		, proj()
		, rimColor(1.f, 1.f, 1.f, 0.f)
		, burnOffset(0.f)
		, burnSpeed(0.f)
		, loadOffset(0.f)
		, dissolveSpeed(0.f)
		, goalOffset(2.9f)
		, onOffset(0.f)
		, isLoad(false)
		, isLoadCompleted(false)
	{
	}

	PBRObj::~PBRObj()
	{
		for (auto objRes : meshResources)
		{
			SafeReset(objRes);
		}

		SafeReset(boneResources);

		/*if (animResources != nullptr)
		{
			for (auto animClip : animResources->animationClipList)
			{
				SafeReset(animClip.second);
			}
		}

		SafeReset(animResources);*/
	}

	bool PBRObj::Init(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	{
		// ���� Ȯ��
		assert(pDevice);
		assert(pDeviceContext);

		D3DDevice = pDevice;
		D3DDeviceContext = pDeviceContext;

		resourceManager = ResourceManager::GetInstance();

		// ������ ���Ͱ� �� ������ ������.. �ϵ� �ڵ� �غ��ϴ�.
		if (objectName != "dogani.model" && objectName != "dogani_throw.model" && objectName != "toadani.model" && objectName != "redani.model" && objectName != "Turret.model")
			isLoadCompleted = true;

		// mesh, material ĳ��
		for (auto& objRes : meshResources)
		{
			// ���⼭ ���ΰ�, ����.
			if (objectName == "tailani.model" || objectName == "dogani.model" || objectName == "dogani_throw.model" || objectName == "toadani.model"
				|| objectName == "Turret.model" || objectName == "redani.model" || objectName == "cockani.model"
				|| objectName == "ElevatorButton.model")
				objRes->mesh = resourceManager->LoadMesh(objRes->ObjName, objRes->isSkinnedMesh, true);
			else
				objRes->mesh = resourceManager->LoadMesh(objRes->ObjName, objRes->isSkinnedMesh);

			// material�� ���� ���
			if (objRes->mesh->materials.empty())
			{
				std::shared_ptr<RenderingMaterial> material = std::make_shared<RenderingMaterial>();

				objRes->material.emplace_back(material);
			}
			// material�� �ִ� ���
			else
			{
				for (int matCnt = 0; matCnt < objRes->mesh->materials.size(); matCnt++)
				{
					// ���⼭ ���� SkinMaterial �ϰ��
					if (objRes->mesh->materials[matCnt] == "SkinMaterial")
					{
						std::shared_ptr<RenderingMaterial> skinMaterial = std::make_shared<RenderingMaterial>();
						skinMaterial->materialName = "SkinMaterial";
						skinMaterial->material_Transparency = 1.0f;
						skinMaterial->roughness = 1.f;

						/*
						*/
						skinMaterial->emissionFactor = 0.f;
						skinMaterial->material_Emissive.x = 0.f;
						skinMaterial->material_Emissive.y = 0.f;

						transparencyMesh.push_back(std::pair{ objRes, matCnt });

						objRes->material.emplace_back(skinMaterial);

						continue;
					}

					if (objRes->mesh->materials[matCnt] == "SilhouetteMaterial")
					{
						// ���ٽ� ���۷� ������ֱ� ���� �� ģ���� ������ ���׸���� ������ݴϴ�.
						std::shared_ptr<RenderingMaterial> silhouetteMaterial = std::make_shared<RenderingMaterial>();
						silhouetteMaterial->materialName = "SilhouetteMaterial";
						silhouetteMaterial->material_Transparency = 0.8f;
						silhouetteMaterial->roughness = 1.f;

						silhouetteMaterial->material_Diffuse = Vector4{ 0.4f, 0.4f, 0.4f, 1.0f };

						transparencyMesh.push_back(std::pair{ objRes, matCnt });

						objRes->material.emplace_back(silhouetteMaterial);

						continue;
					}

					// ���⼭ ���� PBR Mat �ϰ��
					if (objRes->mesh->materials[matCnt] == "PBR_Mat")
					{
						std::shared_ptr<RenderingMaterial> basicMaterial = std::make_shared<RenderingMaterial>();
						basicMaterial->materialName = "PBR_Mat";
						basicMaterial->material_Transparency = 1.0f;
						basicMaterial->roughness = 1.f;

						/*
						*/
						basicMaterial->emissionFactor = 0.f;
						basicMaterial->material_Emissive.x = 0.f;
						basicMaterial->material_Emissive.y = 0.f;

						transparencyMesh.push_back(std::pair{ objRes, matCnt });

						objRes->material.emplace_back(basicMaterial);

						continue;
					}

					std::shared_ptr<RenderingMaterial> material = std::make_shared<RenderingMaterial>();

					// �ܺ� ���͸��������� �ҷ��´�.
					std::ifstream ifs("BinaryFile/Material/" + objRes->mesh->materials[matCnt] + ".mat", std::ios_base::binary);
					boost::iostreams::filtering_streambuf<boost::iostreams::input> buffer;
					buffer.push(boost::iostreams::zlib_decompressor());
					buffer.push(ifs);
					boost::archive::binary_iarchive ia(buffer);

					FBXBinaryData::MaterialData materialData;

					ia >> materialData;

					material->materialName = materialData.materialName;

					std::wstring convertTexName;
					if (materialData.albedoMap != "")
					{
						convertTexName.assign(materialData.albedoMap.begin(), materialData.albedoMap.end());
						material->albedoTex = resourceManager->LoadTexture(L"Resources/Texture/" + convertTexName);
					}

					if (materialData.normalMap != "")
					{
						convertTexName.assign(materialData.normalMap.begin(), materialData.normalMap.end());
						material->normalTex = resourceManager->LoadTexture(L"Resources/Texture/" + convertTexName);
					}

					if (materialData.metallicMap != "")
					{
						convertTexName.assign(materialData.metallicMap.begin(), materialData.metallicMap.end());
						material->metallicTex = resourceManager->LoadTexture(L"Resources/Texture/" + convertTexName);
					}

					if (materialData.roughnessMap != "")
					{
						convertTexName.assign(materialData.roughnessMap.begin(), materialData.roughnessMap.end());
						material->roughnessTex = resourceManager->LoadTexture(L"Resources/Texture/" + convertTexName);
					}

					if (materialData.AOMap != "")
					{
						convertTexName.assign(materialData.AOMap.begin(), materialData.AOMap.end());
						material->aoTex = resourceManager->LoadTexture(L"Resources/Texture/" + convertTexName);
					}

					if (materialData.emissiveMap != "")
					{
						convertTexName.assign(materialData.emissiveMap.begin(), materialData.emissiveMap.end());
						material->emissiveTex = resourceManager->LoadTexture(L"Resources/Texture/" + convertTexName);
					}

					material->material_Ambient.x = materialData.material_Ambient.x;
					material->material_Ambient.y = materialData.material_Ambient.y;
					material->material_Ambient.z = materialData.material_Ambient.z;
					material->material_Ambient.w = materialData.material_Ambient.w;

					material->material_Diffuse.x = materialData.material_Diffuse.x;
					material->material_Diffuse.y = materialData.material_Diffuse.y;
					material->material_Diffuse.z = materialData.material_Diffuse.z;
					material->material_Diffuse.w = materialData.material_Diffuse.w;

					material->material_Specular.x = materialData.material_Specular.x;
					material->material_Specular.y = materialData.material_Specular.y;
					material->material_Specular.z = materialData.material_Specular.z;
					material->material_Specular.w = materialData.material_Specular.w;

					material->material_Emissive.x = materialData.material_Emissive.x;
					material->material_Emissive.y = materialData.material_Emissive.y;
					material->material_Emissive.z = materialData.material_Emissive.z;
					material->material_Emissive.w = materialData.material_Emissive.w;

					material->material_Reflectivity = materialData.material_Reflectivity;
					material->material_Transparency = materialData.material_Transparency;

					if (material->materialName == "black_Shadow")
					{
						material->material_Transparency = 0.5f;
					}

					// ������ ������ ��.
					if (material->material_Transparency > 0.0f)
					{
						transparencyMesh.push_back(std::pair{ objRes, matCnt });
					}

					if (material->emissionFactor > 0.0f)
					{
						material->material_Emissive.w = material->emissionFactor;
					}

					material->metallic = materialData.metallic;
					material->roughness = materialData.roughness;

					objRes->material.emplace_back(material);
				}
			}
		}

		// �� ���� ��ŭ ����.
		//if (boneResources != nullptr /* && objAnimator->animResources != nullptr*/)
		//{
		//	objAnimator = std::make_unique<Animator>(boneResources->bones.size());
		//	nowBoneTM.assign(boneResources->bones.size(), SimpleMath::Matrix::Identity);
		//}

		return true;
	}

	void PBRObj::Update(const XMMATRIX& _world, const XMMATRIX& _view, const XMMATRIX& _proj)
	{
		// ��� �ֱ�
		world = _world;
		view = _view;
		proj = _proj;
	}

	void PBRObj::PlayAnim()
	{
		objAnimator->PlayAnim();
	}

	void PBRObj::PreRender(float tick)
	{
		// ��Ʈ�������� �ٲ��ش�.
		if (boneResources != nullptr)
		{
			std::vector<SimpleMath::Matrix> animVector;

			if (objAnimator != nullptr)
			{
				animVector = objAnimator->UpdateBoneTM(tick);
				boneResources->setNode = false;
			}

			if (animVector.empty())
				return;

			// �ش� AnimVector�� ������Ʈ.
			for (size_t i = 0; i < boneResources->bones.size(); i++)
			{
				Bone tmpBone = boneResources->bones[i];
				//Matrix modifiedAnimTM = animVector[i];

				// �θ� TM���� ����� ���ο� AnimVector�� ������ش�.
				if (tmpBone.parentIdx != -1)
					animVector[i] *= animVector[tmpBone.parentIdx];

				/*while(tmpBone.parentIdx != -1)
				{
					modifiedAnimTM *= animVector[tmpBone.parentIdx];
					tmpBone = boneResources->bones[tmpBone.parentIdx];
				}

				tmpBone = boneResources->bones[i];*/

				if (boneResources->setNode != true)
					nowBoneTM[i] = tmpBone.offsetTransform * animVector[i];
				// nowBoneTM[i] = tmpBone.offsetTransform * modifiedAnimTM;
			}

			boneResources->setNode = true;

		}

		// Offset�� ������ݴϴ�.]
		if (burnSpeed > 0.f)
			burnOffset += burnSpeed * tick;

		// ƽ�� ���� �����µ� ����
		loadOffset += dissolveSpeed * tick;
		if (isLoadCompleted == true)
		{
			onOffset += dissolveSpeed * tick;
		}



	}

	void PBRObj::Render()
	{
		assert(D3DDeviceContext);

		for (auto& objRes : meshResources)
		{
			if (!objRes->isRender)
				continue;

			// Mesh�� ������ ���� �ٸ� Render �Լ��� �θ���.
			if (objRes->isSkinnedMesh != true)
				continue;
			else
				continue;
			//SkinnedRender(objRes);
		}
	}

	void PBRObj::Render(std::shared_ptr<IRenderer> renderer)
	{
		assert(D3DDeviceContext);

		for (auto& objRes : meshResources)
		{
			if (!objRes->isRender)
				continue;

			// Mesh�� ������ ���� �ٸ� Render �Լ��� �θ���.
			renderer->GraphicsDebugBeginEvent(objRes->ObjName);
			if (objRes->isSkinnedMesh != true)
				StaticRender(objRes, renderer);
			else
				SkinnedRender(objRes, renderer);
			renderer->GraphicsDebugEndEvent();
		}
	}

	void PBRObj::YieldRender(std::shared_ptr<IRenderer> renderer)
	{
		for (auto& objRes : transparencyMesh)
		{
			if (!objRes.first->isRender)
				continue;

			auto material = objRes.first->material[objRes.second];

			renderer->GraphicsDebugBeginEvent(material->materialName);
			YieldRender(objRes, renderer);
			renderer->GraphicsDebugEndEvent();
		}
	}

	void PBRObj::ObjectTrailRender(std::shared_ptr<IRenderer> renderer)
	{
		for (auto& mesh : meshResources)
		{
			if (!mesh->isRender)
				continue;

			TrailRender(mesh, renderer);
		}
	}

	/*void PBRObj::SkinAlphaRender(std::shared_ptr<IRenderer> renderer)
	{

	}*/

	void PBRObj::SetMeshResources(std::shared_ptr<MeshResources> objResources)
	{
		meshResources.push_back(objResources);
	}

	void PBRObj::SetMeshResourcesFront(std::shared_ptr<MeshResources> objResources)
	{
		meshResources.insert(meshResources.begin(), objResources);
	}

	void PBRObj::SetBoneResource(std::shared_ptr<BoneResources> boneResources)
	{
		this->boneResources = boneResources;

		if (boneResources != nullptr)
		{
			objAnimator = std::make_unique<Animator>(boneResources->bones.size());
			nowBoneTM.assign(boneResources->bones.size(), SimpleMath::Matrix::Identity);
		}
	}

	void PBRObj::SetAnimationResource(std::shared_ptr<AnimationResources> animResources)
	{
		objAnimator->animResources = animResources;
	}

	const std::string& PBRObj::GetObjName()
	{
		return objectName;
	}

	std::vector<std::shared_ptr<MeshResources>> PBRObj::GetMeshResources()
	{
		return meshResources;
	}

	void PBRObj::SetAnimationClip(std::string animName)
	{
		if (objAnimator->animResources != nullptr)
		{
			objAnimator->SetAnimationClip(animName);
		}
	}

	void PBRObj::AddAnimationState(const std::string& stateName, const std::string& animClipName, float speed, bool loop)
	{
		if (objAnimator->animResources != nullptr)
		{
			std::shared_ptr<AnimatorState> animatorState = std::make_shared<AnimatorState>();

			animatorState->SetStateName(stateName);

			animatorState->SetMotion(animClipName);

			animatorState->SetSpeed(speed);

			animatorState->SetLoop(loop);

			objAnimator->SetAnimatorState(stateName, animatorState);
		}
	}

	void PBRObj::ChangeAnimationStateSpeed(const std::string& stateName, float speed)
	{
		if (objAnimator->animResources != nullptr)
		{
			objAnimator->ChangeAnimatorStateSpeed(stateName, speed);
		}
	}

	void PBRObj::SettingAnimationStateTransition(const std::string& stateName, const std::string& stateStart, const std::string& stateEnd, bool hasExitTime, float exitTime, float transitionDuration)
	{
		if (objAnimator->animResources != nullptr)
		{
			if (stateName == "AnyState")
			{
				auto anyState = objAnimator->GetAnyState();

				std::shared_ptr<AnimatorTransition> transition = std::make_shared<AnimatorTransition>();

				transition->SetTransitionState(stateStart, stateEnd);

				transition->SetAnimationStateStart(stateStart);

				transition->SetAnimationStateEnd(stateEnd);

				transition->SetHasExitTime(hasExitTime);

				transition->SetExitTime(exitTime);

				transition->SetTransitionDuration(transitionDuration);

				anyState->AddTransition(transition);
			}
			else
			{
				// Animator�� �ִ� states �鿡�� �ش� state name�� �ִ��� ã�´�.
				auto tmp = objAnimator->GetAnimatorStates();

				auto findIt = tmp.find(stateName);

				// �ش� stateName�� ����ִٸ�
				if (findIt != tmp.end())
				{
					std::shared_ptr<AnimatorTransition> transition = std::make_shared<AnimatorTransition>();

					transition->SetTransitionState(stateStart, stateEnd);

					transition->SetAnimationStateStart(stateStart);

					transition->SetAnimationStateEnd(stateEnd);

					transition->SetHasExitTime(hasExitTime);

					transition->SetExitTime(exitTime);

					transition->SetTransitionDuration(transitionDuration);

					findIt->second->AddTransition(transition);
				}
			}
		}
	}

	void PBRObj::EntryPlayAnimationState(const std::string& stateName)
	{
		objAnimator->EntryAnimatorState(stateName);
	}

	void PBRObj::AddCondition(const std::string& transitionName, const std::string& parameterName, const std::string& conditionName, float val)
	{
		objAnimator->AddCondition(transitionName, parameterName, conditionName, val);
	}

	void PBRObj::AddCondition(const std::string& transitionName, const std::string& parameterName, const std::string& conditionName, int val)
	{
		objAnimator->AddCondition(transitionName, parameterName, conditionName, val);
	}

	void PBRObj::AddCondition(const std::string& transitionName, const std::string& parameterName, const std::string& conditionName, bool val)
	{
		objAnimator->AddCondition(transitionName, parameterName, conditionName, val);
	}

	void PBRObj::AddCondition(const std::string& transitionName, const std::string& parameterName, const std::string& conditionName)
	{
		objAnimator->AddCondition(transitionName, parameterName, conditionName);
	}

	void PBRObj::AddFloat(const std::string& name, float val)
	{
		objAnimator->AddFloat(name, val);
	}

	void PBRObj::AddInteger(const std::string& name, int val)
	{
		objAnimator->AddInteger(name, val);
	}

	void PBRObj::AddBool(const std::string& name, bool val)
	{
		objAnimator->AddBool(name, val);
	}

	void PBRObj::AddTrigger(const std::string& name)
	{
		objAnimator->AddTrigger(name);
	}

	void PBRObj::SetFloat(const std::string& name, float val)
	{
		objAnimator->SetFloat(name, val);
	}

	void PBRObj::SetInteger(const std::string& name, int val)
	{
		objAnimator->SetInteger(name, val);
	}

	void PBRObj::SetBool(const std::string& name, bool val)
	{
		objAnimator->SetBool(name, val);
	}

	void PBRObj::SetTrigger(const std::string& name)
	{
		objAnimator->SetTrigger(name);
	}

	void PBRObj::SetAnimCurrentFrame(int val)
	{
		objAnimator->SetNowFrame(val);
	}

	int PBRObj::GetAnimCurrentFrame()
	{
		return objAnimator->GetNowFrame();
	}

	int PBRObj::GetAnimCurrentTotalFrame()
	{
		return objAnimator->GetTotalFrame();
	}

	std::string PBRObj::GetCurrentStateName()
	{
		return objAnimator->GetCurrentAnimatorStateName();
	}

	void PBRObj::SetIsMesh(std::string meshName, bool val)
	{
		auto it = std::find_if(meshResources.begin(), meshResources.end(),
			[&meshName](std::shared_ptr<MeshResources> mesh) { return mesh->ObjName == meshName; });

		if (it != meshResources.end())
			(*it)->isRender = val;

		/*auto it2 = std::find_if(transparencyMesh.begin(), transparencyMesh.end(),
			[&meshName](std::pair<std::shared_ptr<MeshResources>, int> meshInfo) { return meshInfo.first->ObjName == meshName; });

		if(it2 != transparencyMesh.end())
			(*it2->first).isRender = val;*/

			// �׳� ��ȸ�ϸ鼭..
		for (auto& transMesh : transparencyMesh)
		{
			if (transMesh.first->ObjName == meshName)
			{
				transMesh.first->isRender = val;
			}
		}
	}

	void PBRObj::SetMaterialColor(const std::string& matName, const SimpleMath::Vector4& color)
	{
		// renderObj�� ��� Mesh���� matName�� color ���� �ٲ۴�.
		for (auto& mesh : meshResources)
		{
			auto it = std::find_if(mesh->material.begin(), mesh->material.end(),
				[&matName](std::shared_ptr<RenderingMaterial> mat) { return mat->materialName == matName; });

			if (it != mesh->material.end())
			{
				(*it)->material_Diffuse = Vector4(color.x, color.y, color.z, 1.0f);
				(*it)->material_Transparency = 1.0f - color.w;
			}
		}

		// transparent�� �����ؼ� �ٲ㺾�ô�
		for (auto& transMesh : transparencyMesh)
		{
			auto it = std::find_if(transMesh.first->material.begin(), transMesh.first->material.end(),
				[&matName](std::shared_ptr<RenderingMaterial> mat) {return mat->materialName == matName; });

			if (it != transMesh.first->material.end())
			{
				(*it)->material_Diffuse = Vector4(color.x, color.y, color.z, 1.0f);
				(*it)->material_Transparency = 1.0f - color.w;
			}
		}
	}

	void PBRObj::SetMaterialEmissiveColor(const std::string& matName, const SimpleMath::Vector4& color)
	{
		// renderObj�� ��� Mesh���� matName�� emissiveColor ���� �ٲ۴�.
		for (auto& mesh : meshResources)
		{
			auto it = std::find_if(mesh->material.begin(), mesh->material.end(),
				[&matName](std::shared_ptr<RenderingMaterial> mat) { return mat->materialName == matName; });

			if (it != mesh->material.end())
			{
				(*it)->material_Emissive = Vector4(color.x, color.y, color.z, color.w);
			}
		}

		// transparent�� �����ؼ� �ٲ㺾�ô�
		for (auto& transMesh : transparencyMesh)
		{
			auto it = std::find_if(transMesh.first->material.begin(), transMesh.first->material.end(),
				[&matName](std::shared_ptr<RenderingMaterial> mat) {return mat->materialName == matName; });

			if (it != transMesh.first->material.end())
			{
				(*it)->material_Emissive = Vector4(color.x, color.y, color.z, 1.0f);
			}
		}
	}

	void PBRObj::SetMaterialMetallicRoughness(const std::string& matName, float metallic, float roughness)
	{
		// renderObj�� ��� Mesh���� matName�� emissiveColor ���� �ٲ۴�.
		for (auto& mesh : meshResources)
		{
			auto it = std::find_if(mesh->material.begin(), mesh->material.end(),
				[&matName](std::shared_ptr<RenderingMaterial> mat) { return mat->materialName == matName; });

			if (it != mesh->material.end())
			{
				(*it)->metallic = metallic;
				(*it)->roughness = roughness;
			}
		}
	}

	void PBRObj::SetAllMaterialAlpha(float alpha)
	{
		// renderObj�� ��� Mesh���� matName�� color ���� �ٲ۴�.
		for (auto& mesh : meshResources)
		{
			for (auto& mat : mesh->material)
			{
				if (mat->materialName == "SkinMaterial")
					continue;

				mat->material_Transparency = 1.0f - alpha;
			}
		}
	}

	void PBRObj::DeleteMaterial(std::string matName)
	{
		for (auto& mesh : meshResources)
		{
			auto it = std::find_if(mesh->material.begin(), mesh->material.end(),
				[&matName](std::shared_ptr<RenderingMaterial> mat) { return mat->materialName == matName; });

			if (it != mesh->material.end())
			{
				mesh->material.erase(it);
			}
		}

		for (auto transMeshIt = transparencyMesh.begin(); transMeshIt != transparencyMesh.end(); )
		{
			auto& transMesh = *transMeshIt;
			auto it = std::find_if(transMesh.first->material.begin(), transMesh.first->material.end(),
				[&matName](std::shared_ptr<RenderingMaterial> mat) { return mat->materialName == matName; });

			if (it != transMesh.first->material.end())
			{
				transMesh.first->material.erase(it);

				transMeshIt = std::find_if(transparencyMesh.begin(), transparencyMesh.end(),
					[&transMesh](std::pair<std::shared_ptr<MeshResources>, int> mesh) { return mesh == transMesh; });

				transMeshIt = transparencyMesh.erase(transMeshIt);
			}
			else
			{
				++transMeshIt;
			}
		}
	}

	void PBRObj::SetDissolveSpeed(float dissolveSpeed)
	{
		this->dissolveSpeed = dissolveSpeed;

		// ���� �����갡 0�̸� �ε嵵 0����
		/*if(dissolveSpeed < 0.0001f)
			loadOffset = 0.f;*/
	}

	void PBRObj::SetRimColor(const SimpleMath::Vector4& color)
	{
		rimColor = color;
	}

	void PBRObj::LightOff()
	{
		for (auto& mesh : meshResources)
		{
			mesh->isLight = false;
		}

		for (auto& mesh : transparencyMesh)
		{
			mesh.first->isLight = false;
		}
	}

	std::unordered_map<std::string, std::shared_ptr<AnimatorState>> PBRObj::GetStates()
	{
		return objAnimator->GetAnimatorStates();
	}

	void PBRObj::StaticRender(const std::shared_ptr<MeshResources>& objRes, const std::shared_ptr<IRenderer>& renderer)
	{
		auto mesh = objRes->mesh;
		auto material = objRes->material;

		auto noise = ResourceManager::GetInstance()->Load2DNoiseTexture();

		// ���������� ��ư
		int silhouetteIdx = mesh->silhouetteIdx;

		if (silhouetteIdx != -1 && silhouetteIdx < material.size()
			&& objRes->ObjName == "Plane.002_ElevatorButton")
		{
			renderer->GraphicsDebugBeginEvent("Silhouette Masking");
			SilhouetteMaskRender(mesh, material[silhouetteIdx], renderer, objRes->ObjName);
			renderer->GraphicsDebugEndEvent();
		}

		for (int matCnt = 0; matCnt < material.size(); matCnt++)
		{
			if (mesh->indexBuffersSize.empty())
				break;

			// ���� üũ
			if (material[matCnt]->material_Transparency > 0.f)
				continue;

			auto& albedo = material[matCnt]->albedoTex;
			auto& normal = material[matCnt]->normalTex;
			auto& metallic = material[matCnt]->metallicTex;
			auto& roughness = material[matCnt]->roughnessTex;
			auto& ao = material[matCnt]->aoTex;
			auto& emissive = material[matCnt]->emissiveTex;


			// ���̴� ����
			ShaderManager::GetInstance()->GetShader("PBRStaticVS")->SetUpShader();

			if (albedo == nullptr)
			{
				// �ش� ������ �����ϴ� ���̴��� �ϳ� �ۿ� ����.
				if (burnSpeed > 0.0f)
					ShaderManager::GetInstance()->GetShader("PaperBurnPS")->SetUpShader();
				else
					ShaderManager::GetInstance()->GetShader("PBRPS")->SetUpShader();
			}
			else
			{
				// �������� ��� �˺����� ������ �����Ѵ�.
				if (
					normal != nullptr &&
					metallic != nullptr &&
					roughness != nullptr &&
					ao != nullptr &&
					emissive != nullptr
					)
				{
					ShaderManager::GetInstance()->GetShader("PBRFullPS")->SetUpShader();
				}
				else if (
					normal != nullptr &&
					metallic != nullptr &&
					roughness != nullptr &&
					ao != nullptr
					)
				{
					ShaderManager::GetInstance()->GetShader("PBRNormalMRAOPS")->SetUpShader();
				}
				else if (
					normal != nullptr &&
					ao != nullptr
					)
				{
					ShaderManager::GetInstance()->GetShader("PBRNormalAOPS")->SetUpShader();
				}
				else if (
					normal != nullptr &&
					metallic != nullptr &&
					roughness != nullptr
					)
				{
					ShaderManager::GetInstance()->GetShader("PBRNormalMetallicRoughnessPS")->SetUpShader();
				}
				else if (
					metallic != nullptr &&
					roughness != nullptr
					)
				{
					ShaderManager::GetInstance()->GetShader("PBRMetallicRoughnessPS")->SetUpShader();
				}
				else if (
					normal != nullptr
					)
				{
					ShaderManager::GetInstance()->GetShader("PBRNormalPS")->SetUpShader();
				}
				else
				{
					if (burnSpeed > 0.0f)
						ShaderManager::GetInstance()->GetShader("PaperBurnAlbedoPS")->SetUpShader();
					else
						ShaderManager::GetInstance()->GetShader("PBRAlbedoPS")->SetUpShader();
				}
			}

			D3DDeviceContext->IASetPrimitiveTopology(mesh->GetPrimitiveTopology());

			UINT stride = sizeof(VertexStruct::PBRStaticVertex);
			UINT offset = 0;
			D3DDeviceContext->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
			D3DDeviceContext->IASetIndexBuffer(mesh->indexBuffers[matCnt].Get(), DXGI_FORMAT_R32_UINT, 0);

			// ��� ���� ����
			cbMatrix cbPerObj;
			cbPerObj.worldViewProj = world * view * proj;
			cbPerObj.worldTransform = world;
			cbPerObj.worldInvTranspose = MathHelper::InverseTranspose(world);
			BufferManager::GetInstance()->GetBuffer("MatrixCB")->SetUpBuffer(0, &cbPerObj, ShaderType::VERTEX);

			// PBR Material
			cbMaterial cbPBRMat;
			cbPBRMat.metallic = material[matCnt]->metallic;
			cbPBRMat.roughness = material[matCnt]->roughness;

			cbPBRMat.AddColor.x = material[matCnt]->material_Diffuse.x;
			cbPBRMat.AddColor.y = material[matCnt]->material_Diffuse.y;
			cbPBRMat.AddColor.z = material[matCnt]->material_Diffuse.z;
			cbPBRMat.AddColor.w = 1.0f - material[matCnt]->material_Transparency;

			cbPBRMat.emissiveColor.x = material[matCnt]->material_Emissive.x;
			cbPBRMat.emissiveColor.y = material[matCnt]->material_Emissive.y;
			cbPBRMat.emissiveColor.z = material[matCnt]->material_Emissive.z;
			cbPBRMat.Offset = burnOffset;
			cbPBRMat.isLight = objRes->isLight;

			BufferManager::GetInstance()->GetBuffer("PBRMaterialCB")->SetUpBuffer(1, &cbPBRMat, ShaderType::VERTEX);
			BufferManager::GetInstance()->GetBuffer("PBRMaterialCB")->SetUpBuffer(0, &cbPBRMat, ShaderType::PIXEL);

			// �ؽ��� ����
			if (albedo != nullptr)
				D3DDeviceContext->PSSetShaderResources(0, 1, albedo->GetMapSRV().GetAddressOf());

			if (normal != nullptr)
				D3DDeviceContext->PSSetShaderResources(1, 1, normal->GetMapSRV().GetAddressOf());

			if (metallic != nullptr)
				D3DDeviceContext->PSSetShaderResources(2, 1, metallic->GetMapSRV().GetAddressOf());

			if (roughness != nullptr)
				D3DDeviceContext->PSSetShaderResources(3, 1, roughness->GetMapSRV().GetAddressOf());

			if (ao != nullptr)
				D3DDeviceContext->PSSetShaderResources(4, 1, ao->GetMapSRV().GetAddressOf());

			if (emissive != nullptr)
				D3DDeviceContext->PSSetShaderResources(5, 1, emissive->GetMapSRV().GetAddressOf());

			D3DDeviceContext->PSSetShaderResources(7, 1, noise->GetMapSRV().GetAddressOf());

			D3DDeviceContext->PSSetSamplers(0, 1, SamplerManager::GetInstance()->GetLinearClamp().GetAddressOf());

			// ���� ������Ʈ ����.
			D3DDeviceContext->RSSetState(mesh->GetRasterState().Get());

			// �ش� ���� �׸���
			D3DDeviceContext->DrawIndexed(mesh->indexBuffersSize[matCnt], 0, 0);

			// ���̴� �ʱ�ȭ
			ID3D11ShaderResourceView* nullSRV[9] = { nullptr };

			D3DDeviceContext->PSSetShaderResources(0, 9, nullSRV);
		}
	}

	void PBRObj::SkinnedRender(const std::shared_ptr<MeshResources>& objRes, const std::shared_ptr<IRenderer>& renderer)
	{
		auto mesh = objRes->mesh;
		auto material = objRes->material;

		auto dxRenderer = std::dynamic_pointer_cast<Renderer>(renderer);
		auto noise = ResourceManager::GetInstance()->Load2DNoiseTexture();

		// �ε� ���� �ʾ����� �׸��� �ʽ��ϴ�.
		if (objRes->ObjName == "dog_Material10_dogani" && isLoadCompleted == false)
			return;

		// �ٸ� ���͵鵵 �׸��� �ʵ��� �մϴ�.
		if (objRes->ObjName == "dog_Material11_dogani_throw" && isLoadCompleted == false)
			return;

		if (objRes->ObjName == "body_Material7_toadani" && isLoadCompleted == false)
			return;

		if (objRes->ObjName == "red_Material10_redani" && isLoadCompleted == false)
			return;

		if (objRes->ObjName == "weapon_Material5_redani" && isLoadCompleted == false)
			return;

		// �ͷ��� ��� �ε���� �̽��� �׷��ݴϴ�.

		/*
		if (objRes->ObjName == "turret_laser.001_Material2_Turret" && isLoadCompleted == false)
			return;
		 */

		 // �׷��Ƚ� ����ſ��� ���׸����� ã�Ƽ� �������
			 // �ٵ� �Ƹ� redģ���� ���� ������ ���⸦ �׸��� �ʵ��� �������
			 // �װ� ���� �Ʒ� �����ؼ� ������ּ���.

		int silhouetteIdx = mesh->silhouetteIdx;

		// ���׸����� ���� ���� �Ƿ翧 ���׸������ ����ŷ�� ���ش�.
			// �޽ô� �ִ� ���� ���ݴϴ�.
		// ����ŷ�� ã�Ƽ� ���ݴϴ�..
			// ���ʹ� ����ŷ�� ���� �ʽ��ϴ�.
		if (silhouetteIdx != -1 && silhouetteIdx < material.size()
			&& (objRes->ObjName == "tail_Material14_tailani"
				|| objRes->ObjName == "hammer_Material2_tailani"
				|| objRes->ObjName == "cock_Material10_cockani"
				))
		{
			renderer->GraphicsDebugBeginEvent("Silhouette Masking");
			SilhouetteMaskRender(mesh, material[silhouetteIdx], renderer, objRes->ObjName);
			renderer->GraphicsDebugEndEvent();
		}


		for (int matCnt = 0; matCnt < material.size(); matCnt++)
		{
			if (mesh->indexBuffersSize.empty())
				break;

			// ���� üũ
			if (material[matCnt]->material_Transparency > 0.f)
				continue;

			/*
			*/
			if (material[matCnt]->materialName == "SilhouetteMaterial" &&
				objRes->ObjName == "cock_Material10_cockani")
				continue;

			auto& albedo = material[matCnt]->albedoTex;
			auto& normal = material[matCnt]->normalTex;
			auto& metallic = material[matCnt]->metallicTex;
			auto& roughness = material[matCnt]->roughnessTex;
			auto& ao = material[matCnt]->aoTex;
			auto& emissive = material[matCnt]->emissiveTex;

			// ���̴� ����
			ShaderManager::GetInstance()->GetShader("PBRSkinnedVS")->SetUpShader();

			if (albedo == nullptr)
			{
				// �ش� ������ �����ϴ� ���̴��� �ϳ� �ۿ� ����.
				if (burnSpeed > 0.0f)
				{
					ShaderManager::GetInstance()->GetShader("SkinPaperBurnPS")->SetUpShader();
					//objRes->isLight = false;
				}
				else
					ShaderManager::GetInstance()->GetShader("PBRSkinPS")->SetUpShader();
			}
			else
			{
				// �������� ��� �˺����� ������ �����Ѵ�.
				if (
					normal != nullptr &&
					metallic != nullptr &&
					roughness != nullptr &&
					ao != nullptr &&
					emissive != nullptr
					)
				{
					ShaderManager::GetInstance()->GetShader("PBRSkinFullPS")->SetUpShader();
				}
				else if (
					normal != nullptr &&
					metallic != nullptr &&
					roughness != nullptr &&
					ao != nullptr
					)
				{
					ShaderManager::GetInstance()->GetShader("PBRSkinNormalMRAOPS")->SetUpShader();
				}
				else if (
					normal != nullptr &&
					ao != nullptr
					)
				{
					ShaderManager::GetInstance()->GetShader("PBRSkinNormalAOPS")->SetUpShader();
				}
				else if (
					normal != nullptr &&
					metallic != nullptr &&
					roughness != nullptr
					)
				{
					ShaderManager::GetInstance()->GetShader("PBRSkinNormalMetallicRoughnessPS")->SetUpShader();
				}
				else if (
					metallic != nullptr &&
					roughness != nullptr
					)
				{
					ShaderManager::GetInstance()->GetShader("PBRSkinMetallicRoughnessPS")->SetUpShader();
				}
				else if (
					normal != nullptr
					)
				{
					ShaderManager::GetInstance()->GetShader("PBRSkinNormalPS")->SetUpShader();
				}
				else
				{
					if (burnSpeed > 0.0f)
						ShaderManager::GetInstance()->GetShader("SkinPaperBurnAlbedoPS")->SetUpShader();
					else
						ShaderManager::GetInstance()->GetShader("PBRSkinAlbedoPS")->SetUpShader();
				}
			}


			D3DDeviceContext->IASetPrimitiveTopology(mesh->GetPrimitiveTopology());

			UINT stride = sizeof(VertexStruct::PBRSkinnedVertex);
			UINT offset = 0;
			D3DDeviceContext->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
			D3DDeviceContext->IASetIndexBuffer(mesh->indexBuffers[matCnt].Get(), DXGI_FORMAT_R32_UINT, 0);

			// ��� ���� ����
			cbMatrix cbPerObj;
			cbPerObj.worldViewProj = world * view * proj;
			cbPerObj.worldTransform = world;
			cbPerObj.worldInvTranspose = MathHelper::InverseTranspose(world);
			BufferManager::GetInstance()->GetBuffer("MatrixCB")->SetUpBuffer(0, &cbPerObj, ShaderType::VERTEX);

			// RimLight ������ screen Info�� �־ ���� ������..
				// ��� �̹̽ú� �����̹Ƿ� ������ ����..
			BufferManager::GetInstance()->GetBuffer("ColorCB")->SetUpBuffer(3, &rimColor, ShaderType::PIXEL);

			// EyePos ������...
			auto cbEyePos = dxRenderer->GetEyePos();
			SimpleMath::Vector4 eyePos = { cbEyePos.x, cbEyePos.y, cbEyePos.z, 1.0f };
			BufferManager::GetInstance()->GetBuffer("ViewPosCB")->SetUpBuffer(2, &eyePos, ShaderType::PIXEL);

			// PBR Material
			cbMaterial cbPBRMat;
			cbPBRMat.metallic = material[matCnt]->metallic;
			cbPBRMat.roughness = material[matCnt]->roughness;

			cbPBRMat.AddColor.x = material[matCnt]->material_Diffuse.x;
			cbPBRMat.AddColor.y = material[matCnt]->material_Diffuse.y;
			cbPBRMat.AddColor.z = material[matCnt]->material_Diffuse.z;
			cbPBRMat.AddColor.w = 1.0f - material[matCnt]->material_Transparency;

			if (material[matCnt]->materialName == "light_red_Turret")
			{
				if (isLoadCompleted == true)
				{
					// �ͷ����� dissolveSpeed�� ���������� �������ݴϴ�.
					if (onOffset > 1.0f)
						onOffset = 1.0f;
					if (onOffset < 0.f)
						onOffset = 0.f;

					cbPBRMat.emissiveColor.x = lerp(0.f, material[matCnt]->material_Emissive.x, onOffset);
					cbPBRMat.emissiveColor.y = lerp(0.f, material[matCnt]->material_Emissive.y, onOffset);
					cbPBRMat.emissiveColor.z = lerp(0.f, material[matCnt]->material_Emissive.z, onOffset);
				}
				else
				{
					cbPBRMat.emissiveColor.x = 0.0f;
					cbPBRMat.emissiveColor.y = 0.0f;
					cbPBRMat.emissiveColor.z = 0.0f;
				}
			}
			else
			{
				cbPBRMat.emissiveColor.x = material[matCnt]->material_Emissive.x;
				cbPBRMat.emissiveColor.y = material[matCnt]->material_Emissive.y;
				cbPBRMat.emissiveColor.z = material[matCnt]->material_Emissive.z;
			}

			cbPBRMat.Offset = burnOffset;
			cbPBRMat.isLight = objRes->isLight;

			BufferManager::GetInstance()->GetBuffer("PBRMaterialCB")->SetUpBuffer(1, &cbPBRMat, ShaderType::VERTEX);
			BufferManager::GetInstance()->GetBuffer("PBRMaterialCB")->SetUpBuffer(0, &cbPBRMat, ShaderType::PIXEL);

			// ��Ų ������Ʈ�� ���� ������
			SimpleMath::Vector4 texOffset = { 0.001f, 0.f, 0.f, 0.f };
			BufferManager::GetInstance()->GetBuffer("TexOffsetCB")->SetUpBuffer(3, &texOffset, ShaderType::VERTEX);

			// Bone Matrix
				// => boneResources�� boneTransform�� �ִϸ��̼ǿ� ���� ��𼱰����� ����ȴٰ� ����.
				// PreRender�κ��� �� ���� �ְ�, �ش� object�� Update�ϼ��� ����.
			cbBoneMatrix cbBone;

			for (int i = 0; i < nowBoneTM.size(); i++)
			{
				cbBone.boneMatrix[i] = nowBoneTM[i];
			}

			BufferManager::GetInstance()->GetBuffer("BoneMatrixCB")->SetUpBuffer(2, &cbBone, ShaderType::VERTEX);

			// �ؽ��� ����
			if (albedo != nullptr)
				D3DDeviceContext->PSSetShaderResources(0, 1, albedo->GetMapSRV().GetAddressOf());

			if (normal != nullptr)
				D3DDeviceContext->PSSetShaderResources(1, 1, normal->GetMapSRV().GetAddressOf());

			if (metallic != nullptr)
				D3DDeviceContext->PSSetShaderResources(2, 1, metallic->GetMapSRV().GetAddressOf());

			if (roughness != nullptr)
				D3DDeviceContext->PSSetShaderResources(3, 1, roughness->GetMapSRV().GetAddressOf());

			if (ao != nullptr)
				D3DDeviceContext->PSSetShaderResources(4, 1, ao->GetMapSRV().GetAddressOf());

			if (emissive != nullptr)
				D3DDeviceContext->PSSetShaderResources(5, 1, emissive->GetMapSRV().GetAddressOf());

			D3DDeviceContext->PSSetShaderResources(7, 1, noise->GetMapSRV().GetAddressOf());

			D3DDeviceContext->PSSetSamplers(0, 1, SamplerManager::GetInstance()->GetLinearClamp().GetAddressOf());

			// ���� ������Ʈ ����.
			D3DDeviceContext->RSSetState(mesh->GetRasterState().Get());

			// �ش� ���� �׸���
			D3DDeviceContext->DrawIndexed(mesh->indexBuffersSize[matCnt], 0, 0);

			// ���̴� �ʱ�ȭ
			ID3D11ShaderResourceView* nullSRV[9] = { nullptr };

			D3DDeviceContext->PSSetShaderResources(0, 9, nullSRV);
		}
	}

	void PBRObj::YieldRender(const std::pair<std::shared_ptr<MeshResources>, int>& transObjRes, const std::shared_ptr<IRenderer>& renderer)
	{
		auto mesh = transObjRes.first->mesh;
		auto material = transObjRes.first->material[transObjRes.second];
		auto objName = transObjRes.first->ObjName;

		auto noise = ResourceManager::GetInstance()->Load2DNoiseTexture();

		if (mesh->indexBuffersSize.empty())
			return;

		// ������ �ƿ� 0�̸� return ���ݴϴ�.
		if (1.0f - material->material_Transparency < 0.00001f)
			return;

		if (isLoadCompleted == true)
		{
			// �ε��� �Ϸ�Ǹ�(�Ƿ翧��) �׸��� �ʽ��ϴ�.
			if (objName == "dog_Material10_dogani" && material->materialName == "SilhouetteMaterial")
				return;

			if (objName == "dog_Material11_dogani_throw" && material->materialName == "SilhouetteMaterial")
				return;

			if (objName == "body_Material7_toadani" && material->materialName == "SilhouetteMaterial")
				return;

			// ���⵵ ��ž�̶� ���� �߰��� �ּž��մϴ�..
			if (objName == "red_Material10_redani" && material->materialName == "SilhouetteMaterial")
				return;

			if (objName == "weapon_Material5_redani" && material->materialName == "SilhouetteMaterial")
				return;

			if (objName == "weapon_Material5_redani" && material->materialName == "SilhouetteMaterial")
				return;

			if (objName == "turret_laser_Material2_Turret" && material->materialName == "SilhouetteMaterial")
				return;
		}

		auto dxRenderer = std::dynamic_pointer_cast<Renderer>(renderer);

		auto& albedo = material->albedoTex;
		auto& normal = material->normalTex;
		auto& metallic = material->metallicTex;
		auto& roughness = material->roughnessTex;
		auto& ao = material->aoTex;
		auto& emissive = material->emissiveTex;

		auto depthBuffer = renderer->GetDepthBuffer()->GetDepthShaderResource();
		auto stencilBuffer = renderer->GetDepthBuffer()->GetStencilShaderResource();

		UINT stride;

		// ���̴� ����
		if (mesh->isSkinnedMesh != true)
		{
			ShaderManager::GetInstance()->GetShader("PBRStaticVS")->SetUpShader();
			stride = sizeof(VertexStruct::PBRStaticVertex);

			if (objName == "Plane.002_ElevatorButton")
			{
				ShaderManager::GetInstance()->GetShader("PBRAlphaVS")->SetUpShader();
				// ��ǻ� �ܰ����� �β��� �����մϴ�.
				//loadOffset += dissolveSpeed;
				if (loadOffset > 1.0f)
					loadOffset = 1.0f;

				if (loadOffset < 0.0f)
					loadOffset = 0.f;

				SimpleMath::Vector4 texOffset = { 800.0f, loadOffset, 0.f, 0.f };
				BufferManager::GetInstance()->GetBuffer("TexOffsetCB")->SetUpBuffer(3, &texOffset, ShaderType::VERTEX);
				BufferManager::GetInstance()->GetBuffer("TexOffsetCB")->SetUpBuffer(4, &texOffset, ShaderType::PIXEL);
			}
		}
		else
		{
			ShaderManager::GetInstance()->GetShader("PBRAlphaSkinnedVS")->SetUpShader();
			stride = sizeof(VertexStruct::PBRSkinnedVertex);

			// load�� �ȵǾ��� ��.. ����� �׷����� �ʽ��ϴ�.
			if (isLoadCompleted == false)
			{
				if (objName == "dog_Material10_dogani" && material->materialName == "peb light_glass_dogani")
					return;

				if (objName == "dog_Material11_dogani_throw" && material->materialName == "peb light_glass_dogani_throw")
					return;

				if (objName == "red_Material10_redani" && material->materialName == "black_si_redani")
					return;
			}

			// load�� ���۵Ǿ��� ���� �׷��ݴϴ�.
			if (isLoad == true)
			{
				// ���⿡ ��ž ���� �߰�
				if ((objName == "dog_Material10_dogani"
					|| objName == "dog_Material11_dogani_throw"
					|| objName == "body_Material7_toadani"
					|| objName == "red_Material10_redani"
					|| objName == "weapon_Material5_redani"
					|| objName == "turret_laser_Material2_Turret")
					&& material->materialName == "SilhouetteMaterial")
					ShaderManager::GetInstance()->GetShader("PBRMonsterVS")->SetUpShader();
			}

			if (objName == "cock_Material10_cockani")
			{
				// ��ǻ� �ܰ����� �β��� �����մϴ�.
				if (loadOffset > 1.0f)
					loadOffset = 1.0f;

				if (loadOffset < 0.0f)
					loadOffset = 0.f;

				SimpleMath::Vector4 texOffset = { 0.05f, loadOffset, 0.f, 0.f };
				BufferManager::GetInstance()->GetBuffer("TexOffsetCB")->SetUpBuffer(3, &texOffset, ShaderType::VERTEX);
				BufferManager::GetInstance()->GetBuffer("TexOffsetCB")->SetUpBuffer(4, &texOffset, ShaderType::PIXEL);

			}
			else
			{
				SimpleMath::Vector4 texOffset = { 0.001f, 0.f, 0.f, 0.f };
				BufferManager::GetInstance()->GetBuffer("TexOffsetCB")->SetUpBuffer(3, &texOffset, ShaderType::VERTEX);

			}

			// Bone Matrix
			// => boneResources�� boneTransform�� �ִϸ��̼ǿ� ���� ��𼱰����� ����ȴٰ� ����.
			// PreRender�κ��� �� ���� �ְ�, �ش� object�� Update�ϼ��� ����.
			cbBoneMatrix cbBone;

			for (int i = 0; i < nowBoneTM.size(); i++)
			{
				cbBone.boneMatrix[i] = nowBoneTM[i];
			}

			BufferManager::GetInstance()->GetBuffer("BoneMatrixCB")->SetUpBuffer(2, &cbBone, ShaderType::VERTEX);
		}

		if (albedo == nullptr)
		{
			// �ش� ������ �����ϴ� ���̴��� �ϳ� �ۿ� ����.
			if (burnSpeed > 0.0f)
				ShaderManager::GetInstance()->GetShader("PaperBurnAlphaPS")->SetUpShader();
			else
				ShaderManager::GetInstance()->GetShader("PBRAlphaPS")->SetUpShader();
		}
		else
		{
			// �������� ��� �˺����� ������ �����Ѵ�.
			if (
				normal != nullptr &&
				metallic != nullptr &&
				roughness != nullptr &&
				ao != nullptr &&
				emissive != nullptr
				)
			{
				ShaderManager::GetInstance()->GetShader("PBRAlphaFullPS")->SetUpShader();
			}
			else if (
				normal != nullptr &&
				metallic != nullptr &&
				roughness != nullptr &&
				ao != nullptr
				)
			{
				ShaderManager::GetInstance()->GetShader("PBRAlphaNormalMRAOPS")->SetUpShader();
			}
			else if (
				normal != nullptr &&
				ao != nullptr
				)
			{
				ShaderManager::GetInstance()->GetShader("PBRAlphaNormalAOPS")->SetUpShader();
			}
			else if (
				normal != nullptr &&
				metallic != nullptr &&
				roughness != nullptr
				)
			{
				ShaderManager::GetInstance()->GetShader("PBRAlphaNormalMetallicRoughnessPS")->SetUpShader();
			}
			else if (
				metallic != nullptr &&
				roughness != nullptr
				)
			{
				ShaderManager::GetInstance()->GetShader("PBRAlphaMetallicRoughnessPS")->SetUpShader();
			}
			else if (
				normal != nullptr
				)
			{
				ShaderManager::GetInstance()->GetShader("PBRAlphaNormalPS")->SetUpShader();
			}
			else
			{
				ShaderManager::GetInstance()->GetShader("PBRAlphaAlbedoPS")->SetUpShader();
			}
		}

		D3DDeviceContext->IASetPrimitiveTopology(mesh->GetPrimitiveTopology());

		if (material->materialName == "SilhouetteMaterial")
		{
			// ���⵵ ���� ��ž �߰�.
			if ((objName == "dog_Material10_dogani"
				|| objName == "dog_Material11_dogani_throw"
				|| objName == "body_Material7_toadani"
				|| objName == "red_Material10_redani"
				|| objName == "weapon_Material5_redani"
				|| objName == "turret_laser_Material2_Turret") && isLoad == true)
				ShaderManager::GetInstance()->GetShader("PBRMonsterSilhouettePS")->SetUpShader();
			else
				ShaderManager::GetInstance()->GetShader("PBRSilhouettePS")->SetUpShader();

			if (objName == "cock_Material10_cockani" || objName == "Plane.002_ElevatorButton")
				ShaderManager::GetInstance()->GetShader("PBREdgePS")->SetUpShader();
		}

		UINT offset = 0;
		D3DDeviceContext->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
		D3DDeviceContext->IASetIndexBuffer(mesh->indexBuffers[transObjRes.second].Get(), DXGI_FORMAT_R32_UINT, 0);

		// ��� ���� ����
		cbMatrix cbPerObj;

		cbPerObj.worldViewProj = world * view * proj;
		cbPerObj.worldTransform = world;
		cbPerObj.worldInvTranspose = MathHelper::InverseTranspose(world);
		BufferManager::GetInstance()->GetBuffer("MatrixCB")->SetUpBuffer(0, &cbPerObj, ShaderType::VERTEX);

		// PBR Material
		cbMaterial cbPBRMat;
		cbPBRMat.metallic = material->metallic;
		cbPBRMat.roughness = material->roughness;

		cbPBRMat.AddColor.x = material->material_Diffuse.x;
		cbPBRMat.AddColor.y = material->material_Diffuse.y;
		cbPBRMat.AddColor.z = material->material_Diffuse.z;
		cbPBRMat.AddColor.w = 1.0f - material->material_Transparency;

		// ���⵵ ��ž ���� �߰�.
		if ((objName == "dog_Material10_dogani"
			|| objName == "dog_Material11_dogani_throw"
			|| objName == "body_Material7_toadani")
			|| objName == "red_Material10_redani"
			|| objName == "weapon_Material5_redani"
			|| objName == "turret_laser_Material2_Turret")
		{
			if (isLoad == true)
			{
				cbPBRMat.emissiveColor.x = 2.5f;
				cbPBRMat.emissiveColor.y = 0.5f;
				cbPBRMat.emissiveColor.z = 3.0f;
			}
			else
			{
				// �߱���
					// �߱����� emissive�� �߰��մϴ�.
				if (material->materialName == "peb light_glass_dogani_throw" || material->materialName == "peb light_glass_dogani")
				{
					cbPBRMat.emissiveColor.x = 10.f;
					cbPBRMat.emissiveColor.y = 0.36f;
					cbPBRMat.emissiveColor.z = 0.71f;
				}
			}
		}
		else
		{
			cbPBRMat.emissiveColor.x = material->material_Emissive.x;
			cbPBRMat.emissiveColor.y = material->material_Emissive.y;
			cbPBRMat.emissiveColor.z = material->material_Emissive.z;
		}

		cbPBRMat.Offset = burnOffset;

		BufferManager::GetInstance()->GetBuffer("PBRMaterialCB")->SetUpBuffer(1, &cbPBRMat, ShaderType::VERTEX);
		BufferManager::GetInstance()->GetBuffer("PBRMaterialCB")->SetUpBuffer(0, &cbPBRMat, ShaderType::PIXEL);

		// ���ϸ� ������Ʈ�� �������ݴϴ�.
		if (material->materialName == "SilhouetteMaterial" && (objName == "tail_Material14_tailani" || objName == "hammer_Material2_tailani"))
		{
			// Silhouette�� Rim Light ����
			SimpleMath::Vector4 rimColor = { 0.5f, 0.5f, 1.0f, 0.2f };

			BufferManager::GetInstance()->GetBuffer("ColorCB")->SetUpBuffer(3, &rimColor, ShaderType::PIXEL);
		}
		else
		{
			// ScreenInfo
			SimpleMath::Vector4 screenInfo = { static_cast<float>(DX11GraphicsCore::GetInstance()->GetScreenWidth()), static_cast<float>(DX11GraphicsCore::GetInstance()->GetScreenHeight()),
			0.f, 0.f };

			if (isLoad == true)
			{
				screenInfo.w = loadOffset;

				if (loadOffset > goalOffset)
				{
					loadOffset = goalOffset;
					isLoadCompleted = true;
					// �� �� �ͷ��̶��..! dissolve ���ǵ带 �÷��ݴϴ�.
					if (objName == "turret_laser_Material2_Turret")
						dissolveSpeed = 2.f;

					isLoad = false;
				}
			}

			BufferManager::GetInstance()->GetBuffer("TextureInfoCB")->SetUpBuffer(1, &screenInfo, ShaderType::PIXEL);

			SimpleMath::Vector4 rimColor = { 0.8f, 0.5f, 1.0f, 0.5f };

			if (objName == "cock_Material10_cockani" || objName == "Plane.002_ElevatorButton")
				rimColor = SimpleMath::Vector4{ 0.8f, 0.98f, 0.f, 0.0f };

			BufferManager::GetInstance()->GetBuffer("ColorCB")->SetUpBuffer(3, &rimColor, ShaderType::PIXEL);
		}

		// EyePos ������...
		auto cbLightObj = dxRenderer->GetLight();
		SimpleMath::Vector4 eyePos = { cbLightObj.EyePosW.x, cbLightObj.EyePosW.y, cbLightObj.EyePosW.z, 1.0f };
		BufferManager::GetInstance()->GetBuffer("ViewPosCB")->SetUpBuffer(2, &eyePos, ShaderType::PIXEL);

		// �ؽ��� ����
		if (albedo != nullptr)
			D3DDeviceContext->PSSetShaderResources(0, 1, albedo->GetMapSRV().GetAddressOf());

		if (normal != nullptr)
			D3DDeviceContext->PSSetShaderResources(1, 1, normal->GetMapSRV().GetAddressOf());

		if (metallic != nullptr)
			D3DDeviceContext->PSSetShaderResources(2, 1, metallic->GetMapSRV().GetAddressOf());

		if (roughness != nullptr)
			D3DDeviceContext->PSSetShaderResources(3, 1, roughness->GetMapSRV().GetAddressOf());

		if (ao != nullptr)
			D3DDeviceContext->PSSetShaderResources(4, 1, ao->GetMapSRV().GetAddressOf());

		if (emissive != nullptr)
			D3DDeviceContext->PSSetShaderResources(5, 1, emissive->GetMapSRV().GetAddressOf());

		D3DDeviceContext->PSSetShaderResources(6, 1, depthBuffer.GetAddressOf());

		D3DDeviceContext->PSSetShaderResources(7, 1, noise->GetMapSRV().GetAddressOf());

		if (stencilBuffer != nullptr)
			D3DDeviceContext->PSSetShaderResources(8, 1, stencilBuffer.GetAddressOf());

		D3DDeviceContext->PSSetSamplers(0, 1, SamplerManager::GetInstance()->GetLinearClamp().GetAddressOf());

		// ���� ������Ʈ ����.
		D3DDeviceContext->RSSetState(RasterizerState::GetSolidRS().Get());

		// �ش� ���� �׸���
		D3DDeviceContext->DrawIndexed(mesh->indexBuffersSize[transObjRes.second], 0, 0);

		// ���̴� �ʱ�ȭ
		ID3D11ShaderResourceView* nullSRV[9] = { nullptr };

		D3DDeviceContext->PSSetShaderResources(0, 9, nullSRV);
	}

	void PBRObj::TrailRender(const std::shared_ptr<MeshResources>& objRes, const std::shared_ptr<IRenderer>& renderer)
	{
		// �⺻������ ��� ���׸����� �������ϴ� ����
		auto mesh = objRes->mesh;
		auto material = objRes->material;

		auto dxRenderer = std::dynamic_pointer_cast<Renderer>(renderer);
		auto depthBuffer = renderer->GetDepthBuffer()->GetDepthShaderResource();

		for (int matCnt = 0; matCnt < material.size(); matCnt++)
		{
			if (mesh->indexBuffersSize.empty())
				break;

			auto& albedo = material[matCnt]->albedoTex;
			auto& normal = material[matCnt]->normalTex;
			auto& metallic = material[matCnt]->metallicTex;
			auto& roughness = material[matCnt]->roughnessTex;
			auto& ao = material[matCnt]->aoTex;
			auto& emissive = material[matCnt]->emissiveTex;

			// ���̴� ����
			ShaderManager::GetInstance()->GetShader("PBRSkinnedVS")->SetUpShader();

			if (albedo == nullptr)
			{
				ShaderManager::GetInstance()->GetShader("PBRAlphaPS")->SetUpShader();
			}
			else
			{
				// �������� ��� �˺����� ������ �����Ѵ�.
				if (
					normal != nullptr &&
					metallic != nullptr &&
					roughness != nullptr &&
					ao != nullptr &&
					emissive != nullptr
					)
				{
					ShaderManager::GetInstance()->GetShader("PBRSkinFullPS")->SetUpShader();
				}
				else if (
					normal != nullptr &&
					metallic != nullptr &&
					roughness != nullptr &&
					ao != nullptr
					)
				{
					ShaderManager::GetInstance()->GetShader("PBRSkinNormalMRAOPS")->SetUpShader();
				}
				else if (
					normal != nullptr &&
					ao != nullptr
					)
				{
					ShaderManager::GetInstance()->GetShader("PBRSkinNormalAOPS")->SetUpShader();
				}
				else if (
					normal != nullptr &&
					metallic != nullptr &&
					roughness != nullptr
					)
				{
					ShaderManager::GetInstance()->GetShader("PBRSkinNormalMetallicRoughnessPS")->SetUpShader();
				}
				else if (
					metallic != nullptr &&
					roughness != nullptr
					)
				{
					ShaderManager::GetInstance()->GetShader("PBRSkinMetallicRoughnessPS")->SetUpShader();
				}
				else if (
					normal != nullptr
					)
				{
					ShaderManager::GetInstance()->GetShader("PBRSkinNormalPS")->SetUpShader();
				}
				else
				{
					ShaderManager::GetInstance()->GetShader("PBRSkinAlbedoPS")->SetUpShader();
				}
			}

			D3DDeviceContext->IASetPrimitiveTopology(mesh->GetPrimitiveTopology());

			UINT stride = sizeof(VertexStruct::PBRSkinnedVertex);
			UINT offset = 0;
			D3DDeviceContext->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
			D3DDeviceContext->IASetIndexBuffer(mesh->indexBuffers[matCnt].Get(), DXGI_FORMAT_R32_UINT, 0);

			// ��� ���� ����
			// ��� ���� ����
			cbMatrix cbPerObj;
			cbPerObj.worldViewProj = world * view * proj;
			cbPerObj.worldTransform = world;
			cbPerObj.worldInvTranspose = MathHelper::InverseTranspose(world);
			BufferManager::GetInstance()->GetBuffer("MatrixCB")->SetUpBuffer(0, &cbPerObj, ShaderType::VERTEX);

			// PBR Material
			cbMaterial cbPBRMat;
			cbPBRMat.metallic = material[matCnt]->metallic;
			cbPBRMat.roughness = material[matCnt]->roughness;

			cbPBRMat.AddColor.x = material[matCnt]->material_Diffuse.x;
			cbPBRMat.AddColor.y = material[matCnt]->material_Diffuse.y;
			cbPBRMat.AddColor.z = material[matCnt]->material_Diffuse.z;
			cbPBRMat.AddColor.w = 1.0f - material[matCnt]->material_Transparency;

			cbPBRMat.emissiveColor.x = material[matCnt]->material_Emissive.x;
			cbPBRMat.emissiveColor.y = material[matCnt]->material_Emissive.y;
			cbPBRMat.emissiveColor.z = material[matCnt]->material_Emissive.z;
			cbPBRMat.Offset = burnOffset;

			BufferManager::GetInstance()->GetBuffer("PBRMaterialCB")->SetUpBuffer(1, &cbPBRMat, ShaderType::VERTEX);
			BufferManager::GetInstance()->GetBuffer("PBRMaterialCB")->SetUpBuffer(0, &cbPBRMat, ShaderType::PIXEL);

			// ScreenInfo
			SimpleMath::Vector4 screenInfo = { static_cast<float>(DX11GraphicsCore::GetInstance()->GetScreenWidth()), static_cast<float>(DX11GraphicsCore::GetInstance()->GetScreenHeight()),
			0.f, 0.f };

			BufferManager::GetInstance()->GetBuffer("TextureInfoCB")->SetUpBuffer(1, &screenInfo, ShaderType::PIXEL);

			// EyePos ������...
			auto cbLightObj = dxRenderer->GetLight();
			SimpleMath::Vector4 eyePos = { cbLightObj.EyePosW.x, cbLightObj.EyePosW.y, cbLightObj.EyePosW.z, 1.0f };
			BufferManager::GetInstance()->GetBuffer("ViewPosCB")->SetUpBuffer(2, &eyePos, ShaderType::PIXEL);

			// Bone Matrix
				// => boneResources�� boneTransform�� �ִϸ��̼ǿ� ���� ��𼱰����� ����ȴٰ� ����.
				// PreRender�κ��� �� ���� �ְ�, �ش� object�� Update�ϼ��� ����.
			cbBoneMatrix cbBone;


			for (int i = 0; i < nowBoneTM.size(); i++)
			{
				cbBone.boneMatrix[i] = nowBoneTM[i];
			}

			BufferManager::GetInstance()->GetBuffer("BoneMatrixCB")->SetUpBuffer(2, &cbBone, ShaderType::VERTEX);

			// �ؽ��� ����
			if (albedo != nullptr)
				D3DDeviceContext->PSSetShaderResources(0, 1, albedo->GetMapSRV().GetAddressOf());

			if (normal != nullptr)
				D3DDeviceContext->PSSetShaderResources(1, 1, normal->GetMapSRV().GetAddressOf());

			if (metallic != nullptr)
				D3DDeviceContext->PSSetShaderResources(2, 1, metallic->GetMapSRV().GetAddressOf());

			if (roughness != nullptr)
				D3DDeviceContext->PSSetShaderResources(3, 1, roughness->GetMapSRV().GetAddressOf());

			if (ao != nullptr)
				D3DDeviceContext->PSSetShaderResources(4, 1, ao->GetMapSRV().GetAddressOf());

			if (emissive != nullptr)
				D3DDeviceContext->PSSetShaderResources(5, 1, emissive->GetMapSRV().GetAddressOf());

			D3DDeviceContext->PSSetShaderResources(6, 1, depthBuffer.GetAddressOf());

			D3DDeviceContext->PSSetSamplers(0, 1, SamplerManager::GetInstance()->GetLinearClamp().GetAddressOf());

			// ���� ������Ʈ ����.
			D3DDeviceContext->RSSetState(mesh->GetRasterState().Get());

			// �ش� ���� �׸���
			D3DDeviceContext->DrawIndexed(mesh->indexBuffersSize[matCnt], 0, 0);

			// ���̴� �ʱ�ȭ
			ID3D11ShaderResourceView* nullSRV[9] = { nullptr };

			D3DDeviceContext->PSSetShaderResources(0, 9, nullSRV);
		}
	}

	void GraphicsEngineSpace::PBRObj::SilhouetteMaskRender(const std::shared_ptr<Mesh>& mesh,
		const std::shared_ptr<RenderingMaterial>& material, const std::shared_ptr<IRenderer>& renderer, const std::string&
		objName)
	{
		// ������ ���� ���� �������� �Ƿ翧 ������ ���ݴϴ�.
			// �⺻������ ���� Ÿ�ٰ� Depth State ������ ���ݴϴ�.
		renderer->SetNullRenderTarget();
		int silhouetteIdx = mesh->silhouetteIdx;

		// ������ ���׸����� �۱� ������..
		if (objName == "tail_Material14_tailani")
			renderer->SetSilhouetteMaskDSState(1, false);
		else if (objName == "hammer_Material2_tailani")
			renderer->SetSilhouetteMaskDSState(2, true);
		else if (objName == "cock_Material10_cockani" || objName == "Plane.002_ElevatorButton")
		{
			// �� ģ���� ��Ų���� ���ݴϴ�.
			silhouetteIdx--;
			renderer->SetSilhouetteMaskDSState(3, true);
		}

		// �⺻���� �˻縦 ���ݴϴ�.
		if (mesh->indexBuffersSize.empty())
			return;

		UINT stride;
		// �Ƿ翧�� pixel shader�� �������� �ʽ��ϴ�.
		if (objName == "Plane.002_ElevatorButton")
		{
			stride = sizeof(VertexStruct::PBRStaticVertex);
			ShaderManager::GetInstance()->GetShader("PBRAlphaVS")->SetUpShader();
		}
		else
		{
			stride = sizeof(VertexStruct::PBRSkinnedVertex);
			ShaderManager::GetInstance()->GetShader("PBRSkinnedVS")->SetUpShader();

			cbBoneMatrix cbBone;

			for (int i = 0; i < nowBoneTM.size(); i++)
			{
				cbBone.boneMatrix[i] = nowBoneTM[i];
			}

			BufferManager::GetInstance()->GetBuffer("BoneMatrixCB")->SetUpBuffer(2, &cbBone, ShaderType::VERTEX);
		}

		// �ȼ� ���̴��� ���ݴϴ�.
			// ������ ���ε� �� �� ������ ���� �޽����� �߻��Ͽ����ϴ�.
		D3DDeviceContext->PSSetShader(nullptr, nullptr, 0);

		D3DDeviceContext->IASetPrimitiveTopology(mesh->GetPrimitiveTopology());

		UINT offset = 0;
		D3DDeviceContext->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
		D3DDeviceContext->IASetIndexBuffer(mesh->indexBuffers[silhouetteIdx].Get(), DXGI_FORMAT_R32_UINT, 0);

		// ��� ���� ����
		cbMatrix cbPerObj;
		cbPerObj.worldViewProj = world * view * proj;
		cbPerObj.worldTransform = world;
		cbPerObj.worldInvTranspose = MathHelper::InverseTranspose(world);
		BufferManager::GetInstance()->GetBuffer("MatrixCB")->SetUpBuffer(0, &cbPerObj, ShaderType::VERTEX);

		// PBR Material
		cbMaterial cbPBRMat;
		cbPBRMat.metallic = material->metallic;
		cbPBRMat.roughness = material->roughness;

		cbPBRMat.AddColor.x = material->material_Diffuse.x;
		cbPBRMat.AddColor.y = material->material_Diffuse.y;
		cbPBRMat.AddColor.z = material->material_Diffuse.z;
		cbPBRMat.AddColor.w = 1.0f - material->material_Transparency;

		cbPBRMat.emissiveColor.x = material->material_Emissive.x;
		cbPBRMat.emissiveColor.y = material->material_Emissive.y;
		cbPBRMat.emissiveColor.z = material->material_Emissive.z;

		cbPBRMat.Offset = burnOffset;
		cbPBRMat.isLight = false;

		BufferManager::GetInstance()->GetBuffer("PBRMaterialCB")->SetUpBuffer(1, &cbPBRMat, ShaderType::VERTEX);

		SimpleMath::Vector4 texOffset = { 0.001f, 0.f, 0.f, 0.f };
		BufferManager::GetInstance()->GetBuffer("TexOffsetCB")->SetUpBuffer(3, &texOffset, ShaderType::VERTEX);
		// ���� ������Ʈ ����.
		D3DDeviceContext->RSSetState(mesh->GetRasterState().Get());

		// �ش� ���� �׸���
		D3DDeviceContext->DrawIndexed(mesh->indexBuffersSize[silhouetteIdx], 0, 0);

		// ���󺹱� �����ݴϴ�.
		renderer->SetDeferredRenderTarget();
		renderer->SetDefaultDSState();
	}
}
