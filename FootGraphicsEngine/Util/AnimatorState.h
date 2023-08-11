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

		// 애니메이션 클립 이름
		std::string motion;		

		// 애니메이션 재생 속도
		float speed;

		// Loop 여부
		bool isLoop;
		
		// 해당 State에 연결된 transition 들
		std::vector<std::shared_ptr<AnimatorTransition>> transitions;
	
	public:
		// 현재 애니메이션의 재생 시간 (0 ~ 1.0f) 재생이 얼마나 진행되었는지..
		// UpdateFrame 에서 구해서 넣어준다.
		float normalizedTime;

	public:
		// CheckCondition이 True가 된 transition
		std::shared_ptr<AnimatorTransition> operateTransition;

	public:
		// AnimatorState 복사 생성자
		AnimatorState(const AnimatorState& other)
		{
			this->stateName = other.stateName;
			this->motion = other.motion;
			this->speed = other.speed;
			this->isLoop = other.isLoop;
			this->normalizedTime = other.normalizedTime;
			
			// transitions 복사
			for (const auto& transition : other.transitions)
			{
				transitions.push_back(std::make_shared<AnimatorTransition>(*transition));
			}

			// operateTransition 복사
			if (other.operateTransition != nullptr)
			{
				operateTransition = std::make_shared<AnimatorTransition>(*other.operateTransition);
			}
		}

		// AnimatorState 대입 연산자
		AnimatorState& operator=(const AnimatorState& other)
		{
			this->stateName = other.stateName;
			this->motion = other.motion;
			this->speed = other.speed;
			this->isLoop = other.isLoop;
			this->normalizedTime = other.normalizedTime;

			// transitions 복사
			for (const auto& transition : other.transitions)
			{
				transitions.push_back(std::make_shared<AnimatorTransition>(*transition));
			}

			// operateTransition 복사
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
