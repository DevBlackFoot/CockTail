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
		// 배열들 세팅
			// 매번 생성할 때 마다 새로 세팅 되기 때문에..
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

		// 칵테일 조합법 24개 세팅
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
			// 1에서 0으로 가는 오프셋
			selectOffset[i] = 1.f;
			selectTick[i] = 0.f;
		}
	}

	BarCoreController::~BarCoreController()
	{
	}

	void BarCoreController::Awake()
	{
		// 각종 캐싱을 해줍니다.
		barCanvas = UIManager::GetInstance()->GetCanvas("BarCanvas");
		shakeCanvas = UIManager::GetInstance()->GetCanvas("ShakeCanvas");
		resultCanvas = UIManager::GetInstance()->GetCanvas("ResultCanvas");
		resourceManager = ResourceManager::GetInstance();
		sound = SoundManager::GetInstance();

		input = InputManager::GetInstance();
		inventoryMap = gameManager->GetInventoryItemDataMap();

		// 버튼들을 캐싱합니다.
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

		// 고객 세팅
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

		// 호버 버튼
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
		// 미니 게임 관련 스프라이트 세팅
			// + 3개라서 레시피 선택 관련 텍스트, 스프라이트 세팅
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

		// 체력
		miniGameHP = shakeCanvas->GetProgressBarUI("MiniGameHp");
		miniGameArrow = shakeCanvas->GetSpriteUI("MiniGameArrow");

		// 0 - Rum, 1 - Vodka, 2 - Gin
		recipeSprite.resize(3);
		recipeSprite[0] = barCanvas->GetSpriteUI("RumRecipeSpriteUI");
		recipeSprite[1] = barCanvas->GetSpriteUI("VodkaRecipeSpriteUI");
		recipeSprite[2] = barCanvas->GetSpriteUI("GinRecipeSpriteUI");

		// 레시피 타이틀
		recipeTitleButton.resize(3);
		recipeTitleButton[0] = barCanvas->GetButtonUI("RumRecipeButton");
		recipeTitleButton[1] = barCanvas->GetButtonUI("VodkaRecipeButton");
		recipeTitleButton[2] = barCanvas->GetButtonUI("GinRecipeButton");

		recipeButton = barCanvas->GetButtonUI("CocktailRecipeButton");
		recipeBackGround = barCanvas->GetSpriteUI("RecipeBackGroundSpriteUI");

		// 스프라이트
		mixCockTail = resultCanvas->GetSpriteUI("MixCockTailSpriteUI");
		mixCockTailName = resultCanvas->GetSpriteUI("MixCockTailNameUI");
		mixBG = resultCanvas->GetSpriteUI("MixFirstBG");
		offerButton = resultCanvas->GetButtonUI("OfferButton");
	}

	void BarCoreController::Start()
	{
		// 튜토리얼 실행 여부에 따라 현재의 State를 변경해줍니다.
		if (gameManager->isFirstCraft == true)
		{
			// 첫 크래프트를 하지 않았을 때 (바에서 로비로 돌아가면 false가 됩니다.)
			nowState = BarState::TUTORIAL;
			// 얘는 update에서 지속적으로 바꿔주면 안됩니다.
			changeFirst = true;
		}
		else
		{
			nowState = BarState::INTRO;
			changeFirst = true;
		}

		// 게스트 순서를 세팅해줍니다.
		// 처음은 그냥 넣어줍니다.
		int rand = gameManager->CreateRandomInt(0, 100) % 3;
		guest[0] = rand;

		// 두 번째는 다를때 까지..
		while (guest[0] == rand)
		{
			rand = gameManager->CreateRandomInt(0, 100) % 3;
		}
		guest[1] = rand;

		// 세 번째도 다를 때 까지.
		// 두 번째는 다를때 까지..
		while (guest[0] == rand || guest[1] == rand)
		{
			rand = gameManager->CreateRandomInt(0, 100) % 3;
		}
		guest[2] = rand;

		// 세 번 해줍니다.
		for (int i = 0; i < 3; i++)
		{
			// a, b 선택
			rand = gameManager->CreateRandomInt(0, 100) % 2;
			// 0, 1, 2에서 0/1, 2/3, 4/5가 됩니다.
			guest[i] = guest[i] * 2 + rand;
		}

		// 첫 손님 세팅
			// nowGuestIdx는 사실상 0이기 때문에..
		nowCustomer = gameManager->GetCustomer(guest[nowGuestIdx]);

		// 버튼 세팅 및 캔버스 세팅을 해줍니다.
		barCanvas->SetEnable(true);
		SetAllButtonEvent();

		// 돈을 가져와줍시다.
		nowGold->SetText(std::to_wstring(gameManager->GetCurrentMoney()));

		// Amount 위치를 세팅해줍니다.
		for (int i = 0; i < 7; i++)
		{
			// 기본적으로 숫자도 가져와 줍시다..
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

		// 업데이트를 돌면서 갱신을 해줍니다..
	}

	void BarCoreController::Update(float tick)
	{
		// 현재 스테이트에 따라서 업데이트를 해줍니다.
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
		// 필요할 때 사용해줍니다.
		// 레시피 스테이트를 지속적으로 업데이트 해봅니다.
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
		// 각종 버튼에 대한 이벤트를 세팅해봅니다.

		// 믹스 버튼의 경우
			// 체크를 성공하면 미니게임으로
			// 아니면 경고를
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
					// 여기서 현재 칵테일의 특징을 가져옵니다.
						// 특징과 같으면 cock

					// 현재 만들고자 하는 칵테일 정보.
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
					// 일단 돌려줍니다.
					for (int num : select)
					{
						auto item = inventoryMap.find(num);

						if (item != inventoryMap.end())
						{
							item->second->count++;
						}
					}
					// select 초기화.
					for (int& num : select)
					{
						num = 0;
					}

					selectIdx = 0;
					AllStuffButtonEnable(true);
				}
			});

		// 리셋 버튼의 경우
			// 인벤토리에 amount를 돌려줍니다.
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
				// select 초기화.
				for (int& num : select)
				{
					num = 0;
				}

				selectIdx = 0;
				sound->PlaySFXSound("Reset", 0.5f);

				ResetSelect();

				AllStuffButtonEnable(true);
			});

		// Stuff 버튼의 경우
			// 누를 때 마다 인벤토리에 amount 감소
			// 누를 때 마다 idx 증가
			// 결국은 인벤토리의 값을 변경 -> UI는 띄워주기만합니다.

		// 인벤토리에서 찾기 위해
		// 람다 함수 이슈

#pragma region StuffButton
		stuffButtons[0]->SetClickEvent([&]
			{
				// select Idx가 클 때 혹시 누를 수도 있으니까 방지.
				if (selectIdx >= 3)
					return;

				// 럼
				sound->PlaySFXSound("GlassBottle", 0.5f);

				auto item = inventoryMap.find(1);

				if (item != inventoryMap.end())
				{
					item->second->count--;

					// 셀렉트
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
				// select Idx가 클 때 혹시 누를 수도 있으니까 방지.
				if (selectIdx >= 3)
					return;

				sound->PlaySFXSound("GlassBottle", 0.5f);

				auto item = inventoryMap.find(2);

				if (item != inventoryMap.end())
				{
					item->second->count--;

					// 셀렉트
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
				// select Idx가 클 때 혹시 누를 수도 있으니까 방지.
				if (selectIdx >= 3)
					return;

				sound->PlaySFXSound("GlassBottle", 0.5f);

				auto item = inventoryMap.find(3);

				if (item != inventoryMap.end())
				{
					item->second->count--;

					// 셀렉트
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
				// select Idx가 클 때 혹시 누를 수도 있으니까 방지.
				if (selectIdx >= 3)
					return;

				sound->PlaySFXSound("GlassBottle", 0.5f);

				auto item = inventoryMap.find(4);

				if (item != inventoryMap.end())
				{
					item->second->count--;

					// 셀렉트
					if (selectIdx < 3)
					{
						SetSelect(4);

						select[selectIdx] = 4;
						selectIdx++;

						if (selectIdx == 3)
						{
							AllStuffButtonEnable(false);

							// 업데이트에서 돌리지 말고 여기서 돌려보자.
								// 검사까지 다 해줍니다.
							SetNotFoundRecipe();
						}
					}
				}
			});

		stuffButtons[4]->SetClickEvent([&]
			{
				// select Idx가 클 때 혹시 누를 수도 있으니까 방지.
				if (selectIdx >= 3)
					return;

				sound->PlaySFXSound("CanSound", 0.5f);

				auto item = inventoryMap.find(5);

				if (item != inventoryMap.end())
				{
					item->second->count--;

					// 셀렉트
					if (selectIdx < 3)
					{
						SetSelect(5);

						select[selectIdx] = 5;
						selectIdx++;

						if (selectIdx == 3)
						{
							AllStuffButtonEnable(false);

							// 업데이트에서 돌리지 말고 여기서 돌려보자.
							SetNotFoundRecipe();
						}
					}
				}
			});

		stuffButtons[5]->SetClickEvent([&]
			{
				// select Idx가 클 때 혹시 누를 수도 있으니까 방지.
				if (selectIdx >= 3)
					return;

				sound->PlaySFXSound("GlassBottle", 0.5f);

				auto item = inventoryMap.find(6);

				if (item != inventoryMap.end())
				{
					item->second->count--;

					// 셀렉트
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
				// select Idx가 클 때 혹시 누를 수도 있으니까 방지.
				if (selectIdx >= 3)
					return;

				sound->PlaySFXSound("CanSound", 0.5f);

				auto item = inventoryMap.find(7);

				if (item != inventoryMap.end())
				{
					item->second->count--;

					// 셀렉트
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

		// 비어 버튼의 경우
			// 현재 상태에 따라서 손님의 대사를 결정해줍니다.
		beerButton->SetClickEvent([&]
			{
				sound->PlaySFXSound("ClickSounds", 0.5f);

				// 현재 selectIdx가 0이 아니면 반환해 줍니다.
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
					// select 초기화.
					for (int& num : select)
					{
						num = 0;
					}

					selectIdx = 0;

					AllStuffButtonEnable(true);
				}

				// 결과 상태를 Beer로 합니다
				resultState = ResultState::BEER;
				// 바로 bye로
				nowState = BarState::BYE;
				changeFirst = true;
			});

		// Close 버튼의 경우
			// 로비로 돌아갑니다.
			// 이 때 모든 캔버스를 꺼줍니다.
		closeButton->SetClickEvent([&]
			{
				SceneManager::GetInstance()->LoadScene(L"LobbyScene");
				barCanvas->SetEnable(false);
				shakeCanvas->SetEnable(false);
				resultCanvas->SetEnable(false);
			});

		// 호버 버튼의 경우
			// 각 경우에 따른 이벤트를 세팅해줍니다.
			// 람다 함수 이슈로 하드 코딩.
#pragma region RecipeHover
		recipeHoverButtons[0]->SetHoverEvent([&]
			{
				// 현재 레시피 상황 따라서 달라지도록
				switch (nowRecipeState)
				{
				case RecipeState::RUM:
				{
					// 럼은 호버가 1로 시작합니다.
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
					// 보드카는 호버가 2로 시작합니다.
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
					// 진은 호버가 3로 시작합니다.
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
				// 현재 레시피 상황 따라서 달라지도록
				switch (nowRecipeState)
				{
				case RecipeState::RUM:
				{
					// 럼은 호버가 1로 시작합니다.
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
					// 보드카는 호버가 2로 시작합니다.
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
					// 진은 호버가 3로 시작합니다.
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
				// 현재 레시피 상황 따라서 달라지도록
				switch (nowRecipeState)
				{
				case RecipeState::RUM:
				{
					// 럼은 호버가 1로 시작합니다.
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
					// 보드카는 호버가 2로 시작합니다.
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
					// 진은 호버가 3로 시작합니다.
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
				// 현재 레시피 상황 따라서 달라지도록
				switch (nowRecipeState)
				{
				case RecipeState::RUM:
				{
					// 럼은 호버가 1로 시작합니다.
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
					// 보드카는 호버가 2로 시작합니다.
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
					// 진은 호버가 3로 시작합니다.
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
				// 현재 레시피 상황 따라서 달라지도록
				switch (nowRecipeState)
				{
				case RecipeState::RUM:
				{
					// 럼은 호버가 1로 시작합니다.
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
					// 보드카는 호버가 2로 시작합니다.
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
					// 진은 호버가 3로 시작합니다.
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
				// 현재 레시피 상황 따라서 달라지도록
				switch (nowRecipeState)
				{
				case RecipeState::RUM:
				{
					// 럼은 호버가 1로 시작합니다.
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
					// 보드카는 호버가 2로 시작합니다.
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
					// 진은 호버가 3로 시작합니다.
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
				// 현재 레시피 상황 따라서 달라지도록
				switch (nowRecipeState)
				{
				case RecipeState::RUM:
				{
					// 럼은 호버가 1로 시작합니다.
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
					// 보드카는 호버가 2로 시작합니다.
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
					// 진은 호버가 3로 시작합니다.
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
				// 현재 레시피 상황 따라서 달라지도록
				switch (nowRecipeState)
				{
				case RecipeState::RUM:
				{
					// 럼은 호버가 1로 시작합니다.
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
					// 보드카는 호버가 2로 시작합니다.
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
					// 진은 호버가 3로 시작합니다.
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
					// 텍스트와 배경을 없애줍니다.
					infoText->SetStyleText(L"");
					infoBack->SetEnable(false);
				});
		}

		// 제공 버튼의 경우
			// 다음씬으로 넘어갑니다. 현재 상황에 따라 State를 결정해줍니다
			// amount를 올려줍니다.
		offerButton->SetClickEvent([&]
			{
				nowState = BarState::BYE;
				changeFirst = true;
				// 리절트를 닫아준다.
				resultCanvas->SetEnable(false);
			});

		// 레시피 버튼을 세팅해줍니다.
		recipeButton->SetClickEvent([&]
			{
				// 현재 레시피의 상태에 따라..!
				if (isRecipeSet == true)
				{
					sound->PlaySFXSound("RecipeClose", 0.5f);

					// 켜져 있으면 꺼줍시다.
					isRecipeSet = !isRecipeSet;
					// 애니메이션을 Start 해줍니다.
					isAnimating = true;

					// 모든 타이틀을 꺼줍니다.
					AllRecipeTitleButtonEnable(false);
					// 레시피들도 꺼줍니다.
					AllRecipeSpriteDisable();

					// 시작점을 0으로
					recipeOffset = 0.f;

					// 시작점 -> 현재 레시피가 켜져있는 곳
					startSpritePos = Vector3{ -733.f, -168.f, 0.49f };
					startButtonPos = Vector3{ -885.f, 193.f, 0.48f };
					// 끝점 -> 레시피가 닫혀야 하는곳
					endSpritePos = Vector3{ -733.f, -834.f, 0.49f };
					endButtonPos = Vector3{ -885.f, -475.f, 0.48f };
					// 우선은 오프셋을 조절하는 식으로 해보고..
						// 별로면 smoothStep을 쓰는 식으로 해보겠습니다.

					// 애니메이션을 시작할 땐 버튼을 disable 해둡니다.
					recipeButton->SetButtonEnable(false);
				}
				else
				{
					sound->PlaySFXSound("RecipeOpen", 0.5f);

					// 꺼져 있으면 켜줍시다.
					isRecipeSet = !isRecipeSet;

					// 이 친구는 애니메이션이 끝나면.. 버튼들을 켜줍니다.
					// 애니메이션을 Start 해줍니다.
					isAnimating = true;
					AllRecipeSpriteDisable();

					// 시작점을 0으로
					recipeOffset = 0.f;

					// 시작점 -> 현재 레시피가 꺼져있는 곳
					startSpritePos = Vector3{ -733.f, -834.f, 0.49f };
					startButtonPos = Vector3{ -885.f, -475.f, 0.48f };
					// 끝점 -> 레시피가 켜져야 하는곳
					endSpritePos = Vector3{ -733.f, -168.f, 0.49f };
					endButtonPos = Vector3{ -885.f, 193.f, 0.48f };

					// 애니메이션을 시작할 땐 버튼을 disable 해둡니다.
					recipeButton->SetButtonEnable(false);
				}
			});
	}

	void BarCoreController::TutorialUpdate(float tick)
	{
		// 클릭 시, 인덱스를 올립니다. 인덱스가 다를 시에만 이미지를 바꿔줍니다.
		// 첫 진입시 모든 버튼을 꺼줍니다.
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

		// 버튼 클릭 한번 할 때 마다 늘려줍니다.
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
		// 첫 진입시 모든 버튼을 꺼줍니다.
			// 튜토리얼 부터 안들어올 수도 있기 때문에..
		if (changeFirst == true)
		{
			// 게스트 입장.
			sound->PlaySFXSound("Guest", 0.5f);

			changeFirst = false;
			// 레시피 관련해서 버튼들이 드로우 되도록 하고
			AllRecipeTitleButtonEnable(true);

			AllButtonEnable(false);
			AllCustomerUIEnable(false);

			dialogIdx = 0;

			// 처음 여기 들어왔을 때. 일단 현재 이 친구의 상태를 확인해봅니다.
				// 즉 커스터머에 맞게 세팅을 해준다는 뜻.
				// 세팅만 해주고 렌더는 안되도록 해준뒤..
			customerName->SetText(nowCustomer->name);

			// 첫 텍스쳐를 가져와봅니다.
			auto tex = resourceManager->LoadPrefabTexture(nowCustomer->smallTalk[dialogIdx].second);

			// 스프라이트 부터 켜줍니다.
			customerSprite->SetEnable(true);
			customerSprite->SetTexture(tex);
			customerSprite->SetWidth(tex->GetWidth());
			customerSprite->SetHeight(tex->GetHeight());

			nowCustomerIdx = guest[nowGuestIdx] / 2;

			SetCustomerPosition();
			// 첫 대사를 가져와 봅시다.
			dialogText->SetStyleText(nowCustomer->smallTalk[dialogIdx].first);

			// 레시피를 반드시..! 반드시 열어줍니다.
			// 모든 레시피를 꺼주고
			AllRecipeSpriteDisable();
			// 현재 버튼 상태 초기화
			isAnimating = false;
			isRecipeSet = true;

			// 버튼 위치 고정
				// 이 친구들은 꺼질 일이 없습니다..
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
			// 알파가 1 이하 일 때 -> 첫 진입시
				// 이후 bye에서 다시 0으로 바꿔줘야함.
				// 손님 들어오는 연출이 될겁니다.
			if (spriteAlpha < 1.0f)
			{
				spriteAlpha += tick * alphaSpeed;
				if (spriteAlpha >= 1.0f)
				{
					// 여기서 모든 애들을 켜줍니다 -> 한번만 불리기 때문
					AllCustomerUIEnable(true);
					spriteAlpha = 1.0f;
				}
				//spriteAlpha = SmoothStep(spriteAlpha);

				customerSprite->SetMaskAlpha(MathHelper::SmoothStep(spriteAlpha));
			}
			else
			{
				// 여기서는 클릭 한번 한번 할 때 마다. 넘어가 주도록 합니다.
				if (input->GetInputState(VK_LBUTTON, KeyState::DOWN))
				{
					dialogIdx++;

					// 스몰토크가 끝날 때 까지만 받아 줍니다.
					if (dialogIdx < nowCustomer->smallTalk.size())
					{
						dialogText->SetStyleText(nowCustomer->smallTalk[dialogIdx].first);
						// 표정도 바꿔줘
						auto tex = resourceManager->LoadPrefabTexture(nowCustomer->smallTalk[dialogIdx].second);
						customerSprite->SetTexture(tex);
						customerSprite->SetWidth(tex->GetWidth());
						customerSprite->SetHeight(tex->GetHeight());
					}
					else
					{
						// 인덱스 끝. 다음 장면으로 전환!
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
		// 여기서는 사실상 대사 한번만 바꿔주고 모든 버튼을 켜주는 방식
		if (changeFirst == true)
		{
			changeFirst = false;

			// 주문 리스트 마음대로 뽑아요
			int orderIdx = gameManager->CreateRandomInt(0, 74);
			nowOrder = gameManager->GetOrder(orderIdx);
			if (nowOrder != nullptr)
				dialogText->SetStyleText(nowOrder->KR);

			// 기존에 있던 Select 혹시 모를 초기화.
			ResetSelect();
			// 모든 버튼 활성화!
			AllButtonEnable(true);
		}

		// 버튼이 모두 default 상태면 꺼줍시다..
		bool notHover = true;
		// amount Text랑 관련된 버튼을 방지하는 역할.
			// 상황에 맞게 버튼을 껐다 켰다 해줍니다.
		for (int i = 0; i < 7; i++)
		{
			// amount text 변경
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

		// 대사 세팅
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
				// 여기선 스몰톡
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

		// 가끔 Hover가 아닌 경우에도 남았는 경우가 있었기 때문에, 모든 버튼이 default 상태면 꺼줍니다.
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
				// enable이 호출 될 때 마다 default로 바꿔주기 때문에 문제가..?
				mixButton->SetButtonEnable(true);
				setMix = true;
			}
		}

		// 레시피 버튼 열리고 닫히고 세팅 해줘야함.
		if (isAnimating)
		{
			recipeOffset += tick * animSpeed;
			if (recipeOffset >= 1.0f)
			{
				recipeOffset = 1.0f;

				// 한 번만 불리는 곳이기 때문에..
				isAnimating = false;
				// 버튼을 사용가능 하도록
				recipeButton->SetButtonEnable(true);
				// 켜진 상태면.. 타이틀 버튼들을 켜줍니다.
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
			// 일단은 각종 버튼이랑 캐릭터 가려줍니다..?
			AllButtonEnable(false);

			shakeCanvas->SetEnable(true);

			// 각종 오브젝트들을 제 위치에 배치 시켜 줍니다.
			MiniGameSetUp();
			countDownBack->SetEnable(true);
			countDown->SetEnable(true);
			countDown->SetText(L"3");
			countDownTick = 3.f;

			hpOffset = 0.f;

			// 이 때 현재 레시피 상태에 따라서 세팅을 달리 해줍니다.
				// 일단 뭐든 애니메이팅이 진행되고 있었을 때 멈추게합니다.
				// 그게 아니면 endPos가 이상한 값이 들어갈 수도 있기 때문입니다.
			if (isAnimating == true)
			{
				isAnimating = false;
				// 그리고 엔드 위치에 가도록 합니다.
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

		// 카운트를 해봅시다.
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

			// 카운트 다운이 끝나기 전엔 아래를 실행시키지 않습니다.
			return;
		}

		// 체력 조정.
			// 기본적으로 0.f이기 때문에..
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

		float radAngle = angle * XM_PI / 180;  // 라디안으로 변환 = 각도 * PI / 180
		Vector3 moveArrowPos{ radius * cos(radAngle), radius * sin(radAngle) - 40, 0.2f };
		miniGameArrow->SetPosition(moveArrowPos);

		// 이건 사실 언제든지 꺼져야하는 것이라..
		if (currentHP <= 0 || miniGameCount >= maxIterations)
		{
			// 종료
			nowState = BarState::OFFER;
			if (correctCount != 3)
				miniGameSuccess = false;
			// 미니게임 창을 닫습니다..
			changeFirst = true;
			shakeCanvas->SetEnable(false);
		}

		// 돌려 보자
		if (isTurning)
		{
			angle += static_cast<int>(angleTheta * tick * 2.f);
			if (angle >= 630)
			{
				angle = startAngle;
				miniGameCount++;
				currentHP -= 10;
				// 돌 수 있는 최대 횟수 8
				sound->PlaySFXSound("MiniGameF", 0.5f);
			}

			// 키 입력 세팅
			if (input->GetInputState('F', KeyState::DOWN))
			{
				constexpr float distanceThreshold = 50.0f; // 일정 거리 범위 설정 (원하는 값으로 조정)
				bool failureOccurred = true; // 일치하지 않는 위치를 찾았는지 여부를 나타내는 변수

				buttonClick = true;

				// 맞았냐..!
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

							// 맞았다!
							// CorrectCount에 따른 처리를 해줍니다.
								// 와이? 한번만 해라 제발..
							// 맞춘 애는 없어지고 -> 원에서 회색
								// Select에서 위치가 올라가야함.
							switch (correctCount)
							{
								// 첫 번째 맞춤
							case 0:
							{
								// 나머지 두개를 위로 올려줍니다.
								selectPlay[correctCount] = true;
								break;
							}
							// 두 번째 맞춤
							case 1:
							{
								// 나머지 한개를 위로 올려줍니다.
								selectPlay[correctCount] = true;

								break;
							}
							// 세 번째 맞춤
							case 2:
							{
								// 나머지 한개를 위로 올려줍니다.
								selectPlay[correctCount] = true;
								// 턴을 끝내면 되겠지
								isTurning = false;

								break;
							}

							default:
								break;
							}

							correctCount++;
							// 실패한게 아님
							failureOccurred = false;

							break;
						}
						else
						{
							// 거리엔 들어왔는데 다른 아이다.
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
			// 여기서 끝났다는 것은 다 맞췄다는 or 체력이 줄었다는 뜻입니다.
			// 마지막 애니메이션이 종료가 될때.
			if (shakePlay[0] == false)
			{
				// 종료
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
				// 미니게임 창을 닫습니다..
				changeFirst = true;
				shakeCanvas->SetEnable(false);
			}
		}

		// 버튼에 따른 세팅
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
			// 기존에 것이 끝났는지 확인
				// 안끝났으면 강제 종료
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
			// 기존의 것이 끝났는지 확인
				// 안끝났으면 강제 종료
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

		// 기본적으로 애니메이션 업데이트는 계속 해줘야합니다.
		for (int i = 0; i < 4; i++)
			PlayRotation(tick, i);
	}

	void BarCoreController::OfferUpdate(float tick)
	{
		if (changeFirst == true)
		{
			resultCanvas->SetEnable(true);
			changeFirst = false;

			// 여기도 알파로 제공 되었으면 좋겠다.
			spriteAlpha = 0.f;

			// 텍스쳐 세팅
			// 칵테일에 따른 텍스쳐 세팅을 해줍니다.
			SetMixedCockTailTexture();
			// 초기화 생각해서 버튼 취소
			offerButton->SetEnable(false);

			// 투명도를 0으로 처리해줍시다.
			mixCockTail->SetMaskAlpha(0.f);
			mixCockTailName->SetMaskAlpha(0.f);

			displayResult = false;
			mixTick = 0.f;

			mixBG->SetEnable(true);
		}

		// 고객 뜨는 거랑 비슷하게 알파 세팅.
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
				// 모든 변경에 Smooth Step을 넣어보자..
				if (spriteAlpha >= 1.0f)
				{
					// 여기서 모든 애들을 켜줍니다 -> 한번만 불리기 때문
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

			// 현재 상태에 따른 분류..
				// 클릭전에 이미 나와 있어야함.
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

		// 돈은 여기서 나오도록 해주고 싶다.
		nowGold->SetText(std::to_wstring(gameManager->GetCurrentMoney()));

		// 사라지는 연출을 주고 싶다.
			// 개스트 인덱스 늘려주기 -> 이건 아예 사라지고 나서.
		if (startBye != true)
		{
			if (input->GetInputState(VK_LBUTTON, KeyState::DOWN))
			{
				// 현재 상태에 따라 분기.
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

						// 돈 올리기
						int nowMoney = gameManager->GetCurrentMoney();
						nowMoney += nowCocktail->amount * 1.5f;
						gameManager->SetCurrentMoney(nowMoney);

						resultState = ResultState::BYE;
						dialogIdx = 0;
						// 첫 씬 띄워주기.
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

						// 돈 올리기
						int nowMoney = gameManager->GetCurrentMoney();
						nowMoney += nowCocktail->amount;
						gameManager->SetCurrentMoney(nowMoney);
						resultState = ResultState::BYE;
						dialogIdx = 0;
						// 첫 씬 띄워주기.
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
						// 돈 올리기
						sound->PlaySFXSound("Cashier", 0.5f);
						int nowMoney = gameManager->GetCurrentMoney();
						nowMoney += 20;
						gameManager->SetCurrentMoney(nowMoney);
						resultState = ResultState::BYE;
						dialogIdx = 0;
						// 첫 씬 띄워주기.
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
					// 여긴 굿 바이만 해줍니다.
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
						// 대화창 닫기
						AllCustomerUIEnable(false);
						// 사라지는 연출을 해주면서.
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
			// 사라지기 시작
			if (spriteAlpha > 0.0f)
			{
				spriteAlpha -= tick * alphaSpeed;
				if (spriteAlpha <= 0.0f)
				{
					// 여기서 모든 애들을 켜줍니다 -> 한번만 불리기 때문
					spriteAlpha = 0.0f;

					// 게스트 idx를 늘립니다. 상태들을 초기화 합니다.
					// 현재 idx의 수치에 따라 다음 씬을 세팅합니다.
					nowGuestIdx++;

					// 기준 조건들 다 false로 재 세팅
					isTurning = false;
					isCorrectCockTail = false;
					startBye = false;
					miniGameSuccess = false;

					// 여기서 nowCustomer를 세팅
					if (nowGuestIdx < 3)
					{
						// 다시 인트로로 넘어가.
						nowCustomer = gameManager->GetCustomer(guest[nowGuestIdx]);
						resultState = ResultState::NONE;
						nowCocktail = nullptr;


						nowState = BarState::INTRO;
						changeFirst = true;
					}
					else
					{
						// 클로즈로 넘어가는 것.
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
		// 클로즈로 넘어 왔으면 로비로 그냥 바로 갑시다.
		if (changeFirst == true)
		{
			SceneManager::GetInstance()->LoadScene(L"LobbyScene");
			barCanvas->SetEnable(false);
			shakeCanvas->SetEnable(false);
			resultCanvas->SetEnable(false);

			// 크래프트 완료 표시를 합니다.
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

		// 이 경우에 커서도 꺼줍시다.
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

		// 이 경우에 커서도 꺼줍시다.
		if (isEnable == false)
		{
			UIManager::GetInstance()->GetCanvas("CursorCanvas")->GetSpriteUI("Info")->SetEnable(false);
			UIManager::GetInstance()->GetCanvas("CursorCanvas")->GetTextUI("InfoText")->SetEnable(false);
		}
	}

	void BarCoreController::AllRecipeTitleButtonEnable(bool isEnable)
	{
		// 꺼줘야하기 때문에 isEnable로
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
		// 포지션 세팅.
			// 원형 세팅
		GenerateCircleRandomPos();

		// 텍스쳐 세팅
		for (int i = 0; i < 3; i++)
		{
			auto tmpTex = GetIconTexture(select[i]);
			// 나갔다가 다시 들어올 때 초기화 시켜줍니다.
			miniGameIcon[i]->SetEnable(true);
			miniGameIcon[i]->SetTexture(tmpTex);
			miniGameIcon[i]->SetWidth(tmpTex->GetWidth());
			miniGameIcon[i]->SetHeight(tmpTex->GetHeight());
			// 랜덤으로 원에 배치를 해줘야 합니다..
			miniGameIcon[i]->SetPosition(uiCirclePos[i]);
			miniGameIcon[i]->SetMaskColor({ 1.0f, 1.0f, 1.0f, 1.0f });

			miniGameSelect[i]->SetTexture(GetIconTexture(select[i]));
			miniGameSelect[i]->SetWidth(tmpTex->GetWidth());
			miniGameSelect[i]->SetHeight(tmpTex->GetHeight());
			miniGameSelect[i]->SetPosition(uiPos[i]);
			miniGameSelect[i]->SetEnable(true);

			miniGameNumber[i]->SetPosition(uiCirclePos[i] + Vector3(0.f, 70.f, 0.f));
		}

		// 현재 칵테일이 맞는 칵테일이냐?
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

		// 모든 조건을 맞춰줍시다.
		miniGameCount = 0;
		correctCount = 0;

		float radAngle = angle * XM_PI / 180;  // 라디안으로 변환 = 각도 * PI / 180
		Vector3 moveArrowPos{ radius * cos(radAngle), radius * sin(radAngle) - 40, 0.2f };
		miniGameArrow->SetPosition(moveArrowPos);

		// 시작하면 돌려야함.
			// 카운트 이후에 돌립시다.
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
		//롭, 렉스, 밥 별로 위치 조정
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

		// 적어도 차이가 20도 이상은 났으면 좋겠다.
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

		// 믹스를 할 때 이미 세팅되어 있다..! 칵테일이

		// 기본적으로 다 세팅을 해준뒤..
		auto cocktailTex = resourceManager->LoadPrefabTexture(nowCocktail->cocktailNameEN);
		if (cocktailTex != nullptr)
		{
			mixCockTail->SetTexture(cocktailTex);
			mixCockTail->SetWidth(cocktailTex->GetWidth());
			mixCockTail->SetHeight(cocktailTex->GetHeight());
		}

		if (miniGameSuccess == true && isCorrectCockTail == true)
		{
			// 내부적으로 칵테일에 따른 텍스쳐 세팅
			mixCockTailName->SetTexture(resourceManager->LoadPrefabTexture(L"Name_" + nowCocktail->cocktailNameEN));
			mixCockTail->SetMaskColor({ 1.0f, 1.0f, 1.0f, 1.f });

			// 여기서 결과를 세팅해줍니다.
			resultState = ResultState::GOOD;
		}
		else
		{
			// 얘는 무조건 덜섞인 칵테일.
			mixCockTail->SetMaskColor({ 0.6f, 0.6f, 0.6f, 1.f });
			// 이름은 무조건 실패한 칵테일..
			mixCockTailName->SetTexture(resourceManager->LoadPrefabTexture(L"Name_Fail"));

			// 여기서 결과를 세팅해줍니다.
			resultState = ResultState::BAD;
		}
	}

	void BarCoreController::PlayRotation(float tick, int idx)
	{
		// 일정 duration 동안
			// lerp 계수를 0 ~ 1로 반복하면
			// 특정 지점부터 특정 지점까지 왔다 갔다가 가능해질 것.
		if (shakePlay[idx] == true)
		{
			shakeTick[idx] += tick;
			shakeOffset[idx] += shakeTick[idx] * shakeSpeed[idx];
			// 부호를 바꿔줍니다.
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
			// 0에서 1로
			miniGameIcon[idx]->SetWidth(lerp(texWidth + (texWidth * -0.2f), texWidth, MathHelper::SmoothStep((selectTick[idx] - 0.2f) * 10.f)));
			miniGameIcon[idx]->SetHeight(lerp(texHeight + (texHeight * 0.2f), texHeight, MathHelper::SmoothStep((selectTick[idx] - 0.2f) * 10.f)));

			// 위로 조금씩 올려줍니다.
			auto pos = miniGameIcon[idx]->GetPosition();
			miniGameIcon[idx]->SetPosition(Vector3::Lerp(pos, Vector3(pos.x, pos.y - 10.f, pos.z), MathHelper::SmoothStep(selectTick[idx] - 0.2f)));
		}
		else if (selectTick[idx] > 0.1f)
		{
			// 0에서 1로 (전 상태에서 변하는 것으로)
			miniGameIcon[idx]->SetWidth(lerp(texWidth + (texWidth * 0.1f), texWidth + (texWidth * -0.2f), MathHelper::SmoothStep((selectTick[idx] - 0.1f) * 10.f)));
			miniGameIcon[idx]->SetHeight(lerp(texHeight + (texHeight * -0.1f), texHeight + (texHeight * 0.2f), MathHelper::SmoothStep((selectTick[idx] - 0.1f) * 10.f)));
		}
		else
		{
			// 0에서 1로
			miniGameIcon[idx]->SetWidth(lerp(texWidth, texWidth + (texWidth * 0.1f), MathHelper::SmoothStep(selectTick[idx] * 10.f)));
			miniGameIcon[idx]->SetHeight(lerp(texHeight, texHeight + (texHeight * -0.1f), MathHelper::SmoothStep(selectTick[idx] * 10.f)));
		}
	}

	void BarCoreController::StartRotation(int idx)
	{
		// 강제적으로 원상 복귀를 시켜줍니다.
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

		// idx에 따라 달라지는 부분들만 세팅해줍니다.
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

		// 포지션 => 변경 가능성이 있기 때문에 잡아줍니다.
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
