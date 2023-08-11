#pragma once

namespace GameEngineSpace
{
	class SoundManager;
}

namespace ClientSpace
{
	class GameManager;
	class OpenStoreTrigger;
	class PlayerController;

	// 상점을 관리하는 UI입니다.
	class StoreController : public ComponentBase
	{
	private:
		// 지금 상점에 들어와 있는지를 확인해서 상점에 있을 때만 동작합니다.
		// 기본적으로 GameManager의 내용을 바탕으로 동작합니다.
		std::shared_ptr<Canvas> storeCanvas;
		std::shared_ptr<GameManager> gameManager;
		std::shared_ptr<GameEngineSpace::SoundManager> sound;

		// 상태를 받아올 트리거..
		std::shared_ptr<OpenStoreTrigger> openTrigger;
		std::shared_ptr<PlayerController> playerController;

		// 스프라이트 들
		std::vector<std::shared_ptr<SpriteUI>> powerSprite;
		std::vector<std::shared_ptr<SpriteUI>> healthSprite;
		std::vector<std::shared_ptr<SpriteUI>> speedSprite;
		std::shared_ptr<ButtonUI> powerButton;
		std::shared_ptr<ButtonUI> healthButton;
		std::shared_ptr<ButtonUI> speedButton;
		std::shared_ptr<TextUI> costText;
		std::shared_ptr<TextUI> moneyText;

		int nowPrice;
		int nowMoney;

	public:
		StoreController(std::weak_ptr<GameObject> _object);
		virtual ~StoreController();

		// 클래스 내부에서 사용할 목록들 캐싱.
		void Init();

		void Update(float tick) override;

	private:
		void SetButtonClickEvent();
		void LevelUp(int intCase);
	};

}
