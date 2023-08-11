#include "pch.h"
#include "LeftPassage1Scene.h"

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

//UI
#include "InGameUIController.h"
#include "SettingUIController.h"

namespace ClientSpace
{
	LeftPassage1Scene::LeftPassage1Scene(tstring sceneName)
		: SceneBase(sceneName)
	{
	}

	LeftPassage1Scene::~LeftPassage1Scene()
	{
	}

	void LeftPassage1Scene::BuildScene()
	{
		// 각종 오브젝트들 생성
		SceneManager::GetInstance()->LoadSceneData("LeftPassage1Scene");

		SetScene();

		// player 생성
		std::shared_ptr<GameObject> player = CreateEmpty();
		player->AddComponent<PlayerController>();

		if (SceneManager::GetInstance()->GetPrevSceneName() == L"Room1Scene")
		{
			GameManager::GetInstance()->isRoom1Clear = true;
			player->GetComponent<Transform>()->SetPosition(room1Pos->GetWorldPosition());
			player->GetComponent<Transform>()->SetRotation(room1Pos->GetWorldRotation());
		}
		else
		{
			// room2에서 온 것이기 때문에..!
			GameManager::GetInstance()->isRoom2Clear = true;
			player->GetComponent<Transform>()->SetPosition(left2Pos->GetWorldPosition());
			player->GetComponent<Transform>()->SetRotation(left2Pos->GetWorldRotation());
		}

		player->AddComponent<PlayerAttackController>();

		SceneManager::GetInstance()->GetCurrentScene()->GetMainCamera()->GetGameObject()->AddComponent<CameraController>();

		CreateUI();
	}

	std::shared_ptr<GameObject> LeftPassage1Scene::CreateUI()
	{
		std::shared_ptr<GameObject> ui = CreateEmpty();
		ui->AddComponent<InGameUIController>();
		return ui;
	}

	void LeftPassage1Scene::SetScene()
	{
		auto scene = SceneManager::GetInstance();

		auto room1Point = scene->FindObjectName(L"Room1Point");
		auto left2Point = scene->FindObjectName(L"Left2Point");

		room1Pos = room1Point->GetComponent<Transform>();
		left2Pos = left2Point->GetComponent<Transform>();

		auto room1Portal = scene->FindObjectName(L"Room1Portal");
		auto room2Portal = scene->FindObjectName(L"Left2Portal");

		room1Portal->AddComponent<NextSceneTrigger>()->SetNextScene(L"Room1Scene");
		room2Portal->AddComponent<NextSceneTrigger>()->SetNextScene(L"Room2Scene");
	}
}