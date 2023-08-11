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
		// ĳ�� ����, �迭���� �ʱ�ȭ ���ݴϴ�.
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
		// ���� ��Ҹ� ĳ�����ݴϴ�.
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

		// �κ��丮 ĳ��
		inventory = gameManager->GetInventoryItemDataMap();

		// ��ư �̺�Ʈ ����..
			// ������ �ٷ� �κ�� ���� ���� �ƴ϶� clear Item�� �������ݴϴ�.
		exitButton->SetClickEvent([&]
			{
				// �κ��丮�� ũ����Ʈ �������� �־��ݴϴ�.
				for (int i = 1; i < 8; i++)
				{
					auto item = inventory.find(i);
					if (item != inventory.end())
					{
						int idx = i - 1;
						item->second->count += craftItems[idx];
					}
				}
				// �κ��丮�� ��� �������� �����մϴ�.
					// ȭ�� ǥ�ÿ� ������ ��ġ�� �ʽ��ϴ�.
				gameManager->ClearItemInfo();

				// ���� ���¸� �ٲ��ݴϴ�.
				firstChange = false;
				// ĵ������ ���� ���� �̵��մϴ�.
				resultCanvas->SetEnable(false);
				SceneManager::GetInstance()->LoadScene(L"LobbyScene");
			});
	}

	void ResultUIController::Start()
	{
		// ó�� ������ num Count�� ������ݴϴ�.
		nowState = CountState::NUM_COUNT;

		// ������ �������η�
		SceneManager::GetInstance()->SetTimeScaleOrigin();

		// ĵ���� ���ݴϴ�.
		resultCanvas->SetEnable(true);
		// ��ư�� �ٽ� ���ݴϴ�.
		exitButton->SetEnable(true);

		// �������� �����صӴϴ�.
			// ���� ���� ���..
		for (int i = 0; i < 14; i++)
		{
			// �ϴ� �迭�� �߰����ݴϴ�.
			int ingredIdx = i + 8;

			auto item = inventory.find(ingredIdx);
			if (item != inventory.end())
			{
				nowIngredients[i] = item->second->count;
			}
		}

		// �ؽ�Ʈ ����
		for (int i = 0; i < 21; i++)
		{
			// 7�� ������ 0����
				// ���Ĵ� item ���� �����ͼ�..

			// ��ǻ� 0
			if (i < 7)
				countText[i]->SetText(to_wstring(nowCraftItems[i]));
			else
			{
				int idx = i - 7;
				countText[i]->SetText(to_wstring(nowIngredients[idx]));
			}
		}

		// ũ����Ʈ�� �صӴϴ�.
		CraftAllItem();
	}

	void ResultUIController::Update(float tick)
	{
		// ���� ���¿� ���� ������Ʈ�� �ٸ��� �����ݴϴ�.
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
		// ƽ�� ���⼭ �ϰ������� ������ݴϴ�.
		tickCount += tick * countSpeed;

		// �ؽ�Ʈ ����
		for (int i = 0; i < 21; i++)
		{
			// ��ǻ� 0
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
			// 7�� �ݺ��ؼ� �־��ݴϴ�.
			auto item = inventory.find(i + 1);
			if (item != inventory.end())
			{
				// 8�� ����� ���� �迭�� ������ �� �ֽ��ϴ�.
				int ingred1Idx = item->second->howMuchNeed[0] - 8;
				int ingred2Idx = item->second->howMuchNeed[1] - 8;

				int craftCount = CraftItem(nowIngredients[ingred1Idx], nowIngredients[ingred2Idx]);

				// �ش� ī��Ʈ�� ���� ũ����Ʈ�� �־��ݴϴ�.
				craftItems[i] = craftCount;

				// �ش� ī��Ʈ * 2�� �� ���� mod�� �־��ݴϴ�.
				modItems[ingred1Idx] = nowIngredients[ingred1Idx] - (2 * craftCount);
				modItems[ingred2Idx] = nowIngredients[ingred2Idx] - (2 * craftCount);
			}
		}
	}

	int ResultUIController::CraftItem(int ingred1, int ingred2)
	{
		// �� ���� 2�� ���� ��, �ּҰ��� �������ϴ�.
		int div1 = ingred1 / 2;
		int div2 = ingred2 / 2;

		return min(div1, div2);
	}

	void ResultUIController::CountUpdate(float tick)
	{
		if (firstChange == true)
		{
			firstChange = false;

			// ��ư ���α�
			exitButton->SetButtonEnable(false);
			tickCount = 0.f;
			goalCount = 0.7f;
			ingredEnd = false;
		}

		// �켱 ingredient ī��Ʈ�� ���ϴ�.

		// �� �ֵ鸶�� ����ӵ��� �ٸ��� ���ݴϴ�.
		bool allCorrect = true;

		// �ϰ�������.
		if (ingredEnd != true)
		{
			int correctIngredCount = 0;

			// ������
			if (tickCount >= goalCount)
			{
				// �ʱ�ȭ.
				for (int i = 0; i < 14; i++)
				{
					nowIngredients[i]--;
					if(nowIngredients[i] < modItems[i])
					{
						correctIngredCount++;
						nowIngredients[i] = modItems[i];
					}
				}

				// ��ī��Ʈ ����
					// ���� ������� �������ϴ�.
				goalCount = MathHelper::SmoothStep(correctIngredCount / 14.f, 0.7f);
			}

			if(correctIngredCount == 14)
			{
				// ��� �ʱ�ȭ ��Ű�� �������� �Ѿ�ϴ�.
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
				// �ʱ�ȭ.
				for (int i = 0; i < 7; i++)
				{
					nowCraftItems[i]++;
					if (nowCraftItems[i] > craftItems[i])
					{
						correctCount++;
						nowCraftItems[i] = craftItems[i];
					}
				}

				// ��ī��Ʈ ����
					// ���� ������� �������ϴ�.
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

		// Ư�� Ű�� ������ ��ŵ�մϴ�.
		if (input->GetInputState(VK_RETURN, KeyState::DOWN) || input->GetInputState(VK_LBUTTON, KeyState::DOWN))
		{
			// ���� ģ������ �������� �����մϴ�.
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
		// ī��Ʈ�� �����ų� ��ư�� ������.

		// ��ư�� ������ Ŭ���� �̵����ݴϴ�.
		if (firstChange == true)
		{
			firstChange = false;
			exitButton->SetButtonEnable(true);
			// ���� �ǵ帮�� �ʽ��ϴ�.
		}
	}

	void ResultUIController::ClearItemUpdate(float tick)
	{
		// ũ����Ʈ�� �ǵ帮�� �ʰ� ��� �۸� ������� �մϴ�.
		if (firstChange == true)
		{
			// ��ư ��ü�� ���ݴϴ�.
			exitButton->SetEnable(false);
			
			firstChange = false;

			tickCount = 0.f;
			goalCount = 0.7f;
		}

		int correctIngredCount = 0;

		// ������
		if (tickCount >= goalCount)
		{
			// �ʱ�ȭ.
			for (int i = 0; i < 14; i++)
			{
				nowIngredients[i]--;
				if (nowIngredients[i] < 0)
				{
					correctIngredCount++;
					nowIngredients[i] = 0;
				}
			}

			// ��ī��Ʈ ����
				// ���� ������� �������ϴ�.
			goalCount = MathHelper::SmoothStep(correctIngredCount / 14.f, 0.7f);
		}

		if (correctIngredCount == 14)
		{
			// ��� �ʱ�ȭ ��Ű�� �������� �Ѿ�ϴ�.
			firstChange = true;
			nowState = CountState::CLEAR_END;
		}

		// Ư�� Ű�� ������ ��ŵ�մϴ�.
		if (input->GetInputState(VK_RETURN, KeyState::DOWN) || input->GetInputState(VK_LBUTTON, KeyState::DOWN))
		{
			// ���� ģ������ �������� �����մϴ�.
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
			// ĵ������ ���� ���� �̵��մϴ�.
			resultCanvas->SetEnable(false);
			SceneManager::GetInstance()->LoadScene(L"LobbyScene");
		}
	}
}
