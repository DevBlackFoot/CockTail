#include "pch.h"
#include "Room2Scene.h"

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

#include "NextSceneTrigger.h"
#include "GameManagerObject.h"
#include "MeleeDogController.h"
#include "ArchorDogController.h"
#include "ToadController.h"

//UI
#include "InGameUIController.h"
#include "SettingUIController.h"
#include "ItemGetUI.h"
#include "PortalEffect.h"

namespace ClientSpace
{
	Room2Scene::Room2Scene(tstring sceneName)
		: SceneBase(sceneName)
	{
	}

	Room2Scene::~Room2Scene()
	{
	}

	void Room2Scene::BuildScene()
	{
		// 각종 오브젝트들 생성
		SceneManager::GetInstance()->LoadSceneData("Room2Scene");

		// NavMesh 생성 
		NavigationManager::GetInstance()->MakeNavMesh("Room2NavMesh");

		int killGoal = 0;

		SetScene();

		auto scene = SceneManager::GetInstance();

		auto leftPassage1Point = scene->FindObjectName(L"LeftPassage1Point");
		auto leftPassage1Portal = scene->FindObjectName(L"LeftPassage1Portal");

		auto straightPassagePoint = scene->FindObjectName(L"StraightPassagePoint");
		auto straightPassagePortal = scene->FindObjectName(L"StraightPassagePortal");

		leftPos = leftPassage1Point->GetComponent<Transform>();
		straightPos = straightPassagePoint->GetComponent<Transform>();

		leftPassage1Portal->AddComponent<NextSceneTrigger>()->SetNextScene(L"LeftPassage1Scene");
		straightPassagePortal->AddComponent<NextSceneTrigger>()->SetNextScene(L"StraightPassageScene");

		// player 생성
		std::shared_ptr<GameObject> player = CreateEmpty();
		player->AddComponent<PlayerController>();
		if (SceneManager::GetInstance()->GetPrevSceneName() == L"LeftPassage1Scene")
		{
			player->GetComponent<Transform>()->SetPosition(leftPos->GetWorldPosition());
			player->GetComponent<Transform>()->SetRotation(leftPos->GetWorldRotation());
		}
		else
		{
			player->GetComponent<Transform>()->SetPosition(straightPos->GetWorldPosition());
			player->GetComponent<Transform>()->SetRotation(straightPos->GetWorldRotation());
		}

		if (GameManager::GetInstance()->isRoom2Clear == false)
		{
			for (int i = 1; i <= 9; i++)
			{
				auto m = scene->FindObjectName(L"Melee " + to_wstring(i));
				m->AddComponent<MeshRenderer>()->LoadPrefab("dogani.model");
				m->AddComponent<MeleeDogController>()->SetLoadCompleted();
				killGoal++;
			}

			for (int i = 1; i <= 2; i++)
			{
				auto m = scene->FindObjectName(L"Toad " + to_wstring(i));
				m->AddComponent<MeshRenderer>()->LoadPrefab("toadani.model");
				m->AddComponent<ToadController>()->SetLoadCompleted();
				killGoal++;
			}

			for (int i = 1; i <= 2; i++)
			{
				auto a = scene->FindObjectName(L"Arc " + to_wstring(i));
				a->AddComponent<MeshRenderer>()->LoadPrefab("dogani_throw.model");
				a->AddComponent<ArchorDogController>()->SetLoadCompleted();
				killGoal++;
			}

			leftPassage1Portal->GetComponent<NextSceneTrigger>()->SetKillCount(killGoal);
			straightPassagePortal->GetComponent<NextSceneTrigger>()->SetKillCount(killGoal);
		}

		player->AddComponent<PlayerAttackController>();

		SceneManager::GetInstance()->GetCurrentScene()->GetMainCamera()->GetGameObject()->AddComponent<CameraController>();


		auto portalEffect = CreateEmpty();
		portalEffect->AddComponent<Particle>()->LoadParticlePrefab("Portal");
		portalEffect->AddComponent<PortalEffect>()->Init(killGoal);
		portalEffect->GetComponent<Transform>()->SetPosition(straightPassagePortal->GetComponent<Transform>()->GetWorldPosition());

		CreateUI();
	}

	std::shared_ptr<GameObject> Room2Scene::CreateUI()
	{
		std::shared_ptr<GameObject> ui = CreateEmpty();
		ui->AddComponent<InGameUIController>();
		ui->AddComponent<ItemGetUI>()->Init();
		return ui;
	}

	void Room2Scene::SetScene()
	{
		auto scene = SceneManager::GetInstance();

		auto leftPassage1Point = scene->FindObjectName(L"LeftPassage1Point");
		auto leftPassage1Portal = scene->FindObjectName(L"LeftPassage1Portal");

		auto straightPassagePoint = scene->FindObjectName(L"StraightPassagePoint");
		auto straightPassagePortal = scene->FindObjectName(L"StraightPassagePortal");

		leftPos = leftPassage1Point->GetComponent<Transform>();
		straightPos = straightPassagePoint->GetComponent<Transform>();

		leftPassage1Portal->AddComponent<NextSceneTrigger>()->SetNextScene(L"LeftPassage1Scene");
		straightPassagePortal->AddComponent<NextSceneTrigger>()->SetNextScene(L"StraightPassageScene");
	}
}
