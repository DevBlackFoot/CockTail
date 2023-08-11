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

	// ������ �����ϴ� UI�Դϴ�.
	class StoreController : public ComponentBase
	{
	private:
		// ���� ������ ���� �ִ����� Ȯ���ؼ� ������ ���� ���� �����մϴ�.
		// �⺻������ GameManager�� ������ �������� �����մϴ�.
		std::shared_ptr<Canvas> storeCanvas;
		std::shared_ptr<GameManager> gameManager;
		std::shared_ptr<GameEngineSpace::SoundManager> sound;

		// ���¸� �޾ƿ� Ʈ����..
		std::shared_ptr<OpenStoreTrigger> openTrigger;
		std::shared_ptr<PlayerController> playerController;

		// ��������Ʈ ��
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

		// Ŭ���� ���ο��� ����� ��ϵ� ĳ��.
		void Init();

		void Update(float tick) override;

	private:
		void SetButtonClickEvent();
		void LevelUp(int intCase);
	};

}
