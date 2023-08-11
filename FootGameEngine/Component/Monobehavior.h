#pragma once
#include "Object/ComponentBase.h"
#include "Define/FootEngineTypedef.h"
#include "PhysicsUserData.h"

namespace GameEngineSpace
{
	class GameObject;

	class Monobehavior : public ComponentBase
	{
	public:
		Monobehavior(std::weak_ptr<GameObject> _gameObject);

		virtual ~Monobehavior();

		virtual void Release() override;

		virtual void OnCollisionEnter(const std::shared_ptr<MoolyEngine::Collision>& collision) {}

		virtual void OnCollisionStay(const std::shared_ptr<MoolyEngine::Collision>& collision) {}

		virtual void OnCollisionExit(const std::shared_ptr<MoolyEngine::Collision>& collision) {}

		virtual void OnTriggerEnter(const std::shared_ptr<MoolyEngine::Trigger>& trigger) {}

		virtual void OnTriggerStay(const std::shared_ptr<MoolyEngine::Trigger>& trigger) {}

		virtual void OnTriggerExit(const std::shared_ptr<MoolyEngine::Trigger>& trigger) {}

		void RegisterCallBack();
	};
}