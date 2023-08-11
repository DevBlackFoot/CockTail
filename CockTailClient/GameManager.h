#pragma once
#include "SceneManager.h"
#include "Util/JsonReader.h"
#include "ItemDrop.h"
#include "Inventory.h"

namespace ClientSpace
{
	class InGameUIController;

	// 클라이언트 전체에서 사용할 Struct;
	struct Customer
	{
		/*// Customer의 상태를 세팅합니다.
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
		// 각종 대사 정보들을 벡터로 가지고 있습니다. + 표정까지 추가로 가지고 있도록 합니다.
			// SmallTalk에 Order까지 추가로 넣습니다.
			// 반드시 순서대로 들어간다고 가정합니다! -> json 파일부터 순서대로 들어올 것 입니다.
		// 대사 / 표정 순입니다.
		std::vector<std::pair<wstring, wstring>> smallTalk;
		std::vector<std::pair<wstring, wstring>> goodTalk;
		std::vector<std::pair<wstring, wstring>> badTalk;
		std::vector<std::pair<wstring, wstring>> beerTalk;
		std::vector<std::pair<wstring, wstring>> byeTalk;


	};

	// 칵테일 주조할때 필요한 데이터들
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

	// 사실상 위의 내용으로 대체될 가능성이 많은 구조체입니다.
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

		// 상점 정보를 담고 있는 벡터
			// 계수 정보
		std::vector<float> powerUpTable;
		std::vector<int> healthUpTable;
		std::vector<float> speedUpTable;

		// 가격 정보를 담고있는 테이블
			// 단계 +1의 정보 (0번 인덱스면 1번으로 올라가기 위한 가격)
			// 2차원 벡터로 1은 힘, 2는 체력, 3은 스피드
		std::vector<std::vector<int>> priceTable;
		// 스킬 정보를 가지고 있는 벡터
		std::vector<std::pair<wstring, wstring>> skillDescTable;
		// 고객 정보를 가지고 있는 맵
		std::unordered_map<int, std::shared_ptr<Customer>> customerMap;
		// 비슷한 방식으로 각종 칵테일 관련 맵들을 만들어줍니다.
		std::unordered_map<int, std::shared_ptr<CockTailStruct>> cockTailMap;
		// 주문 요청 리스트
		std::unordered_map<int, std::shared_ptr<CockTailList>> cocktailOrderMap;

		ItemDrop* itemDrop;
		Inventory* inventory;

		// 난수 머신
		std::mt19937 randgen;
		// 그 회차의 크래프트를 돌렸는가.
			// 돌리면 true로.
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

		// HP 다음씬으로 넘기기위한 과정
		void SetHPs();

		void SetCurrentHP(int value) { currentHP = value; }
		int GetCurrentHP() { return currentHP; }
		void SetMaxHP(int value) { maxHP = value; }
		int GetMaxHP() { return maxHP; }

		// 공격 데이터
		void SetPower(int _power) { power = _power; }
		int GetPower() { return power; }
		float GetAttack1() { return attack1; }
		float GetAttack2() { return attack2; }
		float GetAttack3() { return attack3; }
		float GetAttack4() { return attack4; }

		// 재화 데이터
		/*void SetMoney();*/
		void SetCurrentMoney(int value) { currentMoney = value; }
		int GetCurrentMoney() { return currentMoney; }

		void SetMaxMoney(int value) { maxMoney = value; }
		int GetMaxMoney() { return maxMoney; }

		// 적 데이터
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

		// 상점 버튼 이벤트 세팅
		void StoreButtonEventSetting();

		std::vector<GameEngineSpace::JsonObject> GetJsonObject(string _name);

		std::unordered_map<std::wstring, std::vector<ItemData>> GetItemDataMap() { return itemDrop->GetItemDataMap(); }
		
		void DropItem(tstring name, Vector3 position);
		// 키 드랍 및 확인용 테스트 함수
		void DropKey(Vector3 position);

		// 아이템 관련
		//std::list<InventoryItem> GetItemList() { return inventory->GetItemList(); }
		//std::vector<int>& GetCraftedItems() { return inventory->GetCraftedItems(); }
		std::unordered_map<int, std::shared_ptr<InventoryItem>>& GetInventoryItemDataMap() { return inventory->GetItemDataMap(); }

		void PushItem(int index, int count = 1);
		void PopItem(int index, int count = 1);
		std::shared_ptr<InventoryItem> FindItem(int index);
		void RemoveItem(int index);

		void ResetRecentItem();
		const std::deque<std::wstring>& GetRecentItem();

		// 스탯 관련
		void StatGUIRender();
		void WriteGameLog(const std::string& logStr);
		void WriteEnemyLog(int id, int state, int maxHP, int remainHP, int dmg);

		/// Floating UI 관련
		// bool 값 Get Set
		bool GetLobbyFirstRun() { return isLobbyFirstRun; }
		// 재생되면 무조건 false로..
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

		// ItemCounter를 무조건 true로 바꿔줍니다.
			// 이후 UI에서 True일 때만 창을 띄워줍니다..
		void ResetItemGetCounter() { itemGetCounter = true; }
		void SetItemGetCounter(bool itemGetCounter) { this->itemGetCounter = itemGetCounter; }
		bool GetItemGetCounter() { return itemGetCounter; }

		float CreateRandomFloat(float min, float max);
		int CreateRandomInt(int min, int max);
		Vector3 RotateVector(float _x, float _y, float _z);

		// 상점 레벨을 1씩 올려주는 함수
		void PowerLevelUp();
		void HealthLevelUp();
		void SpeedLevelUp();

		int GetNowPrice() { return nowPrice; }
		int GetPowerLevel() { return nowPowerLevel; }
		int GetHealthLevel() { return nowHealthLevel; }
		int GetSpeedLevel() { return nowSpeedLevel; }

		// 스텟이 변할때 호출해서 실시간으로 세팅해 줍시다.
		void SetUpgradeStat();
		void ResetStat();

		void CreateItem(Vector3 position, std::vector<ItemData> itemVec, float _random);
		void CreateHealItem(Vector3 position);

		// 최근, 조합된 아이템 정보만 날려줍니다.
		void ClearItemInfo();

		bool GetIsBackDash() { return isBackDash; }
		void SetIsBackDash(bool val) { isBackDash = val; }

		bool GetCraftDone() { return craftDone; }
		void SetCraftDone(bool isCraft) { craftDone = isCraft; }

		// 각종 맵 데이터 반환(직접적으로 주지 않습니다..)
		const std::shared_ptr<Customer>& GetCustomer(int idx);
		const std::shared_ptr<CockTailList>& GetOrder(int idx);
		const std::shared_ptr<CockTailStruct>& GetCockTail(int cocktailIdx);
		const std::shared_ptr<CockTailStruct>& GetCockTailCombi(int cocktailCombi);
		// 칵테일 맵은 사용가능성이 있으니..
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

		// 술의 변수
		int alcoholIngredient[7];

		// 배율
		float attack1;
		float attack2;
		float attack3;
		float attack4;
		float moveSpeed;

		int currentKillCounter = 0;

		std::string playerStatPath;
		std::string logFilePath;

		// LogFile Stream으로 사용해줄..
		ofstream logFileStream;

		// 적의 에너지
		int enemyHP;

		/// floating UI관련 변수
		// 해당 씬에 첫 진입인지
		bool isLobbyFirstRun;
		bool isTutorialFirstRun;
		bool isDead;
		// 특정 트리거 이후로는 true로 세팅.
		bool isLobbyPlay;
		bool isTutorialPlay;

		// item을 먹었을 때 띄울 시스템 메시지.
		bool itemGetCounter;

		// 상정 관련 변수를 여기서 관리해주겠습니다.
			// 사실상 인덱스가 됩니다.
		int nowPowerLevel;
		int nowHealthLevel;
		int nowSpeedLevel;
		// 현재 가격
		int nowPrice;

		bool isBackDash = true;

	public:
		// 재우
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
		// 첫 상점 오픈 이후.
		bool isStoreCutSceneEnd = false;

		bool isRoom1Clear = false;
		bool isRoom2Clear = false;
		bool isRoom3Clear = false;

	public:
		// 플레이어 애니메이션 스피드 런타임 조정을 위한 변수
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

		// 몬스터(근접) 애니메이션 스피드 조정
		static float dogMeleeIdle1AnimSpeed;
		static float dogMeleeIdle2AnimSpeed;
		static float dogMeleeWalkAnimSpeed;
		static float dogMeleeAttackAnimSpeed;
		static float dogMeleeDamageAnimSpeed;
		static float dogMeleeDeathAnimSpeed;

		// 몬스터(원거리) 애니메이션 스피드 조정
		static float dogArcherIdle1AnimSpeed;
		static float dogArcherIdle2AnimSpeed;
		static float dogArcherWalkAnimSpeed;
		static float dogArcherAttackAnimSpeed;
		static float dogArcherDamageAnimSpeed;
		static float dogArcherDeathAnimSpeed;

		// 컷신 관련
		static bool isStage1CutScene;			// uiRoom1 컷씬 체크
		static bool isNeedKeyCutScenePlay;		// 키가 필요하다는 컷씬이 나왔는가.

	private:
		std::vector<std::wstring> SplitText(const std::wstring& s, const std::wregex& sep_regex = std::wregex{ L"[/]" });
		void LoadStoreVector();
		void LoadCustomerMap();
		void LoadCockTailMap();
		void LoadOrderMap();
	};
}