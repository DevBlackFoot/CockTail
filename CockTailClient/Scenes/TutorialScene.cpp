#include "pch.h"
#include "TutorialScene.h"
#include "NextSceneTrigger.h"
#include "TutorialStartTrigger.h"

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
#include "OnHitCollider.h"
#include "GameManager.h"
#include "ItemObject.h"
#include "FloatingUI.h"
#include "ToadController.h"
#include "CockController.h"
#include "MeleeDogController.h"

//UI
#include "InGameUIController.h"
#include "SettingUIController.h"
#include "CutScenePlay.h"

namespace ClientSpace
{
	TutorialScene::TutorialScene(tstring sceneName)
		: SceneBase(sceneName)
	{
		playerEntryPos = Vector3::Zero;
		playerEntryRot = Vector3::Zero;

		isInside = false;

		insideSceneMaskColor = Vector4{ powf(0.2488f, 2.2f), powf(0.2769f, 2.2f), powf(0.5566f, 2.2f), 1.0f };
	}

	TutorialScene::~TutorialScene()
	{
	}

	void TutorialScene::BuildScene()
	{
		SoundManager::GetInstance()->StopAllSound();
		SoundManager::GetInstance()->PlayBackgroundSound("BattleBGM", 1.0f);

		// 각종 오브젝트들 생성
		SceneManager::GetInstance()->LoadSceneData("tutorialScene");

		// NavMesh 생성  
		NavigationManager::GetInstance()->MakeNavMesh("TutorialNavMesh");

		//SetScene();
		auto scene = SceneManager::GetInstance();

		auto nextPortal = scene->FindObjectName(L"NextPortal");
		auto playerSpawn = scene->FindObjectName(L"PlayerSpawn");

		playerEntryPos = playerSpawn->GetComponent<Transform>()->GetWorldPosition();
		playerEntryRot = playerSpawn->GetComponent<Transform>()->GetLocalEulerRotation();

		nextPortal->AddComponent<NextSceneTrigger>()->SetNextScene(L"Room1Scene");


		std::vector <JsonObject> scriptJsons = GameManager::GetInstance()->GetJsonObject("lobbyScriptData");
		for (auto scriptJsonObject : scriptJsons)
		{
			TutoriaDialog tutoriaDialogStruct;

			tutoriaDialogStruct.eventIndex = scriptJsonObject.GetInt(L"EventIndex");
			tutoriaDialogStruct.scriptEvent = scriptJsonObject.GetString(L"ScriptEvent");
			//tutoriaDialogStruct.scriptStringEN = scriptJsonObject.GetString(L"ScriptStringEN");
			tutoriaDialogStruct.scriptStringKR = scriptJsonObject.GetString(L"ScriptStringKR");

			tutoriaDialogMap[tutoriaDialogStruct.eventIndex] = tutoriaDialogStruct;
		}

		// cock 생성
		std::shared_ptr<GameObject> cock = CreateEmpty();
		cock->AddComponent<MeshRenderer>();
		cock->AddComponent<CockController>();
		cock->SetName(L"Cock");
		cock->GetComponent<MeshRenderer>()->LoadPrefab("cockani.model");
		cock->GetComponent<Transform>()->SetPosition({ 2.15, 1.2, -31.21 });
		cock->GetComponent<Transform>()->Rotate(Vector3::Up, 135.f);
		// 칵의 실루엣을 꺼줍시다.
		cock->GetComponent<MeshRenderer>()->GetRenderObj()->SetMaterialColor("SilhouetteMaterial", { 0.5094f, 0.3431f, 0.1173f, 0.0f });
		cock->SetGameObjectEnable(false);

		auto ui = CreateUI();

		auto rellyPoint = SceneManager::GetInstance()->FindObjectName(L"RellyPoint");
		rellyPoint->AddComponent<TutorialStartTrigger>();
		rellyPoint->GetComponent<TutorialStartTrigger>()->SetUIController(ui->GetComponent<InGameUIController>().get());
		rellyPoint->GetComponent<TutorialStartTrigger>()->SetCock(cock.get());

		// DirectionalLight Test
		/*std::shared_ptr<GameObject> directionalLight = CreateEmpty();
		directionalLight->AddComponent<Light>();
		directionalLight->GetComponent<Light>()->ChangeType(LightType::Directional);
		directionalLight->GetComponent<Light>()->SetColor(SimpleMath::Vector3{ Colors::White });
		directionalLight->GetComponent<Light>()->SetDirection(SimpleMath::Vector3{ 0.5f, -0.5f, 0.5f });
		directionalLight->GetComponent<Light>()->SetPower(1.0f);*/

		std::shared_ptr<GameObject> player = CreateEmpty();
		player->SetName(L"Player");
		//player->AddComponent<NavMeshAgent>();
		player->GetComponent<Transform>()->SetPosition(playerEntryPos);
		player->GetComponent<Transform>()->SetLocalEulerRotation(playerEntryRot);
		player->AddComponent<PlayerController>();
		//Vector3 target = rellyPoint->GetComponent<Transform>()->GetWorldPosition();
		//player->AddComponent<PlayerAttackController>();

		ObjectPool::GetInstance()->RegisterGameObject("Item", 30, [&]()
		{
			std::shared_ptr<GameObject> item = CreateEmpty();

			item->AddComponent<ItemObject>();

			item->SetGameObjectEnable(false);

			return item;
		});


		if (GameManager::GetInstance()->GetTutorialFirstRun() == true)
		{
			std::shared_ptr<GameObject> attackUI = CreateEmpty();
			attackUI->AddComponent<FloatingUI>()->Init("Resources/UI/Hud/Attack_Text.png", FloatingUIName::ATTACK);

			std::shared_ptr<GameObject> dodgeUI = CreateEmpty();
			dodgeUI->AddComponent<FloatingUI>()->Init("Resources/UI/Hud/Space_Text.png", FloatingUIName::DODGE);
		}

		/*
		std::shared_ptr<GameObject> red = CreateEmpty();
		red->AddComponent<MeshRenderer>()->Init("red.model");
		red->GetComponent<Transform>()->SetPosition({ 5.f, 5.f, -15.f });
		red->AddComponent<NavMeshAgent>();
		red->AddComponent<Rigidbody>();
		red->AddComponent<CapsuleCollider>();
		red->GetComponent<CapsuleCollider>()->SetCenter({ 0.f, 1.0f, 0.f });
		red->GetComponent<CapsuleCollider>()->InitRenderObj();
		red->GetComponent<Rigidbody>()->InitRigidBody();
		red->GetComponent<Rigidbody>()->SetPhysAxisLock(false, false, false, true, true, true);
		red->AddComponent<MoveController>();
		red->GetComponent<MoveController>()->SetTarget(player->GetComponent<Transform>());

		std::shared_ptr<GameObject> axis = CreateEmpty();
		axis->AddComponent<PosColorRenderer>()->Init("Axis");*/

		// Fallow Camera
		auto cameraObj = SceneManager::GetInstance()->GetCurrentScene()->GetMainCamera()->GetGameObject();
		cameraObj->AddComponent<CameraController>();
		//cameraObj->AddComponent<BasicCameraController>();

		
		if (InGameUIController::isTutorialCutScene == true)
		{
			// 멈춰준다.
			player->GetComponent<PlayerController>()->SetIsPlayerControl(false);

			std::shared_ptr<GameObject> tutorialCutScene = CreateEmpty();

			tutorialCutScene->AddComponent<CutScenePlay>()->Init(ui->GetComponent<InGameUIController>().get(), CutScenePlayType::Tutorial);
			
			// 허수아비 생성
			/*auto enemy = CreateEmpty();
			enemy->GetComponent<Transform>()->SetPosition({ 20.86, 2.31, -32.46 });
			enemy->GetComponent<Transform>()->Rotate(Vector3::Up, 225.f);
			enemy->AddComponent<MeshRenderer>()->LoadPrefab("dogani.model");
			enemy->AddComponent<MeleeDogController>();
			enemy->GetComponent<MeleeDogController>()->SetSpawnEnemy(true);
			enemy->GetComponent<MeleeDogController>()->SetTutorialMode(true);*/
		}

		//auto _toad = CreateEmpty();
		//_toad->AddComponent<ToadController>();
		//_toad->AddComponent<MeshRenderer>()->LoadPrefab("toadani.model");
		//auto _toadPos = nextPortal->GetComponent<Transform>()->GetWorldPosition();
		//_toadPos.x -= 20;
		//_toadPos.z -= 20;
		//_toad->GetComponent<Transform>()->SetPosition(_toadPos);

		/*for (int i = 0; i < 6; i++)
		{
			for (int j = 0; j < 6; j++)
			{
				std::shared_ptr<GameObject> pbrObj = CreateEmpty();
				pbrObj->GetComponent<Transform>()->SetPosition({ _toadPos.x + 1.5f + (j * 1.5f) + (i * 1.5f), _toadPos.y + 5.f, _toadPos.z - 1.5f - (j * 1.5f) + (i * 1.5f) });
				pbrObj->AddComponent<MeshRenderer>()->Init("SphereMesh");
				pbrObj->GetComponent<MeshRenderer>()->SetMaterialColor("PBR_Mat", { 1.0f, 0.5f, 0.5f, 1.0f });
				pbrObj->GetComponent<MeshRenderer>()->SetMaterialMetallicRoughness("PBR_Mat", 1.0f - 0.2f * j, 0.f + 0.2f * i);
			}
		}*/

		// 크래프트를 가능하게 합니다.
			// 치트를 안써야 가능합니다.
		GameManager::GetInstance()->SetCraftDone(false);
	}
	
	std::shared_ptr<GameObject> TutorialScene::CreateUI()
	{
		std::shared_ptr<GameObject> ui = CreateEmpty();
		ui->AddComponent<InGameUIController>();
		ui->GetComponent<InGameUIController>()->SetTutoriaDialogMap(tutoriaDialogMap);
		return ui;
	}

	void TutorialScene::SetScene()
	{

	}
}