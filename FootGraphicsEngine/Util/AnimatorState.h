#pragma once

namespace GraphicsEngineSpace
{
	class AnimatorTransition;
	class AnimatorParmeter;
	class Animator;

	class AnimatorState
	{
	private:
		std::string stateName;

		// �ִϸ��̼� Ŭ�� �̸�
		std::string motion;		

		// �ִϸ��̼� ��� �ӵ�
		float speed;

		// Loop ����
		bool isLoop;
		
		// �ش� State�� ����� transition ��
		std::vector<std::shared_ptr<AnimatorTransition>> transitions;
	
	public:
		// ���� �ִϸ��̼��� ��� �ð� (0 ~ 1.0f) ����� �󸶳� ����Ǿ�����..
		// UpdateFrame ���� ���ؼ� �־��ش�.
		float normalizedTime;

	public:
		// CheckCondition�� True�� �� transition
		std::shared_ptr<AnimatorTransition> operateTransition;

	public:
		// AnimatorState ���� ������
		AnimatorState(const AnimatorState& other)
		{
			this->stateName = other.stateName;
			this->motion = other.motion;
			this->speed = other.speed;
			this->isLoop = other.isLoop;
			this->normalizedTime = other.normalizedTime;
			
			// transitions ����
			for (const auto& transition : other.transitions)
			{
				transitions.push_back(std::make_shared<AnimatorTransition>(*transition));
			}

			// operateTransition ����
			if (other.operateTransition != nullptr)
			{
				operateTransition = std::make_shared<AnimatorTransition>(*other.operateTransition);
			}
		}

		// AnimatorState ���� ������
		AnimatorState& operator=(const AnimatorState& other)
		{
			this->stateName = other.stateName;
			this->motion = other.motion;
			this->speed = other.speed;
			this->isLoop = other.isLoop;
			this->normalizedTime = other.normalizedTime;

			// transitions ����
			for (const auto& transition : other.transitions)
			{
				transitions.push_back(std::make_shared<AnimatorTransition>(*transition));
			}

			// operateTransition ����
			if (other.operateTransition != nullptr)
			{
				operateTransition = std::make_shared<AnimatorTransition>(*other.operateTransition);
			}

			return *this;
		}

	public:
		AnimatorState();
		~AnimatorState();

	public:
		const std::string& GetStateName() { return stateName; }

		bool GetLoop() { return isLoop; }

		const std::string& GetMotion() { return motion; }

		float GetSpeed() { return speed; }

		void SetStateName(std::string _stateName) { stateName = _stateName; }

		void SetMotion(std::string _motion) { motion = _motion; }

		void SetSpeed(float _speed) { speed = _speed; }

		void SetLoop(bool val) { isLoop = val; }

		std::shared_ptr<AnimatorTransition> GetTransition(const std::string& name);

		void AddTransition(std::shared_ptr<AnimatorTransition> transition) { transitions.push_back(transition); }

		std::string UpdateState(std::shared_ptr<AnimatorParmeter> parameter, Animator* animator);
	};
}
