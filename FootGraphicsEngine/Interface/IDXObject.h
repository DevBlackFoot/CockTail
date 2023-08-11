#pragma once
#include "Resources/ObjectResources.h"
#include "Util/AnimatorState.h"
#include "Util/AnimatorTransition.h"

/// Dll ��ũ��
#ifdef FOOTGRAPHICSENGINE_EXPORTS
#define GRAPHICSENGINE_DECLSPEC __declspec(dllexport)
#else
#define GRAPHICSENGINE_DECLSPEC __declspec(dllimport)
#endif

namespace GraphicsEngineSpace
{
	class IRenderer;

	/**
	 * \brief Factory���� �����ϴ� ��� ������Ʈ�� ���̽��� �Ǵ� DXObj..
	 *
	 * �۵� ����� Interface�� �̾ƾ��Ѵ�.
	 * �⺻������ Device, DC. ���� ���ҽ��� ������ �־�� �Ѵ�.(�ش� ������ �� �������̽��� ��ӹ��� ������Ʈ�鿡 �־��ش�)
	 * 2022.06.21(ȭ) Dev.BlackFoot
	 */
	class GRAPHICSENGINE_DECLSPEC IDXObject
	{
	public:
		IDXObject() = default;
		virtual ~IDXObject() = default;

		// �ʱ�ȭ. Device�� DC�� �޾Ƽ�? �����غ��� �ش� ������.. ���ӿ��������� �� �ʿ䰡 ����������..
		// ����Ʈ �����ͷ� �ٲ��൵ ������ ��. 
		virtual bool Init(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext) abstract;
		// ���� ����� �޾Ƽ� ������Ʈ
		virtual void Update(const XMMATRIX& _world, const XMMATRIX& _view, const XMMATRIX& _proj) abstract;
		
		// tick�� �޾Ƽ� ������Ʈ ���ִ� PreRender
		virtual void PreRender(float tick) abstract;
		// ������ ������Ʈ�� ��� ������ �� ����̽��� �����͸� ������ �ִ� ���� ������..
		virtual void Render() abstract;
		virtual void Render(std::shared_ptr<IRenderer> renderer) abstract;
		virtual void YieldRender(std::shared_ptr<IRenderer> renderer) abstract;

		// �ļ� �κ��� �������� ��..��.. �� ��� ����..
			// �������� ���� ������Ʈ�� Device�� �־��ش�.
		// ������Ʈ�� ���� �־��ֱ�
		virtual void SetMeshResources(std::shared_ptr<MeshResources> meshResources) {}
		virtual void SetMeshResourcesFront(std::shared_ptr<MeshResources> objResources) {}
		virtual void SetBoneResource(std::shared_ptr<BoneResources> boneResources) {}
		virtual void SetAnimationResource(std::shared_ptr<AnimationResources> animResources) {}

		// ������Ʈ�� �з��� �� �ִ� �̸��� �־�����..
		virtual const std::string& GetObjName() abstract;
		virtual void SetObjName(const std::string& objName) {}
		virtual std::vector<std::shared_ptr<MeshResources>> GetMeshResources() abstract;
		virtual std::shared_ptr<BoneResources> GetBoneResources() { return nullptr; }
		//virtual std::shared_ptr<AnimationResources> GetAnimationResources() { return nullptr; }

		// ���� Matrix Getter
		virtual const SimpleMath::Matrix& GetWorld() abstract;
		virtual const SimpleMath::Matrix& GetView() abstract;
		virtual const SimpleMath::Matrix& GetProj() abstract;

		////////////////////////////////////////////////////////////////////////////////////////////////
		//										Animator ����
		////////////////////////////////////////////////////////////////////////////////////////////////
		 
		// ���� State �ִϸ��̼� ���� �����
		virtual void PlayAnim() {};

		// State �̸�, Ŭ�� �̸�, ��� �ӵ�, ���� ����
		virtual void AddAnimationState(const std::string& stateName, const std::string& animClipName, float speed, bool loop) {};

		virtual void ChangeAnimationStateSpeed(const std::string& stateName, float speed) {};

		// Transition�� �߰��� State�̸�, ���� State�̸�, �� State�̸�, hasExitTime, exitTime, Transition Duration
		virtual void SettingAnimationStateTransition(const std::string& stateName, const std::string& stateStart, const std::string& stateEnd, bool hasExitTime, float exitTime, float transitionDuration) {};
		
		// Entry�� ������ Aniamtion State
		virtual void EntryPlayAnimationState(const std::string& stateName) {}
		
		// Transition �̸�, Parameter �̸�, ���� �̸�, ��(���°� Ʈ����)
		virtual void AddCondition(const std::string& transitionName, const std::string& parameterName, const std::string& conditionName, float val) {};
		virtual void AddCondition(const std::string& transitionName, const std::string& parameterName, const std::string& conditionName, int val) {};
		virtual void AddCondition(const std::string& transitionName, const std::string& parameterName, const std::string& conditionName, bool val) {};
		virtual void AddCondition(const std::string& transitionName, const std::string& parameterName, const std::string& conditionName) {};

		virtual void AddFloat(const std::string& name, float val) {}
		virtual void AddInteger(const std::string&, int val) {}
		virtual void AddBool(const std::string&, bool val) {}
		virtual void AddTrigger(const std::string& name) {}

		virtual void SetFloat(const std::string& name, float val) {}
		virtual void SetInteger(const std::string& name, int val) {}
		virtual void SetBool(const std::string& name, bool val) {}
		virtual void SetTrigger(const std::string& name) {}

		virtual int GetAnimCurrentFrame() { return 0; }
		virtual int GetAnimCurrentTotalFrame() { return 0; }

		virtual void SetIsLight(bool val) {}
		virtual bool GetIsLight() { return true; }
		virtual std::string GetCurrentStateName() { return ""; }

		virtual void SetIsMesh(std::string meshName, bool val) {}

		virtual void SetMaterialColor(const std::string& matName, const SimpleMath::Vector4& color) {}
		virtual void SetMaterialEmissiveColor(const std::string& matName, const SimpleMath::Vector4& color) {}
		virtual void SetMaterialMetallicRoughness(const std::string& matName, float metallic, float roughness) {}
		virtual void DeleteMaterial(std::string matName) {}
		virtual void SetBurnSpeed(float burnSpeed) {}
		virtual void SetDissolveSpeed(float dissolveSpeed) {}

		virtual void SetAnimCurrentFrame(int val) {};
		virtual void SetAllMaterialAlpha(float alpha) {}
		virtual void ObjectTrailRender(std::shared_ptr<IRenderer> renderer) {}
		virtual void SetAnimationClip(std::string animName) {}

		// RimLight ����
		virtual void SetRimColor(const SimpleMath::Vector4& color) {}

		// Time ����..
		virtual void SetTotalTime(float gameTotalTime) {}

		virtual void LightOff() {};

		// ���� load ���� ��Ű��
		virtual void SetLoad(bool isLoad) {}
		virtual void SetLoadCompleted(bool isCompleted) {}
		virtual bool GetLoadCompleted() { return true; }
		virtual void SetGoalHeight(float goalHeight) {}

		virtual std::unordered_map<std::string, std::shared_ptr<AnimatorState>> GetStates() { return std::unordered_map<std::string, std::shared_ptr<AnimatorState>>(); };
	};
}
