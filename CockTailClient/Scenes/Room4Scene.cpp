#include "pch.h"
#include "Room4Scene.h"

#include "Component/Camera.h"
#include "Component/BasicCameraController.h"
#include "Component/BoxCollider.h"
#include "Component/SphereCollider.h"
#include "Component/CapsuleCollider.h"
#include "Component/PosColorRenderer.h"
#include "Component/MeshRenderer.h"
#include "Component/Light.h"
#include "Component/SpriteEffect.h"
#include "CameraController.h"

#include "Scripts/MoveController.h"
#include "Scripts/PlayerController.h"
#include "PlayerAttackController.h"
#include "Room4Door.h"
#include "Room4Hole.h"

#include "NextSceneTrigger.h"
#include "GameManagerObject.h"

#include "Room4Event.h"

#include "EnemyController.h"

#include "CutSceneTrigger.h"
#include "EnemyWaveSpawn.h"

//UI
#include "InGameUIController.h"
#include "SettingUIController.h"
#include "ItemGetUI.h"

#include "CutScenePlay.h"

namespace ClientSpace
{
	Room4Scene::Room4Scene(tstring sceneName)
		: SceneBase(sceneName)
	{
	}

	Room4Scene::~Room4Scene()
	{
	}

	void Room4Scene::BuildScene()
	{
		// 각종 오브젝트들 생성
		SceneManager::GetInstance()->LoadSceneData("Room4Scene");

		// NavMesh 생성 
		NavigationManager::GetInstance()->MakeNavMesh("Room4NavMesh");
		int killGoal = 0;
		// 스크립트 가져오기
		std::vector <JsonObject> scriptJsons = GameManager::GetInstance()->GetJsonObject("lobbyScriptData");
		for (auto scriptJsonObject : scriptJsons)
		{
			Stage4Dialog stage4DialogStruct;

			stage4DialogStruct.eventIndex = scriptJsonObject.GetInt(L"EventIndex");
			stage4DialogStruct.scriptEvent = scriptJsonObject.GetString(L"ScriptEvent");
			//stage4DialogStruct.scriptStringEN = scriptJsonObject.GetString(L"ScriptStringEN");
			stage4DialogStruct.scriptStringKR = scriptJsonObject.GetString(L"ScriptStringKR");

			stage4DialogMap[stage4DialogStruct.eventIndex] = stage4DialogStruct;
		}

		SetScene();

		// player 생성
		std::shared_ptr<GameObject> player = CreateEmpty();
		player->AddComponent<PlayerController>();
		/*if (SceneManager::GetInstance()->GetPrevSceneName() == L"LeftPassage2Scene")
		{
			player->GetComponent<Transform>()->SetPosition(leftPos->GetWorldPosition());
			player->GetComponent<Transform>()->SetRotation(leftPos->GetWorldRotation());
		}*/
		player->GetComponent<Transform>()->SetPosition(leftPos->GetWorldPosition());
		player->GetComponent<Transform>()->SetRotation(leftPos->GetWorldRotation());

		player->AddComponent<PlayerAttackController>();

		// 플레이어가 생성된 뒤에 enemyWave를 생성합니다.
		auto enemyWave = CreateEmpty();
		enemyWave->AddComponent<EnemyWaveSpawn>()->Init();
		enemyWave->GetComponent<EnemyWaveSpawn>()->SetAllEnemy();
		enemyWave->GetComponent<EnemyWaveSpawn>()->SetPlayerController(player->GetComponent<PlayerController>());

		SceneManager::GetInstance()->GetCurrentScene()->GetMainCamera()->GetGameObject()->AddComponent<CameraController>();

		/// <summary>
		/// 재우 - 컷씬 제작
		/// </summary>
		auto ui = CreateUI();
		auto scene = SceneManager::GetInstance();
		auto cutSceneTrigger = scene->FindObjectName(L"CutSceneTrigger");
		auto room4Door = scene->FindObjectName(L"Room4Door");
		auto room4DoorCollider = scene->FindObjectName(L"Room4DoorCollider");
		room4Door->AddComponent<Room4Door>();
		cutSceneTrigger->AddComponent<CutSceneTrigger>();
		cutSceneTrigger->GetComponent<CutSceneTrigger>()->SetUIController(ui->GetComponent<InGameUIController>().get());
		cutSceneTrigger->GetComponent<CutSceneTrigger>()->SetPlayerController(player->GetComponent<PlayerController>().get());

		if (InGameUIController::isStage4CutScene == true)
		{
			std::shared_ptr<GameObject> room4CutScene = CreateEmpty();

			room4CutScene->AddComponent<CutScenePlay>()->Init(ui->GetComponent<InGameUIController>().get(), CutScenePlayType::Stage4);
			room4CutScene->GetComponent<CutScenePlay>()->SetRoom4Door(room4Door->GetComponent<Room4Door>().get());
			room4CutScene->GetComponent<CutScenePlay>()->SetRoom4DoorCollider(room4DoorCollider.get());
			room4CutScene->GetComponent<CutScenePlay>()->SetRoom4WaveSpawn(enemyWave->GetComponent<EnemyWaveSpawn>().get());
		}
	}

	std::shared_ptr<GameObject> Room4Scene::CreateUI()
	{
		std::shared_ptr<GameObject> ui = CreateEmpty();
		ui->AddComponent<InGameUIController>();
		ui->GetComponent<InGameUIController>()->SetStage4DialogMap(stage4DialogMap);
		ui->AddComponent<ItemGetUI>()->Init();

		return ui;
	}

	void Room4Scene::SetScene()
	{
		auto scene = SceneManager::GetInstance();

		auto leftPortal = scene->FindObjectName(L"LeftPassage2Portal");
		auto leftPoint = scene->FindObjectName(L"LeftPassage2Point");

		leftPos = leftPoint->GetComponent<Transform>();

		leftPortal->AddComponent<NextSceneTrigger>()->SetNextScene(L"LeftPassage2Scene");

		auto enemySpawn = scene->FindObjectName(L"EnemySpawn");
		auto keySpawn = scene->FindObjectName(L"KeySpawn");

		auto keySpawnPos = keySpawn->GetComponent<Transform>();

		//std::shared_ptr<GameObject> enemy = CreateEmpty();
		//enemy->AddComponent<MeshRenderer>()->LoadPrefab("toadani.model");
		//enemy->AddComponent<EnemyController>();
		//enemy->GetComponent<Transform>()->SetScale({2.0f, 2.0f, 2.0f});
		//enemy->SetGameObjectEnable(false);

		//ObjectPool::GetInstance()->RegisterGameObject("dog", 1, [&]()
		//	{
		//		return enemy;
		//	});

		//room1Portal->AddComponent<NextSceneTrigger>()->SetNextScene(L"Room1Scene");

		//std::shared_ptr<GameObject> gameManager = CreateEmpty();
		//gameManager->AddComponent<Room4Event>()->SetToad(enemy);
		//gameManager->GetComponent<Room4Event>()->SetKeySpawnPos(keySpawnPos);
	}
}