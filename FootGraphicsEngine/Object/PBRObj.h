
#pragma once
#include "Interface/IDXObject.h"
#include "Interface/IRenderer.h"
#include "Resources/ObjectResources.h"
#include "Util/Animator.h"

namespace GraphicsEngineSpace
{
	class ResourceManager;
	//class IRenderer;

	// 모든 객체가 가지고 있지 않아도 되는 bone struct는 여기로 빼준다
		// Skinned mesh만 가지고 있으면 되기 때문
	__declspec(align(16)) struct cbBoneMatrix
	{
		SimpleMath::Matrix boneMatrix[180];
	};

	/**
	 * \brief PBR모델을 화면에 그리는 PBR Obj
	 */
	class PBRObj : public IDXObject
	{
		// 멤버 변수 목록
		ComPtr<ID3D11Device> D3DDevice;
		ComPtr<ID3D11DeviceContext> D3DDeviceContext;

		// 각종 정보가 포함된 리소스
			// Mesh가 여러개 존재할 수 있다.
		std::vector<std::shared_ptr<MeshResources>> meshResources;
		std::shared_ptr<BoneResources> boneResources;

		// 투명한 mesh들을 그릴 수 있게 만드는 vector
			// 투명한 mesh의 정보와, 해당 material Index를 가지고 있는 vector
		std::vector<std::pair<std::shared_ptr<MeshResources>, int>> transparencyMesh;
		//std::shared_ptr<AnimationResources> animResources;

		// ResourceManager 캐싱 용도
		std::shared_ptr<ResourceManager> resourceManager;

		std::unique_ptr<Animator> objAnimator;

		std::vector<SimpleMath::Matrix> nowBoneTM;

		bool isLight = true;

		// 라이팅 관련 각종 변수들 => Deferred를 위해 라이팅 Pass로 옮긴다.

		// TM 행렬
		SimpleMath::Matrix world;
		SimpleMath::Matrix view;
		SimpleMath::Matrix proj;

		// 쉐이더에 전달할 매트릭스
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

		// 림라이트 관련 색상 변수
		SimpleMath::Vector4 rimColor;

		float burnOffset;
		float burnSpeed;

		// 플레이어 구분용 obj Name 함수 추가.
		std::string objectName;

		// 몬스터 로딩용 정보들.
		float loadOffset;
		// 몬스터의 local 위치의 최종값을 정하는 goalOffset
		float goalOffset;
		// edge 디졸브 관련 속도
		float dissolveSpeed;
		// 터렛의 on.
		float onOffset;
		// 로드 할 것인가 => Start 지점
		bool isLoad;
		// 로드가 완료 되었는가. => 완료 지점.
		bool isLoadCompleted;

	public:
		// 복사 생성자
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

		// 대입 연산자
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

		// deferred pass로 옮길 것이기 때문에 내부적으로 구현을 하지 않음
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
		// 실루엣 마스킹 렌더..
		void SilhouetteMaskRender(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<RenderingMaterial>& material, const std::shared_ptr<IRenderer>
		                          & renderer, const std::string& objName);
	};
}