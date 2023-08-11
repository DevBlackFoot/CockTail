#pragma once
#include "InteractionTrigger.h"

namespace GameEngineSpace
{
	class InputManager;
}

namespace ClientSpace
{

	class OpenStoreTrigger : public InteractionTrigger
	{
		// 한번만 켜주면 되기 때문에.
		bool onCanvas;
		bool onPopup;
		std::shared_ptr<Canvas> storeCanvas;
		std::shared_ptr<Canvas> popupCanvas;

		// 칵을 변경시키기 위해
		std::shared_ptr<IDXObject> cockRenderer;
		std::shared_ptr<GameManager> gameManager;
		SimpleMath::Vector4 rim;
		float alpha;
		float dissolveSpeed;

	public :
		OpenStoreTrigger(std::weak_ptr<GameObject> _obj);
		virtual ~OpenStoreTrigger();

		virtual void Awake() override;
		virtual void Update(float tick) override;
		virtual void LateUpdate(float tick) override;

		virtual void DebugIMGUIRender(float tick) override;

		bool GetOnPopUp() { return onPopup; }
		bool GetOnCanvas() { return onCanvas; }

	private:
		void SetButtonEvent();
	};

}
