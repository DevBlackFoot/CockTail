#include "pch.h"
#include "ResultUIController.h"

#include "UIManager.h"

namespace ClientSpace
{
	ResultUIController::ResultUIController(std::weak_ptr<GameObject> obj)
		: ComponentBase(obj)
		, countSpeed(1.0f)
		, firstChange(true)
		, goalCount(0.5f)
		, tickCount(0.7f)
		, nowState(CountState::NONE)
		, ingredEnd(false)
	{
		// 캐싱 제외, 배열들을 초기화 해줍니다.
		for (int i = 0; i < 7; i++)
		{
			craftItems[i] = 0;
			nowCraftItems[i] = 0;
		}

		for (int i = 0; i < 14; i++)
		{
			modItems[i] = 0;
			nowIngredients[i] = 0;
		}
	}

	ResultUIController::~ResultUIController()
	{
	}

	void ResultUIController::Awake()
	{
		// 각종 요소를 캐싱해줍니다.
		gameManager = GameManager::GetInstance();
		input = InputManager::GetInstance();

		resultCanvas = UIManager::GetInstance()->GetCanvas("ItemResultCanvas");
		exitButton = resultCanvas->GetButtonUI("ResultLobbyButton");
		UIManager::GetInstance()->GetCanvas("InGameCanvas")->SetEnable(false);

		for (int i = 0; i < 21; i++)
		{
			std::string itemResultIdx = to_string(i);

			countText[i] = resultCanvas->GetTextUI("ItemResultTextUI" + itemResultIdx);
		}

		// 인벤토리 캐싱
		inventory = gameManager->GetInventoryItemDataMap();

		// 버튼 이벤트 세팅..
			// 누르면 바로 로비로 가는 것이 아니라 clear Item을 세팅해줍니다.
		exitButton->SetClickEvent([&]
			{
				// 인벤토리에 크래프트 아이템을 넣어줍니다.
				for (int i = 1; i < 8; i++)
				{
					auto item = inventory.find(i);
					if (item != inventory.end())
					{
						int idx = i - 1;
						item->second->count += craftItems[idx];
					}
				}
				// 인벤토리에 재료 아이템을 삭제합니다.
					// 화면 표시엔 영향을 끼치지 않습니다.
				gameManager->ClearItemInfo();

				// 현재 상태를 바꿔줍니다.
				firstChange = false;
				// 캔버스를 끄고 씬을 이동합니다.
				resultCanvas->SetEnable(false);
				SceneManager::GetInstance()->LoadScene(L"LobbyScene");
			});
	}

	void ResultUIController::Start()
	{
		// 처음 들어오면 num Count로 만들어줍니다.
		nowState = CountState::NUM_COUNT;

		// 스케일 오리지널로
		SceneManager::GetInstance()->SetTimeScaleOrigin();

		// 캔버스 켜줍니다.
		resultCanvas->SetEnable(true);
		// 버튼도 다시 켜줍니다.
		exitButton->SetEnable(true);

		// 시작점을 세팅해둡니다.
			// 현재 얻은 재료..
		for (int i = 0; i < 14; i++)
		{
			// 일단 배열에 추가해줍니다.
			int ingredIdx = i + 8;

			auto item = inventory.find(ingredIdx);
			if (item != inventory.end())
			{
				nowIngredients[i] = item->second->count;
			}
		}

		// 텍스트 세팅
		for (int i = 0; i < 21; i++)
		{
			// 7전 까지는 0으로
				// 이후는 item 개수 가져와서..

			// 사실상 0
			if (i < 7)
				countText[i]->SetText(to_wstring(nowCraftItems[i]));
			else
			{
				int idx = i - 7;
				countText[i]->SetText(to_wstring(nowIngredients[idx]));
			}
		}

		// 크래프트를 해둡니다.
		CraftAllItem();
	}

	void ResultUIController::Update(float tick)
	{
		// 현재 상태에 따라 업데이트를 다르게 돌려줍니다.
		switch (nowState)
		{
		case CountState::NUM_COUNT:
			CountUpdate(tick);
			break;

		case CountState::COUNT_END:
			CountEndUpdate(tick);
			break;

		case CountState::CLEAR_ITEM:
			ClearItemUpdate(tick);
			break;

		case CountState::CLEAR_END:
			ClearEndUpdate(tick);
			break;

		case CountState::NONE:
		default:
			break;
		}
	}

	void ResultUIController::LateUpdate(float tick)
	{
		// 틱은 여기서 일괄적으로 계산해줍니다.
		tickCount += tick * countSpeed;

		// 텍스트 세팅
		for (int i = 0; i < 21; i++)
		{
			// 사실상 0
			if (i < 7)
				countText[i]->SetText(to_wstring(nowCraftItems[i]));
			else
			{
				int idx = i - 7;
				countText[i]->SetText(to_wstring(nowIngredients[idx]));
			}
		}
	}

	void ResultUIController::CraftAllItem()
	{
		for (int i = 0; i < 7; i++)
		{
			// 7번 반복해서 넣어줍니다.
			auto item = inventory.find(i + 1);
			if (item != inventory.end())
			{
				// 8을 빼줘야 현재 배열에 접근할 수 있습니다.
				int ingred1Idx = item->second->howMuchNeed[0] - 8;
				int ingred2Idx = item->second->howMuchNeed[1] - 8;

				int craftCount = CraftItem(nowIngredients[ingred1Idx], nowIngredients[ingred2Idx]);

				// 해당 카운트를 최종 크래프트에 넣어줍니다.
				craftItems[i] = craftCount;

				// 해당 카운트 * 2를 뺀 값을 mod에 넣어줍니다.
				modItems[ingred1Idx] = nowIngredients[ingred1Idx] - (2 * craftCount);
				modItems[ingred2Idx] = nowIngredients[ingred2Idx] - (2 * craftCount);
			}
		}
	}

	int ResultUIController::CraftItem(int ingred1, int ingred2)
	{
		// 두 값을 2로 나눈 뒤, 최소값을 가져갑니다.
		int div1 = ingred1 / 2;
		int div2 = ingred2 / 2;

		return min(div1, div2);
	}

	void ResultUIController::CountUpdate(float tick)
	{
		if (firstChange == true)
		{
			firstChange = false;

			// 버튼 꺼두기
			exitButton->SetButtonEnable(false);
			tickCount = 0.f;
			goalCount = 0.7f;
			ingredEnd = false;
		}

		// 우선 ingredient 카운트를 셉니다.

		// 각 애들마다 연산속도를 다르게 해줍니다.
		bool allCorrect = true;

		// 일괄적으로.
		if (ingredEnd != true)
		{
			int correctIngredCount = 0;

			// 재료부터
			if (tickCount >= goalCount)
			{
				// 초기화.
				for (int i = 0; i < 14; i++)
				{
					nowIngredients[i]--;
					if(nowIngredients[i] < modItems[i])
					{
						correctIngredCount++;
						nowIngredients[i] = modItems[i];
					}
				}

				// 골카운트 세팅
					// 많이 맞출수록 느려집니다.
				goalCount = MathHelper::SmoothStep(correctIngredCount / 14.f, 0.7f);
			}

			if(correctIngredCount == 14)
			{
				// 모두 초기화 시키고 다음으로 넘어갑니다.
				tickCount = 0.f;
				goalCount = 0.7f;
				ingredEnd = true;
			}
		}
		else
		{
			int correctCount = 0;

			if(tickCount >= goalCount)
			{
				// 초기화.
				for (int i = 0; i < 7; i++)
				{
					nowCraftItems[i]++;
					if (nowCraftItems[i] > craftItems[i])
					{
						correctCount++;
						nowCraftItems[i] = craftItems[i];
					}
				}

				// 골카운트 세팅
					// 많이 맞출수록 느려집니다.
				goalCount = MathHelper::SmoothStep(correctCount / 7.f, 0.7f);
			}

			if(correctCount == 7)
			{
				tickCount = 0.f;
				goalCount = 0.7f;
				firstChange = true;
				nowState = CountState::COUNT_END;
			}
		}

		// 특정 키를 누르면 스킵합니다.
		if (input->GetInputState(VK_RETURN, KeyState::DOWN) || input->GetInputState(VK_LBUTTON, KeyState::DOWN))
		{
			// 현재 친구들을 최종으로 변경합니다.
			for (int i = 0; i < 21; i++)
			{
				if (i < 7)
				{
					nowCraftItems[i] = craftItems[i];
				}
				else
				{
					int idx = i - 7;
					nowIngredients[idx] = modItems[idx];
				}
			}

			firstChange = true;
			nowState = CountState::COUNT_END;
		}
	}

	void ResultUIController::CountEndUpdate(float tick)
	{
		// 카운트가 끝나거나 버튼을 눌렀다.

		// 버튼을 눌러야 클리어 이동해줍니다.
		if (firstChange == true)
		{
			firstChange = false;
			exitButton->SetButtonEnable(true);
			// 따로 건드리지 않습니다.
		}
	}

	void ResultUIController::ClearItemUpdate(float tick)
	{
		// 크래프트는 건드리지 않고 재료 템만 사라지게 합니다.
		if (firstChange == true)
		{
			// 버튼 자체를 꺼줍니다.
			exitButton->SetEnable(false);
			
			firstChange = false;

			tickCount = 0.f;
			goalCount = 0.7f;
		}

		int correctIngredCount = 0;

		// 재료부터
		if (tickCount >= goalCount)
		{
			// 초기화.
			for (int i = 0; i < 14; i++)
			{
				nowIngredients[i]--;
				if (nowIngredients[i] < 0)
				{
					correctIngredCount++;
					nowIngredients[i] = 0;
				}
			}

			// 골카운트 세팅
				// 많이 맞출수록 느려집니다.
			goalCount = MathHelper::SmoothStep(correctIngredCount / 14.f, 0.7f);
		}

		if (correctIngredCount == 14)
		{
			// 모두 초기화 시키고 다음으로 넘어갑니다.
			firstChange = true;
			nowState = CountState::CLEAR_END;
		}

		// 특정 키를 누르면 스킵합니다.
		if (input->GetInputState(VK_RETURN, KeyState::DOWN) || input->GetInputState(VK_LBUTTON, KeyState::DOWN))
		{
			// 현재 친구들을 최종으로 변경합니다.
			for (int i = 0; i < 14; i++)
			{
				nowIngredients[i] = 0;
			}

			firstChange = true;
			nowState = CountState::CLEAR_END;
		}
	}

	void ResultUIController::ClearEndUpdate(float tick)
	{
		if (firstChange == true)
		{
			firstChange = false;
			// 캔버스를 끄고 씬을 이동합니다.
			resultCanvas->SetEnable(false);
			SceneManager::GetInstance()->LoadScene(L"LobbyScene");
		}
	}
}
