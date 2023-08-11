#include "pch.h"
#include "ItemGetUI.h"

#include "UIManager.h"

namespace ClientSpace
{
	ItemGetUI::ItemGetUI(std::weak_ptr<GameObject> _object)
		: ComponentBase(_object)
		, systemMessageText(L" x1�� ȹ�� �Ͽ����ϴ�.")
		, tickCount(0.f)
		, needOffText(false)
	{
	}

	ItemGetUI::~ItemGetUI()
	{
	}

	void ItemGetUI::Init()
	{
		ingameCanvas = UIManager::GetInstance()->GetMainCanvas()->GetCanvasUI("InGameCanvas");
		gameManager = GameManager::GetInstance();
		recentTextUI.assign(7, nullptr);

		// ��ȸ �ϸ鼭 ����ݴϴ�.
		for(int i = 0; i < 7; i++)
		{
			string tmpstr = "SystemMessageText" + std::to_string(i);
			recentTextUI[i] = ingameCanvas->GetTextUI(tmpstr);
		}
	}

	void ItemGetUI::Update(float tick)
	{
		if(gameManager->GetItemGetCounter() == true)
		{
			tickCount = 0.f;

			// ���� ƽ �ʱ�ȭ�� ���� ���ؼ� ������ ȹ�� �� ƽ�� 0�� �Ǹ� false�� �ٲ��ݴϴ�.
			gameManager->SetItemGetCounter(false);
		}

		if(tickCount > 3.0f)
		{
			tickCount = 3.f;

			if(needOffText == true)
			{
				// �ؽ�Ʈ�� ��ȸ�ϸ鼭 ���ݴϴ�.
				/*for(auto& text : recentTextUI)
				{
					text->SetText(L"");
					text->SetEnable(false);
				}*/

				// ���⼭ �ʱ�ȭ�� �����ݴϴ�.
					// �ؽ�Ʈ ��ġ�� ����������ϴ�.
				gameManager->ResetRecentItem();

				needOffText = false;
			}
		}
		else
		{
			// ���ʷ� ������ ��� UI�� ���ݴϴ�.
			if(needOffText == false)
			{
				for (auto& text : recentTextUI)
				{
					text->SetText(L"");
					text->SetEnable(true);
				}

				needOffText = true;
			}

			// ���� �Ŵ������� deque�� ���´����� ����� ���� ������ �޸� ���ݴϴ�.
			auto recent = gameManager->GetRecentItem();

			int recentSize = recent.size();

			// size�� ����� �ֵ��� L""�� �������� recent�� �°� �������ݴϴ�.
			for(int i = 0; i < 7; i++)
			{
				if(recentSize > i)
				{
					auto it = recent.rbegin() + i;
					std::wstring tmpstr = it->data();
					tmpstr += systemMessageText;
					recentTextUI[i]->SetText(tmpstr);
				}
			}

			tickCount += tick;
		}
	}
}
