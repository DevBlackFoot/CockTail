#include "pch.h"
#include "Room1Scene.h"
#include "PlayerRellyPointTrigger.h"
#include "EnemySpawner.h"

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
#include "Component/Particle.h"

#include "Scripts/MoveController.h"
#include "Scripts/PlayerController.h"
#include "PlayerAttackController.h"

//#include "EnemyController.h"
#include "MeleeDogController.h"
#include "ArchorDogController.h"
#include "EnemyNavAgent.h"
#include "EnemyHitTrigger.h"
#include "NextSceneTrigger.h"
#include "Room1Door.h"
#include "GameManagerObject.h"
#include "ToadController.h"

#include "test.h"

#include "PortalEffect.h"

//UI
#include "InGameUIController.h"
#include "SettingUIController.h"
#include "ItemGetUI.h"

#include "CutScenePlay.h"

namespace ClientSpace
{
	Room1Scene::Room1Scene(tstring sceneName)
		: SceneBase(sceneName)
	{
	}

	Room1Scene::~Room1Scene()
	{
	}

	void Room1Scene::BuildScene()
	{
		auto scene = SceneManager::GetInstance();
		int killGoal = 0;
		// 각종 오브젝트들 생성
		scene->LoadSceneData("Room1Scene");

		// NavMesh 생성 
		NavigationManager::GetInstance()->MakeNavMesh("Room1NavMesh");

		// 스크립트 가져오기
		std::vector <JsonObject> scriptJsons = GameManager::GetInstance()->GetJsonObject("lobbyScriptData");
		for (auto scriptJsonObject : scriptJsons)
		{
			Stage1Dialog stage1DialogStruct;
			Stage4Dialog stage4DialogStruct;

			stage1DialogStruct.eventIndex = scriptJsonObject.GetInt(L"EventIndex");
			stage1DialogStruct.scriptEvent = scriptJsonObject.GetString(L"ScriptEvent");
			//stage1DialogStruct.scriptStringEN = scriptJsonObject.GetString(L"ScriptStringEN");
			stage1DialogStruct.scriptStringKR = scriptJsonObject.GetString(L"ScriptStringKR");

			stage4DialogStruct.eventIndex = scriptJsonObject.GetInt(L"EventIndex");
			stage4DialogStruct.scriptEvent = scriptJsonObject.GetString(L"ScriptEvent");
			//stage4DialogStruct.scriptStringEN = scriptJsonObject.GetString(L"ScriptStringEN");
			stage4DialogStruct.scriptStringKR = scriptJsonObject.GetString(L"ScriptStringKR");

			stage1DialogMap[stage1DialogStruct.eventIndex] = stage1DialogStruct;
			stage4DialogMap[stage4DialogStruct.eventIndex] = stage4DialogStruct;
		}

		auto ui = CreateUI();

		SetScene();

		// 엘레베이터 버튼 프리팹 입니다.
		auto elevatorButton = scene->FindObjectName(L"ElevatorButton");

		auto leftPortal = scene->FindObjectName(L"LeftPortal");
		leftPortal->AddComponent<NextSceneTrigger>()->SetNextScene(L"LeftPassage1Scene");
		//leftPortal->SetGameObjectEnable(false);

		// 포탈 위치에 생기는 파티클
		auto portalEffect = CreateEmpty();
		portalEffect->SetName(L"PortalEffect");

		// player 생성
		std::shared_ptr<GameObject> player = CreateEmpty();
		player->AddComponent<PlayerController>();
		player->AddComponent<PlayerAttackController>();

		// 문 동작시키는 그거
		auto button = scene->FindObjectName(L"ElevatorButtonEvent");
		button->AddComponent<Room1Door>();
		button->AddComponent<Particle>()->LoadParticlePrefab("Interaction");

		// 키가 없을때
		if (GameManager::GetInstance()->isGetKey == false)
		{

			if (scene->GetPrevSceneName() != L"LeftPassage1Scene")
			{
				auto rellyPoint = scene->FindObjectName(L"RellyPoint");
				rellyPoint->AddComponent<PlayerRellyPointTrigger>();
				rellyPoint->GetComponent<PlayerRellyPointTrigger>()->SetUIController(ui->GetComponent<InGameUIController>().get());
				player->GetComponent<Transform>()->SetPosition(playerEntryPos);
				player->GetComponent<Transform>()->SetLocalEulerRotation(playerEntryRot);
				Vector3 target = rellyPoint->GetComponent<Transform>()->GetWorldPosition();
				player->AddComponent<PlayerController>()->SetTarget(target);
			}

			else
			{
				auto _point = scene->FindObjectName(L"LeftPoint");
				auto _pos = _point->GetComponent<Transform>();
				player->GetComponent<Transform>()->SetPosition(_pos->GetWorldPosition());
				player->GetComponent<Transform>()->SetLocalEulerRotation(_pos->GetLocalEulerRotation());
			}

			if (GameManager::GetInstance()->isRoom1Clear == false)
			{
				for (int i = 1; i <= 10; i++)
				{
					auto m = scene->FindObjectName(L"Melee " + to_wstring(i));
					m->AddComponent<MeshRenderer>()->LoadPrefab("dogani.model");
					m->AddComponent<MeleeDogController>()->SetLoadCompleted();
					killGoal++;
				}

				for (int i = 1; i <= 1; i++)
				{
					auto m = scene->FindObjectName(L"Toad " + to_wstring(i));
					m->AddComponent<MeshRenderer>()->LoadPrefab("toadani.model");
					m->AddComponent<ToadController>()->SetLoadCompleted();
					killGoal++;
				}

				leftPortal->GetComponent<NextSceneTrigger>()->SetKillCount(killGoal);
				button->GetComponent<Room1Door>()->SetKillCount(killGoal);
			}
			// 포탈의 위치에..
			portalEffect->AddComponent<Particle>()->LoadParticlePrefab("Portal");
			portalEffect->AddComponent<PortalEffect>()->Init(killGoal);
			portalEffect->GetComponent<Transform>()->SetPosition(leftPortal->GetComponent<Transform>()->GetWorldPosition());
			//portalEffect->SetGameObjectEnable(false);
		}
		// 키가 존재할때
		else
		{
			player->GetComponent<Transform>()->SetPosition({ 41.82, 3.79, -26.6 });
			player->GetComponent<Transform>()->SetLocalEulerRotation(playerEntryRot);
		}

		/*for (int i = 1; i <= 2; i++)
		{
			auto a = scene->FindObjectName(L"Arc " + to_wstring(i));
			a->AddComponent<MeshRenderer>()->LoadPrefab("dogani_throw.model");
			a->AddComponent<ArchorDogController>();
		}*/

		// DirectionalLight Test
		/*std::shared_ptr<GameObject> directionalLight = CreateEmpty();
		directionalLight->AddComponent<Light>();
		directionalLight->GetComponent<Light>()->ChangeType(LightType::Directional);
		directionalLight->GetComponent<Light>()->SetColor(SimpleMath::Vector3{ Colors::White });
		directionalLight->GetComponent<Light>()->SetDirection(SimpleMath::Vector3{ 0.5f, -0.5f, 0.5f });
		directionalLight->GetComponent<Light>()->SetPower(1.0f);*/


		/*	std::shared_ptr<GameObject> dummy = CreateEmpty();
			dummy->AddComponent<MeshRenderer>()->Init("dog.model");
			dummy->GetComponent<Transform>()->SetPosition({ 20.f, 5.f, -45.f });
			dummy->AddComponent<EnemyController>();*/

			/*	std::shared_ptr<GameObject> testmob = CreateEmpty();
				testmob->AddComponent<MeshRenderer>()->Init("red.model");
				testmob->GetComponent<Transform>()->SetPosition({ 20.f, 5.f, -50.f });
				testmob->AddComponent<EnemyController>();*/

				//std::shared_ptr<GameObject> red = CreateEmpty();
				//red->AddComponent<MeshRenderer>()->Init("red.model");
				//red->GetComponent<Transform>()->SetPosition({ 30.f, 5.f, -35.f });
				//red->AddComponent<NavMeshAgent>();
				//red->AddComponent<Rigidbody>();
				//red->AddComponent<CapsuleCollider>();
				//red->GetComponent<CapsuleCollider>()->SetCenter({ 0.f, 1.0f, 0.f });
				//red->GetComponent<CapsuleCollider>()->InitRenderObj();
				//red->GetComponent<Rigidbody>()->InitRigidBody();
				//red->GetComponent<Rigidbody>()->SetPhysAxisLock(false, false, false, true, true, true);
				//red->AddComponent<MoveController>();
				//red->GetComponent<MoveController>()->SetTarget(player->GetComponent<Transform>());

				/*std::shared_ptr<GameObject> axis = CreateEmpty();
				axis->AddComponent<PosColorRenderer>()->Init("Axis");*/
		scene->GetCurrentScene()->GetMainCamera()->GetGameObject()->AddComponent<CameraController>();

		// 테스트용 파티클 생성
		/*
		std::shared_ptr<GameObject> testParticle = CreateEmpty();
		testParticle->AddComponent<Particle>()->Init("Hit");
		testParticle->GetComponent<Transform>()->SetPosition(playerEntryPos);
		testParticle->GetComponent<Transform>()->SetScale({0.5f, 0.5f, 0.5f});

		*/

		std::shared_ptr<GameObject> room1CutScene = CreateEmpty();

		// stage1 처음 들어왔다면
		if (GameManager::isStage1CutScene == true)
		{
			// 테스트 때문에 나눠서 받아야해
			if (GameManager::GetInstance()->isGetKey == true)
				room1CutScene->AddComponent<CutScenePlay>()->Init(ui->GetComponent<InGameUIController>().get(), CutScenePlayType::Stage1_HasKey);
			else
				room1CutScene->AddComponent<CutScenePlay>()->Init(ui->GetComponent<InGameUIController>().get(), CutScenePlayType::Stage1);
		}
		else
		{
			leftPortal->SetGameObjectEnable(true);
			portalEffect->SetGameObjectEnable(true);

			// 키를 갖고있다면... Stage4에서 무조건 Room1로 텔포시키니까 이렇게 하자?
			if (GameManager::GetInstance()->isGetKey == true)
				room1CutScene->AddComponent<CutScenePlay>()->Init(ui->GetComponent<InGameUIController>().get(), CutScenePlayType::Stage1_HasKey);
			else
			{
				room1CutScene->AddComponent<CutScenePlay>()->Init(ui->GetComponent<InGameUIController>().get(), CutScenePlayType::Stage1);
				room1CutScene->GetComponent<CutScenePlay>()->SetStage1CameraMoveType(Stage1CameraMoveType::ElevatorPortal);
				GameManager::GetInstance()->isCutSceneCameraMove = false;
			}
		}

		// 혹시 모르니 Room1도 넣어둡니다 => 전투를 해야 크래프트가 열립니다.
		GameManager::GetInstance()->SetCraftDone(false);
	}

	std::shared_ptr<GameObject> Room1Scene::CreateUI()
	{
		std::shared_ptr<GameObject> ui = CreateEmpty();
		ui->AddComponent<InGameUIController>();
		ui->GetComponent<InGameUIController>()->SetStage1DialogMap(stage1DialogMap);
		ui->GetComponent<InGameUIController>()->SetStage4DialogMap(stage4DialogMap);
		ui->AddComponent<ItemGetUI>()->Init();
		return ui;
	}

	void Room1Scene::SetScene()
	{
		auto scene = SceneManager::GetInstance();

		auto entryPoint = scene->FindObjectName(L"EntryPoint");
		auto elevator = scene->FindObjectName(L"Elevator");

		enemySpawn = scene->FindObjectName(L"EnemySpawn");

		playerEntryPos = entryPoint->GetComponent<Transform>()->GetWorldPosition();
		playerEntryRot = entryPoint->GetComponent<Transform>()->GetLocalEulerRotation();

		elevator->AddComponent<NextSceneTrigger>()->SetNextScene(L"ElevatorScene");

		enemySpawn->AddComponent<EnemySpawner>();

		auto toad = CreateEmpty();
		auto _pos = entryPoint->GetComponent<Transform>()->GetWorldPosition();
		_pos.z += 3.0f;
		_pos.x += 3.0f;
		toad->GetComponent<Transform>()->SetPosition(_pos);
		auto _rot = PhysicsManager::GetInstance()->EulerToQuat({ 0.0f,180.0f,0.0f });
		toad->GetComponent<Transform>()->SetRotation(_rot);
		toad->AddComponent<MeshRenderer>()->LoadPrefab("toadani.model");
	}
}