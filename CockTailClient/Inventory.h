#pragma once


namespace ClientSpace
{

	struct InventoryItem
	{
		//int itemIndex;
		wstring IngredientNameKR;
		wstring IngredientNameEN;
		wstring DescriptionKR;
		wstring DescriptionEN;
		int maxCount;
		int slotNum;
		int ingredientID;
		int Amount;
		int count;
		std::vector<int> howMuchNeed;
	};


	class Inventory
	{
	private:
		// 사실상 인벤토리의 핵심기능을 담당하는 itemDataMap.
		std::unordered_map<int, std::shared_ptr<InventoryItem>> itemDataMap;

		// UI를 위해서 넣어둘 String 값.
			// queue처럼 동작하는 벡터를 만들어줍니다.
		std::deque<std::wstring> recentItem;

	public:
		Inventory();
		~Inventory() = default;

		void PushItem(int index, int _count = 1);
		void PopItem(int index, int _count = 1);
		std::shared_ptr<InventoryItem> FindItem(int index);
		void RemoveItem(int index);

		void CreateItemDatas(std::vector<GameEngineSpace::JsonObject> itemData);

		std::unordered_map<int, std::shared_ptr<InventoryItem>>& GetItemDataMap() { return itemDataMap; }

		const std::deque<std::wstring>& GetRecentItem() { return recentItem; }
		void ClearRecentItem();
		void ClearIngredientItem();
		void ClearAllIngredientItem();
	};
}
