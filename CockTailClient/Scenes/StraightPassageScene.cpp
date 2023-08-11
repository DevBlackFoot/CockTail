#include "pch.h"
#include "StraightPassageScene.h"

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
	StraightPassageScene::StraightPassageScene(tstring sceneName)
		: SceneBase(sceneName)
	{
	}

	StraightPassageScene::~StraightPassageScene()
	{
	}

	void StraightPassageScene::BuildScene()
	{
		// 각종 오브젝트들 생성
		SceneManager::GetInstance()->LoadSceneData("StraightPassageScene");

		SetScene();

		// player 생성
		std::shared_ptr<GameObject> player = CreateEmpty();
		player->AddComponent<PlayerController>();

		if (SceneManager::GetInstance()->GetPrevSceneName() == L"Room2Scene")
		{
			GameManager::GetInstance()->isRoom2Clear = true;
			player->GetComponent<Transform>()->SetPosition(room2Pos->GetWorldPosition());
			player->GetComponent<Transform>()->SetRotation(room2Pos->GetWorldRotation());
		}
		else
		{
			// room 3를 깬 것이기 때문에
			GameManager::GetInstance()->isRoom3Clear = true;
			player->GetComponent<Transform>()->SetPosition(room3Pos->GetWorldPosition());
			player->GetComponent<Transform>()->SetRotation(room3Pos->GetWorldRotation());
		}

		player->AddComponent<PlayerAttackController>();

		SceneManager::GetInstance()->GetCurrentScene()->GetMainCamera()->GetGameObject()->AddComponent<CameraController>();

		CreateUI();
	}

	std::shared_ptr<GameObject> StraightPassageScene::CreateUI()
	{
		std::shared_ptr<GameObject> ui = CreateEmpty();
		ui->AddComponent<InGameUIController>();
		return ui;
	}

	void StraightPassageScene::SetScene()
	{
		auto scene = SceneManager::GetInstance();

		auto room2Portal = scene->FindObjectName(L"Room2Portal");
		auto room2Point = scene->FindObjectName(L"Room2Point");

		auto room3Portal = scene->FindObjectName(L"Room3Portal");
		auto room3Point = scene->FindObjectName(L"Room3Point");

		room2Pos = room2Point->GetComponent<Transform>();
		room3Pos = room3Point->GetComponent<Transform>();

		room2Portal->AddComponent<NextSceneTrigger>()->SetNextScene(L"Room2Scene");
		room3Portal->AddComponent<NextSceneTrigger>()->SetNextScene(L"LeftRoom3Scene");
	}
}