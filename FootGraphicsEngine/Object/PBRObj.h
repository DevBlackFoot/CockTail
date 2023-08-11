
#pragma once
#include "Interface/IDXObject.h"
#include "Interface/IRenderer.h"
#include "Resources/ObjectResources.h"
#include "Util/Animator.h"

namespace GraphicsEngineSpace
{
	class ResourceManager;
	//class IRenderer;

	// ��� ��ü�� ������ ���� �ʾƵ� �Ǵ� bone struct�� ����� ���ش�
		// Skinned mesh�� ������ ������ �Ǳ� ����
	__declspec(align(16)) struct cbBoneMatrix
	{
		SimpleMath::Matrix boneMatrix[180];
	};

	/**
	 * \brief PBR���� ȭ�鿡 �׸��� PBR Obj
	 */
	class PBRObj : public IDXObject
	{
		// ��� ���� ���
		ComPtr<ID3D11Device> D3DDevice;
		ComPtr<ID3D11DeviceContext> D3DDeviceContext;

		// ���� ������ ���Ե� ���ҽ�
			// Mesh�� ������ ������ �� �ִ�.
		std::vector<std::shared_ptr<MeshResources>> meshResources;
		std::shared_ptr<BoneResources> boneResources;

		// ������ mesh���� �׸� �� �ְ� ����� vector
			// ������ mesh�� ������, �ش� material Index�� ������ �ִ� vector
		std::vector<std::pair<std::shared_ptr<MeshResources>, int>> transparencyMesh;
		//std::shared_ptr<AnimationResources> animResources;

		// ResourceManager ĳ�� �뵵
		std::shared_ptr<ResourceManager> resourceManager;

		std::unique_ptr<Animator> objAnimator;

		std::vector<SimpleMath::Matrix> nowBoneTM;

		bool isLight = true;

		// ������ ���� ���� ������ => Deferred�� ���� ������ Pass�� �ű��.

		// TM ���
		SimpleMath::Matrix world;
		SimpleMath::Matrix view;
		SimpleMath::Matrix proj;

		// ���̴��� ������ ��Ʈ����
		__declspec(align(16)) struct cbMatrix
		{
			SimpleMath::Matrix worldViewProj;
			SimpleMath::Matrix worldTransform;
			SimpleMath::Matrix worldInvTranspose;
		};

		// PBR Material;
		__declspec(align(16)) struct cbMaterial
		{
			float metallic;
			float roughness;
			SimpleMath::Vector2 pad1;

			SimpleMath::Vector4 AddColor;

			SimpleMath::Vector3 emissiveColor;
			float Offset;

			BOOL isLight = true;
		};

		// ������Ʈ ���� ���� ����
		SimpleMath::Vector4 rimColor;

		float burnOffset;
		float burnSpeed;

		// �÷��̾� ���п� obj Name �Լ� �߰�.
		std::string objectName;

		// ���� �ε��� ������.
		float loadOffset;
		// ������ local ��ġ�� �������� ���ϴ� goalOffset
		float goalOffset;
		// edge ������ ���� �ӵ�
		float dissolveSpeed;
		// �ͷ��� on.
		float onOffset;
		// �ε� �� ���ΰ� => Start ����
		bool isLoad;
		// �ε尡 �Ϸ� �Ǿ��°�. => �Ϸ� ����.
		bool isLoadCompleted;

	public:
		// ���� ������
		PBRObj(const PBRObj& other)
		{
			for (const auto& meshResource : other.meshResources)
			{
				meshResources.push_back(std::make_shared<MeshResources>(*meshResource));
			}

			if (other.boneResources)
				boneResources = std::make_shared<BoneResources>(*other.boneResources);
			else
				boneResources = nullptr;

			transparencyMesh.reserve(other.transparencyMesh.size());
			for (const auto& meshPair : other.transparencyMesh)
			{
				std::shared_ptr<MeshResources> newMesh = std::make_shared<MeshResources>(*meshPair.first);
				transparencyMesh.emplace_back(std::make_pair(newMesh, meshPair.second));
			}

			resourceManager = other.resourceManager;

			if (other.objAnimator)
				objAnimator = std::make_unique<Animator>(*other.objAnimator);
			else
				objAnimator = nullptr;

			rimColor = other.rimColor;
			nowBoneTM = other.nowBoneTM;
			isLight = other.isLight;
			world = other.world;
			view = other.view;
			proj = other.proj;
			D3DDevice = other.D3DDevice;
			D3DDeviceContext = other.D3DDeviceContext;

			burnOffset = other.burnOffset;
			burnSpeed = other.burnSpeed;

			loadOffset = other.loadOffset;
			goalOffset = other.goalOffset;
			onOffset = other.onOffset;
			dissolveSpeed = other.dissolveSpeed;
			isLoad = other.isLoad;
			isLoadCompleted = other.isLoadCompleted;

			objectName.assign(other.objectName.begin(), other.objectName.end());
		}

		// ���� ������
		PBRObj& operator=(const PBRObj& other)
		{
			for (const auto& meshResource : other.meshResources)
			{
				meshResources.push_back(std::make_shared<MeshResources>(*meshResource));
			}

			if (other.boneResources)
				boneResources = std::make_shared<BoneResources>(*other.boneResources);
			else
				boneResources = nullptr;

			transparencyMesh.reserve(other.transparencyMesh.size());
			for (const auto& meshPair : other.transparencyMesh)
			{
				std::shared_ptr<MeshResources> newMesh = std::make_shared<MeshResources>(*meshPair.first);
				transparencyMesh.emplace_back(std::make_pair(newMesh, meshPair.second));
			}
			resourceManager = other.resourceManager;

			if (other.objAnimator)
				objAnimator = std::make_unique<Animator>(*other.objAnimator);
			else
				objAnimator = nullptr;

			rimColor = other.rimColor;
			nowBoneTM = other.nowBoneTM;
			isLight = other.isLight;
			world = other.world;
			view = other.view;
			proj = other.proj;
			D3DDevice = other.D3DDevice;
			D3DDeviceContext = other.D3DDeviceContext;

			burnOffset = other.burnOffset;
			burnSpeed = other.burnSpeed;

			loadOffset = other.loadOffset;
			goalOffset = other.goalOffset;
			onOffset = other.onOffset;
			dissolveSpeed = other.dissolveSpeed;
			isLoad = other.isLoad;
			isLoadCompleted = other.isLoadCompleted;

			objectName.assign(other.objectName.begin(), other.objectName.end());

			return *this;
		}

	public:
		GRAPHICSENGINE_DECLSPEC PBRObj();
		~PBRObj() override;

		virtual bool Init(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext) override;
		virtual void Update(const XMMATRIX& _world, const XMMATRIX& _view, const XMMATRIX& _proj) override;

		virtual void PlayAnim() override;

		// deferred pass�� �ű� ���̱� ������ ���������� ������ ���� ����
		virtual void PreRender(float tick) override;
		virtual void Render() override;
		virtual void Render(std::shared_ptr<IRenderer> renderer) override;
		virtual void YieldRender(std::shared_ptr<IRenderer> renderer) override;
		virtual void ObjectTrailRender(std::shared_ptr<IRenderer> renderer) override;
		virtual void SetMeshResources(std::shared_ptr<MeshResources> objResources) override;
		virtual void SetMeshResourcesFront(std::shared_ptr<MeshResources> objResources) override;
		virtual void SetBoneResource(std::shared_ptr<BoneResources> boneResources) override;
		virtual void SetAnimationResource(std::shared_ptr<AnimationResources> animResources) override;

		virtual const std::string& GetObjName() override;
		virtual void SetObjName(const std::string& objName) override { this->objectName = objName; }
		virtual std::vector<std::shared_ptr<MeshResources>> GetMeshResources() override;
		//virtual std::shared_ptr<AnimationResources> GetAnimationResources() override { return objAnimator->animResources; }

		virtual void SetAnimationClip(std::string animName) override;

		virtual void AddAnimationState(const std::string& stateName, const std::string& animClipName, float speed, bool loop) override;
		virtual void ChangeAnimationStateSpeed(const std::string& stateName, float speed) override;
		virtual void SettingAnimationStateTransition(const std::string& stateName, const std::string& stateStart, const std::string& stateEnd, bool hasExitTime, float exitTime, float transitionDuration) override;
		virtual void EntryPlayAnimationState(const std::string& stateName) override;
		virtual void AddCondition(const std::string& transitionName, const std::string& parameterName, const std::string& conditionName, float val) override;
		virtual void AddCondition(const std::string& transitionName, const std::string& parameterName, const std::string& conditionName, int val) override;
		virtual void AddCondition(const std::string& transitionName, const std::string& parameterName, const std::string& conditionName, bool val) override;
		virtual void AddCondition(const std::string& transitionName, const std::string& parameterName, const std::string& conditionName) override;

		virtual void AddFloat(const std::string& name, float val) override;
		virtual void AddInteger(const std::string& name, int val) override;
		virtual void AddBool(const std::string& name, bool val) override;
		virtual void AddTrigger(const std::string& name) override;

		virtual void SetFloat(const std::string& name, float val) override;
		virtual void SetInteger(const std::string& name, int val) override;
		virtual void SetBool(const std::string& name, bool val) override;
		virtual void SetTrigger(const std::string& name) override;

		virtual void SetAnimCurrentFrame(int val) override;

		virtual int GetAnimCurrentFrame() override;
		virtual int GetAnimCurrentTotalFrame() override;

		virtual const SimpleMath::Matrix& GetWorld() override { return world; }
		virtual const SimpleMath::Matrix& GetView() override { return view; }
		virtual const SimpleMath::Matrix& GetProj() override { return proj; }

		const std::vector<SimpleMath::Matrix>& GetNowBoneTM() { return nowBoneTM; }

		virtual void SetIsLight(bool val) override { isLight = val; }
		virtual bool GetIsLight() override { return isLight; }

		std::string GetCurrentStateName() override;

		virtual void SetIsMesh(std::string meshName, bool val) override;

		virtual void SetMaterialColor(const std::string& matName, const SimpleMath::Vector4& color) override;
		virtual void SetMaterialEmissiveColor(const std::string& matName, const SimpleMath::Vector4& color) override;
		virtual void SetMaterialMetallicRoughness(const std::string& matName, float metallic, float roughness) override;
		virtual void SetAllMaterialAlpha(float alpha) override;
		virtual void DeleteMaterial(std::string matName) override;
		virtual void SetBurnSpeed(float burnSpeed) override { this->burnSpeed = burnSpeed; }
		virtual void SetDissolveSpeed(float dissolveSpeed) override;

		virtual void SetRimColor(const SimpleMath::Vector4& color) override;

		virtual void LightOff() override;

		virtual void SetLoad(bool isLoad) override { this->isLoad = isLoad; }
		virtual void SetLoadCompleted(bool isCompleted) override { this->isLoadCompleted = isCompleted; }
		virtual bool GetLoadCompleted() override { return isLoadCompleted; }
		virtual void SetGoalHeight(float goalHeight) override { this->goalOffset = goalHeight; }

		virtual std::unordered_map<std::string, std::shared_ptr<AnimatorState>> GetStates() override;

	private:
		void StaticRender(const std::shared_ptr<MeshResources>& objRes, const std::shared_ptr<IRenderer>& renderer);
		void SkinnedRender(const std::shared_ptr<MeshResources>& objRes, const std::shared_ptr<IRenderer>& renderer);
		void YieldRender(const std::pair<std::shared_ptr<MeshResources>, int>& transObjRes, const std::shared_ptr<IRenderer>& renderer);
		void TrailRender(const std::shared_ptr<MeshResources>& objRes, const std::shared_ptr<IRenderer>& renderer);
		// �Ƿ翧 ����ŷ ����..
		void SilhouetteMaskRender(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<RenderingMaterial>& material, const std::shared_ptr<IRenderer>
		                          & renderer, const std::string& objName);
	};
}