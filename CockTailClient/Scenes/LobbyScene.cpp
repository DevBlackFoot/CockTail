#include "pch.h"
#include "LobbyScene.h"
#include "NextSceneTrigger.h"

#include "Component/Camera.h"
#include "Component/BasicCameraController.h"
#include "Component/BoxCollider.h"
#include "Component/SphereCollider.h"
#include "Component/CapsuleCollider.h"
#include "Component/PosColorRenderer.h"
#include "Component/MeshRenderer.h"
#include "Component/Light.h"
#include "Component/Particle.h"
#include "Component/SpriteEffect.h"
#include "CameraController.h"

#include "DamageTest.h"
#include "PlayerControllerMK2.h"

#include "Scripts/MoveController.h"
#include "Scripts/PlayerController.h"
#include "PlayerAttackController.h"
#include "OpenStoreTrigger.h"

#include "EnemyController.h"
#include "GameManager.h"

#include "FloatingUI.h"
#include "CutScenePlay.h"

#include "CockController.h"

//UI
#include "EnemySpawn.h"
#include "InGameUIController.h"
#include "InterationParticle.h"
#include "MeleeDogController.h"
#include "SettingUIController.h"

#include "OpenCockTailTrigger.h"
#include "StoreController.h"
#include "Util/JsonReader.h"

namespace ClientSpace
{
	LobbyScene::LobbyScene(tstring sceneName)
		: SceneBase(sceneName)
	{
		playerEntryPos = Vector3::Zero;
		playerEntryRot = Vector3::Zero;

		insideSceneMaskColor = Vector4{ powf((73.f / 255.f), 2.2f), powf((64.f / 255.f), 2.2f), powf((62.f / 255.f), 2.2f), 1.0f };
		rimLightColor = Vector4{ 255.f / 255.f, 200.f / 255.f, 136.f / 255.f, 1.f };
	}

	LobbyScene::~LobbyScene()
	{
	}

	void LobbyScene::BuildScene()
	{
		SoundManager::GetInstance()->StopAllSound();
		SoundManager::GetInstance()->PlayBackgroundSound("LobbyBGM", 1.0f);

		// 각종 오브젝트들 생성
		SceneManager::GetInstance()->LoadSceneData("lobbyScene");

		auto _gameManager = GameManager::GetInstance();
		_gameManager->isRoom1Clear = false;
		_gameManager->isRoom2Clear = false;
		_gameManager->isRoom3Clear = false;

		std::vector <JsonObject> scriptJsons = GameManager::GetInstance()->GetJsonObject("lobbyScriptData");
		for (auto scriptJsonObject : scriptJsons)
		{
			LobbyDialog lobbyDialogStruct;

			lobbyDialogStruct.eventIndex = scriptJsonObject.GetInt(L"EventIndex");

			lobbyDialogStruct.scriptEvent = scriptJsonObject.GetString(L"ScriptEvent");

			//lobbyDialogStruct.scriptStringEN = scriptJsonObject.GetString(L"ScriptStringEN");

			lobbyDialogStruct.scriptStringKR = scriptJsonObject.GetString(L"ScriptStringKR");

			lobbydialogMap[lobbyDialogStruct.eventIndex] = lobbyDialogStruct;
		}

		uiController = CreateUI();
		SetScene();
		// NavMesh 생성 
		//NavigationManager::GetInstance()->MakeNavMesh("TestNavMesh");

		// DirectionalLight Test
		/*std::shared_ptr<GameObject> directionalLight = CreateEmpty();
		directionalLight->AddComponent<Light>();
		directionalLight->GetComponent<Light>()->ChangeType(LightType::Directional);
		directionalLight->GetComponent<Light>()->SetColor(SimpleMath::Vector3{ Colors::White });
		directionalLight->GetComponent<Light>()->SetDirection(SimpleMath::Vector3{ 0.5f, -0.5f, 0.5f });
		directionalLight->GetComponent<Light>()->SetPower(1.0f);*/

		// cock 생성
		std::shared_ptr<GameObject> cock = CreateEmpty();
		cock->AddComponent<MeshRenderer>();
		cock->AddComponent<CockController>();
		cock->SetName(L"Cock");
		cock->GetComponent<MeshRenderer>()->LoadPrefab("cockani.model");
		cock->GetComponent<Transform>()->SetPosition({ -0.15, 2.6, -11.95 });
		cock->GetComponent<Transform>()->Rotate(Vector3::Up, 135.f);

		std::shared_ptr<GameObject> player = CreateEmpty();
		player->AddComponent<PlayerController>();
		player->GetComponent<PlayerController>()->IsLobbyScene(true);
		player->GetComponent<Transform>()->SetPosition(playerEntryPos);
		player->GetComponent<Transform>()->Rotate(playerEntryRot);

		int temp = player->GetComponent<PlayerController>()->GetMaxHP();
		int temp2 = player->GetComponent<PlayerController>()->GetHP();
		player->GetComponent<PlayerController>()->SetHP(player->GetComponent<PlayerController>()->GetMaxHP());

		// 플레이어가 세팅이 된 뒤
		auto scene = SceneManager::GetInstance();
		auto interactNPC = scene->FindObjectName(L"InteractNPC");
		interactNPC->AddComponent<StoreController>()->Init();

		// Follow Camera
		SceneManager::GetInstance()->GetCurrentScene()->GetMainCamera()->GetGameObject()->AddComponent<CameraController>();
		//SceneManager::GetInstance()->GetCurrentScene()->GetMainCamera()->GetGameObject()->GetComponent<CameraController>()->PlayBGM("LobbyBGM");

		// Free Fly Camera
		//SceneManager::GetInstance()->GetCurrentScene()->GetMainCamera()->GetGameObject()->AddComponent<BasicCameraController>();

		// Hud관련 게임 오브젝트 추가
			// 해당 씬에 첫 진입일 때.
		if (GameManager::GetInstance()->GetLobbyFirstRun() == true)
		{
			std::shared_ptr<GameObject> moveUI = CreateEmpty();
			moveUI->AddComponent<FloatingUI>()->Init("Resources/UI/Hud/Move_Text.png", FloatingUIName::MOVE);
		}

		// 전에있던 씬이 바 였고 처음 제작했다면
		if (SceneManager::GetInstance()->GetPrevSceneName() == L"BarOperationScene" && GameManager::GetInstance()->isFirstCraft)
		{
			std::shared_ptr<GameObject> lobbyCutScene = CreateEmpty();

			lobbyCutScene->AddComponent<CutScenePlay>()->Init(uiController->GetComponent<InGameUIController>().get(), CutScenePlayType::LobbyFirstCraftDone);

			GameManager::GetInstance()->isFirstCraft = false;
		}
		// 로비 컷씬을 먼저 보여주고 게임매니저 LobbyFirstRun을 보여주어야한다.
		else if (uiController->GetComponent<InGameUIController>()->isLobbyCutScene == true)
		{
			std::shared_ptr<GameObject> lobbyCutScene = CreateEmpty();

			lobbyCutScene->AddComponent<CutScenePlay>()->Init(uiController->GetComponent<InGameUIController>().get(), CutScenePlayType::Lobby);
		}
		else
		{
			if (GameManager::GetInstance()->playerDeadCnt == 1 && GameManager::GetInstance()->isPlayerFirstDead)
			{
				std::shared_ptr<GameObject> lobbyCutScene = CreateEmpty();

				lobbyCutScene->AddComponent<CutScenePlay>()->Init(uiController->GetComponent<InGameUIController>().get(), CutScenePlayType::LobbySettlement);

				GameManager::GetInstance()->isPlayerFirstDead = false;
			}
		}

		/*
		for (int i = 0; i < 6; i++)
		{
			for (int j = 0; j < 6; j++)
			{
				std::shared_ptr<GameObject> pbrObj = CreateEmpty();
				pbrObj->GetComponent<Transform>()->SetPosition({ playerEntryPos.x + 1.5f + (j * 1.5f) + (i * 1.5f), playerEntryPos.y + 5.f, playerEntryPos.z - 1.5f - (j * 1.5f) + (i * 1.5f) });
				pbrObj->AddComponent<MeshRenderer>()->Init("SphereMesh");
				pbrObj->GetComponent<MeshRenderer>()->SetMaterialColor("PBR_Mat", { 1.0f, 1.0f, 1.0f, 1.0f });
				pbrObj->GetComponent<MeshRenderer>()->SetMaterialMetallicRoughness("PBR_Mat", 1.0f - 0.2f * j, 0.f + 0.2f * i);
			}
		}
		*/


		//auto makeCockTail = scene->FindObjectName(L"MakeCockTail");
		//Vector3 effectPos = makeCockTail->GetComponent<Transform>()->GetWorldPosition();
		//std::shared_ptr<GameObject> _temp = CreateEmpty();
		//_temp->GetComponent<Transform>()->SetPosition(effectPos);
		//// 뒷 배경
		//std::shared_ptr<GameObject> effect = CreateEmpty();
		//_temp->SetChild(effect);
		//effect->AddComponent<GameEngineSpace::SpriteEffect>()->Init("Resources/UI/Hud/Boss_HP_Bar_BG.png");
		//effect->GetComponent<Transform>()->SetPosition({0.0f, 2.0f, 0.0f}, Space::LOCAL);
		//effect->GetComponent<Transform>()->SetScale({2.f, 0.4f, 1.0f});
		//// 실제 hp
		//std::shared_ptr<GameObject> hpBar = CreateEmpty();
		//hpBar->AddComponent<GameEngineSpace::SpriteEffect>()->Init("Resources/UI/Hud/Boss_HP_Bar.png");
		//// fill Count
		//hpBar->GetComponent<GameEngineSpace::SpriteEffect>()->SetEffectSrc({0.f, 0.f, 0.5f, 1.0f});
		//effect->SetChild(hpBar);

		 // 테스트용 Effect 생성. Spawn Effect 예제
		/*
		std::shared_ptr<GameObject> effect = CreateEmpty();
		effect->AddComponent<MeshRenderer>()->LoadPrefab("dogani.model");
		effect->AddComponent<MeleeDogController>();
		Vector3 effectPos = playerEntryPos;
		effectPos.x += 7.0f;
		//effectPos.z += 5.0f;
		effect->GetComponent<MeleeDogController>()->SetSpawnEnemy(true);
		effect->GetComponent<Transform>()->SetPosition(effectPos);
		 */
	}

	std::shared_ptr<GameObject> LobbyScene::CreateUI()
	{
		std::shared_ptr<GameObject> ui = CreateEmpty();
		ui->AddComponent<InGameUIController>();
		ui->GetComponent<InGameUIController>()->SetDialogMap(lobbydialogMap);
		return ui;
	}

	void LobbyScene::SetScene()
	{
		auto scene = SceneManager::GetInstance();

		auto entryPoint = scene->FindObjectName(L"EntryPoint");
		auto exitPortal = scene->FindObjectName(L"ExitPortal");
		auto interactNPC = scene->FindObjectName(L"InteractNPC");
		//auto makeCockTail = scene->FindObjectName(L"MakeCockTail");
		auto swapCharacter = scene->FindObjectName(L"SwapCharacter");

		auto npcPos = interactNPC->GetComponent<Transform>()->GetWorldPosition();
		if (scene->GetPrevSceneName() != L"BarOperationScene")
		{
			playerEntryPos = entryPoint->GetComponent<Transform>()->GetWorldPosition();
			playerEntryRot = entryPoint->GetComponent<Transform>()->GetLocalEulerRotation();
		}
		else
		{
			// 전 씬이면.. 위치를 달리 해줍니다
			if (GameManager::GetInstance()->isStoreCutSceneEnd)
				playerEntryPos = Vector3{ npcPos.x + 1.f, npcPos.y - 0.7f, npcPos.z - 1.f };
			else
				playerEntryPos = Vector3{ npcPos.x + 1.5f, npcPos.y - 0.7f, npcPos.z - 1.5f };

			playerEntryRot = Vector3{ 0.f, 315.f, 0.f };
		}

		//exitPortal->AddComponent<NextSceneTrigger>()->SetNextScene(L"Elevator2Scene");
		//exitPortal->AddComponent<NextSceneTrigger>()->SetNextScene(L"Room4Scene");
		//exitPortal->AddComponent<NextSceneTrigger>()->SetNextScene(L"Room1Scene");
		exitPortal->AddComponent<NextSceneTrigger>()->SetNextScene(L"TutorialScene");
		//makeCockTail->AddComponent<OpenCockTailTrigger>();
		//auto cockTailPos = makeCockTail->GetComponent<Transform>()->GetWorldPosition();
		interactNPC->GetComponent<Transform>()->SetPosition({ npcPos.x + 0.5f, npcPos.y, npcPos.z - 0.5f });
		interactNPC->AddComponent<OpenStoreTrigger>();

		// 위치를 여기서 알게되므로..
		// 파티클 두개
		//makeCockTail->AddComponent<Particle>()->LoadParticlePrefab("Interaction");
		interactNPC->AddComponent<Particle>()->LoadParticlePrefab("Interaction");
		// 둘다 파티클 넣어주기
		//makeCockTail->AddComponent<InteractionParticle>();
		interactNPC->AddComponent<InteractionParticle>();

	}
}