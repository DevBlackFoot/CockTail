#include "pch.h"
#include "RightPassage1Scene.h"

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
	RightPassage1Scene::RightPassage1Scene(tstring sceneName)
		: SceneBase(sceneName)
	{
	}

	RightPassage1Scene::~RightPassage1Scene()
	{
	}

	void RightPassage1Scene::BuildScene()
	{
		// 각종 오브젝트들 생성
		SceneManager::GetInstance()->LoadSceneData("RightPassage1Scene");

		SetScene();

		// player 생성
		std::shared_ptr<GameObject> player = CreateEmpty();
		player->AddComponent<PlayerController>();
		if (SceneManager::GetInstance()->GetPrevSceneName() == L"Room1Scene")
		{
			player->GetComponent<Transform>()->SetPosition(room1Pos->GetWorldPosition());
			player->GetComponent<Transform>()->SetRotation(room1Pos->GetWorldRotation());
		}

		else
		{
			player->GetComponent<Transform>()->SetPosition(room3Pos->GetWorldPosition());
			player->GetComponent<Transform>()->SetRotation(room3Pos->GetWorldRotation());
		}

		player->AddComponent<PlayerAttackController>();

		SceneManager::GetInstance()->GetCurrentScene()->GetMainCamera()->GetGameObject()->AddComponent<CameraController>();

		CreateUI();
	}

	std::shared_ptr<GameObject> RightPassage1Scene::CreateUI()
	{
		std::shared_ptr<GameObject> ui = CreateEmpty();
		ui->AddComponent<InGameUIController>();
		return ui;
	}

	void RightPassage1Scene::SetScene()
	{
		auto scene = SceneManager::GetInstance();

		auto room1Portal = scene->FindObjectName(L"Room1Portal");
		auto room1Point = scene->FindObjectName(L"Room1Point");

		auto rightRoom3Portal = scene->FindObjectName(L"RightRoom3Portal");
		auto rightRoom3Point = scene->FindObjectName(L"RightRoom3Point");

		room1Pos = room1Point->GetComponent<Transform>();
		room3Pos = rightRoom3Point->GetComponent<Transform>();

		room1Portal->AddComponent<NextSceneTrigger>()->SetNextScene(L"Room1Scene");
		rightRoom3Portal->AddComponent<NextSceneTrigger>()->SetNextScene(L"RightRoom3Scene");
	}
}