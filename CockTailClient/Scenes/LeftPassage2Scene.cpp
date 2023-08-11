#include "pch.h"
#include "LeftPassage2Scene.h"

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
	LeftPassage2Scene::LeftPassage2Scene(tstring sceneName)
		: SceneBase(sceneName)
	{
	}

	LeftPassage2Scene::~LeftPassage2Scene()
	{
	}

	void LeftPassage2Scene::BuildScene()
	{
		// 각종 오브젝트들 생성
		SceneManager::GetInstance()->LoadSceneData("LeftPassage2Scene");

		SetScene();

		// player 생성
		std::shared_ptr<GameObject> player = CreateEmpty();
		player->AddComponent<PlayerController>();
		if (SceneManager::GetInstance()->GetPrevSceneName() == L"LeftRoom3Scene")
		{
			GameManager::GetInstance()->isRoom3Clear = true;
			player->GetComponent<Transform>()->SetPosition(room3Pos->GetWorldPosition());
			player->GetComponent<Transform>()->SetRotation(room3Pos->GetWorldRotation());
		}
		else
		{
			player->GetComponent<Transform>()->SetPosition(room4Pos->GetWorldPosition());
			player->GetComponent<Transform>()->SetRotation(room4Pos->GetWorldRotation());
		}
		player->AddComponent<PlayerAttackController>();

		SceneManager::GetInstance()->GetCurrentScene()->GetMainCamera()->GetGameObject()->AddComponent<CameraController>();

		CreateUI();
	}

	std::shared_ptr<GameObject> LeftPassage2Scene::CreateUI()
	{
		std::shared_ptr<GameObject> ui = CreateEmpty();
		ui->AddComponent<InGameUIController>();
		return ui;
	}

	void LeftPassage2Scene::SetScene()
	{
		auto scene = SceneManager::GetInstance();

		auto room3Portal = scene->FindObjectName(L"Room3Portal");
		auto room3Point = scene->FindObjectName(L"Room3Point");

		auto room4Portal = scene->FindObjectName(L"Room4Portal");
		auto room4Point = scene->FindObjectName(L"Room4Point");

		room3Pos = room3Point->GetComponent<Transform>();
		room4Pos = room4Point->GetComponent<Transform>();

		room3Portal->AddComponent<NextSceneTrigger>()->SetNextScene(L"LeftRoom3Scene");
		room4Portal->AddComponent<NextSceneTrigger>()->SetNextScene(L"Room4Scene");
	}
}