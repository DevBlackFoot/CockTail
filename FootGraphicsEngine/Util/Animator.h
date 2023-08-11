#pragma once
#include "Resources/ObjectResources.h"
#include "Util/AnimatorState.h"
#include "Util/AnimatorParmeter.h"
#include "Util/AnimatorTransition.h"

namespace GraphicsEngineSpace
{
	class KeyFrame;
	class AnimationClipResource;
	struct Condition;

	// ���� �ִϸ��̼� Ŭ���� ������ �ְ�,
	// Tick�� �޾� ���� �����ӿ� ������ ���� �������� Bone TM�� ������ִ� Helper Class
	// 2022.12.22 Dev.BlackFoot
	class Animator
	{
	private:
		// ���� AnimClip
		std::shared_ptr<AnimationClipResource> nowAnimClip;

		// ���� Ű �������� index������ ������ ���� ����
		int nowKeyFrameIdx;

		// ���� �ݺ� �ִϸ��̼����� �� ������ ����.
		bool isLoop;

		// ��� ����
		bool isPlay;

		// DeltaTick���� ���� ���� ������ Ÿ��
		float nowFrameTime;

		// DeltaTick���� ���� ���� ������ Ÿ��
		float animDeltaTime;

		// ������ ������ BoneTransForm
		std::vector<SimpleMath::Matrix> modifiedBoneTM;

		// 2023-02-16 ����� �׽�Ʈ��!
	public:
		std::shared_ptr<AnimationResources> animResources;	// pbrobj�� �����ִ��� animator�� �����ְ� �Ͽ���.

	private:
		// Any�� ���λ���.
		std::unordered_map<std::string, std::shared_ptr<AnimatorState>> states;

		std::shared_ptr<AnimatorParmeter> animatorParameter;

		std::shared_ptr<AnimatorState> currentAnimatorState;

		std::shared_ptr<AnimatorState> anyAnimatorState;

		std::string currentStateName;
		
		float currentTransitionTime = 0.0f;

		float currentTransitionDuration = 0.0f;

		std::vector<SimpleMath::Matrix> blendBoneTM;

	public:
		Animator(const Animator& other)
			: nowAnimClip(other.nowAnimClip),
			nowKeyFrameIdx(other.nowKeyFrameIdx),
			isLoop(other.isLoop),
			isPlay(other.isPlay),
			nowFrameTime(other.nowFrameTime),
			modifiedBoneTM(other.modifiedBoneTM),
			states(other.states),
			animatorParameter(std::make_shared<AnimatorParmeter>(*other.animatorParameter)),
			currentAnimatorState(other.currentAnimatorState),
			anyAnimatorState(std::make_shared<AnimatorState>(*other.anyAnimatorState)),
			currentStateName(other.currentStateName),
			currentTransitionTime(other.currentTransitionTime),
			currentTransitionDuration(other.currentTransitionDuration),
			blendBoneTM(other.blendBoneTM)
		{
			animResources = std::make_shared<AnimationResources>(*other.animResources);
		}

		// ���� ������
		Animator& operator=(const Animator& other)
		{
			if (this == &other) return *this;

			nowAnimClip = other.nowAnimClip;
			nowKeyFrameIdx = other.nowKeyFrameIdx;
			isLoop = other.isLoop;
			isPlay = other.isPlay;
			nowFrameTime = other.nowFrameTime;
			modifiedBoneTM = other.modifiedBoneTM;
			states = other.states;
			animatorParameter = std::make_shared<AnimatorParmeter>(*other.animatorParameter);
			currentAnimatorState = other.currentAnimatorState;
			anyAnimatorState = std::make_shared<AnimatorState>(*other.anyAnimatorState);
			currentStateName = other.currentStateName;
			currentTransitionTime = other.currentTransitionTime;
			currentTransitionDuration = other.currentTransitionDuration;
			blendBoneTM = other.blendBoneTM;

			animResources = std::make_shared<AnimationResources>(*other.animResources);

			return *this;
		}

	public:
		// BoneSize�� �޾Ƽ� Vector�� �ʱ�ȭ���� �ش�.
		Animator(int boneSize);
		virtual ~Animator();

		// DeltaTick�� �޾� �ð��� ����ϸ鼭 ���ο� BoneTM�� ���� �̾��ش�.
			// KeyFrame���� Bone Idx���� ����ؼ� �����ѵ� �־��ش�..
		std::vector<SimpleMath::Matrix> UpdateBoneTM(float tick);

		std::vector<SimpleMath::Matrix> UpdateFrame(float tick);

		// Object���� ������ animationClip�� ������ �� �ְ� ���ִ� �Լ�
			// loop ���α��� ����
		void PlayAnim();
		void SetAnimationClip(std::string animName);
		
		void SetNowFrame(int val);
		int GetNowFrame() { return nowKeyFrameIdx; }

		int GetTotalFrame();

		std::string GetCurrentAnimatorStateName();

		std::unordered_map<std::string, std::shared_ptr<AnimatorState>> GetAnimatorStates() { return states; }
		
		std::shared_ptr<AnimatorState> GetAnyState() { return anyAnimatorState; }

		void SetAnimatorState(const std::string& stateName, std::shared_ptr<AnimatorState> animatorState);

		void ChangeAnimatorStateSpeed(const std::string& stateName, float speed);

		void EntryAnimatorState(const std::string& name);

		void AddCondition(const std::string& transitionName, const std::string& parameterName, const std::string& conditionName, float val);
		void AddCondition(const std::string& transitionName, const std::string& parameterName, const std::string& conditionName, int val);
		void AddCondition(const std::string& transitionName, const std::string& parameterName, const std::string& conditionName, bool val);
		void AddCondition(const std::string& transitionName, const std::string& parameterName, const std::string& conditionName);

		void AddFloat(const std::string& name, float val);
		void AddInteger(const std::string& name, int val);
		void AddBool(const std::string& name, bool val);
		void AddTrigger(const std::string& name);

		void SetFloat(const std::string& name, float val);
		void SetInteger(const std::string& name, int val);
		void SetBool(const std::string& name, bool val);
		void SetTrigger(const std::string& name);

		friend class AnimatorState;
	};
}