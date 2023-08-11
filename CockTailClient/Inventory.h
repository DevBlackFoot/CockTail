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
		// ��ǻ� �κ��丮�� �ٽɱ���� ����ϴ� itemDataMap.
		std::unordered_map<int, std::shared_ptr<InventoryItem>> itemDataMap;

		// UI�� ���ؼ� �־�� String ��.
			// queueó�� �����ϴ� ���͸� ������ݴϴ�.
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
