#include "pch.h"
#include "BarCoreController.h"

#include "GameManager.h"
#include "UIManager.h"

namespace ClientSpace
{
	BarCoreController::BarCoreController(std::weak_ptr<GameObject> obj)
		: ComponentBase(obj)
		, nowGuestIdx(0)
		, nowState(BarState::NONE)
		, nowRecipeState(RecipeState::NONE)
		, resultState(ResultState::NONE)
		, blockClick(false)
		, changeFirst(true)
		, angle(270.0f)
		, startAngle(270.0f)
		, angleTheta(60.0f)
		, maxIterations(8)
		, radius(340.0f)
		, selectIdx(0)
		, dialogIdx(0)
		, tutorialIdx(1)
		, prevTutorialIdx(0)
		, nowCustomerIdx(0)
		, spriteAlpha(0.f)
		, alphaSpeed(1.1f)
		, isCorrectCockTail(true)
		, miniGameCount(0)
		, correctCount(0)
		, startBye(false)
		, isTurning(false)
		, setMix(false)
		, recipeOffset(0.f)
		, isAnimating(false)
		, isRecipeSet(true)
		, animSpeed(1.0f)
		, dialogTestIdx(0)
		, maxHP(80)
		, currentHP(80)
		, prevHP(80)
		, hpOffset(0.f)
		, guestTick(0.f)
		, guestIn(false)
		, fTick(0.f)
		, buttonClick(false)
		, countDownTick(3.0f)
		, mixTick(0.f)
		, displayResult(false)
		, priceText(L"/n/cFFF2DEPRICE : /cFFAD00")
	{
		// �迭�� ����
			// �Ź� ������ �� ���� ���� ���� �Ǳ� ������..
		guest[0] = 0;
		guest[1] = 1;
		guest[2] = 2;

		uiPos[0] = Vector3{ -650.0f ,-150.0f, 0.25f };
		uiPos[1] = Vector3{ -650.0f ,5.0f, 0.25f };
		uiPos[2] = Vector3{ -650.0f ,150.0f, 0.25f };

		uiCirclePos[0] = Vector3::Zero;
		uiCirclePos[1] = Vector3::Zero;
		uiCirclePos[2] = Vector3::Zero;

		for (int i = 0; i < 3; i++)
			select[i] = 0;

		gameManager = GameManager::GetInstance();
		cockTailMap = gameManager->GetCockTailMap();

		// Ĭ���� ���չ� 24�� ����
		int nowIdx = 0;
		for (auto& cocktail : cockTailMap)
		{
			auto nowCockTail = cocktail.second;
			checkIngredient[nowIdx] = nowCockTail->cocktailIndex;
			nowIdx++;
		}

		for (int i = 0; i < 4; i++)
		{
			shakeTick[i] = 0.f;
			shakeOffset[i] = 0.f;
			shakeSpeed[i] = 6.f;
			shakePlay[i] = false;
		}

		for (int i = 0; i < 3; i++)
		{
			selectPlay[i] = false;
			// 1���� 0���� ���� ������
			selectOffset[i] = 1.f;
			selectTick[i] = 0.f;
		}
	}

	BarCoreController::~BarCoreController()
	{
	}

	void BarCoreController::Awake()
	{
		// ���� ĳ���� ���ݴϴ�.
		barCanvas = UIManager::GetInstance()->GetCanvas("BarCanvas");
		shakeCanvas = UIManager::GetInstance()->GetCanvas("ShakeCanvas");
		resultCanvas = UIManager::GetInstance()->GetCanvas("ResultCanvas");
		resourceManager = ResourceManager::GetInstance();
		sound = SoundManager::GetInstance();

		input = InputManager::GetInstance();
		inventoryMap = gameManager->GetInventoryItemDataMap();

		// ��ư���� ĳ���մϴ�.
		stuffButtons.resize(7);
		stuffButtons[0] = barCanvas->GetButtonUI("RumButton");
		stuffButtons[1] = barCanvas->GetButtonUI("VodkaButton");
		stuffButtons[2] = barCanvas->GetButtonUI("GinButton");
		stuffButtons[3] = barCanvas->GetButtonUI("LiquorButton");
		stuffButtons[4] = barCanvas->GetButtonUI("JuiceButton");
		stuffButtons[5] = barCanvas->GetButtonUI("SodaButton");
		stuffButtons[6] = barCanvas->GetButtonUI("FruitButton");

		mixButton = barCanvas->GetButtonUI("MixButton");
		resetButton = barCanvas->GetButtonUI("ResetButton");
		beerButton = barCanvas->GetButtonUI("BeerMachineButton");
		closeButton = barCanvas->GetButtonUI("SoldOutButton");

		nowGold = barCanvas->GetTextUI("GoldText");

		tutorial = barCanvas->GetSpriteUI("BarTutorialSprite");

		// �� ����
		customerName = barCanvas->GetTextUI("nameText");
		customerSprite = barCanvas->GetSpriteUI("CustomerSprite");
		dialogText = barCanvas->GetTextUI("CustomerdialogText");
		requestSprite = barCanvas->GetSpriteUI("ReQuestSpriteUI");

		amountTexts.resize(7);

		// stuff amount
		for (int i = 0; i < 7; i++)
		{
			std::string amountIdx = to_string(i);

			amountTexts[i] = barCanvas->GetTextUI("AmountText" + amountIdx);
		}

		// ȣ�� ��ư
		recipeHoverButtons.resize(8);

		for (int i = 0; i < 8; i++)
		{
			std::string uiIdx = to_string(i);

			recipeHoverButtons[i] = barCanvas->GetButtonUI("InformationButton" + uiIdx);
		}

		infoText = barCanvas->GetTextUI("InformationText");
		infoBack = barCanvas->GetSpriteUI("InformationBackGround");

		selectText.resize(3);
		selectSprite.resize(3);

		miniGameSelect.resize(3);
		miniGameIcon.resize(3);
		miniGameNumber.resize(3);
		// �̴� ���� ���� ��������Ʈ ����
			// + 3���� ������ ���� ���� �ؽ�Ʈ, ��������Ʈ ����
		for (int i = 0; i < 3; i++)
		{
			std::string uiIdx = to_string(i);

			selectText[i] = barCanvas->GetTextUI("SelectText" + uiIdx);
			selectSprite[i] = barCanvas->GetSpriteUI("SelectMixSprite" + uiIdx);

			miniGameSelect[i] = shakeCanvas->GetSpriteUI("SelectSprite" + uiIdx);
			miniGameIcon[i] = shakeCanvas->GetSpriteUI("MiniGameSprite" + uiIdx);
			miniGameNumber[i] = shakeCanvas->GetTextUI("NumberText" + uiIdx);
		}
		shaker = shakeCanvas->GetSpriteUI("MiniGameShakerSpriteUI");
		FSprite = shakeCanvas->GetSpriteUI("F");
		countDownBack = shakeCanvas->GetSpriteUI("CountBackGround");
		countDown = shakeCanvas->GetTextUI("CountText");

		// ü��
		miniGameHP = shakeCanvas->GetProgressBarUI("MiniGameHp");
		miniGameArrow = shakeCanvas->GetSpriteUI("MiniGameArrow");

		// 0 - Rum, 1 - Vodka, 2 - Gin
		recipeSprite.resize(3);
		recipeSprite[0] = barCanvas->GetSpriteUI("RumRecipeSpriteUI");
		recipeSprite[1] = barCanvas->GetSpriteUI("VodkaRecipeSpriteUI");
		recipeSprite[2] = barCanvas->GetSpriteUI("GinRecipeSpriteUI");

		// ������ Ÿ��Ʋ
		recipeTitleButton.resize(3);
		recipeTitleButton[0] = barCanvas->GetButtonUI("RumRecipeButton");
		recipeTitleButton[1] = barCanvas->GetButtonUI("VodkaRecipeButton");
		recipeTitleButton[2] = barCanvas->GetButtonUI("GinRecipeButton");

		recipeButton = barCanvas->GetButtonUI("CocktailRecipeButton");
		recipeBackGround = barCanvas->GetSpriteUI("RecipeBackGroundSpriteUI");

		// ��������Ʈ
		mixCockTail = resultCanvas->GetSpriteUI("MixCockTailSpriteUI");
		mixCockTailName = resultCanvas->GetSpriteUI("MixCockTailNameUI");
		mixBG = resultCanvas->GetSpriteUI("MixFirstBG");
		offerButton = resultCanvas->GetButtonUI("OfferButton");
	}

	void BarCoreController::Start()
	{
		// Ʃ�丮�� ���� ���ο� ���� ������ State�� �������ݴϴ�.
		if (gameManager->isFirstCraft == true)
		{
			// ù ũ����Ʈ�� ���� �ʾ��� �� (�ٿ��� �κ�� ���ư��� false�� �˴ϴ�.)
			nowState = BarState::TUTORIAL;
			// ��� update���� ���������� �ٲ��ָ� �ȵ˴ϴ�.
			changeFirst = true;
		}
		else
		{
			nowState = BarState::INTRO;
			changeFirst = true;
		}

		// �Խ�Ʈ ������ �������ݴϴ�.
		// ó���� �׳� �־��ݴϴ�.
		int rand = gameManager->CreateRandomInt(0, 100) % 3;
		guest[0] = rand;

		// �� ��°�� �ٸ��� ����..
		while (guest[0] == rand)
		{
			rand = gameManager->CreateRandomInt(0, 100) % 3;
		}
		guest[1] = rand;

		// �� ��°�� �ٸ� �� ����.
		// �� ��°�� �ٸ��� ����..
		while (guest[0] == rand || guest[1] == rand)
		{
			rand = gameManager->CreateRandomInt(0, 100) % 3;
		}
		guest[2] = rand;

		// �� �� ���ݴϴ�.
		for (int i = 0; i < 3; i++)
		{
			// a, b ����
			rand = gameManager->CreateRandomInt(0, 100) % 2;
			// 0, 1, 2���� 0/1, 2/3, 4/5�� �˴ϴ�.
			guest[i] = guest[i] * 2 + rand;
		}

		// ù �մ� ����
			// nowGuestIdx�� ��ǻ� 0�̱� ������..
		nowCustomer = gameManager->GetCustomer(guest[nowGuestIdx]);

		// ��ư ���� �� ĵ���� ������ ���ݴϴ�.
		barCanvas->SetEnable(true);
		SetAllButtonEvent();

		// ���� �������ݽô�.
		nowGold->SetText(std::to_wstring(gameManager->GetCurrentMoney()));

		// Amount ��ġ�� �������ݴϴ�.
		for (int i = 0; i < 7; i++)
		{
			// �⺻������ ���ڵ� ������ �ݽô�..
			int idx = i + 1;
			auto item = inventoryMap.find(idx);

			if (item != inventoryMap.end())
			{
				amountTexts[i]->SetText(to_string(item->second->count));
			}

			switch (i)
			{
			case 0:
				amountTexts[i]->SetPosition({ -775.0f, 328.0f, amountTexts[i]->GetPosition().z });
				break;
			case 1:
				amountTexts[i]->SetPosition({ -588.0f, 328.0f, amountTexts[i]->GetPosition().z });
				break;
			case 2:
				amountTexts[i]->SetPosition({ -413.0f, 328.0f, amountTexts[i]->GetPosition().z });
				break;
			case 3:
				amountTexts[i]->SetPosition({ -240.0f, 328.0f, amountTexts[i]->GetPosition().z });
				break;
			case 4:
				amountTexts[i]->SetPosition({ -62.0f, 328.0f, amountTexts[i]->GetPosition().z });
				break;
			case 5:
				amountTexts[i]->SetPosition({ 121.0f, 328.0f, amountTexts[i]->GetPosition().z });
				break;
			case 6:
				amountTexts[i]->SetPosition({ 284.0f, 328.0f, amountTexts[i]->GetPosition().z });
				break;
			default:
				break;
			}
		}

		// ������Ʈ�� ���鼭 ������ ���ݴϴ�..
	}

	void BarCoreController::Update(float tick)
	{
		// ���� ������Ʈ�� ���� ������Ʈ�� ���ݴϴ�.
		switch (nowState)
		{
		case BarState::TUTORIAL:
			TutorialUpdate(tick);
			break;

		case BarState::INTRO:
			IntroUpdate(tick);
			break;

		case BarState::ORDER:
			OrderUpdate(tick);
			break;

		case BarState::MINI_GAME:
			MiniGameUpdate(tick);
			break;

		case BarState::OFFER:
			OfferUpdate(tick);
			break;

		case BarState::BYE:
			ByeUpdate(tick);
			break;

		case BarState::CLOSE:
			CloseUpdate(tick);
			break;

		case BarState::NONE:
		default:
			return;
		}

	}

	void BarCoreController::LateUpdate(float tick)
	{
		// �ʿ��� �� ������ݴϴ�.
		// ������ ������Ʈ�� ���������� ������Ʈ �غ��ϴ�.
		if (recipeSprite[0]->GetEnable() == false && recipeSprite[1]->GetEnable() == false && recipeSprite[2]->GetEnable() == false)
		{
			nowRecipeState = RecipeState::NONE;
		}
		else if (recipeSprite[0]->GetEnable() == true)
		{
			nowRecipeState = RecipeState::RUM;
		}
		else if (recipeSprite[1]->GetEnable() == true)
		{
			nowRecipeState = RecipeState::VODKA;
		}
		else if (recipeSprite[2]->GetEnable() == true)
		{
			nowRecipeState = RecipeState::GIN;
		}
	}

	void BarCoreController::SetAllButtonEvent()
	{
		// ���� ��ư�� ���� �̺�Ʈ�� �����غ��ϴ�.

		// �ͽ� ��ư�� ���
			// üũ�� �����ϸ� �̴ϰ�������
			// �ƴϸ� ���
		mixButton->SetClickEvent([&]
			{
				if (selectIdx < 3)
					return;

				sound->PlaySFXSound("ClickSounds", 0.5f);

				bool isHas = false;
				int determine = select[0] * 10000 + select[1] * 100 + select[2];
				for (int num : checkIngredient)
				{
					if (num == determine)
					{
						isHas = true;
						break;
					}
				}

				if (isHas == true)
				{
					// ���⼭ ���� Ĭ������ Ư¡�� �����ɴϴ�.
						// Ư¡�� ������ cock

					// ���� ������� �ϴ� Ĭ���� ����.
					nowCocktail = gameManager->GetCockTailCombi(determine);
					for (int feature : nowCocktail->feature)
					{
						if (feature == nowOrder->request1 || feature == nowOrder->request2)
						{
							isCorrectCockTail = true;
							break;
						}
					}
					selectIdx = 0;

					ResetSelect();
					nowState = BarState::MINI_GAME;
					changeFirst = true;
				}
				else
				{
					// �ϴ� �����ݴϴ�.
					for (int num : select)
					{
						auto item = inventoryMap.find(num);

						if (item != inventoryMap.end())
						{
							item->second->count++;
						}
					}
					// select �ʱ�ȭ.
					for (int& num : select)
					{
						num = 0;
					}

					selectIdx = 0;
					AllStuffButtonEnable(true);
				}
			});

		// ���� ��ư�� ���
			// �κ��丮�� amount�� �����ݴϴ�.
		resetButton->SetClickEvent([&]
			{
				sound->PlaySFXSound("ClickSounds", 0.5f);

				for (int num : select)
				{
					auto item = inventoryMap.find(num);

					if (item != inventoryMap.end())
					{
						item->second->count++;
					}
				}
				// select �ʱ�ȭ.
				for (int& num : select)
				{
					num = 0;
				}

				selectIdx = 0;
				sound->PlaySFXSound("Reset", 0.5f);

				ResetSelect();

				AllStuffButtonEnable(true);
			});

		// Stuff ��ư�� ���
			// ���� �� ���� �κ��丮�� amount ����
			// ���� �� ���� idx ����
			// �ᱹ�� �κ��丮�� ���� ���� -> UI�� ����ֱ⸸�մϴ�.

		// �κ��丮���� ã�� ����
		// ���� �Լ� �̽�

#pragma region StuffButton
		stuffButtons[0]->SetClickEvent([&]
			{
				// select Idx�� Ŭ �� Ȥ�� ���� ���� �����ϱ� ����.
				if (selectIdx >= 3)
					return;

				// ��
				sound->PlaySFXSound("GlassBottle", 0.5f);

				auto item = inventoryMap.find(1);

				if (item != inventoryMap.end())
				{
					item->second->count--;

					// ����Ʈ
					if (selectIdx < 3)
					{
						SetSelect(1);
						select[selectIdx] = 1;
						selectIdx++;

						if (selectIdx == 3)
						{
							AllStuffButtonEnable(false);
							SetNotFoundRecipe();
						}
					}
				}
			});

		stuffButtons[1]->SetClickEvent([&]
			{
				// select Idx�� Ŭ �� Ȥ�� ���� ���� �����ϱ� ����.
				if (selectIdx >= 3)
					return;

				sound->PlaySFXSound("GlassBottle", 0.5f);

				auto item = inventoryMap.find(2);

				if (item != inventoryMap.end())
				{
					item->second->count--;

					// ����Ʈ
					if (selectIdx < 3)
					{
						SetSelect(2);

						select[selectIdx] = 2;
						selectIdx++;

						if (selectIdx == 3)
						{
							AllStuffButtonEnable(false);
							SetNotFoundRecipe();
						}
					}
				}
			});

		stuffButtons[2]->SetClickEvent([&]
			{
				// select Idx�� Ŭ �� Ȥ�� ���� ���� �����ϱ� ����.
				if (selectIdx >= 3)
					return;

				sound->PlaySFXSound("GlassBottle", 0.5f);

				auto item = inventoryMap.find(3);

				if (item != inventoryMap.end())
				{
					item->second->count--;

					// ����Ʈ
					if (selectIdx < 3)
					{
						SetSelect(3);

						select[selectIdx] = 3;
						selectIdx++;

						if (selectIdx == 3)
						{
							AllStuffButtonEnable(false);
							SetNotFoundRecipe();
						}
					}
				}
			});

		stuffButtons[3]->SetClickEvent([&]
			{
				// select Idx�� Ŭ �� Ȥ�� ���� ���� �����ϱ� ����.
				if (selectIdx >= 3)
					return;

				sound->PlaySFXSound("GlassBottle", 0.5f);

				auto item = inventoryMap.find(4);

				if (item != inventoryMap.end())
				{
					item->second->count--;

					// ����Ʈ
					if (selectIdx < 3)
					{
						SetSelect(4);

						select[selectIdx] = 4;
						selectIdx++;

						if (selectIdx == 3)
						{
							AllStuffButtonEnable(false);

							// ������Ʈ���� ������ ���� ���⼭ ��������.
								// �˻���� �� ���ݴϴ�.
							SetNotFoundRecipe();
						}
					}
				}
			});

		stuffButtons[4]->SetClickEvent([&]
			{
				// select Idx�� Ŭ �� Ȥ�� ���� ���� �����ϱ� ����.
				if (selectIdx >= 3)
					return;

				sound->PlaySFXSound("CanSound", 0.5f);

				auto item = inventoryMap.find(5);

				if (item != inventoryMap.end())
				{
					item->second->count--;

					// ����Ʈ
					if (selectIdx < 3)
					{
						SetSelect(5);

						select[selectIdx] = 5;
						selectIdx++;

						if (selectIdx == 3)
						{
							AllStuffButtonEnable(false);

							// ������Ʈ���� ������ ���� ���⼭ ��������.
							SetNotFoundRecipe();
						}
					}
				}
			});

		stuffButtons[5]->SetClickEvent([&]
			{
				// select Idx�� Ŭ �� Ȥ�� ���� ���� �����ϱ� ����.
				if (selectIdx >= 3)
					return;

				sound->PlaySFXSound("GlassBottle", 0.5f);

				auto item = inventoryMap.find(6);

				if (item != inventoryMap.end())
				{
					item->second->count--;

					// ����Ʈ
					if (selectIdx < 3)
					{
						SetSelect(6);

						select[selectIdx] = 6;
						selectIdx++;

						if (selectIdx == 3)
						{
							AllStuffButtonEnable(false);
							SetNotFoundRecipe();
						}
					}
				}
			});

		stuffButtons[6]->SetClickEvent([&]
			{
				// select Idx�� Ŭ �� Ȥ�� ���� ���� �����ϱ� ����.
				if (selectIdx >= 3)
					return;

				sound->PlaySFXSound("CanSound", 0.5f);

				auto item = inventoryMap.find(7);

				if (item != inventoryMap.end())
				{
					item->second->count--;

					// ����Ʈ
					if (selectIdx < 3)
					{
						SetSelect(7);

						select[selectIdx] = 7;
						selectIdx++;

						if (selectIdx == 3)
						{
							AllStuffButtonEnable(false);
							SetNotFoundRecipe();
						}
					}
				}
			});
#pragma endregion

		// ��� ��ư�� ���
			// ���� ���¿� ���� �մ��� ��縦 �������ݴϴ�.
		beerButton->SetClickEvent([&]
			{
				sound->PlaySFXSound("ClickSounds", 0.5f);

				// ���� selectIdx�� 0�� �ƴϸ� ��ȯ�� �ݴϴ�.
				if (selectIdx != 0)
				{
					for (int num : select)
					{
						auto item = inventoryMap.find(num);

						if (item != inventoryMap.end())
						{
							item->second->count++;
						}
					}
					// select �ʱ�ȭ.
					for (int& num : select)
					{
						num = 0;
					}

					selectIdx = 0;

					AllStuffButtonEnable(true);
				}

				// ��� ���¸� Beer�� �մϴ�
				resultState = ResultState::BEER;
				// �ٷ� bye��
				nowState = BarState::BYE;
				changeFirst = true;
			});

		// Close ��ư�� ���
			// �κ�� ���ư��ϴ�.
			// �� �� ��� ĵ������ ���ݴϴ�.
		closeButton->SetClickEvent([&]
			{
				SceneManager::GetInstance()->LoadScene(L"LobbyScene");
				barCanvas->SetEnable(false);
				shakeCanvas->SetEnable(false);
				resultCanvas->SetEnable(false);
			});

		// ȣ�� ��ư�� ���
			// �� ��쿡 ���� �̺�Ʈ�� �������ݴϴ�.
			// ���� �Լ� �̽��� �ϵ� �ڵ�.
#pragma region RecipeHover
		recipeHoverButtons[0]->SetHoverEvent([&]
			{
				// ���� ������ ��Ȳ ���� �޶�������
				switch (nowRecipeState)
				{
				case RecipeState::RUM:
				{
					// ���� ȣ���� 1�� �����մϴ�.
					int idx = 10;
					auto cockTail = cockTailMap.find(idx);

					if (cockTail != cockTailMap.end())
					{
						infoText->SetStyleText(cockTail->second->descriptionKR + priceText + std::to_wstring(cockTail->second->amount));
					}

					infoText->SetPosition(Vector3{ -370.f, -412.f, 0.4f });
					infoBack->SetEnable(true);
					infoBack->SetPosition(Vector3{ -270.f, -393.f, 0.4f });
					infoBack->SetWidth(460.f);
					infoBack->SetHeight(73.f);
					break;
				}
				case RecipeState::VODKA:
				{
					// ����ī�� ȣ���� 2�� �����մϴ�.
					int idx = 20;
					auto cockTail = cockTailMap.find(idx);

					if (cockTail != cockTailMap.end())
					{
						infoText->SetStyleText(cockTail->second->descriptionKR + priceText + std::to_wstring(cockTail->second->amount));
					}
					infoText->SetPosition(Vector3{ -370.f, -406.f, 0.4f });

					infoBack->SetEnable(true);
					infoBack->SetPosition(Vector3{ -270.f, -388.f, 0.4f });
					infoBack->SetWidth(460.f);
					infoBack->SetHeight(73.f);
					break;
				}

				case RecipeState::GIN:
				{
					// ���� ȣ���� 3�� �����մϴ�.
					int idx = 30;
					auto cockTail = cockTailMap.find(idx);

					if (cockTail != cockTailMap.end())
					{
						infoText->SetStyleText(cockTail->second->descriptionKR + priceText + std::to_wstring(cockTail->second->amount));
					}
					infoText->SetPosition(Vector3{ -370.f, -406.0f, 0.4f });

					infoBack->SetEnable(true);
					infoBack->SetPosition(Vector3{ -270.f, -388.f, 0.4f });
					infoBack->SetWidth(460.f);
					infoBack->SetHeight(73.f);
					break;
				}

				case RecipeState::NONE:
				default:
					break;
				}
			});

		recipeHoverButtons[1]->SetHoverEvent([&]
			{
				// ���� ������ ��Ȳ ���� �޶�������
				switch (nowRecipeState)
				{
				case RecipeState::RUM:
				{
					// ���� ȣ���� 1�� �����մϴ�.
					int idx = 11;
					auto cockTail = cockTailMap.find(idx);

					if (cockTail != cockTailMap.end())
					{
						infoText->SetStyleText(cockTail->second->descriptionKR + priceText + std::to_wstring(cockTail->second->amount));
					}

					infoText->SetPosition(Vector3{ -370.f , -362.0f, 0.4f });
					infoBack->SetEnable(true);
					infoBack->SetPosition(Vector3{ -270.f, -328.f, 0.4f });
					infoBack->SetWidth(460.f);
					infoBack->SetHeight(99.f);
					break;
				}
				case RecipeState::VODKA:
				{
					// ����ī�� ȣ���� 2�� �����մϴ�.
					int idx = 21;
					auto cockTail = cockTailMap.find(idx);

					if (cockTail != cockTailMap.end())
					{
						infoText->SetStyleText(cockTail->second->descriptionKR + priceText + std::to_wstring(cockTail->second->amount));
					}
					infoText->SetPosition(Vector3{ -370.f, -359.0f, 0.4f });

					infoBack->SetEnable(true);
					infoBack->SetPosition(Vector3{ -270.f, -325.f, 0.4f });
					infoBack->SetWidth(460.f);
					infoBack->SetHeight(99.f);
					break;
				}

				case RecipeState::GIN:
				{
					// ���� ȣ���� 3�� �����մϴ�.
					int idx = 31;
					auto cockTail = cockTailMap.find(idx);

					if (cockTail != cockTailMap.end())
					{
						infoText->SetStyleText(cockTail->second->descriptionKR + priceText + std::to_wstring(cockTail->second->amount));
					}
					infoText->SetPosition(Vector3{ -370.f, -360.0f, 0.4f });

					infoBack->SetEnable(true);
					infoBack->SetPosition(Vector3{ -270.f, -326.f, 0.4f });
					infoBack->SetWidth(460.f);
					infoBack->SetHeight(99.f);
					break;
				}

				case RecipeState::NONE:
				default:
					break;
				}
			});

		recipeHoverButtons[2]->SetHoverEvent([&]
			{
				// ���� ������ ��Ȳ ���� �޶�������
				switch (nowRecipeState)
				{
				case RecipeState::RUM:
				{
					// ���� ȣ���� 1�� �����մϴ�.
					int idx = 12;
					auto cockTail = cockTailMap.find(idx);

					if (cockTail != cockTailMap.end())
					{
						infoText->SetStyleText(cockTail->second->descriptionKR + priceText + std::to_wstring(cockTail->second->amount));
					}

					infoText->SetPosition(Vector3{ -370.f, -299.0f, 0.4f });
					infoBack->SetEnable(true);
					infoBack->SetPosition(Vector3{ -270.f, -266.f, 0.4f });
					infoBack->SetWidth(460.f);
					infoBack->SetHeight(99.f);
					break;
				}
				case RecipeState::VODKA:
				{
					// ����ī�� ȣ���� 2�� �����մϴ�.
					int idx = 22;
					auto cockTail = cockTailMap.find(idx);

					if (cockTail != cockTailMap.end())
					{
						infoText->SetStyleText(cockTail->second->descriptionKR + priceText + std::to_wstring(cockTail->second->amount));
					}
					infoText->SetPosition(Vector3{ -370.f, -295.0f, 0.4f });

					infoBack->SetEnable(true);
					infoBack->SetPosition(Vector3{ -270.f, -262.f, 0.4f });
					infoBack->SetWidth(460.f);
					infoBack->SetHeight(99.f);
					break;
				}

				case RecipeState::GIN:
				{
					// ���� ȣ���� 3�� �����մϴ�.
					int idx = 32;
					auto cockTail = cockTailMap.find(idx);

					if (cockTail != cockTailMap.end())
					{
						infoText->SetStyleText(cockTail->second->descriptionKR + priceText + std::to_wstring(cockTail->second->amount));
					}
					infoText->SetPosition(Vector3{ -370.f, -299.0f, 0.4f });

					infoBack->SetEnable(true);
					infoBack->SetPosition(Vector3{ -270.f, -265.f, 0.4f });
					infoBack->SetWidth(460.f);
					infoBack->SetHeight(99.f);
					break;
				}

				case RecipeState::NONE:
				default:
					break;
				}
			});

		recipeHoverButtons[3]->SetHoverEvent([&]
			{
				// ���� ������ ��Ȳ ���� �޶�������
				switch (nowRecipeState)
				{
				case RecipeState::RUM:
				{
					// ���� ȣ���� 1�� �����մϴ�.
					int idx = 13;
					auto cockTail = cockTailMap.find(idx);

					if (cockTail != cockTailMap.end())
					{
						infoText->SetStyleText(cockTail->second->descriptionKR + priceText + std::to_wstring(cockTail->second->amount));
					}

					infoText->SetPosition(Vector3{ -370.f , -238.0f, 0.4f });

					infoBack->SetEnable(true);
					infoBack->SetPosition(Vector3{ -270.f, -205.f, 0.4f });
					infoBack->SetWidth(460.f);
					infoBack->SetHeight(99.f);
					break;
				}
				case RecipeState::VODKA:
				{
					// ����ī�� ȣ���� 2�� �����մϴ�.
					int idx = 23;
					auto cockTail = cockTailMap.find(idx);

					if (cockTail != cockTailMap.end())
					{
						infoText->SetStyleText(cockTail->second->descriptionKR + priceText + std::to_wstring(cockTail->second->amount));
					}
					infoText->SetPosition(Vector3{ -370.f, -236.0f, 0.4f });

					infoBack->SetEnable(true);
					infoBack->SetPosition(Vector3{ -270.f, -202.f, 0.4f });
					infoBack->SetWidth(460.f);
					infoBack->SetHeight(99.f);
					break;
				}

				case RecipeState::GIN:
				{
					// ���� ȣ���� 3�� �����մϴ�.
					int idx = 33;
					auto cockTail = cockTailMap.find(idx);

					if (cockTail != cockTailMap.end())
					{
						infoText->SetStyleText(cockTail->second->descriptionKR + priceText + std::to_wstring(cockTail->second->amount));
					}
					infoText->SetPosition(Vector3{ -370.f, -237.0f, 0.4f });

					infoBack->SetEnable(true);
					infoBack->SetPosition(Vector3{ -270.f, -203.f, 0.4f });
					infoBack->SetWidth(460.f);
					infoBack->SetHeight(99.f);
					break;
				}

				case RecipeState::NONE:
				default:
					break;
				}
			});

		recipeHoverButtons[4]->SetHoverEvent([&]
			{
				// ���� ������ ��Ȳ ���� �޶�������
				switch (nowRecipeState)
				{
				case RecipeState::RUM:
				{
					// ���� ȣ���� 1�� �����մϴ�.
					int idx = 14;
					auto cockTail = cockTailMap.find(idx);

					if (cockTail != cockTailMap.end())
					{
						infoText->SetStyleText(cockTail->second->descriptionKR + priceText + std::to_wstring(cockTail->second->amount));
					}

					infoText->SetPosition(Vector3{ -370.f , -178.0f, 0.4f });

					infoBack->SetEnable(true);
					infoBack->SetPosition(Vector3{ -270.f, -130.f, 0.4f });
					infoBack->SetWidth(460.f);
					infoBack->SetHeight(125.f);
					break;
				}
				case RecipeState::VODKA:
				{
					// ����ī�� ȣ���� 2�� �����մϴ�.
					int idx = 24;
					auto cockTail = cockTailMap.find(idx);

					if (cockTail != cockTailMap.end())
					{
						infoText->SetStyleText(cockTail->second->descriptionKR + priceText + std::to_wstring(cockTail->second->amount));
					}
					infoText->SetPosition(Vector3{ -370.f, -174.0f, 0.4f });

					infoBack->SetEnable(true);
					infoBack->SetPosition(Vector3{ -270.f, -140.f, 0.4f });
					infoBack->SetWidth(460.f);
					infoBack->SetHeight(99.f);
					break;
				}

				case RecipeState::GIN:
				{
					// ���� ȣ���� 3�� �����մϴ�.
					int idx = 34;
					auto cockTail = cockTailMap.find(idx);

					if (cockTail != cockTailMap.end())
					{
						infoText->SetStyleText(cockTail->second->descriptionKR + priceText + std::to_wstring(cockTail->second->amount));
					}
					infoText->SetPosition(Vector3{ -370.f, -174.0f, 0.4f });

					infoBack->SetEnable(true);
					infoBack->SetPosition(Vector3{ -270.f, -141.f, 0.4f });
					infoBack->SetWidth(460.f);
					infoBack->SetHeight(99.f);
					break;
				}

				case RecipeState::NONE:
				default:
					break;
				}
			});

		recipeHoverButtons[5]->SetHoverEvent([&]
			{
				// ���� ������ ��Ȳ ���� �޶�������
				switch (nowRecipeState)
				{
				case RecipeState::RUM:
				{
					// ���� ȣ���� 1�� �����մϴ�.
					int idx = 15;
					auto cockTail = cockTailMap.find(idx);

					if (cockTail != cockTailMap.end())
					{
						infoText->SetStyleText(cockTail->second->descriptionKR + priceText + std::to_wstring(cockTail->second->amount));
					}

					infoText->SetPosition(Vector3{ -370.f, -115.0f, 0.4f });

					infoBack->SetEnable(true);
					infoBack->SetPosition(Vector3{ -270.f, -83.f, 0.4f });
					infoBack->SetWidth(460.f);
					infoBack->SetHeight(99.f);
					break;
				}
				case RecipeState::VODKA:
				{
					// ����ī�� ȣ���� 2�� �����մϴ�.
					int idx = 25;
					auto cockTail = cockTailMap.find(idx);

					if (cockTail != cockTailMap.end())
					{
						infoText->SetStyleText(cockTail->second->descriptionKR + priceText + std::to_wstring(cockTail->second->amount));
					}
					infoText->SetPosition(Vector3{ -370.f, -110.0f, 0.4f });

					infoBack->SetEnable(true);
					infoBack->SetPosition(Vector3{ -270.f, -77.f, 0.4f });
					infoBack->SetWidth(460.f);
					infoBack->SetHeight(99.f);
					break;
				}

				case RecipeState::GIN:
				{
					// ���� ȣ���� 3�� �����մϴ�.
					int idx = 35;
					auto cockTail = cockTailMap.find(idx);

					if (cockTail != cockTailMap.end())
					{
						infoText->SetStyleText(cockTail->second->descriptionKR + priceText + std::to_wstring(cockTail->second->amount));
					}
					infoText->SetPosition(Vector3{ -370.f, -113.0f, 0.4f });

					infoBack->SetEnable(true);
					infoBack->SetPosition(Vector3{ -270.f, -80.f, 0.4f });
					infoBack->SetWidth(460.f);
					infoBack->SetHeight(99.f);
					break;
				}

				case RecipeState::NONE:
				default:
					break;
				}
			});

		recipeHoverButtons[6]->SetHoverEvent([&]
			{
				// ���� ������ ��Ȳ ���� �޶�������
				switch (nowRecipeState)
				{
				case RecipeState::RUM:
				{
					// ���� ȣ���� 1�� �����մϴ�.
					int idx = 16;
					auto cockTail = cockTailMap.find(idx);

					if (cockTail != cockTailMap.end())
					{
						infoText->SetStyleText(cockTail->second->descriptionKR + priceText + std::to_wstring(cockTail->second->amount));
					}

					infoText->SetPosition(Vector3{ -370.f , -49.0f, 0.4f });

					infoBack->SetEnable(true);
					infoBack->SetPosition(Vector3{ -270.f, -16.f, 0.4f });
					infoBack->SetWidth(460.f);
					infoBack->SetHeight(99.f);
					break;
				}
				case RecipeState::VODKA:
				{
					// ����ī�� ȣ���� 2�� �����մϴ�.
					int idx = 26;
					auto cockTail = cockTailMap.find(idx);

					if (cockTail != cockTailMap.end())
					{
						infoText->SetStyleText(cockTail->second->descriptionKR + priceText + std::to_wstring(cockTail->second->amount));
					}
					infoText->SetPosition(Vector3{ -370.f, -47.0f, 0.4f });

					infoBack->SetEnable(true);
					infoBack->SetPosition(Vector3{ -242.f, -13.f, 0.4f });
					infoBack->SetWidth(515.f);
					infoBack->SetHeight(99.f);
					break;
				}

				case RecipeState::GIN:
				{
					// ���� ȣ���� 3�� �����մϴ�.
					int idx = 36;
					auto cockTail = cockTailMap.find(idx);

					if (cockTail != cockTailMap.end())
					{
						infoText->SetStyleText(cockTail->second->descriptionKR + priceText + std::to_wstring(cockTail->second->amount));
					}
					infoText->SetPosition(Vector3{ -370.f, -49.0f, 0.4f });

					infoBack->SetEnable(true);
					infoBack->SetPosition(Vector3{ -270.f, -15.f, 0.4f });
					infoBack->SetWidth(460.f);
					infoBack->SetHeight(99.f);
					break;
				}

				case RecipeState::NONE:
				default:
					break;
				}
			});

		recipeHoverButtons[7]->SetHoverEvent([&]
			{
				// ���� ������ ��Ȳ ���� �޶�������
				switch (nowRecipeState)
				{
				case RecipeState::RUM:
				{
					// ���� ȣ���� 1�� �����մϴ�.
					int idx = 17;
					auto cockTail = cockTailMap.find(idx);

					if (cockTail != cockTailMap.end())
					{
						infoText->SetStyleText(cockTail->second->descriptionKR + priceText + std::to_wstring(cockTail->second->amount));
					}

					infoText->SetPosition(Vector3{ -370.f , 11.0f, 0.4f });

					infoBack->SetEnable(true);
					infoBack->SetPosition(Vector3{ -270.f, 43.f, 0.4f });
					infoBack->SetWidth(460.f);
					infoBack->SetHeight(99.f);
					break;
				}
				case RecipeState::VODKA:
				{
					// ����ī�� ȣ���� 2�� �����մϴ�.
					int idx = 27;
					auto cockTail = cockTailMap.find(idx);

					if (cockTail != cockTailMap.end())
					{
						infoText->SetStyleText(cockTail->second->descriptionKR + priceText + std::to_wstring(cockTail->second->amount));
					}
					infoText->SetPosition(Vector3{ -370.f, 11.0f, 0.4f });

					infoBack->SetEnable(true);
					infoBack->SetPosition(Vector3{ -247.f, 45.f, 0.4f });
					infoBack->SetWidth(506.f);
					infoBack->SetHeight(99.f);
					break;
				}

				case RecipeState::GIN:
				{
					// ���� ȣ���� 3�� �����մϴ�.
					int idx = 37;
					auto cockTail = cockTailMap.find(idx);

					if (cockTail != cockTailMap.end())
					{
						infoText->SetStyleText(cockTail->second->descriptionKR + priceText + std::to_wstring(cockTail->second->amount));
					}
					infoText->SetPosition(Vector3{ -370.f, 12.0f, 0.4f });

					infoBack->SetEnable(true);
					infoBack->SetPosition(Vector3{ -270.f, 46.f, 0.4f });
					infoBack->SetWidth(460.f);
					infoBack->SetHeight(99.f);
					break;
				}

				case RecipeState::NONE:
				default:
					break;
				}
			});
#pragma endregion

		for (int i = 0; i < 8; i++)
		{
			// exit
			recipeHoverButtons[i]->SetOutEvent([&]
				{
					// �ؽ�Ʈ�� ����� �����ݴϴ�.
					infoText->SetStyleText(L"");
					infoBack->SetEnable(false);
				});
		}

		// ���� ��ư�� ���
			// ���������� �Ѿ�ϴ�. ���� ��Ȳ�� ���� State�� �������ݴϴ�
			// amount�� �÷��ݴϴ�.
		offerButton->SetClickEvent([&]
			{
				nowState = BarState::BYE;
				changeFirst = true;
				// ����Ʈ�� �ݾ��ش�.
				resultCanvas->SetEnable(false);
			});

		// ������ ��ư�� �������ݴϴ�.
		recipeButton->SetClickEvent([&]
			{
				// ���� �������� ���¿� ����..!
				if (isRecipeSet == true)
				{
					sound->PlaySFXSound("RecipeClose", 0.5f);

					// ���� ������ ���ݽô�.
					isRecipeSet = !isRecipeSet;
					// �ִϸ��̼��� Start ���ݴϴ�.
					isAnimating = true;

					// ��� Ÿ��Ʋ�� ���ݴϴ�.
					AllRecipeTitleButtonEnable(false);
					// �����ǵ鵵 ���ݴϴ�.
					AllRecipeSpriteDisable();

					// �������� 0����
					recipeOffset = 0.f;

					// ������ -> ���� �����ǰ� �����ִ� ��
					startSpritePos = Vector3{ -733.f, -168.f, 0.49f };
					startButtonPos = Vector3{ -885.f, 193.f, 0.48f };
					// ���� -> �����ǰ� ������ �ϴ°�
					endSpritePos = Vector3{ -733.f, -834.f, 0.49f };
					endButtonPos = Vector3{ -885.f, -475.f, 0.48f };
					// �켱�� �������� �����ϴ� ������ �غ���..
						// ���θ� smoothStep�� ���� ������ �غ��ڽ��ϴ�.

					// �ִϸ��̼��� ������ �� ��ư�� disable �صӴϴ�.
					recipeButton->SetButtonEnable(false);
				}
				else
				{
					sound->PlaySFXSound("RecipeOpen", 0.5f);

					// ���� ������ ���ݽô�.
					isRecipeSet = !isRecipeSet;

					// �� ģ���� �ִϸ��̼��� ������.. ��ư���� ���ݴϴ�.
					// �ִϸ��̼��� Start ���ݴϴ�.
					isAnimating = true;
					AllRecipeSpriteDisable();

					// �������� 0����
					recipeOffset = 0.f;

					// ������ -> ���� �����ǰ� �����ִ� ��
					startSpritePos = Vector3{ -733.f, -834.f, 0.49f };
					startButtonPos = Vector3{ -885.f, -475.f, 0.48f };
					// ���� -> �����ǰ� ������ �ϴ°�
					endSpritePos = Vector3{ -733.f, -168.f, 0.49f };
					endButtonPos = Vector3{ -885.f, 193.f, 0.48f };

					// �ִϸ��̼��� ������ �� ��ư�� disable �صӴϴ�.
					recipeButton->SetButtonEnable(false);
				}
			});
	}

	void BarCoreController::TutorialUpdate(float tick)
	{
		// Ŭ�� ��, �ε����� �ø��ϴ�. �ε����� �ٸ� �ÿ��� �̹����� �ٲ��ݴϴ�.
		// ù ���Խ� ��� ��ư�� ���ݴϴ�.
		if (changeFirst == true)
		{
			AllButtonEnable(false);
			tutorial->SetEnable(true);
			changeFirst = false;
		}

		if (tutorialIdx != prevTutorialIdx)
		{
			std::wstring nowPath = L"BarTutorial" + std::to_wstring(tutorialIdx);
			auto tex = resourceManager->LoadPrefabTexture(nowPath);
			tutorial->SetTexture(tex);
			prevTutorialIdx = tutorialIdx;
		}

		// ��ư Ŭ�� �ѹ� �� �� ���� �÷��ݴϴ�.
		if (input->GetInputState(VK_LBUTTON, KeyState::DOWN))
		{
			if (tutorialIdx == 8)
			{
				tutorial->SetEnable(false);
				nowState = BarState::INTRO;
				changeFirst = true;
			}

			tutorialIdx++;
		}
	}

	void BarCoreController::IntroUpdate(float tick)
	{
		// ù ���Խ� ��� ��ư�� ���ݴϴ�.
			// Ʃ�丮�� ���� �ȵ��� ���� �ֱ� ������..
		if (changeFirst == true)
		{
			// �Խ�Ʈ ����.
			sound->PlaySFXSound("Guest", 0.5f);

			changeFirst = false;
			// ������ �����ؼ� ��ư���� ��ο� �ǵ��� �ϰ�
			AllRecipeTitleButtonEnable(true);

			AllButtonEnable(false);
			AllCustomerUIEnable(false);

			dialogIdx = 0;

			// ó�� ���� ������ ��. �ϴ� ���� �� ģ���� ���¸� Ȯ���غ��ϴ�.
				// �� Ŀ���͸ӿ� �°� ������ ���شٴ� ��.
				// ���ø� ���ְ� ������ �ȵǵ��� ���ص�..
			customerName->SetText(nowCustomer->name);

			// ù �ؽ��ĸ� �����ͺ��ϴ�.
			auto tex = resourceManager->LoadPrefabTexture(nowCustomer->smallTalk[dialogIdx].second);

			// ��������Ʈ ���� ���ݴϴ�.
			customerSprite->SetEnable(true);
			customerSprite->SetTexture(tex);
			customerSprite->SetWidth(tex->GetWidth());
			customerSprite->SetHeight(tex->GetHeight());

			nowCustomerIdx = guest[nowGuestIdx] / 2;

			SetCustomerPosition();
			// ù ��縦 ������ ���ô�.
			dialogText->SetStyleText(nowCustomer->smallTalk[dialogIdx].first);

			// �����Ǹ� �ݵ��..! �ݵ�� �����ݴϴ�.
			// ��� �����Ǹ� ���ְ�
			AllRecipeSpriteDisable();
			// ���� ��ư ���� �ʱ�ȭ
			isAnimating = false;
			isRecipeSet = true;

			// ��ư ��ġ ����
				// �� ģ������ ���� ���� �����ϴ�..
			recipeBackGround->SetPosition(Vector3{ -733.f, -168.f, 0.49f });
			recipeButton->SetPosition(Vector3{ -885.f, 193.f, 0.48f });

			guestTick = 0.f;
			guestIn = false;
		}

		if (guestTick > 0.6f)
		{
			guestTick = 0.f;
			guestIn = true;
		}
		else
		{
			guestTick += tick;
		}

		if (guestIn == true)
		{
			// ���İ� 1 ���� �� �� -> ù ���Խ�
				// ���� bye���� �ٽ� 0���� �ٲ������.
				// �մ� ������ ������ �ɰ̴ϴ�.
			if (spriteAlpha < 1.0f)
			{
				spriteAlpha += tick * alphaSpeed;
				if (spriteAlpha >= 1.0f)
				{
					// ���⼭ ��� �ֵ��� ���ݴϴ� -> �ѹ��� �Ҹ��� ����
					AllCustomerUIEnable(true);
					spriteAlpha = 1.0f;
				}
				//spriteAlpha = SmoothStep(spriteAlpha);

				customerSprite->SetMaskAlpha(MathHelper::SmoothStep(spriteAlpha));
			}
			else
			{
				// ���⼭�� Ŭ�� �ѹ� �ѹ� �� �� ����. �Ѿ �ֵ��� �մϴ�.
				if (input->GetInputState(VK_LBUTTON, KeyState::DOWN))
				{
					dialogIdx++;

					// ������ũ�� ���� �� ������ �޾� �ݴϴ�.
					if (dialogIdx < nowCustomer->smallTalk.size())
					{
						dialogText->SetStyleText(nowCustomer->smallTalk[dialogIdx].first);
						// ǥ���� �ٲ���
						auto tex = resourceManager->LoadPrefabTexture(nowCustomer->smallTalk[dialogIdx].second);
						customerSprite->SetTexture(tex);
						customerSprite->SetWidth(tex->GetWidth());
						customerSprite->SetHeight(tex->GetHeight());
					}
					else
					{
						// �ε��� ��. ���� ������� ��ȯ!
						nowState = BarState::ORDER;
						changeFirst = true;
						dialogIdx = 0;
					}
				}
			}
		}
	}

	void BarCoreController::OrderUpdate(float tick)
	{
		// ���⼭�� ��ǻ� ��� �ѹ��� �ٲ��ְ� ��� ��ư�� ���ִ� ���
		if (changeFirst == true)
		{
			changeFirst = false;

			// �ֹ� ����Ʈ ������� �̾ƿ�
			int orderIdx = gameManager->CreateRandomInt(0, 74);
			nowOrder = gameManager->GetOrder(orderIdx);
			if (nowOrder != nullptr)
				dialogText->SetStyleText(nowOrder->KR);

			// ������ �ִ� Select Ȥ�� �� �ʱ�ȭ.
			ResetSelect();
			// ��� ��ư Ȱ��ȭ!
			AllButtonEnable(true);
		}

		// ��ư�� ��� default ���¸� ���ݽô�..
		bool notHover = true;
		// amount Text�� ���õ� ��ư�� �����ϴ� ����.
			// ��Ȳ�� �°� ��ư�� ���� �״� ���ݴϴ�.
		for (int i = 0; i < 7; i++)
		{
			// amount text ����
			auto item = inventoryMap.find(i + 1);
			if (item != inventoryMap.end())
			{
				int count = item->second->count;
				amountTexts[i]->SetText(to_wstring(count));

				if (count == 0)
					stuffButtons[i]->SetButtonEnable(false);
			}

			if (stuffButtons[i]->GetButtonState() != ButtonState::DEFAULT)
				notHover = false;
		}

		// ��� ����
		/*
		if (input->GetInputState('B', KeyState::DOWN))
		{
			static ResultState testState = ResultState::NONE;
			static int testGuestIdx = 0;
			static int testIdx = 0;
			static std::shared_ptr<Customer> testCustomer = nullptr;
			static bool testSet = false;

			if (testSet == false)
			{
				testSet = true;
				testCustomer = gameManager->GetCustomer(testGuestIdx);
			}

			switch (testState)
			{
				// ���⼱ ������
			case ResultState::NONE:
			{
				if (testIdx < testCustomer->smallTalk.size())
				{
					dialogText->SetStyleText(testCustomer->smallTalk[testIdx].first);
					testIdx++;
				}
				else
				{
					testState = ResultState::GOOD;
					testIdx = 0;
				}
				break;
			}
			case ResultState::GOOD:
			{
				if (testIdx < testCustomer->goodTalk.size())
				{
					dialogText->SetStyleText(testCustomer->goodTalk[testIdx].first);
					testIdx++;
				}
				else
				{
					testState = ResultState::BAD;
					testIdx = 0;
				}
					break;
			}
			case ResultState::BAD:
			{
				if (testIdx < testCustomer->badTalk.size())
				{
					dialogText->SetStyleText(testCustomer->badTalk[testIdx].first);
					testIdx++;
				}
				else
				{
					testState = ResultState::BEER;
					testIdx = 0;
				}
				break;
			}
			case ResultState::BEER:
			{
				if (testIdx < testCustomer->beerTalk.size())
				{
					dialogText->SetStyleText(testCustomer->beerTalk[testIdx].first);
					testIdx++;
				}
				else
				{
					testState = ResultState::BYE;
					testIdx = 0;
				}
				break;
			}
			case ResultState::BYE:
			{
				if (testIdx < testCustomer->byeTalk.size())
				{
					dialogText->SetStyleText(testCustomer->byeTalk[testIdx].first);
					testIdx++;
				}
				else
				{
					testState = ResultState::NONE;
					testIdx = 0;
					testGuestIdx++;

					testSet = false;

					if(testGuestIdx > 5)
						testGuestIdx = 0;
				}
				break;
			}
			default:
			{
				break;
			}
			}
		}
		 */

		// ���� Hover�� �ƴ� ��쿡�� ���Ҵ� ��찡 �־��� ������, ��� ��ư�� default ���¸� ���ݴϴ�.
		if (notHover == true)
		{
			UIManager::GetInstance()->GetCanvas("CursorCanvas")->GetSpriteUI("Info")->SetEnable(false);
			UIManager::GetInstance()->GetCanvas("CursorCanvas")->GetTextUI("InfoText")->SetEnable(false);
		}

		if (selectIdx < 3)
		{
			setMix = false;
			mixButton->SetButtonEnable(false);
		}
		else
		{
			bool isHas = false;
			int determine = select[0] * 10000 + select[1] * 100 + select[2];
			for (int num : checkIngredient)
			{
				if (num == determine)
				{
					isHas = true;
					break;
				}
			}

			if (setMix == false && isHas == true)
			{
				// enable�� ȣ�� �� �� ���� default�� �ٲ��ֱ� ������ ������..?
				mixButton->SetButtonEnable(true);
				setMix = true;
			}
		}

		// ������ ��ư ������ ������ ���� �������.
		if (isAnimating)
		{
			recipeOffset += tick * animSpeed;
			if (recipeOffset >= 1.0f)
			{
				recipeOffset = 1.0f;

				// �� ���� �Ҹ��� ���̱� ������..
				isAnimating = false;
				// ��ư�� ��밡�� �ϵ���
				recipeButton->SetButtonEnable(true);
				// ���� ���¸�.. Ÿ��Ʋ ��ư���� ���ݴϴ�.
				if (isRecipeSet == true)
				{
					AllRecipeTitleButtonEnable(true);
				}
			}
			//recipeOffset = MathHelper::SmoothStep(recipeOffset);
			Vector3 nowSpritePos = Vector3::Lerp(startSpritePos, endSpritePos, MathHelper::SmoothStep(recipeOffset));
			Vector3 nowButtonPos = Vector3::Lerp(startButtonPos, endButtonPos, MathHelper::SmoothStep(recipeOffset));

			recipeButton->SetPosition(nowButtonPos);
			recipeBackGround->SetPosition(nowSpritePos);
		}
	}

	void BarCoreController::MiniGameUpdate(float tick)
	{
		if (changeFirst == true)
		{
			changeFirst = false;
			// �ϴ��� ���� ��ư�̶� ĳ���� �����ݴϴ�..?
			AllButtonEnable(false);

			shakeCanvas->SetEnable(true);

			// ���� ������Ʈ���� �� ��ġ�� ��ġ ���� �ݴϴ�.
			MiniGameSetUp();
			countDownBack->SetEnable(true);
			countDown->SetEnable(true);
			countDown->SetText(L"3");
			countDownTick = 3.f;

			hpOffset = 0.f;

			// �� �� ���� ������ ���¿� ���� ������ �޸� ���ݴϴ�.
				// �ϴ� ���� �ִϸ������� ����ǰ� �־��� �� ���߰��մϴ�.
				// �װ� �ƴϸ� endPos�� �̻��� ���� �� ���� �ֱ� �����Դϴ�.
			if (isAnimating == true)
			{
				isAnimating = false;
				// �׸��� ���� ��ġ�� ������ �մϴ�.
				recipeButton->SetPosition(endButtonPos);
				recipeBackGround->SetPosition(endSpritePos);
			}

			for (int i = 0; i < 4; i++)
			{
				shakeTick[i] = 0.f;
				shakeOffset[i] = 0.f;
				shakeSpeed[i] = 6.f;
				shakePlay[i] = false;

				if (i < 3)
					miniGameIcon[i]->SetRotation(0.f);
				else
					shaker->SetRotation(0.f);
			}

			for (int i = 0; i < 3; i++)
			{
				miniGameSelect[i]->SetEnable(true);
				miniGameSelect[i]->SetMaskAlpha(1.0f);

				miniGameIcon[i]->SetEnable(true);
				miniGameIcon[i]->SetMaskAlpha(1.0f);

				miniGameNumber[i]->SetEnable(true);

				selectPlay[i] = false;
				selectOffset[i] = 1.f;
				selectTick[i] = 0.f;
			}

			buttonClick = false;
			fTick = 0.f;
			spriteAlpha = 1.f;

			sound->PlaySFXSound("CountDown", 0.5f);
		}

		// ī��Ʈ�� �غ��ô�.
		if (countDownTick > 0.f)
		{
			countDownTick -= tick;
			if (countDownTick < 0.f)
			{
				countDownTick = 0.f;
				isTurning = true;

				countDown->SetEnable(false);
				countDownBack->SetEnable(false);
			}
			else if (countDownTick < 1.f)
				countDown->SetText(L"1");
			else if (countDownTick < 2.f)
				countDown->SetText(L"2");

			// ī��Ʈ �ٿ��� ������ ���� �Ʒ��� �����Ű�� �ʽ��ϴ�.
			return;
		}

		// ü�� ����.
			// �⺻������ 0.f�̱� ������..
		if (prevHP != currentHP)
		{
			hpOffset += tick * 2.0f;
			if (hpOffset > 1.0f)
			{
				hpOffset = 0.f;
				prevHP = currentHP;
			}
		}
		float nowHP = lerp(static_cast<float>(prevHP), static_cast<float>(currentHP), MathHelper::SmoothStep(hpOffset));
		miniGameHP->SetFillPercent(nowHP / static_cast<float>(maxHP));

		float radAngle = angle * XM_PI / 180;  // �������� ��ȯ = ���� * PI / 180
		Vector3 moveArrowPos{ radius * cos(radAngle), radius * sin(radAngle) - 40, 0.2f };
		miniGameArrow->SetPosition(moveArrowPos);

		// �̰� ��� �������� �������ϴ� ���̶�..
		if (currentHP <= 0 || miniGameCount >= maxIterations)
		{
			// ����
			nowState = BarState::OFFER;
			if (correctCount != 3)
				miniGameSuccess = false;
			// �̴ϰ��� â�� �ݽ��ϴ�..
			changeFirst = true;
			shakeCanvas->SetEnable(false);
		}

		// ���� ����
		if (isTurning)
		{
			angle += static_cast<int>(angleTheta * tick * 2.f);
			if (angle >= 630)
			{
				angle = startAngle;
				miniGameCount++;
				currentHP -= 10;
				// �� �� �ִ� �ִ� Ƚ�� 8
				sound->PlaySFXSound("MiniGameF", 0.5f);
			}

			// Ű �Է� ����
			if (input->GetInputState('F', KeyState::DOWN))
			{
				constexpr float distanceThreshold = 50.0f; // ���� �Ÿ� ���� ���� (���ϴ� ������ ����)
				bool failureOccurred = true; // ��ġ���� �ʴ� ��ġ�� ã�Ҵ��� ���θ� ��Ÿ���� ����

				buttonClick = true;

				// �¾ҳ�..!
				for (int i = correctCount; i < 3; i++)
				{
					const auto& circlePos = uiCirclePos[i];
					float distance = sqrt(pow(circlePos.x - moveArrowPos.x, 2) + pow(circlePos.y - moveArrowPos.y, 2));

					if (distance <= distanceThreshold)
					{
						if (i == correctCount)
						{
							sound->PlaySFXSound("MiniGameS", 0.5f);
							StartRotation(0);

							// �¾Ҵ�!
							// CorrectCount�� ���� ó���� ���ݴϴ�.
								// ����? �ѹ��� �ض� ����..
							// ���� �ִ� �������� -> ������ ȸ��
								// Select���� ��ġ�� �ö󰡾���.
							switch (correctCount)
							{
								// ù ��° ����
							case 0:
							{
								// ������ �ΰ��� ���� �÷��ݴϴ�.
								selectPlay[correctCount] = true;
								break;
							}
							// �� ��° ����
							case 1:
							{
								// ������ �Ѱ��� ���� �÷��ݴϴ�.
								selectPlay[correctCount] = true;

								break;
							}
							// �� ��° ����
							case 2:
							{
								// ������ �Ѱ��� ���� �÷��ݴϴ�.
								selectPlay[correctCount] = true;
								// ���� ������ �ǰ���
								isTurning = false;

								break;
							}

							default:
								break;
							}

							correctCount++;
							// �����Ѱ� �ƴ�
							failureOccurred = false;

							break;
						}
						else
						{
							// �Ÿ��� ���Դµ� �ٸ� ���̴�.
							StartRotation(i + 1);
						}
					}
				}

				if (failureOccurred == true)
				{
					sound->PlaySFXSound("MiniGameF", 0.5f);
					currentHP -= 10;
				}
			}
		}
		else
		{
			// ���⼭ �����ٴ� ���� �� ����ٴ� or ü���� �پ��ٴ� ���Դϴ�.
			// ������ �ִϸ��̼��� ���ᰡ �ɶ�.
			if (shakePlay[0] == false)
			{
				// ����
				nowState = BarState::OFFER;
				if (correctCount != 3)
				{
					sound->PlaySFXSound("MiniGameCompleteF", 0.5f);
					miniGameSuccess = false;
				}
				else
				{
					sound->PlaySFXSound("MiniGameCompleteS", 0.5f);
					miniGameSuccess = true;
				}
				// �̴ϰ��� â�� �ݽ��ϴ�..
				changeFirst = true;
				shakeCanvas->SetEnable(false);
			}
		}

		// ��ư�� ���� ����
		if (buttonClick == true)
		{
			fTick += tick;
			if (fTick > 0.1f)
			{
				buttonClick = false;
				fTick = 0.f;
			}
			FSprite->SetTexture(resourceManager->LoadPrefabTexture(L"FButtonPress"));
		}
		else
			FSprite->SetTexture(resourceManager->LoadPrefabTexture(L"FButton"));

		if (selectPlay[0] == true)
		{
			selectOffset[0] -= tick * 2.0f;

			if (selectOffset[0] < 0.f)
			{
				selectPlay[0] = false;
				selectOffset[0] = 0.f;
				miniGameIcon[0]->SetEnable(false);
				miniGameSelect[0]->SetEnable(false);
				miniGameNumber[0]->SetEnable(false);
				miniGameSelect[1]->SetPosition(uiPos[0]);
				miniGameSelect[2]->SetPosition(uiPos[1]);
			}

			PlayScaleAnim(tick, 0);
			miniGameIcon[0]->SetMaskAlpha(selectOffset[0]);
			miniGameSelect[0]->SetMaskAlpha(selectOffset[0]);
			miniGameSelect[1]->SetPosition(Vector3::Lerp(uiPos[0], uiPos[1], MathHelper::SmoothStep(selectOffset[0])));
			miniGameSelect[2]->SetPosition(Vector3::Lerp(uiPos[1], uiPos[2], MathHelper::SmoothStep(selectOffset[0])));
		}

		if (selectPlay[1] == true)
		{
			// ������ ���� �������� Ȯ��
				// �ȳ������� ���� ����
			if (selectPlay[0] == true)
			{
				selectPlay[0] = false;
				selectOffset[0] = 0.f;
				miniGameIcon[0]->SetEnable(false);
				miniGameSelect[0]->SetEnable(false);
				miniGameNumber[0]->SetEnable(false);
				miniGameSelect[1]->SetPosition(uiPos[0]);
				miniGameSelect[2]->SetPosition(uiPos[1]);
			}
			else
			{
				selectOffset[1] -= tick * 2.0f;

				if (selectOffset[1] < 0.f)
				{
					selectPlay[1] = false;
					selectOffset[1] = 0.f;
					miniGameIcon[1]->SetEnable(false);
					miniGameSelect[1]->SetEnable(false);
					miniGameNumber[1]->SetEnable(false);
					miniGameSelect[2]->SetPosition(uiPos[0]);
				}

				PlayScaleAnim(tick, 1);
				miniGameIcon[1]->SetMaskAlpha(selectOffset[1]);
				miniGameSelect[1]->SetMaskAlpha(selectOffset[1]);
				miniGameSelect[2]->SetPosition(Vector3::Lerp(uiPos[0], uiPos[1], MathHelper::SmoothStep(selectOffset[1])));
			}
		}

		if (selectPlay[2] == true)
		{
			// ������ ���� �������� Ȯ��
				// �ȳ������� ���� ����
			if (selectPlay[1] == true)
			{
				selectPlay[1] = false;
				selectOffset[1] = 0.f;
				miniGameIcon[1]->SetEnable(false);
				miniGameSelect[1]->SetEnable(false);
				miniGameNumber[1]->SetEnable(false);
				miniGameSelect[2]->SetPosition(uiPos[0]);
			}
			else
			{
				selectOffset[2] -= tick * 2.0f;

				if (selectOffset[2] < 0.f)
				{
					selectPlay[2] = false;
					selectOffset[2] = 0.f;
					miniGameIcon[2]->SetEnable(false);
					miniGameSelect[2]->SetEnable(false);
					miniGameNumber[2]->SetEnable(false);
				}

				PlayScaleAnim(tick, 2);
				miniGameIcon[2]->SetMaskAlpha(selectOffset[2]);
				miniGameSelect[2]->SetMaskAlpha(selectOffset[2]);
			}
		}

		// �⺻������ �ִϸ��̼� ������Ʈ�� ��� ������մϴ�.
		for (int i = 0; i < 4; i++)
			PlayRotation(tick, i);
	}

	void BarCoreController::OfferUpdate(float tick)
	{
		if (changeFirst == true)
		{
			resultCanvas->SetEnable(true);
			changeFirst = false;

			// ���⵵ ���ķ� ���� �Ǿ����� ���ڴ�.
			spriteAlpha = 0.f;

			// �ؽ��� ����
			// Ĭ���Ͽ� ���� �ؽ��� ������ ���ݴϴ�.
			SetMixedCockTailTexture();
			// �ʱ�ȭ �����ؼ� ��ư ���
			offerButton->SetEnable(false);

			// ������ 0���� ó�����ݽô�.
			mixCockTail->SetMaskAlpha(0.f);
			mixCockTailName->SetMaskAlpha(0.f);

			displayResult = false;
			mixTick = 0.f;

			mixBG->SetEnable(true);
		}

		// �� �ߴ� �Ŷ� ����ϰ� ���� ����.
		if (displayResult != true)
		{
			auto tex = mixCockTail->GetTexture();
			float texWidth = tex->GetWidth();
			float texHeight = tex->GetHeight();

			mixTick += tick;
			if (mixTick >= 1.5f)
			{
				mixTick = 0.f;
				displayResult = true;
				mixBG->SetEnable(false);
			}
			else if (mixTick >= 1.f)
			{
				mixCockTail->SetMaskAlpha(1.f);
				mixCockTail->SetWidth(texWidth);
				mixCockTail->SetHeight(texHeight);
			}
			else
			{
				mixCockTail->SetMaskAlpha(MathHelper::SmoothStep(mixTick));
				mixCockTail->SetWidth(lerp(texWidth + (texWidth * 0.5f), texWidth, mixTick));
				mixCockTail->SetHeight(lerp(texHeight + (texHeight * 0.5f), texHeight, mixTick));
			}
		}
		else
		{
			if (spriteAlpha < 1.0f)
			{
				spriteAlpha += tick * alphaSpeed;
				// ��� ���濡 Smooth Step�� �־��..
				if (spriteAlpha >= 1.0f)
				{
					// ���⼭ ��� �ֵ��� ���ݴϴ� -> �ѹ��� �Ҹ��� ����
					spriteAlpha = 1.0f;
					offerButton->SetEnable(true);
				}
				//spriteAlpha = SmoothStep(spriteAlpha);

				//mixCockTail->SetMaskAlpha(SmoothStep(spriteAlpha));
				mixCockTailName->SetMaskAlpha(MathHelper::SmoothStep(spriteAlpha));
			}
		}
	}

	void BarCoreController::ByeUpdate(float tick)
	{
		if (changeFirst == true)
		{
			changeFirst = false;
			AllButtonEnable(false);

			dialogIdx = 0;

			// ���� ���¿� ���� �з�..
				// Ŭ������ �̹� ���� �־����.
			switch (resultState)
			{
			case ResultState::GOOD:
			{
				auto tex = resourceManager->LoadPrefabTexture(nowCustomer->goodTalk[dialogIdx].second);
				customerSprite->SetTexture(tex);
				customerSprite->SetWidth(tex->GetWidth());
				customerSprite->SetHeight(tex->GetHeight());

				dialogText->SetStyleText(nowCustomer->goodTalk[dialogIdx].first);
				break;
			}
			case ResultState::BAD:
			{
				auto tex = resourceManager->LoadPrefabTexture(nowCustomer->badTalk[dialogIdx].second);
				customerSprite->SetTexture(tex);
				customerSprite->SetWidth(tex->GetWidth());
				customerSprite->SetHeight(tex->GetHeight());

				dialogText->SetStyleText(nowCustomer->badTalk[dialogIdx].first);
				break;
			}
			case ResultState::BEER:
			{
				auto tex = resourceManager->LoadPrefabTexture(nowCustomer->beerTalk[dialogIdx].second);
				customerSprite->SetTexture(tex);
				customerSprite->SetWidth(tex->GetWidth());
				customerSprite->SetHeight(tex->GetHeight());

				dialogText->SetStyleText(nowCustomer->beerTalk[dialogIdx].first);
				break;
			}
			case ResultState::NONE:
			default:
			{
				break;
			}
			}

			spriteAlpha = 1.0f;
		}

		// ���� ���⼭ �������� ���ְ� �ʹ�.
		nowGold->SetText(std::to_wstring(gameManager->GetCurrentMoney()));

		// ������� ������ �ְ� �ʹ�.
			// ����Ʈ �ε��� �÷��ֱ� -> �̰� �ƿ� ������� ����.
		if (startBye != true)
		{
			if (input->GetInputState(VK_LBUTTON, KeyState::DOWN))
			{
				// ���� ���¿� ���� �б�.
				dialogIdx++;

				switch (resultState)
				{
				case ResultState::GOOD:
				{
					if (dialogIdx < nowCustomer->goodTalk.size())
					{
						auto tex = resourceManager->LoadPrefabTexture(nowCustomer->goodTalk[dialogIdx].second);
						customerSprite->SetTexture(tex);
						customerSprite->SetWidth(tex->GetWidth());
						customerSprite->SetHeight(tex->GetHeight());

						dialogText->SetStyleText(nowCustomer->goodTalk[dialogIdx].first);
					}
					else
					{
						sound->PlaySFXSound("Cashier", 0.5f);

						// �� �ø���
						int nowMoney = gameManager->GetCurrentMoney();
						nowMoney += nowCocktail->amount * 1.5f;
						gameManager->SetCurrentMoney(nowMoney);

						resultState = ResultState::BYE;
						dialogIdx = 0;
						// ù �� ����ֱ�.
						auto tex = resourceManager->LoadPrefabTexture(nowCustomer->byeTalk[dialogIdx].second);
						customerSprite->SetTexture(tex);
						customerSprite->SetWidth(tex->GetWidth());
						customerSprite->SetHeight(tex->GetHeight());

						dialogText->SetStyleText(nowCustomer->byeTalk[dialogIdx].first);
					}
					break;
				}
				case ResultState::BAD:
				{
					if (dialogIdx < nowCustomer->badTalk.size())
					{
						auto tex = resourceManager->LoadPrefabTexture(nowCustomer->badTalk[dialogIdx].second);
						customerSprite->SetTexture(tex);
						customerSprite->SetWidth(tex->GetWidth());
						customerSprite->SetHeight(tex->GetHeight());

						dialogText->SetStyleText(nowCustomer->badTalk[dialogIdx].first);
					}
					else
					{
						sound->PlaySFXSound("Cashier", 0.5f);

						// �� �ø���
						int nowMoney = gameManager->GetCurrentMoney();
						nowMoney += nowCocktail->amount;
						gameManager->SetCurrentMoney(nowMoney);
						resultState = ResultState::BYE;
						dialogIdx = 0;
						// ù �� ����ֱ�.
						auto tex = resourceManager->LoadPrefabTexture(nowCustomer->byeTalk[dialogIdx].second);
						customerSprite->SetTexture(tex);
						customerSprite->SetWidth(tex->GetWidth());
						customerSprite->SetHeight(tex->GetHeight());

						dialogText->SetStyleText(nowCustomer->byeTalk[dialogIdx].first);
					}
					break;
				}
				case ResultState::BEER:
				{
					if (dialogIdx < nowCustomer->beerTalk.size())
					{
						auto tex = resourceManager->LoadPrefabTexture(nowCustomer->beerTalk[dialogIdx].second);
						customerSprite->SetTexture(tex);
						customerSprite->SetWidth(tex->GetWidth());
						customerSprite->SetHeight(tex->GetHeight());

						dialogText->SetStyleText(nowCustomer->beerTalk[dialogIdx].first);
					}
					else
					{
						// �� �ø���
						sound->PlaySFXSound("Cashier", 0.5f);
						int nowMoney = gameManager->GetCurrentMoney();
						nowMoney += 20;
						gameManager->SetCurrentMoney(nowMoney);
						resultState = ResultState::BYE;
						dialogIdx = 0;
						// ù �� ����ֱ�.
						auto tex = resourceManager->LoadPrefabTexture(nowCustomer->byeTalk[dialogIdx].second);
						customerSprite->SetTexture(tex);
						customerSprite->SetWidth(tex->GetWidth());
						customerSprite->SetHeight(tex->GetHeight());

						dialogText->SetStyleText(nowCustomer->byeTalk[dialogIdx].first);
					}
					break;
				}
				case ResultState::BYE:
				{
					// ���� �� ���̸� ���ݴϴ�.
					if (dialogIdx < nowCustomer->byeTalk.size())
					{
						auto tex = resourceManager->LoadPrefabTexture(nowCustomer->byeTalk[dialogIdx].second);
						customerSprite->SetTexture(tex);
						customerSprite->SetWidth(tex->GetWidth());
						customerSprite->SetHeight(tex->GetHeight());

						dialogText->SetStyleText(nowCustomer->byeTalk[dialogIdx].first);
					}
					else
					{
						// ��ȭâ �ݱ�
						AllCustomerUIEnable(false);
						// ������� ������ ���ָ鼭.
						startBye = true;
					}
				}

				case ResultState::NONE:
				default:
				{
					break;
				}
				}
			}
		}
		else
		{
			// ������� ����
			if (spriteAlpha > 0.0f)
			{
				spriteAlpha -= tick * alphaSpeed;
				if (spriteAlpha <= 0.0f)
				{
					// ���⼭ ��� �ֵ��� ���ݴϴ� -> �ѹ��� �Ҹ��� ����
					spriteAlpha = 0.0f;

					// �Խ�Ʈ idx�� �ø��ϴ�. ���µ��� �ʱ�ȭ �մϴ�.
					// ���� idx�� ��ġ�� ���� ���� ���� �����մϴ�.
					nowGuestIdx++;

					// ���� ���ǵ� �� false�� �� ����
					isTurning = false;
					isCorrectCockTail = false;
					startBye = false;
					miniGameSuccess = false;

					// ���⼭ nowCustomer�� ����
					if (nowGuestIdx < 3)
					{
						// �ٽ� ��Ʈ�η� �Ѿ.
						nowCustomer = gameManager->GetCustomer(guest[nowGuestIdx]);
						resultState = ResultState::NONE;
						nowCocktail = nullptr;


						nowState = BarState::INTRO;
						changeFirst = true;
					}
					else
					{
						// Ŭ����� �Ѿ�� ��.
						nowState = BarState::CLOSE;
						changeFirst = true;
					}
				}
				//spriteAlpha = MathHelper::SmoothStep(spriteAlpha);

				customerSprite->SetMaskAlpha(MathHelper::SmoothStep(spriteAlpha));
			}
		}
	}

	void BarCoreController::CloseUpdate(float tick)
	{
		// Ŭ����� �Ѿ� ������ �κ�� �׳� �ٷ� ���ô�.
		if (changeFirst == true)
		{
			SceneManager::GetInstance()->LoadScene(L"LobbyScene");
			barCanvas->SetEnable(false);
			shakeCanvas->SetEnable(false);
			resultCanvas->SetEnable(false);

			// ũ����Ʈ �Ϸ� ǥ�ø� �մϴ�.
			gameManager->SetCraftDone(true);
		}
	}

	void BarCoreController::AllButtonEnable(bool isEnable)
	{
		for (auto& stuff : stuffButtons)
			stuff->SetButtonEnable(isEnable);

		mixButton->SetButtonEnable(isEnable);
		resetButton->SetButtonEnable(isEnable);
		beerButton->SetButtonEnable(isEnable);
		//closeButton->SetButtonEnable(isEnable);

		for (auto& recipeHover : recipeHoverButtons)
			recipeHover->SetButtonEnable(isEnable);

		for (auto& recipe : recipeTitleButton)
			recipe->SetButtonEnable(isEnable);

		recipeButton->SetButtonEnable(isEnable);

		// �� ��쿡 Ŀ���� ���ݽô�.
		if (isEnable == false)
		{
			UIManager::GetInstance()->GetCanvas("CursorCanvas")->GetSpriteUI("Info")->SetEnable(false);
			UIManager::GetInstance()->GetCanvas("CursorCanvas")->GetTextUI("InfoText")->SetEnable(false);
		}
	}

	void BarCoreController::AllStuffButtonEnable(bool isEnable)
	{
		for (auto& stuff : stuffButtons)
			stuff->SetButtonEnable(isEnable);

		// �� ��쿡 Ŀ���� ���ݽô�.
		if (isEnable == false)
		{
			UIManager::GetInstance()->GetCanvas("CursorCanvas")->GetSpriteUI("Info")->SetEnable(false);
			UIManager::GetInstance()->GetCanvas("CursorCanvas")->GetTextUI("InfoText")->SetEnable(false);
		}
	}

	void BarCoreController::AllRecipeTitleButtonEnable(bool isEnable)
	{
		// ������ϱ� ������ isEnable��
		for (auto& recipe : recipeTitleButton)
			recipe->SetEnable(isEnable);
	}

	void BarCoreController::AllRecipeSpriteDisable()
	{
		for (auto& recipe : recipeSprite)
			recipe->SetEnable(false);
	}

	void BarCoreController::AllCustomerUIEnable(bool isEnable)
	{
		customerName->SetEnable(isEnable);
		dialogText->SetEnable(isEnable);
		requestSprite->SetEnable(isEnable);
	}

	void BarCoreController::MiniGameSetUp()
	{
		// ������ ����.
			// ���� ����
		GenerateCircleRandomPos();

		// �ؽ��� ����
		for (int i = 0; i < 3; i++)
		{
			auto tmpTex = GetIconTexture(select[i]);
			// �����ٰ� �ٽ� ���� �� �ʱ�ȭ �����ݴϴ�.
			miniGameIcon[i]->SetEnable(true);
			miniGameIcon[i]->SetTexture(tmpTex);
			miniGameIcon[i]->SetWidth(tmpTex->GetWidth());
			miniGameIcon[i]->SetHeight(tmpTex->GetHeight());
			// �������� ���� ��ġ�� ����� �մϴ�..
			miniGameIcon[i]->SetPosition(uiCirclePos[i]);
			miniGameIcon[i]->SetMaskColor({ 1.0f, 1.0f, 1.0f, 1.0f });

			miniGameSelect[i]->SetTexture(GetIconTexture(select[i]));
			miniGameSelect[i]->SetWidth(tmpTex->GetWidth());
			miniGameSelect[i]->SetHeight(tmpTex->GetHeight());
			miniGameSelect[i]->SetPosition(uiPos[i]);
			miniGameSelect[i]->SetEnable(true);

			miniGameNumber[i]->SetPosition(uiCirclePos[i] + Vector3(0.f, 70.f, 0.f));
		}

		// ���� Ĭ������ �´� Ĭ�����̳�?
		if (isCorrectCockTail == true)
		{
			prevHP = 80;
			currentHP = 80;
		}
		else
		{
			prevHP = 60;
			currentHP = 60;
		}

		maxHP = 80;

		miniGameHP->SetFillPercent(static_cast<float>(currentHP) / static_cast<float>(maxHP));

		angle = 270.f;

		// ��� ������ �����ݽô�.
		miniGameCount = 0;
		correctCount = 0;

		float radAngle = angle * XM_PI / 180;  // �������� ��ȯ = ���� * PI / 180
		Vector3 moveArrowPos{ radius * cos(radAngle), radius * sin(radAngle) - 40, 0.2f };
		miniGameArrow->SetPosition(moveArrowPos);

		// �����ϸ� ��������.
			// ī��Ʈ ���Ŀ� �����ô�.
		//isTurning = true;
	}

	const std::shared_ptr<Texture>& BarCoreController::GetIconTexture(int idx)
	{
		switch (idx)
		{
		case 1:
			return resourceManager->LoadPrefabTexture(L"I_rum");

		case 2:
			return resourceManager->LoadPrefabTexture(L"I_vodka");

		case 3:
			return resourceManager->LoadPrefabTexture(L"I_gin");

		case 4:
			return resourceManager->LoadPrefabTexture(L"I_liqueur");

		case 5:
			return resourceManager->LoadPrefabTexture(L"I_juice");

		case 6:
			return resourceManager->LoadPrefabTexture(L"I_soda");

		case 7:
			return resourceManager->LoadPrefabTexture(L"I_fruit");

		default:
			return nullptr;
		}
	}

	void BarCoreController::SetCustomerPosition()
	{
		//��, ����, �� ���� ��ġ ����
		switch (nowCustomerIdx)
		{
		case 0:
			customerSprite->SetPosition(Vector3{ 0.f, -44.f, 0.6f });
			break;

		case 1:
			customerSprite->SetPosition(Vector3{ 0.f, -91.f, 0.6f });
			break;

		case 2:
			customerSprite->SetPosition(Vector3{ 0.f, -45.f, 0.6f });
			break;


		default:
			break;

		}
	}

	void BarCoreController::GenerateCircleRandomPos()
	{
		float degree[3] = { 0.f, 0.f, 0.f };
		float randDegree = gameManager->CreateRandomFloat(0.f, 360.f);
		while (randDegree < 30.f)
		{
			randDegree = gameManager->CreateRandomFloat(0.f, 360.f);
		}
		degree[0] = randDegree;

		// ��� ���̰� 20�� �̻��� ������ ���ڴ�.
		while (abs(degree[0] - randDegree) < 20.f)
		{
			randDegree = gameManager->CreateRandomFloat(0.f, 360.f);
		}
		degree[1] = randDegree;

		while (abs(degree[0] - randDegree) < 20.f || abs(degree[1] - randDegree) < 20.f)
		{
			randDegree = gameManager->CreateRandomFloat(0.f, 360.f);
		}
		degree[2] = randDegree;

		for (int i = 0; i < 3; i++)
		{
			float rad = degree[i] * XM_PI / 180.0f;
			Vector3 _uiPos = Vector3{ radius * cos(rad), radius * sin(rad) - 45.f, 0.25f };

			uiCirclePos[i] = _uiPos;
		}
	}

	void BarCoreController::SetMixedCockTailTexture()
	{

		// �ͽ��� �� �� �̹� ���õǾ� �ִ�..! Ĭ������

		// �⺻������ �� ������ ���ص�..
		auto cocktailTex = resourceManager->LoadPrefabTexture(nowCocktail->cocktailNameEN);
		if (cocktailTex != nullptr)
		{
			mixCockTail->SetTexture(cocktailTex);
			mixCockTail->SetWidth(cocktailTex->GetWidth());
			mixCockTail->SetHeight(cocktailTex->GetHeight());
		}

		if (miniGameSuccess == true && isCorrectCockTail == true)
		{
			// ���������� Ĭ���Ͽ� ���� �ؽ��� ����
			mixCockTailName->SetTexture(resourceManager->LoadPrefabTexture(L"Name_" + nowCocktail->cocktailNameEN));
			mixCockTail->SetMaskColor({ 1.0f, 1.0f, 1.0f, 1.f });

			// ���⼭ ����� �������ݴϴ�.
			resultState = ResultState::GOOD;
		}
		else
		{
			// ��� ������ ������ Ĭ����.
			mixCockTail->SetMaskColor({ 0.6f, 0.6f, 0.6f, 1.f });
			// �̸��� ������ ������ Ĭ����..
			mixCockTailName->SetTexture(resourceManager->LoadPrefabTexture(L"Name_Fail"));

			// ���⼭ ����� �������ݴϴ�.
			resultState = ResultState::BAD;
		}
	}

	void BarCoreController::PlayRotation(float tick, int idx)
	{
		// ���� duration ����
			// lerp ����� 0 ~ 1�� �ݺ��ϸ�
			// Ư�� �������� Ư�� �������� �Դ� ���ٰ� �������� ��.
		if (shakePlay[idx] == true)
		{
			shakeTick[idx] += tick;
			shakeOffset[idx] += shakeTick[idx] * shakeSpeed[idx];
			// ��ȣ�� �ٲ��ݴϴ�.
			if (shakeOffset[idx] < 0.f || shakeOffset[idx] > 1.f)
				shakeSpeed[idx] *= -1.f;

			if (shakeTick[idx] > 0.3f)
			{
				shakeTick[idx] = 0.f;

				if (idx == 0)
					shaker->SetRotation(0.f);
				else
					miniGameIcon[idx - 1]->SetRotation(0.f);

				shakePlay[idx] = false;
			}
			else
			{

				if (idx == 0)
					shaker->SetRotation(lerp(-20.f, 20.f, MathHelper::SmoothStep(shakeOffset[0])));
				else
					miniGameIcon[idx - 1]->SetRotation(lerp(-10.f, 10.f, MathHelper::SmoothStep(shakeOffset[idx])));
			}
		}
	}

	void BarCoreController::PlayScaleAnim(float tick, int idx)
	{
		selectTick[idx] += tick;

		auto texture = miniGameIcon[idx]->GetTexture();
		if (texture == nullptr)
			return;

		float texWidth = texture->GetWidth();
		float texHeight = texture->GetHeight();

		if (selectTick[idx] > 0.2f)
		{
			// 0���� 1��
			miniGameIcon[idx]->SetWidth(lerp(texWidth + (texWidth * -0.2f), texWidth, MathHelper::SmoothStep((selectTick[idx] - 0.2f) * 10.f)));
			miniGameIcon[idx]->SetHeight(lerp(texHeight + (texHeight * 0.2f), texHeight, MathHelper::SmoothStep((selectTick[idx] - 0.2f) * 10.f)));

			// ���� ���ݾ� �÷��ݴϴ�.
			auto pos = miniGameIcon[idx]->GetPosition();
			miniGameIcon[idx]->SetPosition(Vector3::Lerp(pos, Vector3(pos.x, pos.y - 10.f, pos.z), MathHelper::SmoothStep(selectTick[idx] - 0.2f)));
		}
		else if (selectTick[idx] > 0.1f)
		{
			// 0���� 1�� (�� ���¿��� ���ϴ� ������)
			miniGameIcon[idx]->SetWidth(lerp(texWidth + (texWidth * 0.1f), texWidth + (texWidth * -0.2f), MathHelper::SmoothStep((selectTick[idx] - 0.1f) * 10.f)));
			miniGameIcon[idx]->SetHeight(lerp(texHeight + (texHeight * -0.1f), texHeight + (texHeight * 0.2f), MathHelper::SmoothStep((selectTick[idx] - 0.1f) * 10.f)));
		}
		else
		{
			// 0���� 1��
			miniGameIcon[idx]->SetWidth(lerp(texWidth, texWidth + (texWidth * 0.1f), MathHelper::SmoothStep(selectTick[idx] * 10.f)));
			miniGameIcon[idx]->SetHeight(lerp(texHeight, texHeight + (texHeight * -0.1f), MathHelper::SmoothStep(selectTick[idx] * 10.f)));
		}
	}

	void BarCoreController::StartRotation(int idx)
	{
		// ���������� ���� ���͸� �����ݴϴ�.
		shakePlay[idx] = true;
		shakeTick[idx] = 0.f;
		shakeOffset[idx] = 0.f;
		shakeSpeed[idx] = 6.f;

		if (idx == 0)
			shaker->SetRotation(0.f);
		else
			miniGameIcon[idx - 1]->SetRotation(0.f);
	}

	void BarCoreController::SetSelect(int idx)
	{
		shared_ptr<Texture> tex;

		// idx�� ���� �޶����� �κе鸸 �������ݴϴ�.
		switch (idx)
		{
		case 1:
		{
			selectText[selectIdx]->SetText(L"RUM");
			tex = resourceManager->LoadPrefabTexture(L"I_rum");
			break;
		}
		case 2:
		{
			selectText[selectIdx]->SetText(L"VODKA");
			tex = resourceManager->LoadPrefabTexture(L"I_vodka");
			break;
		}
		case 3:
		{
			selectText[selectIdx]->SetText(L"GIN");
			tex = resourceManager->LoadPrefabTexture(L"I_gin");
			break;
		}
		case 4:
		{
			selectText[selectIdx]->SetText(L"LIQUOR");
			tex = resourceManager->LoadPrefabTexture(L"I_liqueur");
			break;
		}
		case 5:
		{
			selectText[selectIdx]->SetText(L"JUICE");
			tex = resourceManager->LoadPrefabTexture(L"I_juice");
			break;
		}
		case 6:
		{
			selectText[selectIdx]->SetText(L"SODA");
			tex = resourceManager->LoadPrefabTexture(L"I_soda");
			break;
		}
		case 7:
		{
			selectText[selectIdx]->SetText(L"FRUIT");
			tex = resourceManager->LoadPrefabTexture(L"I_fruit");
			break;
		}
		default:
		{
			selectText[selectIdx]->SetText(L"");
			tex = resourceManager->LoadPrefabTexture(L"AlertEffect");
			break;
		}
		}

		// ������ => ���� ���ɼ��� �ֱ� ������ ����ݴϴ�.
		selectText[selectIdx]->SetPosition({ 485.0f + 180.f * selectIdx, 283.0f, 0.45f });
		selectText[selectIdx]->SetFixedWidth(70.f);
		selectText[selectIdx]->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });

		selectSprite[selectIdx]->SetTexture(tex);
		selectSprite[selectIdx]->SetWidth(tex->GetWidth() * 0.35f);
		selectSprite[selectIdx]->SetHeight(tex->GetHeight() * 0.35f);
		selectSprite[selectIdx]->SetEnable(true);
	}

	void BarCoreController::ResetSelect()
	{
		for (int i = 0; i < 3; i++)
		{
			selectText[i]->SetText(L"");
			selectText[i]->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
			selectSprite[i]->SetEnable(false);
		}
	}

	void BarCoreController::SetNotFoundRecipe()
	{
		bool isHas = false;
		int determine = select[0] * 10000 + select[1] * 100 + select[2];
		for (int num : checkIngredient)
		{
			if (num == determine)
			{
				isHas = true;
				break;
			}
		}

		if (isHas == false)
		{
			for (int i = 0; i < 3; i++)
			{
				selectSprite[i]->SetEnable(false);
				selectText[i]->SetUsingFixedWidth(false);
				selectText[i]->SetColor({ powf((255.f / 255.f), 2.2f), powf((255.f / 255.f), 2.2f), powf((89.f / 255.f), 2.2f), 1.f });

				if (i == 0)
				{
					selectText[i]->SetText(L"RECIPE");
					selectText[i]->SetPosition(Vector3{ 492.0f, 283.0f, 0.45f });
				}
				else if (i == 1)
				{
					selectText[i]->SetText(L"NOT");
					selectText[i]->SetPosition(Vector3{ 662.0f, 283.0f, 0.45f });
				}
				else
				{
					selectText[i]->SetText(L"FOUND");
					selectText[i]->SetPosition(Vector3{ 844.0f, 283.0f, 0.45f });
				}
			}
		}
	}
}
