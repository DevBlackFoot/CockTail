#include "GraphicsPch.h"
#include "AnimatorTransition.h"
#include "AnimatorParmeter.h"

namespace GraphicsEngineSpace
{
	AnimatorTransition::AnimatorTransition()
		: transitionState("")
		, animationStateStart("")
		, animationStateEnd("")
		, isHasExitTime(false)
		, exitTime(0.f)
		, elapsedTime(0.f)
		, transitionDuration(0.f)
	{}

	AnimatorTransition::~AnimatorTransition()
	{}

	void AnimatorTransition::SetTransitionState(std::string stateStart, std::string stateEnd)
	{
		transitionState = stateStart + "->" + stateEnd;
	}


	/// <summary>
	/// 전이는 하나라도 컨디션이 만족하면 이루어져야 한다.
	/// </summary>
	/// <param name="parameter"></param>
	/// <returns></returns>
	bool AnimatorTransition::CheckCondition(std::shared_ptr<AnimatorParmeter>& parameter)
	{
		for (const auto& condition : conditions)
		{
			auto& param = parameter->parameters[condition.parameterName];

			switch (param.parameterType)
			{
			case ParameterType::Int:
			{
				if (condition.type == ConditionType::Equals && param.intValue == condition.intVal)
					return true;
				else if (condition.type == ConditionType::NotEqual && param.intValue != condition.intVal)
					return true;
				else if (condition.type == ConditionType::Greater && param.intValue > condition.intVal)
					return true;
				else if (condition.type == ConditionType::Less && param.intValue < condition.intVal)
					return true;
				else
					continue;
			}
			break;
			case ParameterType::Float:
			{
				if (condition.type == ConditionType::Greater && param.floatValue > condition.floatVal)
					return true;
				else if (condition.type == ConditionType::Less && param.floatValue < condition.floatVal)
					return true;
				else
					continue;
			}
			break;
			case ParameterType::Bool:
			{
				if (param.boolValue == condition.boolVal)
					return true;
				else
					continue;
			}
			break;
			case ParameterType::Trigger:
			{
				if (param.isTrigger)
				{
					param.isTrigger = false;

					for (auto& [key, val] : parameter->parameters)
					{
						if (val.parameterType == ParameterType::Trigger)
						{
							val.isTrigger = false;
						}
					}

					return true;
				}
				else
					continue;
			}
			break;
			default:
				break;
			}
		}

		// 다 만족하지 않는 경우 false
		return false;
	}
}