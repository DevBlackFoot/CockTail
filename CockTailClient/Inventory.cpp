#include "pch.h"
#include "Inventory.h"
#include "Slot.h"

#include "UIManager.h"

namespace ClientSpace
{
	Inventory::Inventory()
	{

	}

	void Inventory::CreateItemDatas(std::vector<GameEngineSpace::JsonObject> itemData)
	{
		for (auto jsonObject : itemData)
		{
			std::shared_ptr<InventoryItem> tmpItem = std::make_shared<InventoryItem>();
			tmpItem->IngredientNameKR = jsonObject.GetString(L"IngredientNameKR");
			tmpItem->IngredientNameEN = jsonObject.GetString(L"IngredientNameEN");
			tmpItem->DescriptionKR = jsonObject.GetString(L"DescriptionKR");
			tmpItem->DescriptionEN = jsonObject.GetString(L"DescriptionEN");
			tmpItem->slotNum = jsonObject.GetInt(L"SlotNum");
			tmpItem->ingredientID = jsonObject.GetInt(L"IngredientID");
			tmpItem->Amount = jsonObject.GetInt(L"Amount");
			tmpItem->howMuchNeed = jsonObject.GetArray<int>(L"HowMuchNeed");
			tmpItem->maxCount = 99;    // maxCount 초기화
			tmpItem->count = 0;        // count 초기화

			// 해당 아이템의 키(ingredientID)를 찾아서 해당 벡터?에 아이템을 추가합니다.
				// 벡터일 이유가 전혀 없다..
			itemDataMap.insert(std::make_pair(tmpItem->ingredientID, tmpItem));
		}

		// 기초 아이템 추가
		for(int i = 1; i < 8; i++)
		{
			auto item = itemDataMap.find(i);

			if(item != itemDataMap.end())
			{
				item->second->count = 5;
			}
		}

		// 여기서 캔버스를 가져와서 버튼 이벤트를 세팅해줍시다.
		auto canvas = UIManager::GetInstance()->GetCanvas("InGameMenuCanvas");

		for (int i = 0; i < 14; i++)
		{
			// 시작점이 8이기 때문
			int idx = i + 8;
			std::string slotIdx = to_string(idx);

			auto invenButton = canvas->GetButtonUI("SlotSpriteUI" + slotIdx);

			// 호버 이벤트 디스크립션 세팅
				// Canvas를 가져오는 것 까지 람다로 넣어야 한다.
				// 람다 변수 이슈로 인해 switch로 쪼갭니다.
			switch (idx)
			{
			case 8:
				invenButton->SetHoverEvent([&]
					{
						UIManager::GetInstance()->GetCanvas("InGameMenuCanvas")->GetTextUI("InventoryDescText")->SetNText(itemDataMap.at(8)->DescriptionKR);
						UIManager::GetInstance()->GetCanvas("InGameMenuCanvas")->GetTextUI("InventoryNameText")->SetText(itemDataMap.at(8)->IngredientNameKR);

					});
				break;

			case 9:
				invenButton->SetHoverEvent([&]
					{
						UIManager::GetInstance()->GetCanvas("InGameMenuCanvas")->GetTextUI("InventoryDescText")->SetNText(itemDataMap.at(9)->DescriptionKR);
						UIManager::GetInstance()->GetCanvas("InGameMenuCanvas")->GetTextUI("InventoryNameText")->SetText(itemDataMap.at(9)->IngredientNameKR);

					});
				break;

			case 10:
				invenButton->SetHoverEvent([&]
					{
						UIManager::GetInstance()->GetCanvas("InGameMenuCanvas")->GetTextUI("InventoryDescText")->SetNText(itemDataMap.at(10)->DescriptionKR);
						UIManager::GetInstance()->GetCanvas("InGameMenuCanvas")->GetTextUI("InventoryNameText")->SetText(itemDataMap.at(10)->IngredientNameKR);

					});
				break;

			case 11:
				invenButton->SetHoverEvent([&]
					{
						UIManager::GetInstance()->GetCanvas("InGameMenuCanvas")->GetTextUI("InventoryDescText")->SetNText(itemDataMap.at(11)->DescriptionKR);
						UIManager::GetInstance()->GetCanvas("InGameMenuCanvas")->GetTextUI("InventoryNameText")->SetText(itemDataMap.at(11)->IngredientNameKR);

					});
				break;

			case 12:
				invenButton->SetHoverEvent([&]
					{
						UIManager::GetInstance()->GetCanvas("InGameMenuCanvas")->GetTextUI("InventoryDescText")->SetNText(itemDataMap.at(12)->DescriptionKR);
						UIManager::GetInstance()->GetCanvas("InGameMenuCanvas")->GetTextUI("InventoryNameText")->SetText(itemDataMap.at(12)->IngredientNameKR);

					});
				break;

			case 13:
				invenButton->SetHoverEvent([&]
					{
						UIManager::GetInstance()->GetCanvas("InGameMenuCanvas")->GetTextUI("InventoryDescText")->SetNText(itemDataMap.at(13)->DescriptionKR);
						UIManager::GetInstance()->GetCanvas("InGameMenuCanvas")->GetTextUI("InventoryNameText")->SetText(itemDataMap.at(13)->IngredientNameKR);
					});
				break;

			case 14:
				invenButton->SetHoverEvent([&]
					{
						UIManager::GetInstance()->GetCanvas("InGameMenuCanvas")->GetTextUI("InventoryDescText")->SetNText(itemDataMap.at(14)->DescriptionKR);
						UIManager::GetInstance()->GetCanvas("InGameMenuCanvas")->GetTextUI("InventoryNameText")->SetText(itemDataMap.at(14)->IngredientNameKR);
					});
				break;

			case 15:
				invenButton->SetHoverEvent([&]
					{
						UIManager::GetInstance()->GetCanvas("InGameMenuCanvas")->GetTextUI("InventoryDescText")->SetNText(itemDataMap.at(15)->DescriptionKR);
						UIManager::GetInstance()->GetCanvas("InGameMenuCanvas")->GetTextUI("InventoryNameText")->SetText(itemDataMap.at(15)->IngredientNameKR);
					});
				break;

			case 16:
				invenButton->SetHoverEvent([&]
					{
						UIManager::GetInstance()->GetCanvas("InGameMenuCanvas")->GetTextUI("InventoryDescText")->SetNText(itemDataMap.at(16)->DescriptionKR);
						UIManager::GetInstance()->GetCanvas("InGameMenuCanvas")->GetTextUI("InventoryNameText")->SetText(itemDataMap.at(16)->IngredientNameKR);
					});
				break;

			case 17:
				invenButton->SetHoverEvent([&]
					{
						UIManager::GetInstance()->GetCanvas("InGameMenuCanvas")->GetTextUI("InventoryDescText")->SetNText(itemDataMap.at(17)->DescriptionKR);
						UIManager::GetInstance()->GetCanvas("InGameMenuCanvas")->GetTextUI("InventoryNameText")->SetText(itemDataMap.at(17)->IngredientNameKR);
					});
				break;

			case 18:
				invenButton->SetHoverEvent([&]
					{
						UIManager::GetInstance()->GetCanvas("InGameMenuCanvas")->GetTextUI("InventoryDescText")->SetNText(itemDataMap.at(18)->DescriptionKR);
						UIManager::GetInstance()->GetCanvas("InGameMenuCanvas")->GetTextUI("InventoryNameText")->SetText(itemDataMap.at(18)->IngredientNameKR);
					});
				break;

			case 19:
				invenButton->SetHoverEvent([&]
					{
						UIManager::GetInstance()->GetCanvas("InGameMenuCanvas")->GetTextUI("InventoryDescText")->SetNText(itemDataMap.at(19)->DescriptionKR);
						UIManager::GetInstance()->GetCanvas("InGameMenuCanvas")->GetTextUI("InventoryNameText")->SetText(itemDataMap.at(19)->IngredientNameKR);
					});
				break;

			case 20:
				invenButton->SetHoverEvent([&]
					{
						UIManager::GetInstance()->GetCanvas("InGameMenuCanvas")->GetTextUI("InventoryDescText")->SetNText(itemDataMap.at(20)->DescriptionKR);
						UIManager::GetInstance()->GetCanvas("InGameMenuCanvas")->GetTextUI("InventoryNameText")->SetText(itemDataMap.at(20)->IngredientNameKR);
					});
				break;

			case 21:
				invenButton->SetHoverEvent([&]
					{
						UIManager::GetInstance()->GetCanvas("InGameMenuCanvas")->GetTextUI("InventoryDescText")->SetNText(itemDataMap.at(21)->DescriptionKR);
						UIManager::GetInstance()->GetCanvas("InGameMenuCanvas")->GetTextUI("InventoryNameText")->SetText(itemDataMap.at(21)->IngredientNameKR);
					});
				break;
			}

			// 나갈 때 "" 세팅
			invenButton->SetOutEvent([&]
				{
					UIManager::GetInstance()->GetCanvas("InGameMenuCanvas")->GetTextUI("InventoryDescText")->SetText(L"");
					UIManager::GetInstance()->GetCanvas("InGameMenuCanvas")->GetTextUI("InventoryNameText")->SetText(L"");
				});
		}
	}

	void Inventory::ClearRecentItem()
	{
		// 큐는 따로 클리어가 없기 때문에.. 비워줍니다.
		recentItem.clear();
		// 텍스트를 다 L""로 세팅해줍니다.
		auto ingameCanvas = UIManager::GetInstance()->GetCanvas("InGameCanvas");
		for(int i = 0; i < 7; i++)
		{
			string tmpstr = "SystemMessageText" + std::to_string(i);
			ingameCanvas->GetTextUI(tmpstr)->SetText(L"");
			//ingameCanvas->SetEnable(false);
		}
	}

	void Inventory::ClearIngredientItem()
	{
		// 아이템의 재료 부분 -> 8번 부터 날려줍니다.
		for(auto& item : itemDataMap)
		{
			if(item.first < 8)
				continue;
			else
				item.second->count = 0;
		}
	}

	void Inventory::ClearAllIngredientItem()
	{
		for (auto& item : itemDataMap)
		{
			item.second->count = 0;
		}
		
		// 기초 아이템 추가
		for (int i = 1; i < 8; i++)
		{
			auto item = itemDataMap.find(i);

			if (item != itemDataMap.end())
			{
				item->second->count = 5;
			}
		}
	}

	void Inventory::PushItem(int ingredientID, int _count /*= 1*/)
	{
		
		auto it = itemDataMap.find(ingredientID);
		if (it != itemDataMap.end())
		{
			auto& item = it->second;
			item->count += _count;

			if (item->count > item->maxCount)
			{
				item->count = item->maxCount;
			}
			//printf("아이템 %d 수량 += %d\n", item.ingredientID, item.count);

			if (recentItem.size() < 7)
				recentItem.push_back(item->IngredientNameKR);
			else
			{
				// 가장 앞에 있는 녀석을 빼주고 넣어줍니다.
				recentItem.pop_front();
				recentItem.push_back(item->IngredientNameKR);
			}

		}
		else
		{
			printf("Not In Item Map!");
		}

	}

	void Inventory::PopItem(int ingredientID, int _count)
	{
		
		auto it = itemDataMap.find(ingredientID);
		if (it != itemDataMap.end())
		{
			auto& item = it->second;
			if (_count > item->count)
			{
				item->count = 0;
				return;
			}

			item->count -= _count;

			//printf("아이템 '%ls' 수량 -= %d\n", item->IngredientNameKR.c_str(), _count);

			if (item->count <= 0)
			{
				item->count = 0;
			}
		}
		else
		{
			// 데이터 맵에서 아이템을 찾지 못한 경우 처리
		}

	}

	std::shared_ptr<InventoryItem> Inventory::FindItem(int ingredientID)
	{
		// 아이템 데이터 맵에서 해당 아이템 키를 검색합니다.
		for (auto& pair : itemDataMap)
		{
			auto item = pair.second;
			if(item->ingredientID == ingredientID)
				return 	item;
		}

		return nullptr;

	}

	void Inventory::RemoveItem(int ingredientID)
	{
	
		auto it = itemDataMap.find(ingredientID);
		if (it != itemDataMap.end())
		{
			itemDataMap.erase(it);
			// 필요한 경우 itemVector에서도 제거 추가 작업 수행
		}

	}
	
}