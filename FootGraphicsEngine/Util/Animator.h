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

	// 현재 애니메이션 클립을 가지고 있고,
	// Tick을 받아 현재 프레임에 각각의 본이 가져야할 Bone TM을 계산해주는 Helper Class
	// 2022.12.22 Dev.BlackFoot
	class Animator
	{
	private:
		// 현재 AnimClip
		std::shared_ptr<AnimationClipResource> nowAnimClip;

		// 현재 키 프레임의 index정보를 가지고 있을 변수
		int nowKeyFrameIdx;

		// 무한 반복 애니메이션으로 할 것인지 여부.
		bool isLoop;

		// 재생 여부
		bool isPlay;

		// DeltaTick들을 더한 현재 프레임 타임
		float nowFrameTime;

		// DeltaTick들을 더한 현재 프레임 타임
		float animDeltaTime;

		// 밖으로 보내줄 BoneTransForm
		std::vector<SimpleMath::Matrix> modifiedBoneTM;

		// 2023-02-16 김재우 테스트중!
	public:
		std::shared_ptr<AnimationResources> animResources;	// pbrobj가 갖고있던걸 animator가 갖고있게 하였다.

	private:
		// Any는 따로뺐다.
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

		// 대입 연산자
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
		// BoneSize를 받아서 Vector를 초기화시켜 준다.
		Animator(int boneSize);
		virtual ~Animator();

		// DeltaTick을 받아 시간을 계산하면서 새로운 BoneTM을 만들어서 뽑아준다.
			// KeyFrame들을 Bone Idx별로 계산해서 보간한뒤 넣어준다..
		std::vector<SimpleMath::Matrix> UpdateBoneTM(float tick);

		std::vector<SimpleMath::Matrix> UpdateFrame(float tick);

		// Object에서 현재의 animationClip을 세팅할 수 있게 해주는 함수
			// loop 여부까지 지정
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