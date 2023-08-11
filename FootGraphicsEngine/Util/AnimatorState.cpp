#include "GraphicsPch.h"
#include "AnimatorState.h"
#include "AnimatorTransition.h"
#include "AnimatorParmeter.h"
#include "Animator.h"

namespace GraphicsEngineSpace
{
	AnimatorState::AnimatorState()
		: motion("")
		, stateName("")
		, speed(1.0f)
		, isLoop(false)
		, normalizedTime(0.f)
	{}

	AnimatorState::~AnimatorState()
	{}

	std::shared_ptr<AnimatorTransition> AnimatorState::GetTransition(const std::string& name)
	{
		auto it = find_if(transitions.begin(), transitions.end(), [&name](const std::shared_ptr<AnimatorTransition>& a)
			{ return (a->GetTransitionName() == name); });

		return *it;
	}

	std::string AnimatorState::UpdateState(std::shared_ptr<AnimatorParmeter> parameter, Animator* animator)
	{
		for (const auto& transition : transitions)
		{
			// exitTime 검사
			if (transition->GetIsHasExitTime() && normalizedTime < transition->exitTime)
			{
				continue;
			}
			// 전이 조건 검사
			if (transition->CheckCondition(parameter))
			{
				operateTransition = transition;
				
				animator->nowKeyFrameIdx = 0;

				animator->currentTransitionTime = 0.0f;

				// 블렌딩 후 다음 애니메이션 바로 return 방지
				animator->nowFrameTime = 100.0f;

				return transition->GetAnimationStateEndName(); // 다음 상태 반환
			}
		}

		return stateName;
	}
}