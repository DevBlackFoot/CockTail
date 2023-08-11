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
			// exitTime �˻�
			if (transition->GetIsHasExitTime() && normalizedTime < transition->exitTime)
			{
				continue;
			}
			// ���� ���� �˻�
			if (transition->CheckCondition(parameter))
			{
				operateTransition = transition;
				
				animator->nowKeyFrameIdx = 0;

				animator->currentTransitionTime = 0.0f;

				// ���� �� ���� �ִϸ��̼� �ٷ� return ����
				animator->nowFrameTime = 100.0f;

				return transition->GetAnimationStateEndName(); // ���� ���� ��ȯ
			}
		}

		return stateName;
	}
}