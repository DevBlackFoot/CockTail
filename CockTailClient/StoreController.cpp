#include "pch.h"
#include "StoreController.h"

#include "UIManager.h"
#include "OpenStoreTrigger.h"
#include "GameManager.h"
#include "Scripts/PlayerController.h"

namespace ClientSpace
{
	StoreController::StoreController(std::weak_ptr<GameObject> _object)
		: ComponentBase(_object)
		, nowPrice(0)
		, nowMoney(0)
	{
	}

	StoreController::~StoreController()
	{
	}

	void StoreController::Init()
	{
		storeCanvas = UIManager::GetInstance()->GetCanvas("StoreCanvas");
		gameManager = GameManager::GetInstance();
		// ���� ������Ʈ�� �پ��ִ� ������Ʈ ĳ��
		openTrigger = GetGameObject()->GetComponent<OpenStoreTrigger>();

		nowPrice = gameManager->GetNowPrice();
		nowMoney = gameManager->GetCurrentMoney();

		SetButtonClickEvent();

		// ���� 5���� �����մϴ�.
		powerSprite.assign(5, nullptr);
		healthSprite.assign(5, nullptr);
		speedSprite.assign(5, nullptr);

		for (int i = 0; i < 5; i++)
		{
			string powerStr = "PowerLevelFill" + std::to_string(i);
			string healthStr = "HealthLevelFill" + std::to_string(i);
			string speedStr = "SpeedLevelFill" + std::to_string(i);

			powerSprite[i] = storeCanvas->GetSpriteUI(powerStr);
			healthSprite[i] = storeCanvas->GetSpriteUI(healthStr);
			speedSprite[i] = storeCanvas->GetSpriteUI(speedStr);
		}

		powerButton = storeCanvas->GetButtonUI("SkillSelectButton0");
		healthButton = storeCanvas->GetButtonUI("SkillSelectButton1");
		speedButton = storeCanvas->GetButtonUI("SkillSelectButton2");

		costText = storeCanvas->GetTextUI("Cost");
		moneyText = storeCanvas->GetTextUI("CurrentMoney");

		auto player = SceneManager::GetInstance()->FindObjectName(L"Player");
		playerController = player->GetComponent<PlayerController>();

		sound = SoundManager::GetInstance();
	}

	void StoreController::Update(float tick)
	{
		// ���� ������ �ƴϸ� ����
		if (openTrigger->GetOnCanvas() != true)
			return;

		// �ƴϸ� ���� ��Ȳ�� ����.. �ؽ�Ʈ ���� ��������Ʈ�� �ٲ��ݴϴ�.
		int nowPower = gameManager->GetPowerLevel();
		int nowHealth = gameManager->GetHealthLevel();
		int nowSpeed = gameManager->GetSpeedLevel();

		// ������ 0�̸� �ʱ�ȭ �����ݴϴ�.
		if(nowPower < 5)
			powerButton->SetEnable(true);
		if(nowHealth < 5)
			healthButton->SetEnable(true);
		if(nowSpeed < 5)
			speedButton->SetEnable(true);


		for (int i = 0; i < nowPower; i++)
			powerSprite[i]->SetEnable(true);
		// �ƴ� �ֵ��� ���ݴϴ�.
		for (int i = nowPower; i < 5; i++)
			powerSprite[i]->SetEnable(false);

		for (int i = 0; i < nowHealth; i++)
			healthSprite[i]->SetEnable(true);
		for (int i = nowHealth; i < 5; i++)
			healthSprite[i]->SetEnable(false);

		for (int i = 0; i < nowSpeed; i++)
			speedSprite[i]->SetEnable(true);
		for (int i = nowSpeed; i < 5; i++)
			speedSprite[i]->SetEnable(false);

		nowPrice = gameManager->GetNowPrice();
		nowMoney = gameManager->GetCurrentMoney();

		moneyText->SetText(to_wstring(nowMoney));
	}

	void StoreController::SetButtonClickEvent()
	{
		// Ŭ�� �̺�Ʈ�� �����մϴ�.
		// ��
		auto tmpButton = storeCanvas->GetButtonUI("SkillSelectButton0");
		tmpButton->SetClickEvent([&]
			{
				if (nowMoney >= nowPrice)
				{
					LevelUp(0);
					sound->PlaySFXSound("PowerUp", 0.5f);
				}
				else
				{
					// ȿ����..
					sound->PlaySFXSound("PowerFail", 0.5f);

				}
			});

		// ü��
		tmpButton = storeCanvas->GetButtonUI("SkillSelectButton1");
		tmpButton->SetClickEvent([&]
			{

				if (nowMoney >= nowPrice)
				{
					LevelUp(1);
					sound->PlaySFXSound("PowerUp", 0.5f);
				}
				else
				{
					// ȿ����..
					sound->PlaySFXSound("PowerFail", 0.5f);

				}
			});

		// ���ǵ�
		tmpButton = storeCanvas->GetButtonUI("SkillSelectButton2");
		tmpButton->SetClickEvent([&]
			{
				if (nowMoney >= nowPrice)
				{
					LevelUp(2);
					sound->PlaySFXSound("PowerUp", 0.5f);
				}
				else
				{
					// ȿ����..
					sound->PlaySFXSound("PowerFail", 0.5f);
				}
			});
	}

	void StoreController::LevelUp(int intCase)
	{
		switch (intCase)
		{
			// ��
		case 0:
		{
			gameManager->PowerLevelUp();
			gameManager->SetUpgradeStat();
			nowMoney -= nowPrice;
			gameManager->SetCurrentMoney(nowMoney);
		}
		break;

		// ü��
		case 1:
		{
			gameManager->HealthLevelUp();
			gameManager->SetUpgradeStat();

			playerController->SetHP(gameManager->GetCurrentHP());
			playerController->SetMaxHP(gameManager->GetMaxHP());

			nowMoney -= nowPrice;
			gameManager->SetCurrentMoney(nowMoney);
		}
		break;

		// ���ǵ�
		case 2:
		{
			gameManager->SpeedLevelUp();
			gameManager->SetUpgradeStat();
			nowMoney -= nowPrice;
			gameManager->SetCurrentMoney(nowMoney);
		}
		break;

		default:
			break;
		}
	}
}
