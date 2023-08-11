#pragma once

namespace GraphicsEngineSpace
{
	class AnimatorParmeter;

	enum class ConditionType
	{
		Greater,
		Less,
		Equals,
		NotEqual,

		None,
	};

	struct Condition
	{
		std::string parameterName;  // �Ķ���� �̸�

		ConditionType type;		// ���� Ÿ��

		float floatVal;			// ���� float ��

		int intVal;				// ���� int ��

		bool boolVal;			// ���� bool ��
	};

	class AnimatorTransition
	{
	private:
		// Transition State Name
		std::string transitionState;

		// State Start
		std::string	animationStateStart;

		// State End
		std::string	animationStateEnd;

		// ���� ������ Ȱ��ȭ �ϴ� �ɼ�
		// Ȱ��ȭ �� -> ���� ������ �����ϰ� �Ǿ� ������ ������ �����ص� ���� ���·� �������� �ʰ� �ȴ�.
		// ��Ȱ��ȭ �� -> ���� ������ �����ϴ� ��� ���̰� ���۵ȴ�.
		bool isHasExitTime;


		// Ʈ�������� ���ӽð�, ������ �Ҷ� ���δ�.
		// ���� �ִϸ��̼� Ŭ���� ���� �ִϸ��̼� Ŭ���� ����(������) �ε巴�� �̾��ִ� ������ �Ѵ�.
		float transitionDuration;

		// ���� ����Ʈ
		std::vector<Condition> conditions;
	
	public:
		// hasExitTime�� Ȱ��ȭ�ÿ� ������ �� �ִ�. (���� ����)
		float exitTime;

		float elapsedTime;

	public:
		// ���� ������
		AnimatorTransition(const AnimatorTransition& other)
			: transitionState(other.transitionState),
			animationStateStart(other.animationStateStart),
			animationStateEnd(other.animationStateEnd),
			isHasExitTime(other.isHasExitTime),
			transitionDuration(other.transitionDuration),
			conditions(other.conditions),
			exitTime(other.exitTime),
			elapsedTime(other.elapsedTime)
		{
		}

		// ���� ������
		AnimatorTransition& operator=(const AnimatorTransition& other)
		{
			if (this != &other) {
				transitionState = other.transitionState;
				animationStateStart = other.animationStateStart;
				animationStateEnd = other.animationStateEnd;
				isHasExitTime = other.isHasExitTime;
				transitionDuration = other.transitionDuration;
				conditions = other.conditions;
				exitTime = other.exitTime;
				elapsedTime = other.elapsedTime;
			}
			return *this;
		}

	public:
		AnimatorTransition();
		~AnimatorTransition();

	public:
		const std::string& GetTransitionName() { return transitionState; }

		const std::string& GetAnimationStateStartName() { return animationStateStart; }

		const std::string& GetAnimationStateEndName() { return animationStateEnd; }

		const float GetTransitionDuration() { return transitionDuration; }

		const float GetIsHasExitTime() { return isHasExitTime; }

		void SetTransitionState(std::string stateStart, std::string stateEnd);

		void SetAnimationStateStart(std::string _animationStateStart) { animationStateStart = _animationStateStart; }
	
		void SetAnimationStateEnd(std::string _animationStateEnd) { animationStateEnd = _animationStateEnd; }

		void SetHasExitTime(bool val) { isHasExitTime = val; }

		void SetExitTime(float val) { exitTime = val; }

		void SetTransitionDuration(float val) { transitionDuration = val; }

		void AddCondition(Condition& condition) { conditions.push_back(condition); }

		bool CheckCondition(std::shared_ptr<AnimatorParmeter>& parameter);

		bool CheckExitTime() { return elapsedTime >= exitTime; }
	};
}
