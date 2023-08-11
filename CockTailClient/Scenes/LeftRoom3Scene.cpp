#include "pch.h"
#include "LeftRoom3Scene.h"

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
#include "MeleeDogController.h"
#include "ArchorDogController.h"
#include "ToadController.h"

#include "NextSceneTrigger.h"
#include "GameManagerObject.h"

//UI
#include "InGameUIController.h"
#include "ItemGetUI.h"
#include "PortalEffect.h"
#include "SettingUIController.h"

namespace ClientSpace
{
	LeftRoom3Scene::LeftRoom3Scene(tstring sceneName)
		: SceneBase(sceneName)
	{
	}

	LeftRoom3Scene::~LeftRoom3Scene()
	{
	}

	void LeftRoom3Scene::BuildScene()
	{
		// 각종 오브젝트들 생성
		SceneManager::GetInstance()->LoadSceneData("LeftRoom3Scene");

		// NavMesh 생성 
		NavigationManager::GetInstance()->MakeNavMesh("Room3NavMesh");

		auto scene = SceneManager::GetInstance();

		auto straightPassagePortal = scene->FindObjectName(L"StraightPassagePortal");
		auto straightPassagePoint = scene->FindObjectName(L"StraightPassagePoint");

		auto leftPassage2Portal = scene->FindObjectName(L"LeftPassage2Portal");
		auto leftPassage2Point = scene->FindObjectName(L"LeftPassage2Point");

		straightPassagePortal->AddComponent<NextSceneTrigger>()->SetNextScene(L"StraightPassageScene");
		leftPassage2Portal->AddComponent<NextSceneTrigger>()->SetNextScene(L"LeftPassage2Scene");

		leftPos = leftPassage2Point->GetComponent<Transform>();
		straightPos = straightPassagePoint->GetComponent<Transform>();

		int killGoal = 0;

		SetScene();

		// player 생성
		std::shared_ptr<GameObject> player = CreateEmpty();
		player->AddComponent<PlayerController>();
		if (SceneManager::GetInstance()->GetPrevSceneName() == L"StraightPassageScene")
		{
			player->GetComponent<Transform>()->SetPosition(straightPos->GetWorldPosition());
			player->GetComponent<Transform>()->SetRotation(straightPos->GetWorldRotation());

			// 테스트중!
		}
		else
		{
			player->GetComponent<Transform>()->SetPosition(leftPos->GetWorldPosition());
			player->GetComponent<Transform>()->SetRotation(leftPos->GetWorldRotation());
		}
		player->AddComponent<PlayerAttackController>();

		if (GameManager::GetInstance()->isRoom3Clear == false)
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

			straightPassagePortal->GetComponent<NextSceneTrigger>()->SetKillCount(killGoal);
			leftPassage2Portal->GetComponent<NextSceneTrigger>()->SetKillCount(killGoal);
		}

		SceneManager::GetInstance()->GetCurrentScene()->GetMainCamera()->GetGameObject()->AddComponent<CameraController>();
		auto portalEffect = CreateEmpty();
		portalEffect->AddComponent<Particle>()->LoadParticlePrefab("Portal");
		portalEffect->AddComponent<PortalEffect>()->Init(killGoal);
		portalEffect->GetComponent<Transform>()->SetPosition(leftPassage2Portal->GetComponent<Transform>()->GetWorldPosition());
		CreateUI();
	}

	std::shared_ptr<GameObject> LeftRoom3Scene::CreateUI()
	{
		std::shared_ptr<GameObject> ui = CreateEmpty();
		ui->AddComponent<InGameUIController>();
		ui->AddComponent<ItemGetUI>()->Init();

		return ui;
	}

	void LeftRoom3Scene::SetScene()
	{

	}
}