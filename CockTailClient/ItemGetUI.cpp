#include "pch.h"
#include "ItemGetUI.h"

#include "UIManager.h"

namespace ClientSpace
{
	ItemGetUI::ItemGetUI(std::weak_ptr<GameObject> _object)
		: ComponentBase(_object)
		, systemMessageText(L" x1를 획득 하였습니다.")
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

		// 순회 하면서 얻어줍니다.
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

			// 무한 틱 초기화를 막기 위해서 아이템 획득 후 틱이 0이 되면 false로 바꿔줍니다.
			gameManager->SetItemGetCounter(false);
		}

		if(tickCount > 3.0f)
		{
			tickCount = 3.f;

			if(needOffText == true)
			{
				// 텍스트를 순회하면서 꺼줍니다.
				/*for(auto& text : recentTextUI)
				{
					text->SetText(L"");
					text->SetEnable(false);
				}*/

				// 여기서 초기화를 시켜줍니다.
					// 텍스트 위치도 변경해줬습니다.
				gameManager->ResetRecentItem();

				needOffText = false;
			}
		}
		else
		{
			// 최초로 들어오면 모든 UI를 켜줍니다.
			if(needOffText == false)
			{
				for (auto& text : recentTextUI)
				{
					text->SetText(L"");
					text->SetEnable(true);
				}

				needOffText = true;
			}

			// 게임 매니저에서 deque을 얻어온다음에 사이즈에 따라 세팅을 달리 해줍니다.
			auto recent = gameManager->GetRecentItem();

			int recentSize = recent.size();

			// size를 벗어나는 애들은 L""로 나머지는 recent에 맞게 세팅해줍니다.
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
