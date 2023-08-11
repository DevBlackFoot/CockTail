#pragma once
#include "Scripts/Trigger.h"

namespace ClientSpace
{
	class GameEngineSpace::InputManager;

	class InteractionTrigger : public Monobehavior
	{
	protected:
		bool isInteracted;
		bool isSelected;
		bool isPossible = true;
		std::shared_ptr<Collider> collider;
		std::shared_ptr<Canvas> mainCanvas;
		std::shared_ptr<InputManager> input;

	public:
		InteractionTrigger(std::weak_ptr<GameObject> _obj);
		virtual ~InteractionTrigger();

		virtual void Awake();
		virtual void Update(float tick) override;
		virtual void OnTriggerEnter(const std::shared_ptr<MoolyEngine::Trigger>& trigger) override;
		virtual void OnTriggerStay(const std::shared_ptr<MoolyEngine::Trigger>& trigger) override;
		virtual void OnTriggerExit(const std::shared_ptr<MoolyEngine::Trigger>& trigger) override;

		bool IsInteracted() { return isInteracted; }
		bool IsSelected() { return isSelected; }

		void InputUpdate();
	};
}