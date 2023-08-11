#pragma once
#include "SceneManager.h"
#include "Util/JsonReader.h"
#include "ItemDrop.h"
#include "Inventory.h"

namespace ClientSpace
{
	class InGameUIController;

	// Ŭ���̾�Ʈ ��ü���� ����� Struct;
	struct Customer
	{
		/*// Customer�� ���¸� �����մϴ�.
		enum class CustomerState
		{
			GREET,
			GOOD,
			BAD,
			BEER,
			BYE,
			NONE,
		};*/

		wstring name;
		// ���� ��� �������� ���ͷ� ������ �ֽ��ϴ�. + ǥ������ �߰��� ������ �ֵ��� �մϴ�.
			// SmallTalk�� Order���� �߰��� �ֽ��ϴ�.
			// �ݵ�� ������� ���ٰ� �����մϴ�! -> json ���Ϻ��� ������� ���� �� �Դϴ�.
		// ��� / ǥ�� ���Դϴ�.
		std::vector<std::pair<wstring, wstring>> smallTalk;
		std::vector<std::pair<wstring, wstring>> goodTalk;
		std::vector<std::pair<wstring, wstring>> badTalk;
		std::vector<std::pair<wstring, wstring>> beerTalk;
		std::vector<std::pair<wstring, wstring>> byeTalk;


	};

	// Ĭ���� �����Ҷ� �ʿ��� �����͵�
	struct CockTailStruct
	{
		wstring cocktailNameKR;
		wstring cocktailNameEN;
		int cocktailIndex;
		int hoverIdx;
		std::vector<int> ingredients;
		wstring descriptionKR;
		wstring descriptionEN;
		std::vector<int> feature;
		int amount;
	};

	// ��ǻ� ���� �������� ��ü�� ���ɼ��� ���� ����ü�Դϴ�.
	struct BarDialog
	{
		int scriptIndex;
		std::wstring nameEN;
		std::wstring nameKR;
		std::wstring scriptEvent;
		std::wstring scriptStringEN;
		std::wstring scriptStringKR;
	};

	struct CockTailList
	{
		int customerScriptID;
		int request1;
		int request2;
		std::wstring EN;
		std::wstring KR;
	};

	class GameManager
	{
	private:
		static std::shared_ptr<GameManager> instance;

		std::unique_ptr<GameEngineSpace::JsonReader> json_ItemDataTable;
		std::unique_ptr<GameEngineSpace::JsonReader> json_ItemDropTable;
		std::unique_ptr<GameEngineSpace::JsonReader> json_CocktailTable;
		std::unique_ptr<GameEngineSpace::JsonReader> json_CocktailListTable;
		std::unique_ptr<GameEngineSpace::JsonReader> json_CharacterDataTable;
		std::unique_ptr<GameEngineSpace::JsonReader> json_LobbyScriptDataTablde;
		std::unique_ptr<GameEngineSpace::JsonReader> json_BarScriptDataTablde;
		std::unique_ptr<GameEngineSpace::JsonReader> json_WaveBattleTable;

		std::vector<GameEngineSpace::JsonObject> itemDataTable;
		std::vector<GameEngineSpace::JsonObject> itemDropTable;
		std::vector<GameEngineSpace::JsonObject> cocktailTable;
		std::vector<GameEngineSpace::JsonObject> cocktailListTable;
		std::vector<GameEngineSpace::JsonObject> ChracterDataTable;
		std::vector<GameEngineSpace::JsonObject> lobbyScriptDataTable;
		std::vector<GameEngineSpace::JsonObject> barScriptDataTable;
		std::vector<GameEngineSpace::JsonObject> waveBattleTable;

		// ���� ������ ��� �ִ� ����
			// ��� ����
		std::vector<float> powerUpTable;
		std::vector<int> healthUpTable;
		std::vector<float> speedUpTable;

		// ���� ������ ����ִ� ���̺�
			// �ܰ� +1�� ���� (0�� �ε����� 1������ �ö󰡱� ���� ����)
			// 2���� ���ͷ� 1�� ��, 2�� ü��, 3�� ���ǵ�
		std::vector<std::vector<int>> priceTable;
		// ��ų ������ ������ �ִ� ����
		std::vector<std::pair<wstring, wstring>> skillDescTable;
		// �� ������ ������ �ִ� ��
		std::unordered_map<int, std::shared_ptr<Customer>> customerMap;
		// ����� ������� ���� Ĭ���� ���� �ʵ��� ������ݴϴ�.
		std::unordered_map<int, std::shared_ptr<CockTailStruct>> cockTailMap;
		// �ֹ� ��û ����Ʈ
		std::unordered_map<int, std::shared_ptr<CockTailList>> cocktailOrderMap;

		ItemDrop* itemDrop;
		Inventory* inventory;

		// ���� �ӽ�
		std::mt19937 randgen;
		// �� ȸ���� ũ����Ʈ�� ���ȴ°�.
			// ������ true��.
		bool craftDone = false;

	public:
		GameManager() = default;
		~GameManager();
		GameManager(const GameManager& other) = delete;
		GameManager(const GameManager&& other) = delete;
		GameManager& operator= (const GameManager& other) = delete;
		GameManager& operator= (const GameManager&& other) = delete;

		static std::shared_ptr<GameManager> GetInstance();

		void InitScene();
		void Initialize();

		// HP ���������� �ѱ������ ����
		void SetHPs();

		void SetCurrentHP(int value) { currentHP = value; }
		int GetCurrentHP() { return currentHP; }
		void SetMaxHP(int value) { maxHP = value; }
		int GetMaxHP() { return maxHP; }

		// ���� ������
		void SetPower(int _power) { power = _power; }
		int GetPower() { return power; }
		float GetAttack1() { return attack1; }
		float GetAttack2() { return attack2; }
		float GetAttack3() { return attack3; }
		float GetAttack4() { return attack4; }

		// ��ȭ ������
		/*void SetMoney();*/
		void SetCurrentMoney(int value) { currentMoney = value; }
		int GetCurrentMoney() { return currentMoney; }

		void SetMaxMoney(int value) { maxMoney = value; }
		int GetMaxMoney() { return maxMoney; }

		// �� ������
		void SetEnemyHP(int value) { enemyHP = value; }
		int GetEnemyHP() { return enemyHP; }

		// portal
		//void SetIsPortalEnable(bool value) { enablePortals = value; }
		//bool GetIsPortalEnable() { return enablePortals; }

		void SetMoveSpeed(int value) { moveSpeed = value; }
		float GetMoveSpeed() { return moveSpeed; }

		void AddKillCount(int value) { currentKillCounter += value; }
		void ResetKillCount() { currentKillCounter = 0; }

		int GetKillCount() { return currentKillCounter; }

		void LoadSound();

		//std::unordered_map<int, int> GetInGameItemMap() { return inGameItemMap; }
		//void PushItemInGame(InGameItem _item);

		void LoadWholeJsonFiles();

		// ���� ��ư �̺�Ʈ ����
		void StoreButtonEventSetting();

		std::vector<GameEngineSpace::JsonObject> GetJsonObject(string _name);

		std::unordered_map<std::wstring, std::vector<ItemData>> GetItemDataMap() { return itemDrop->GetItemDataMap(); }
		
		void DropItem(tstring name, Vector3 position);
		// Ű ��� �� Ȯ�ο� �׽�Ʈ �Լ�
		void DropKey(Vector3 position);

		// ������ ����
		//std::list<InventoryItem> GetItemList() { return inventory->GetItemList(); }
		//std::vector<int>& GetCraftedItems() { return inventory->GetCraftedItems(); }
		std::unordered_map<int, std::shared_ptr<InventoryItem>>& GetInventoryItemDataMap() { return inventory->GetItemDataMap(); }

		void PushItem(int index, int count = 1);
		void PopItem(int index, int count = 1);
		std::shared_ptr<InventoryItem> FindItem(int index);
		void RemoveItem(int index);

		void ResetRecentItem();
		const std::deque<std::wstring>& GetRecentItem();

		// ���� ����
		void StatGUIRender();
		void WriteGameLog(const std::string& logStr);
		void WriteEnemyLog(int id, int state, int maxHP, int remainHP, int dmg);

		/// Floating UI ����
		// bool �� Get Set
		bool GetLobbyFirstRun() { return isLobbyFirstRun; }
		// ����Ǹ� ������ false��..
		void SetLobbyFirstRun() { isLobbyFirstRun = false; }
		bool GetTutorialFirstRun() { return isTutorialFirstRun; }
		void SetTutorialFirstRun() { isTutorialFirstRun = false; }
		bool GetLobbyPlay() { return isLobbyPlay; }
		bool GetTutorialPlay() { return isTutorialPlay; }
		void SetLobbyPlay() { isLobbyPlay = true; }
		void SetTutorialPlay() { isTutorialPlay = true; }

		//
		void SetPlayerDead() { isDead = true; }
		bool GetPlayerDead() { return isDead; }

		// ItemCounter�� ������ true�� �ٲ��ݴϴ�.
			// ���� UI���� True�� ���� â�� ����ݴϴ�..
		void ResetItemGetCounter() { itemGetCounter = true; }
		void SetItemGetCounter(bool itemGetCounter) { this->itemGetCounter = itemGetCounter; }
		bool GetItemGetCounter() { return itemGetCounter; }

		float CreateRandomFloat(float min, float max);
		int CreateRandomInt(int min, int max);
		Vector3 RotateVector(float _x, float _y, float _z);

		// ���� ������ 1�� �÷��ִ� �Լ�
		void PowerLevelUp();
		void HealthLevelUp();
		void SpeedLevelUp();

		int GetNowPrice() { return nowPrice; }
		int GetPowerLevel() { return nowPowerLevel; }
		int GetHealthLevel() { return nowHealthLevel; }
		int GetSpeedLevel() { return nowSpeedLevel; }

		// ������ ���Ҷ� ȣ���ؼ� �ǽð����� ������ �ݽô�.
		void SetUpgradeStat();
		void ResetStat();

		void CreateItem(Vector3 position, std::vector<ItemData> itemVec, float _random);
		void CreateHealItem(Vector3 position);

		// �ֱ�, ���յ� ������ ������ �����ݴϴ�.
		void ClearItemInfo();

		bool GetIsBackDash() { return isBackDash; }
		void SetIsBackDash(bool val) { isBackDash = val; }

		bool GetCraftDone() { return craftDone; }
		void SetCraftDone(bool isCraft) { craftDone = isCraft; }

		// ���� �� ������ ��ȯ(���������� ���� �ʽ��ϴ�..)
		const std::shared_ptr<Customer>& GetCustomer(int idx);
		const std::shared_ptr<CockTailList>& GetOrder(int idx);
		const std::shared_ptr<CockTailStruct>& GetCockTail(int cocktailIdx);
		const std::shared_ptr<CockTailStruct>& GetCockTailCombi(int cocktailCombi);
		// Ĭ���� ���� ��밡�ɼ��� ������..
		const std::unordered_map<int, std::shared_ptr<CockTailStruct>>& GetCockTailMap() { return cockTailMap; }

		void ResetGameState();

	private:
		//bool enablePortals;
		int currentHP;
		int maxHP;
		int power;
		int originPower;
		int currentMoney;
		int maxMoney;

		// ���� ����
		int alcoholIngredient[7];

		// ����
		float attack1;
		float attack2;
		float attack3;
		float attack4;
		float moveSpeed;

		int currentKillCounter = 0;

		std::string playerStatPath;
		std::string logFilePath;

		// LogFile Stream���� �������..
		ofstream logFileStream;

		// ���� ������
		int enemyHP;

		/// floating UI���� ����
		// �ش� ���� ù ��������
		bool isLobbyFirstRun;
		bool isTutorialFirstRun;
		bool isDead;
		// Ư�� Ʈ���� ���ķδ� true�� ����.
		bool isLobbyPlay;
		bool isTutorialPlay;

		// item�� �Ծ��� �� ��� �ý��� �޽���.
		bool itemGetCounter;

		// ���� ���� ������ ���⼭ �������ְڽ��ϴ�.
			// ��ǻ� �ε����� �˴ϴ�.
		int nowPowerLevel;
		int nowHealthLevel;
		int nowSpeedLevel;
		// ���� ����
		int nowPrice;

		bool isBackDash = true;

	public:
		// ���
		bool isCutSceneCameraMove = false;
		bool isCutSceneTrigger = false;
		bool isGetKey = false;
		int playerDeadCnt = 0;
		bool isPlayerFirstDead = true;
		bool isFirstCraft = true;
		bool isAttack3 = false;
		bool isDodge = false;
		bool isAttack4 = false;
		bool isTutorialDodgeClear = false;
		// ù ���� ���� ����.
		bool isStoreCutSceneEnd = false;

		bool isRoom1Clear = false;
		bool isRoom2Clear = false;
		bool isRoom3Clear = false;

	public:
		// �÷��̾� �ִϸ��̼� ���ǵ� ��Ÿ�� ������ ���� ����
		static float idleLobbyAnimSpeed;
		static float idleAnimSpeed;
		static float walkLobbyAnimSpeed;
		static float walkAnimSpeed;
		static float attack1AnimSpeed;
		static float attack2AnimSpeed;
		static float attack3AnimSpeed;
		static float attack4AnimSpeed;
		static float damageAnimSpeed;
		static float damageUpAnimSpeed;
		static float dodgeAnimSpeed;

		// ����(����) �ִϸ��̼� ���ǵ� ����
		static float dogMeleeIdle1AnimSpeed;
		static float dogMeleeIdle2AnimSpeed;
		static float dogMeleeWalkAnimSpeed;
		static float dogMeleeAttackAnimSpeed;
		static float dogMeleeDamageAnimSpeed;
		static float dogMeleeDeathAnimSpeed;

		// ����(���Ÿ�) �ִϸ��̼� ���ǵ� ����
		static float dogArcherIdle1AnimSpeed;
		static float dogArcherIdle2AnimSpeed;
		static float dogArcherWalkAnimSpeed;
		static float dogArcherAttackAnimSpeed;
		static float dogArcherDamageAnimSpeed;
		static float dogArcherDeathAnimSpeed;

		// �ƽ� ����
		static bool isStage1CutScene;			// uiRoom1 �ƾ� üũ
		static bool isNeedKeyCutScenePlay;		// Ű�� �ʿ��ϴٴ� �ƾ��� ���Դ°�.

	private:
		std::vector<std::wstring> SplitText(const std::wstring& s, const std::wregex& sep_regex = std::wregex{ L"[/]" });
		void LoadStoreVector();
		void LoadCustomerMap();
		void LoadCockTailMap();
		void LoadOrderMap();
	};
}