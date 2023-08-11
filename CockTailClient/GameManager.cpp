#include "pch.h"
#include "GameManager.h"
#include "Scripts/PlayerController.h"
#include <random>
#include <chrono>
// Scene.h
#include "TitleScene.h"
#include "Scenes/LobbyScene.h"
#include "Scenes/TutorialScene.h"
#include "Scenes/Room1Scene.h"
#include "Scenes/LeftPassage1Scene.h"
#include "Scenes/LeftPassage2Scene.h"
#include "Scenes/Room2Scene.h"
#include "Scenes/BarOperationScene.h"
#include "Scenes/StraightPassageScene.h"
#include "Scenes/LeftRoom3Scene.h"
#include "Scenes/RightRoom3Scene.h"
#include "Scenes/Room4Scene.h"
#include "Scenes/RightPassage1Scene.h"
#include "Scenes/RightPassage2Scene.h"
#include "Scenes/BossScene.h"
#include "Scenes/ItemResultScene.h"
#include "Scenes/ElevatorScene.h"
#include "Scenes/Elevator2Scene.h"
#include "InteractionItem.h"
#include "ItemObject.h"
#include "DataBinaryLayout.h"
#include "UIManager.h"
#include "InGameUIController.h"
#include "HPItem.h"

#include "../../FootGraphicsEngine/GraphicsEngineFramework.h"

namespace ClientSpace
{
	std::shared_ptr<GameManager> GameManager::instance = nullptr;

	GameManager::~GameManager()
	{
	}

	float GameManager::CreateRandomFloat(float min, float max)
	{
		//std::mt19937 gen(value);
		std::uniform_real_distribution<float> dis(min, max);
		return dis(randgen);
	}

	int GameManager::CreateRandomInt(int min, int max)
	{
		/*auto now = std::chrono::system_clock::now();
		auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
		auto value = now_ms.time_since_epoch().count();

		std::mt19937 gen(value);*/
		std::uniform_int_distribution<int> dis(min, max);
		return dis(randgen);
	}

	Vector3 GameManager::RotateVector(float _x, float _y, float _z)
	{
		Vector3 _upVector = { 0.f,1.0f,0.f };

		const double _PI = acos(-1);
		float x_rad = _x * _PI / 180.0f;
		float y_rad = _y * _PI / 180.0f;
		float z_rad = _z * _PI / 180.0f;

		float _c1 = cos(y_rad);
		float _c2 = cos(x_rad);
		float _c3 = cos(z_rad);
		float _s1 = sin(y_rad);
		float _s2 = sin(x_rad);
		float _s3 = sin(z_rad);

		float m_11 = (_c1 * _c3) + (_s1 * _s2 * _s3);
		float m_12 = _c2 * _s3;
		float m_13 = -(_s1)*_c3 + (_c1 * _s2 * _s3);

		float m_21 = -(_c1)*_s3 + (_s1 * _s2 * _c3);
		float m_22 = _c2 * _c3;
		float m_23 = (_s1 * _s3) + (_c1 * _s2 * _c3);

		float m_31 = _c2 * _s1;
		float m_32 = -(_s2);
		float m_33 = _c1 * _c2;

		Vector3 _result((m_11 * _upVector.x) + (m_12 * _upVector.y) + (m_13 * _upVector.z),
			(m_21 * _upVector.x) + (m_22 * _upVector.y) + (m_23 * _upVector.z),
			(m_31 * _upVector.x) + (m_32 * _upVector.y) + (m_33 * _upVector.z));

		return _result;
	}

	void GameManager::CreateItem(Vector3 position, std::vector<ItemData> itemVec, float _random)
	{
		float _dropRate = 0.0f;
		for (auto _itemData : itemVec)
		{
			_dropRate += _itemData.percentage;
			if (_random <= _dropRate)
			{
				shared_ptr<GameObject> _object = make_shared<GameObject>();
				_object->AddComponent<Transform>();
				_object->GetComponent<Transform>()->SetPosition({ position.x, position.y + 4.0f, position.z });
				_object->AddComponent<ItemObject>()->SetItemInfo(_itemData.itemIndex);
				// 그냥 Scene 에 올려주고 Awake Start 차례로 실행함
				SceneManager::GetInstance()->GetCurrentScene()->InstantiateGameObject(_object);

				//_object->GetComponent<ItemObject>()->PushObjectRandomVector(-45.0f, 45.0f);
				return;
			}
		}
	}

	void GameManager::CreateHealItem(Vector3 position)
	{
		shared_ptr<GameObject> _object = make_shared<GameObject>();
		_object->AddComponent<Transform>();
		_object->GetComponent<Transform>()->SetPosition({ position.x, position.y + 4.0f, position.z });
		_object->AddComponent<HPItem>();

		SceneManager::GetInstance()->GetCurrentScene()->InstantiateGameObject(_object);
	}

	// 로비로 돌아갈 때 한 번만 호출해줍니다. (최근 내용을 삭제해줍니다.)
	void GameManager::ClearItemInfo()
	{
		inventory->ClearRecentItem();
		inventory->ClearIngredientItem();
	}

	const std::shared_ptr<Customer>& GameManager::GetCustomer(int idx)
	{
		auto findIt = customerMap.find(idx);

		if (findIt != customerMap.end())
			return findIt->second;

		return nullptr;
	}

	const std::shared_ptr<CockTailList>& GameManager::GetOrder(int idx)
	{
		auto findIt = cocktailOrderMap.find(idx);

		if(findIt != cocktailOrderMap.end())
			return findIt->second;

		return nullptr;
	}

	const std::shared_ptr<CockTailStruct>& GameManager::GetCockTail(int cocktailIdx)
	{
		auto findIt = cockTailMap.find(cocktailIdx);

		if(findIt != cockTailMap.end())
			return findIt->second;

		return nullptr;
	}

	const std::shared_ptr<CockTailStruct>& GameManager::GetCockTailCombi(int cocktailCombi)
	{
		for(auto it : cockTailMap)
		{
			if(it.second->cocktailIndex == cocktailCombi)
				return it.second;
		}

		return nullptr;
	}

	std::vector<std::wstring> GameManager::SplitText(const std::wstring& s, const std::wregex& sep_regex)
	{
		std::wsregex_token_iterator iter(s.begin(), s.end(), sep_regex, -1);
		std::wsregex_token_iterator end;
		return { iter, end };
	}

	void GameManager::LoadStoreVector()
	{
		// 상점의 경우 제이슨을 읽어와서 여기에서 다 사용해줍니다.
		JsonReader upgradeReader = JsonReader("Resources/Table/UpgradeTable.json");
		auto upgradeObj = upgradeReader.GetJsonObjects();

		std::vector<int> powerPrice;
		std::vector<int> healthPrice;
		std::vector<int> speedPrice;

		std::pair<wstring, wstring> powerDesc;
		std::pair<wstring, wstring> healthDesc;
		std::pair<wstring, wstring> speedDesc;


		for (auto jsonObj : upgradeObj)
		{
			wstring nowStr = jsonObj.GetString(L"UPgradeName");

			// 순서대로 들어온다고 가정합니다.
			if (nowStr == L"PowerUp")
			{
				float powerCoef = jsonObj.GetFloat(L"Power");
				powerUpTable.push_back(powerCoef);

				int nowNum = jsonObj.GetInt(L"UpgradeNum");
				if (nowNum == 0)
				{
					wstring desc = jsonObj.GetString(L"Description");
					wstring point = jsonObj.GetString(L"Point");

					powerDesc = std::pair<wstring, wstring>({ desc, point });
				}
				else
				{
					int tmpPrice = jsonObj.GetInt(L"Amount");
					powerPrice.push_back(tmpPrice);
				}
			}
			else if (nowStr == L"HealthUp")
			{
				int nowHealth = jsonObj.GetInt(L"HP");
				healthUpTable.push_back(nowHealth);

				int nowNum = jsonObj.GetInt(L"UpgradeNum");
				if (nowNum == 0)
				{
					wstring desc = jsonObj.GetString(L"Description");
					wstring point = jsonObj.GetString(L"Point");

					healthDesc = std::pair<wstring, wstring>({ desc, point });
				}
				else
				{
					int tmpPrice = jsonObj.GetInt(L"Amount");
					healthPrice.push_back(tmpPrice);
				}
			}
			else if (nowStr == L"SpeedUp")
			{
				float speedCoef = jsonObj.GetFloat(L"Movement");
				speedUpTable.push_back(speedCoef);

				int nowNum = jsonObj.GetInt(L"UpgradeNum");
				if (nowNum == 0)
				{
					wstring desc = jsonObj.GetString(L"Description");
					wstring point = jsonObj.GetString(L"Point");

					speedDesc = std::pair<wstring, wstring>({ desc, point });
				}
				else
				{
					int tmpPrice = jsonObj.GetInt(L"Amount");
					speedPrice.push_back(tmpPrice);
				}
			}
		}

		// 다 돈 다음에 넣어줍니다.
		priceTable.push_back(powerPrice);
		priceTable.push_back(healthPrice);
		priceTable.push_back(speedPrice);

		skillDescTable.push_back(powerDesc);
		skillDescTable.push_back(healthDesc);
		skillDescTable.push_back(speedDesc);
	}

	void GameManager::LoadCustomerMap()
	{
		// bar Script를 사용해줍시다..
		wstring nowType = L"";
		wstring nowState = L"";
		int tmpidx = 0;
		std::shared_ptr<Customer> tmpCustomer;
		for(auto& data : barScriptDataTable)
		{
			// 현재 상황을 가져옵니다.
			wstring tmpState = data.GetString(L"ScriptState");
			auto slice = SplitText(tmpState);
			nowState = slice[0];
			wstring spriteName = data.GetString(L"SpriteName");
			wstring tmpScript = data.GetString(L"ScriptStringKR");

			// 0번은 무조건 state, 1번은 타입..
			if(slice[1] != nowType)
			{
				if(tmpCustomer != nullptr)
				{
					customerMap.insert(std::make_pair(tmpidx, tmpCustomer));
					tmpidx++;
				}
				// tmpCustomer를 새로 만들어줍니다.
				tmpCustomer = std::make_shared<Customer>();
				// 여기서 type을 바꿔줍니다.
				nowType = slice[1];
				// 이름을 넣어줍니다.
					// 한번만 넣어주면 됩니다.
				wstring nameKR = data.GetString(L"NameKR");
				tmpCustomer->name = nameKR;
			}

			// nowState에 따라서 넣어줍니다.
			if(nowState == L"Smalltalk" || nowState == L"Order")
			{
				tmpCustomer->smallTalk.push_back(std::make_pair(tmpScript, spriteName));
			}
			else if(nowState == L"Good")
			{
				tmpCustomer->goodTalk.push_back(std::make_pair(tmpScript, spriteName));
			}
			else if(nowState == L"Bad")
			{
				tmpCustomer->badTalk.push_back(std::make_pair(tmpScript, spriteName));
			}
			else if (nowState == L"Beer")
			{
				tmpCustomer->beerTalk.push_back(std::make_pair(tmpScript, spriteName));
			}
			else if (nowState == L"Goodbye")
			{
				tmpCustomer->byeTalk.push_back(std::make_pair(tmpScript, spriteName));
			}
		}

		if (tmpCustomer != nullptr)
		{
			customerMap.insert(std::make_pair(tmpidx, tmpCustomer));
		}
	}

	void GameManager::LoadCockTailMap()
	{
		for (auto json : cocktailTable)
		{
			std::shared_ptr<CockTailStruct> tmpCockTail = std::make_shared<CockTailStruct>();

			tmpCockTail->cocktailNameKR = json.GetString(L"CocktailNameKR");
						  
			tmpCockTail->cocktailNameEN = json.GetString(L"CocktailNameEN");
						  
			tmpCockTail->cocktailIndex = json.GetInt(L"CocktailIndex");

			tmpCockTail->hoverIdx = json.GetInt(L"HoverIndex");
						  
			tmpCockTail->ingredients = json.GetArray<int>(L"Ingredients");
						  
			tmpCockTail->descriptionKR = json.GetString(L"DescriptionKR");
						  
			tmpCockTail->descriptionEN = json.GetString(L"DescriptionEN");
						  
			tmpCockTail->feature = json.GetArray<int>(L"Feature");
						  
			tmpCockTail->amount = json.GetInt(L"Amount");

			cockTailMap.insert(std::make_pair(tmpCockTail->hoverIdx, tmpCockTail));
		}
	}

	void GameManager::LoadOrderMap()
	{
		for(auto json : cocktailListTable)
		{
			std::shared_ptr<CockTailList> tmpOrder = std::make_shared<CockTailList>();

			tmpOrder->customerScriptID = json.GetInt(L"CustomerScriptID");

			tmpOrder->request1 = json.GetInt(L"Request1");

			tmpOrder->request2 = json.GetInt(L"Request2");

			tmpOrder->EN = json.GetString(L"EN");

			tmpOrder->KR = json.GetString(L"KR");

			// ID는 0~74번 까지 입니다 => 상황 따라서 랜덤으로 내뱉습니다.
			cocktailOrderMap.insert(std::make_pair(tmpOrder->customerScriptID, tmpOrder));
		}
	}

	float GameManager::idleLobbyAnimSpeed = 1.5f;
	float GameManager::idleAnimSpeed = 1.5f;
	float GameManager::walkLobbyAnimSpeed = 2.f;
	float GameManager::walkAnimSpeed = 2.f;
	float GameManager::attack1AnimSpeed = 5.f;
	float GameManager::attack2AnimSpeed = 2.5f;
	float GameManager::attack3AnimSpeed = 3.5f;
	float GameManager::attack4AnimSpeed = 5.0f;
	float GameManager::damageAnimSpeed = 4.5f;
	float GameManager::damageUpAnimSpeed = 4.5f;
	float GameManager::dodgeAnimSpeed = 1.0f;

	/*float GameManager::idleLobbyAnimSpeed = 1.f;
	float GameManager::idleAnimSpeed = 1.f;
	float GameManager::walkLobbyAnimSpeed = 1.f;
	float GameManager::walkAnimSpeed = 1.f;
	float GameManager::attack1AnimSpeed = 1.f;
	float GameManager::attack2AnimSpeed = 1.f;
	float GameManager::attack3AnimSpeed = 1.f;
	float GameManager::attack4AnimSpeed = 1.f;
	float GameManager::damageAnimSpeed = 1.f;
	float GameManager::damageUpAnimSpeed = 1.f;*/

	float GameManager::dogMeleeIdle1AnimSpeed = 2.0f;
	float GameManager::dogMeleeIdle2AnimSpeed = 2.0f;
	float GameManager::dogMeleeWalkAnimSpeed = 3.0f;
	float GameManager::dogMeleeAttackAnimSpeed = 1.5f;
	float GameManager::dogMeleeDamageAnimSpeed = 5.0f;
	float GameManager::dogMeleeDeathAnimSpeed = 3.0f;

	float GameManager::dogArcherIdle1AnimSpeed = 1.0f;
	float GameManager::dogArcherIdle2AnimSpeed = 1.0f;
	float GameManager::dogArcherWalkAnimSpeed = 1.0f;
	float GameManager::dogArcherAttackAnimSpeed = 1.0f;
	float GameManager::dogArcherDamageAnimSpeed = 1.0f;
	float GameManager::dogArcherDeathAnimSpeed = 1.0f;

	bool GameManager::isStage1CutScene = true;
	bool GameManager::isNeedKeyCutScenePlay = false;


	std::shared_ptr<GameManager> GameManager::GetInstance()
	{
		if (instance == nullptr)
			instance = std::make_shared<GameManager>();

		return instance;
	}

	/// <summary>
	/// 담당자 : 이재환 , 정해민
	/// </summary>
	///  
	void GameManager::InitScene()
	{
		LoadSound();

		// 씬들 생성하는 곳 입니다 
		std::shared_ptr<TitleScene> titleScene = std::make_shared<TitleScene>(TEXT("TitleScene"));
		GameEngineSpace::SceneManager::GetInstance()->AddGameScene(titleScene);

		std::shared_ptr<LobbyScene> lobbyScene = std::make_shared<LobbyScene>(TEXT("LobbyScene"));
		GameEngineSpace::SceneManager::GetInstance()->AddGameScene(lobbyScene);

		std::shared_ptr<ClientSpace::Room1Scene> room1Scene = std::make_shared<ClientSpace::Room1Scene>(TEXT("Room1Scene"));
		GameEngineSpace::SceneManager::GetInstance()->AddGameScene(room1Scene);

		std::shared_ptr<TutorialScene> tutorialScene = std::make_shared<TutorialScene>(TEXT("TutorialScene"));
		GameEngineSpace::SceneManager::GetInstance()->AddGameScene(tutorialScene);

		std::shared_ptr<ClientSpace::Room2Scene> room2Scene = std::make_shared<ClientSpace::Room2Scene>(TEXT("Room2Scene"));
		GameEngineSpace::SceneManager::GetInstance()->AddGameScene(room2Scene);

		std::shared_ptr<ClientSpace::LeftRoom3Scene> leftRoom3Scene = std::make_shared<ClientSpace::LeftRoom3Scene>(TEXT("LeftRoom3Scene"));
		GameEngineSpace::SceneManager::GetInstance()->AddGameScene(leftRoom3Scene);

		std::shared_ptr<ClientSpace::Room4Scene> room4Scene = std::make_shared<ClientSpace::Room4Scene>(TEXT("Room4Scene"));
		GameEngineSpace::SceneManager::GetInstance()->AddGameScene(room4Scene);

		std::shared_ptr<ClientSpace::LeftPassage1Scene> leftPassage1Scene = std::make_shared<ClientSpace::LeftPassage1Scene>(TEXT("LeftPassage1Scene"));
		GameEngineSpace::SceneManager::GetInstance()->AddGameScene(leftPassage1Scene);

		std::shared_ptr<ClientSpace::LeftPassage2Scene> leftPassage2Scene = std::make_shared<ClientSpace::LeftPassage2Scene>(TEXT("LeftPassage2Scene"));
		GameEngineSpace::SceneManager::GetInstance()->AddGameScene(leftPassage2Scene);

		std::shared_ptr<ClientSpace::StraightPassageScene> straightPassageScene = std::make_shared<ClientSpace::StraightPassageScene>(TEXT("StraightPassageScene"));
		GameEngineSpace::SceneManager::GetInstance()->AddGameScene(straightPassageScene);

		std::shared_ptr<ClientSpace::BarOperationScene> barOperationScene = std::make_shared<ClientSpace::BarOperationScene>(TEXT("BarOperationScene"));
		GameEngineSpace::SceneManager::GetInstance()->AddGameScene(barOperationScene);

		std::shared_ptr<ClientSpace::ElevatorScene> elevatorScene = std::make_shared<ClientSpace::ElevatorScene>(TEXT("ElevatorScene"));
		GameEngineSpace::SceneManager::GetInstance()->AddGameScene(elevatorScene);

		std::shared_ptr<ClientSpace::Elevator2Scene> elevator2Scene = std::make_shared<ClientSpace::Elevator2Scene>(TEXT("Elevator2Scene"));
		GameEngineSpace::SceneManager::GetInstance()->AddGameScene(elevator2Scene);

		std::shared_ptr<ClientSpace::BossScene> bossScene = std::make_shared<ClientSpace::BossScene>(TEXT("BossScene"));
		GameEngineSpace::SceneManager::GetInstance()->AddGameScene(bossScene);

		std::shared_ptr<ClientSpace::ItemResultScene> resultScene = std::make_shared<ClientSpace::ItemResultScene>(TEXT("ItemResultScene"));
		GameEngineSpace::SceneManager::GetInstance()->AddGameScene(resultScene);

		GameEngineSpace::SceneManager::GetInstance()->LoadScene(TEXT("TitleScene"));
	}

	void GameManager::Initialize()
	{
		currentHP = 3;
		currentKillCounter = 0;
		currentMoney = 0;
		maxMoney = 100000;
		nowPrice = 0;

		nowPowerLevel = 0;
		nowHealthLevel = 0;
		nowSpeedLevel = 0;

		LoadWholeJsonFiles();

		/*for (auto _json : ChracterDataTable)
		{
			power = _json.GetInt(L"Power");
			maxHP = _json.GetInt(L"HP");
			attack1 = _json.GetFloat(L"attack1");
			attack2 = _json.GetFloat(L"attack2");
			attack3 = _json.GetFloat(L"attack3");
			moveSpeed = _json.GetFloat(L"Movement");
		}*/

		playerStatPath = "BinaryFile/Stats/Player.stat";
		time_t timer = time(NULL);
		tm t = tm();
		auto lt = localtime_s(&t, &timer);

		// 게임을 시작할 때 현재 시간에 맞는 로그를 생성합니다.
		logFilePath = "Log/Log"
			+ to_string(t.tm_year + 1900) + "-"
			+ to_string(t.tm_mon + 1) + "-"
			+ to_string(t.tm_mday) + "_"
			+ to_string(t.tm_hour) + "_"
			+ to_string(t.tm_min) + ".csv";

		// 로그의 첫 줄을 넣어줍니다.
		/*
		*/
		logFileStream.open(logFilePath.data());

		if (logFileStream.is_open())
		{
			// csv에 들어가는 순서로 넣어준 것입니다. 이 부분은 변경 가능합니다.
			logFileStream << "Enemy ID,Enemy State,Enemy MaxHP,Enemy RemainHP,DMG\n";
			logFileStream.close();
		}

		// 플레이어 스텟에 대한 바이너리 파일이 있다면 들고오고 아니면 만들어줄것이다.
		ClientSpace::PlayerData binaryPlayerStat;

		std::ifstream ifs(playerStatPath, std::ios_base::binary);

		// 파일이 존재하지 않는 경우
		if (!ifs.is_open())
		{
			// 새로운 스텟 데이터를 만들어줘야 하는 경우에는 여기서 처리한다.

			std::ofstream ofs(playerStatPath, std::ios_base::binary);
			boost::iostreams::filtering_stream<boost::iostreams::output> buffer;
			buffer.push(boost::iostreams::zlib_compressor());
			buffer.push(ofs);
			boost::archive::binary_oarchive oaPlayerStat(buffer);

			binaryPlayerStat.hp = 5;
			binaryPlayerStat.power = 40;
			binaryPlayerStat.attack1 = 0.6f;
			binaryPlayerStat.attack2 = 1.2f;
			binaryPlayerStat.attack3 = 1.5f;
			binaryPlayerStat.attack4 = 0.5f;
			binaryPlayerStat.moveSpeed = 1.0f;
			binaryPlayerStat.enemyHp = 120;

			oaPlayerStat << binaryPlayerStat;
		}
		else
		{
			boost::iostreams::filtering_streambuf<boost::iostreams::input> buffer;
			buffer.push(boost::iostreams::zlib_decompressor());
			buffer.push(ifs);
			boost::archive::binary_iarchive iaPlayerStat(buffer);
			iaPlayerStat >> binaryPlayerStat;
		}

		maxHP = binaryPlayerStat.hp;
		originPower = binaryPlayerStat.power;
		power = originPower;
		attack1 = binaryPlayerStat.attack1;
		attack2 = binaryPlayerStat.attack2;
		attack3 = binaryPlayerStat.attack3;
		attack4 = binaryPlayerStat.attack4;
		moveSpeed = binaryPlayerStat.moveSpeed;
		enemyHP =binaryPlayerStat.enemyHp;

		itemDrop = new ItemDrop();
		itemDrop->CreateItemDatas(itemDropTable);

		inventory = new Inventory();
		inventory->CreateItemDatas(itemDataTable);

		/// floating UI관련 변수
		// init 할 때는 true로.
		isLobbyFirstRun = true;
		isTutorialFirstRun = true;
		// init 할 때는 false로
		isLobbyPlay = false;
		isTutorialPlay = false;

		itemGetCounter = false;

		auto now = std::chrono::system_clock::now();
		auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
		auto value = now_ms.time_since_epoch().count();

		randgen = std::mt19937(value);

		// 다 한 다음에 세팅
		StoreButtonEventSetting();
	}

	void GameManager::SetHPs()
	{
		std::shared_ptr<PlayerController> player = GameEngineSpace::SceneManager::GetInstance()->FindObjectName(L"Player")->GetComponent<PlayerController>();
		if (player->GetHP() == 0)
			player->SetHP(maxHP);

		currentHP = player->GetHP();
		maxHP = player->GetMaxHP();
	}

	//void GameManager::SetMoney()
	//{
	//	currentMoney = 0;
	//	maxMoney = 10000;
	//}

	void GameManager::LoadSound()
	{
		auto soundManager = SoundManager::GetInstance();

		// 배경BGM
		soundManager->LoadBackgroundSound("Resources/Sounds/Common/Lobbybgm.wav", "LobbyBGM", true);
		soundManager->LoadBackgroundSound("Resources/Sounds/Common/Battlebgm.wav", "BattleBGM", true);

		// Battle
		soundManager->LoadSFXSound("Resources/Sounds/Battle/AttackNoti.wav", "AttackNoti", false);
		soundManager->LoadSFXSound("Resources/Sounds/Battle/BossDead.wav", "BossDead", false);
		soundManager->LoadSFXSound("Resources/Sounds/Battle/BossHit.wav", "BossHit", false);
		soundManager->LoadSFXSound("Resources/Sounds/Battle/BossSpawn.wav", "BossSpawn", false);
		soundManager->LoadSFXSound("Resources/Sounds/Battle/DogHit.wav", "DogHit", false);
		soundManager->LoadSFXSound("Resources/Sounds/Battle/ElevatorEngineStart.wav", "ElevatorEngineStart", false);
		soundManager->LoadSFXSound("Resources/Sounds/Battle/Elevatorexample.wav", "Elevatorexample", false);
		soundManager->LoadSFXSound("Resources/Sounds/Battle/ElevatorLoop.wav", "MoveElevator", true);
		soundManager->LoadSFXSound("Resources/Sounds/Battle/ElevatorStart.wav", "ElevatorSwitchOn", false);
		soundManager->LoadSFXSound("Resources/Sounds/Battle/ElevatorStop.wav", "ElevatorStop", false);
		soundManager->LoadSFXSound("Resources/Sounds/Battle/ESC.wav", "ESC", false);
		soundManager->LoadSFXSound("Resources/Sounds/Battle/GetItem.wav", "GetItem", false);
		soundManager->LoadSFXSound("Resources/Sounds/Battle/HP.wav", "HP", false);
		soundManager->LoadSFXSound("Resources/Sounds/Battle/MissionClear.wav", "MissionClear", false);
		soundManager->LoadSFXSound("Resources/Sounds/Battle/PerfectDodge.wav", "PerfectDodge", false);
		soundManager->LoadSFXSound("Resources/Sounds/Battle/PortalOpen.wav", "PortalOpen", false);
		soundManager->LoadSFXSound("Resources/Sounds/Battle/TailAttack1.wav", "PlayerTailAttack1", false);
		soundManager->LoadSFXSound("Resources/Sounds/Battle/TailAttack2.wav", "PlayerTailAttack2", false);
		soundManager->LoadSFXSound("Resources/Sounds/Battle/TailAttack3.wav", "PlayerTailAttack3", false);
		soundManager->LoadSFXSound("Resources/Sounds/Battle/TailAttack4.wav", "PlayerTailAttack4", false);
		soundManager->LoadSFXSound("Resources/Sounds/Battle/TailDamaged.wav", "TailDamaged", false);
		soundManager->LoadSFXSound("Resources/Sounds/Battle/TailDodge.wav", "TailDodge", false);
		soundManager->LoadSFXSound("Resources/Sounds/Battle/ToadDie.wav", "ToadDie", false);
		soundManager->LoadSFXSound("Resources/Sounds/Battle/TutorialGood.wav", "TutorialGood", false);

		// Craft
		soundManager->LoadSFXSound("Resources/Sounds/Craft/ClickSounds.wav", "ClickSounds", false);
		soundManager->LoadSFXSound("Resources/Sounds/Craft/miniGameS.wav", "MiniGameS", false);
		soundManager->LoadSFXSound("Resources/Sounds/Craft/miniGameF.wav", "MiniGameF", false);
		soundManager->LoadSFXSound("Resources/Sounds/Craft/miniGameCompleteS.wav", "MiniGameCompleteS", false);
		soundManager->LoadSFXSound("Resources/Sounds/Craft/miniGameCompleteF.wav", "MiniGameCompleteF", false);
		soundManager->LoadSFXSound("Resources/Sounds/Craft/RecipeOpen.wav", "RecipeOpen", false);
		soundManager->LoadSFXSound("Resources/Sounds/Craft/RecipeClose.wav", "RecipeClose", false);
		soundManager->LoadSFXSound("Resources/Sounds/Craft/GlassBottle.wav", "GlassBottle", false);
		soundManager->LoadSFXSound("Resources/Sounds/Craft/CanSound.wav", "CanSound", false);
		soundManager->LoadSFXSound("Resources/Sounds/Craft/CountDown.wav", "CountDown", false);
		soundManager->LoadSFXSound("Resources/Sounds/Craft/Guest.wav", "Guest", false);
		soundManager->LoadSFXSound("Resources/Sounds/Craft/Cashier.wav", "Cashier", false);
		soundManager->LoadSFXSound("Resources/Sounds/Craft/Reset.wav", "Reset", false);

		// 강화
		soundManager->LoadSFXSound("Resources/Sounds/Craft/PowerUp.wav", "PowerUp", false);
		soundManager->LoadSFXSound("Resources/Sounds/Craft/PowerFail.wav", "PowerFail", false);
	}

	void GameManager::LoadWholeJsonFiles()
	{
		json_ItemDataTable = std::make_unique<JsonReader>("Resources/Table/IngredientTable.json");
		json_ItemDropTable = std::make_unique<JsonReader>("Resources/Table/ItemDropTable.json");
		json_CocktailTable = std::make_unique<JsonReader>("Resources/Table/CocktailTable.Json");
		json_CocktailListTable = std::make_unique<JsonReader>("Resources/Table/ListTable.Json");
		json_CharacterDataTable = std::make_unique<JsonReader>("Resources/Table/PlayerStatus.json");
		json_LobbyScriptDataTablde = std::make_unique<JsonReader>("Resources/Table/SceneTable.json");
		json_BarScriptDataTablde = std::make_unique<JsonReader>("Resources/Table/CustomerTable.json");
		json_WaveBattleTable = std::make_unique<JsonReader>("Resources/Table/WaveBattle.json");

		itemDataTable = json_ItemDataTable->GetJsonObjects();
		itemDropTable = json_ItemDropTable->GetJsonObjects();
		cocktailTable = json_CocktailTable->GetJsonObjects();
		cocktailListTable = json_CocktailListTable->GetJsonObjects();
		ChracterDataTable = json_CharacterDataTable->GetJsonObjects();
		lobbyScriptDataTable = json_LobbyScriptDataTablde->GetJsonObjects();
		barScriptDataTable = json_BarScriptDataTablde->GetJsonObjects();
		waveBattleTable = json_WaveBattleTable->GetJsonObjects();

		LoadStoreVector();
		LoadCustomerMap();
		LoadCockTailMap();
		LoadOrderMap();
	}

	void GameManager::StoreButtonEventSetting()
	{
		auto canvas = UIManager::GetInstance()->GetCanvas("StoreCanvas");
		auto tmpButton = canvas->GetButtonUI("SkillSelectButton0");

		// 0 - Power;
		tmpButton->SetHoverEvent([&]
			{
				auto storeCanvas = UIManager::GetInstance()->GetCanvas("StoreCanvas");
				if (nowPowerLevel < priceTable[0].size())
				{
					// 기술 세팅
					storeCanvas->GetTextUI("SkillDesc")->SetNText(skillDescTable[0].first);
					storeCanvas->GetTextUI("SkillPoint")->SetStyleText(skillDescTable[0].second);

					// amount 세팅
					storeCanvas->GetTextUI("Cost")->SetText(std::to_wstring(priceTable[0][nowPowerLevel]));
					nowPrice = priceTable[0][nowPowerLevel];

					if (nowPrice > currentMoney)
						storeCanvas->GetTextUI("Cost")->SetColor({ 1.f, 0.f, 0.f, 1.f });
					else
						storeCanvas->GetTextUI("Cost")->SetColor({ 1.f, 1.f, 1.f, 1.f });
				}
				else
				{
					// 풀강이면 표시하지 않는다. + 자기 자신을 무효화 한다.
					// 기술 세팅
					storeCanvas->GetTextUI("SkillDesc")->SetNText(L"");
					storeCanvas->GetTextUI("SkillPoint")->SetNText(L"");

					// amount 세팅
					storeCanvas->GetTextUI("Cost")->SetText(L"");
					nowPrice = 0;
					storeCanvas->GetButtonUI("SkillSelectButton0")->SetEnable(false);
				}
			});

		tmpButton = canvas->GetButtonUI("SkillSelectButton1");

		// 1 - Health;
		tmpButton->SetHoverEvent([&]
			{
				auto storeCanvas = UIManager::GetInstance()->GetCanvas("StoreCanvas");
				if (nowHealthLevel < priceTable[1].size())
				{
					storeCanvas->GetTextUI("SkillDesc")->SetNText(skillDescTable[1].first);
					storeCanvas->GetTextUI("SkillPoint")->SetStyleText(skillDescTable[1].second);

					storeCanvas->GetTextUI("Cost")->SetText(std::to_wstring(priceTable[1][nowHealthLevel]));
					nowPrice = priceTable[1][nowHealthLevel];

					if(nowPrice > currentMoney)
						storeCanvas->GetTextUI("Cost")->SetColor({ 1.f, 0.f, 0.f, 1.f });
					else
						storeCanvas->GetTextUI("Cost")->SetColor({ 1.f, 1.f, 1.f, 1.f });
				}
				else
				{
					// 풀강이면 표시하지 않는다.

					// 기술 세팅
					storeCanvas->GetTextUI("SkillDesc")->SetNText(L"");
					storeCanvas->GetTextUI("SkillPoint")->SetNText(L"");

					// amount 세팅
					storeCanvas->GetTextUI("Cost")->SetText(L"");
					nowPrice = 0;

					storeCanvas->GetButtonUI("SkillSelectButton1")->SetEnable(false);
				}
			});

		tmpButton = canvas->GetButtonUI("SkillSelectButton2");

		// 2 - Power;
		tmpButton->SetHoverEvent([&]
			{
				auto storeCanvas = UIManager::GetInstance()->GetCanvas("StoreCanvas");
				if (nowSpeedLevel < priceTable[2].size())
				{
					// 기술 세팅
					storeCanvas->GetTextUI("SkillDesc")->SetNText(skillDescTable[2].first);
					storeCanvas->GetTextUI("SkillPoint")->SetStyleText(skillDescTable[2].second);

					// amount 세팅
					storeCanvas->GetTextUI("Cost")->SetText(std::to_wstring(priceTable[2][nowSpeedLevel]));
					nowPrice = priceTable[2][nowSpeedLevel];

					if (nowPrice > currentMoney)
						storeCanvas->GetTextUI("Cost")->SetColor({ 1.f, 0.f, 0.f, 1.f });
					else
						storeCanvas->GetTextUI("Cost")->SetColor({ 1.f, 1.f, 1.f, 1.f });
				}
				else
				{
					// 풀강이면 표시하지 않는다.
					// 기술 세팅
					storeCanvas->GetTextUI("SkillDesc")->SetNText(L"");
					storeCanvas->GetTextUI("SkillPoint")->SetNText(L"");

					// amount 세팅
					storeCanvas->GetTextUI("Cost")->SetText(L"");
					nowPrice = 0;
					storeCanvas->GetButtonUI("SkillSelectButton2")->SetEnable(false);
				}
			});

		for (int i = 0; i < 3; i++)
		{
			std::string idx = to_string(i);

			auto outButton = canvas->GetButtonUI("SkillSelectButton" + idx);

			outButton->SetOutEvent([&]
				{
					auto storeCanvas = UIManager::GetInstance()->GetCanvas("StoreCanvas");

					// 기술 세팅
					storeCanvas->GetTextUI("SkillDesc")->SetNText(L"");
					storeCanvas->GetTextUI("SkillPoint")->SetNText(L"");

					// amount 세팅
					storeCanvas->GetTextUI("Cost")->SetText(L"");
					nowPrice = 0;
				});
		}

	}

	std::vector<GameEngineSpace::JsonObject> GameManager::GetJsonObject(string _name)
	{
		if (_name == "itemData")
		{
			return itemDataTable;
		}

		else if (_name == "itemDrop")
		{
			return itemDropTable;
		}

		else if (_name == "cocktailItemData")
		{
			return cocktailTable;
		}

		else if (_name == "cocktailList")
		{
			return cocktailListTable;
		}

		else if (_name == "characterData")
		{
			return ChracterDataTable;
		}

		else if (_name == "lobbyScriptData")
		{
			return lobbyScriptDataTable;
		}

		else if (_name == "barScriptData")
		{
			return barScriptDataTable;
		}

		if (_name == "waveBattle")
		{
			return waveBattleTable;
		}

		return std::vector<GameEngineSpace::JsonObject>();
	}

	void GameManager::DropItem(tstring name, Vector3 position)
	{
		auto _itemVector = GetItemDataMap()[name];
		float _totalRate = 0.0;
		for (auto _itemData : _itemVector)
		{
			_totalRate += _itemData.percentage;
		}

		int count = 0;

		if (name == L"Dog")
			count = CreateRandomInt(4, 5);
		//1,3
		else if (name == L"Toad")
		{
			count = CreateRandomInt(7, 8);
			CreateHealItem(position);
		}

		for (int i = 0; i < count; i++)
		{
			float _random = CreateRandomFloat(0.0f, 1.0f);
			_random *= _totalRate;

			CreateItem(position, _itemVector, _random);
		}
	}

	void GameManager::DropKey(Vector3 position)
	{
		/*shared_ptr<GameObject> _object = make_shared<GameObject>();
		_object->AddComponent<Transform>();
		_object->GetComponent<Transform>()->SetPosition({ position.x, position.y + 1.0f, position.z });
		_object->AddComponent<ItemObject>()->SetItemInfo(0);

		SceneManager::GetInstance()->GetCurrentScene()->InstantiateGameObject(_object);*/

		shared_ptr<GameObject> item = ObjectPool::GetInstance()->AcquireGameObject("Item");

		if (item != nullptr)
		{
			item->GetComponent<Transform>()->SetPosition({ position.x, position.y + 1.0f, position.z });
			item->GetComponent<ItemObject>()->SetItemInfo(0);
			item->GetComponent<ItemObject>()->PushObjectRandomVector(-45.0f, 45.0f);
			item->GetComponent<ItemObject>()->SetPlayer();
		}
	}

	void GameManager::PushItem(int index, int count /*= 1*/)
	{
		inventory->PushItem(index, count);
		ResetItemGetCounter();
	}

	void GameManager::PopItem(int index, int count /*= 1*/)
	{
		inventory->PopItem(index, count);
	}


	std::shared_ptr<InventoryItem> GameManager::FindItem(int index)
	{
		return inventory->FindItem(index);
	}

	void GameManager::RemoveItem(int index)
	{
		inventory->RemoveItem(index);
	}

	void GameManager::ResetRecentItem()
	{
		inventory->ClearRecentItem();
	}

	const std::deque<std::wstring>& GameManager::GetRecentItem()
	{
		return inventory->GetRecentItem();
	}


	void GameManager::StatGUIRender()
	{
		auto imgui = ImGUIManager::GetInstance();

		// 입력 받을 곳의 Text
		/*
		static char msgBox[128] = "";
		// 해당 텍스트를 가져서 지니고 있을 string
		static std::string outputText = "None";

		if(imgui->Begin("Log Menu"))
		{
			imgui->InputText(" ", "Log File Name", msgBox, sizeof(msgBox));
			// 버튼과 Input 영역을 같은줄에 둡니다.
			imgui->SameLine();

			// 버튼을 누르면 => if문 안의 내용을 실행합니다.
			if (imgui->Button("Create"))
			{
				// msgBox내의 내용을 outputText로 옮깁니다.
				outputText.assign(msgBox, 128);
				// 옮김과 동시에 input text 영역을 지워줍니다.
				memset(&msgBox[0], 0, sizeof(msgBox));

				// extension 확인
				std::string ext;
				// 첫번째 점을 찾습니다.
				int extIdx = outputText.rfind(".");

				if(extIdx != -1)
				{
					// Idx에서 끝까지 뽑아내줍니다
						// 뒤의 null 문자로 인식을 제대로 못했기 때문에 4개만 뽑아줍니다..
					ext = outputText.substr(extIdx, 4);
				}

				if(ext == ".txt" || ext == ".csv")
				{
					logFilePath = "Log/" + outputText;

					// 기본적으로 같은 이름의 파일이 존재하면, 해당 파일을 지우고 새로 생성합니다.
					logFileStream.open(logFilePath.data());

					if(logFileStream.is_open())
						logFileStream.close();
				}
				else
				{
					//WriteGameLog(outputText);
					WriteEnemyLog(1, outputText, 120, 60, 20);
				}
			}
		}
		imgui->End();
		*/

		// 현재 스텟 확인용 창
		if (imgui->Begin("Now Stat"))
		{
			imgui->Text("Power : " + to_string(power));
			imgui->Text("Speed : " + to_string(moveSpeed));
			imgui->Text("MaxHP : " + to_string(maxHP));
			imgui->Text("CurrentHP : " + to_string(currentHP));
			imgui->Spacing();
			imgui->Text("Power Level : " + to_string(nowPowerLevel));
			imgui->Text("Health Level : " + to_string(nowHealthLevel));
			imgui->Text("Speed Level : " + to_string(nowSpeedLevel));

		}
		imgui->End();

		if (imgui->CollapsingHeader("Stat Menu"))
		{
			static int tmpHp = maxHP;
			static float tmpSpeed = moveSpeed;
			static int tmpPower = power;

			static int tmpEnemyHp = enemyHP;

			imgui->DragInt("Player HP", &tmpHp, 1, 1, 100);
			imgui->DragFloat("Player Speed", &tmpSpeed, 0.05f, 1.0f, 3.0f);
			imgui->DragInt("Player DMG", &tmpPower, 1, 1, 1000);

			imgui->DragInt("Enemy HP", &tmpEnemyHp, 1, 1, 1000);

			if (imgui->Button("Set Stat"))
			{
				// 내것도 바꿔줍니다.
				maxHP = tmpHp;
				currentHP = tmpHp;

				std::shared_ptr<PlayerController> player = GameEngineSpace::SceneManager::GetInstance()->FindObjectName(L"Player")->GetComponent<PlayerController>();

				// Player의 것을 바꿔줍니다.
				player->SetMaxHP(tmpHp);
				player->SetHP(tmpHp);

				moveSpeed = tmpSpeed;
				power = tmpPower;

				enemyHP = tmpEnemyHp;

				ClientSpace::PlayerData binaryPlayerStat;

				std::ofstream ofs(playerStatPath, std::ios_base::binary);
				boost::iostreams::filtering_stream<boost::iostreams::output> buffer;
				buffer.push(boost::iostreams::zlib_compressor());
				buffer.push(ofs);
				boost::archive::binary_oarchive oaPlayerStat(buffer);

				binaryPlayerStat.hp = tmpHp;
				binaryPlayerStat.power = tmpPower;
				binaryPlayerStat.attack1 = 1.0f;
				binaryPlayerStat.attack2 = 1.0f;
				binaryPlayerStat.attack3 = 2.0f;
				binaryPlayerStat.attack4 = 0.5f;
				binaryPlayerStat.moveSpeed = tmpSpeed;
				binaryPlayerStat.enemyHp = tmpEnemyHp;

				oaPlayerStat << binaryPlayerStat;
			}
		}
	}

	void GameManager::WriteGameLog(const std::string& logStr)
	{
		// 이미 있는 파일에 넣어줍니다.
		logFileStream.open(logFilePath.data(), std::ios::app);

		// 입력 받고 한줄 띄워줍니다.
		if (logFileStream.is_open())
		{
			logFileStream << logStr.c_str();

			logFileStream << "\n";

			logFileStream.close();
		}
	}

	void GameManager::WriteEnemyLog(int id, int state, int maxHP, int remainHP, int dmg)
	{
		// csv를 가정합니다.
		// extension 확인
		std::string ext;
		// 첫번째 점을 찾습니다.
		int extIdx = logFilePath.rfind(".");

		if (extIdx != -1)
		{
			// Idx에서 끝까지 뽑아내줍니다
				// 뒤의 null 문자로 인식을 제대로 못했기 때문에 4개만 뽑아줍니다..
			ext = logFilePath.substr(extIdx, 4);
		}

		if (ext != ".csv")
			return;

		// 아닐 때..
		logFileStream.open(logFilePath.data(), std::ios::app);

		// 입력 받고 한줄 띄워줍니다.
		if (logFileStream.is_open())
		{
			std::string logStr = "";
			logStr = to_string(id) + ","
				+ to_string(state) + ","
				+ to_string(maxHP) + ","
				+ to_string(remainHP) + ","
				+ to_string(dmg);

			logFileStream << logStr.c_str();

			logFileStream << "\n";

			logFileStream.close();
		}
	}

	void GameManager::PowerLevelUp()
	{
		nowPowerLevel++;

		if (nowPowerLevel > powerUpTable.size() - 1)
			nowPowerLevel = powerUpTable.size() - 1;
	}

	void GameManager::HealthLevelUp()
	{
		nowHealthLevel++;

		if (nowHealthLevel > healthUpTable.size() - 1)
			nowHealthLevel = healthUpTable.size() - 1;
	}

	void GameManager::SpeedLevelUp()
	{
		nowSpeedLevel++;

		if (nowSpeedLevel > speedUpTable.size() - 1)
			nowSpeedLevel = speedUpTable.size() - 1;
	}

	void GameManager::SetUpgradeStat()
	{
		// 인덱스로 찾아와서 제이슨에서 넣어줍니다.
		// 파워의 경우 초기 파워에서 곱해줍니다.
		power = static_cast<int>(originPower * powerUpTable[nowPowerLevel]);
		moveSpeed = speedUpTable[nowSpeedLevel];
		maxHP = healthUpTable[nowHealthLevel];
		currentHP = maxHP;
	}

	// 초기 스텟을 얻어옵니다.
	void GameManager::ResetStat()
	{
		power = originPower;

		// 스텟 창이 있다고 가정합니다.
			// 초기화 하면서 불러왔기 때문에
		ClientSpace::PlayerData binaryPlayerStat;

		std::ifstream ifs(playerStatPath, std::ios_base::binary);

		if (ifs.is_open())
		{
			boost::iostreams::filtering_streambuf<boost::iostreams::input> buffer;
			buffer.push(boost::iostreams::zlib_decompressor());
			buffer.push(ifs);
			boost::archive::binary_iarchive iaPlayerStat(buffer);
			iaPlayerStat >> binaryPlayerStat;
			ifs.close();
		}

		maxHP = binaryPlayerStat.hp;
		currentHP = maxHP;
		moveSpeed = binaryPlayerStat.moveSpeed;

		// 모든 레벨도 초기화 합니다.
		nowPowerLevel = 0;
		nowSpeedLevel = 0;
		nowHealthLevel = 0;
	}

	void GameManager::ResetGameState()
	{
		isCutSceneCameraMove = false;
		isCutSceneTrigger = false;
		isGetKey = false;
		playerDeadCnt = 0;
		isPlayerFirstDead = true;
		isFirstCraft = true;
		isAttack3 = false;
		isDodge = false;
		isAttack4 = false;
		isTutorialDodgeClear = false;
		isStoreCutSceneEnd = false;

		isLobbyFirstRun = true;
		isTutorialFirstRun = true;
		isLobbyPlay = false;
		isTutorialPlay = false;
		itemGetCounter = false;

		power = 40;
		moveSpeed = 1.0f;
		maxHP = 5;
		currentHP = 5;
		currentKillCounter = 0;
		currentMoney = 0;
		maxMoney = 100000;
		nowPrice = 0;

		nowPowerLevel = 0;
		nowHealthLevel = 0;
		nowSpeedLevel = 0;

		isStage1CutScene = true;
		isNeedKeyCutScenePlay = false;

		craftDone = false;

		InGameUIController::isBarCutScene = true;
		InGameUIController::isfirstInBar = false;

		InGameUIController::isLobbyCutScene = true;
		InGameUIController::isTutorialCutScene = true;
		InGameUIController::isStage1KeyCutScene = true;
		InGameUIController::isStage4CutScene = true;
		InGameUIController::isElevatorCutScene = true;

		inventory->ClearAllIngredientItem();
	}
}
