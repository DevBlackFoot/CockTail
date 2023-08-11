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
		// 현재 오브젝트에 붙어있는 컴포넌트 캐싱
		openTrigger = GetGameObject()->GetComponent<OpenStoreTrigger>();

		nowPrice = gameManager->GetNowPrice();
		nowMoney = gameManager->GetCurrentMoney();

		SetButtonClickEvent();

		// 각각 5개로 세팅합니다.
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
		// 현재 상점이 아니면 리턴
		if (openTrigger->GetOnCanvas() != true)
			return;

		// 아니면 현재 상황에 따라서.. 텍스트 색과 스프라이트를 바꿔줍니다.
		int nowPower = gameManager->GetPowerLevel();
		int nowHealth = gameManager->GetHealthLevel();
		int nowSpeed = gameManager->GetSpeedLevel();

		// 레벨이 0이면 초기화 시켜줍니다.
		if(nowPower < 5)
			powerButton->SetEnable(true);
		if(nowHealth < 5)
			healthButton->SetEnable(true);
		if(nowSpeed < 5)
			speedButton->SetEnable(true);


		for (int i = 0; i < nowPower; i++)
			powerSprite[i]->SetEnable(true);
		// 아닌 애들을 꺼줍니다.
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
		// 클릭 이벤트를 세팅합니다.
		// 힘
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
					// 효과음..
					sound->PlaySFXSound("PowerFail", 0.5f);

				}
			});

		// 체력
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
					// 효과음..
					sound->PlaySFXSound("PowerFail", 0.5f);

				}
			});

		// 스피드
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
					// 효과음..
					sound->PlaySFXSound("PowerFail", 0.5f);
				}
			});
	}

	void StoreController::LevelUp(int intCase)
	{
		switch (intCase)
		{
			// 힘
		case 0:
		{
			gameManager->PowerLevelUp();
			gameManager->SetUpgradeStat();
			nowMoney -= nowPrice;
			gameManager->SetCurrentMoney(nowMoney);
		}
		break;

		// 체력
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

		// 스피드
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
