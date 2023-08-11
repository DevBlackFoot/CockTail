#include "pch.h"
#include "RightPassage2Scene.h"

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
	RightPassage2Scene::RightPassage2Scene(tstring sceneName)
		: SceneBase(sceneName)
	{
	}

	RightPassage2Scene::~RightPassage2Scene()
	{
	}

	void RightPassage2Scene::BuildScene()
	{
		// 각종 오브젝트들 생성
		SceneManager::GetInstance()->LoadSceneData("RightPassage2Scene");

		SetScene();

		// player 생성
		std::shared_ptr<GameObject> player = CreateEmpty();
		player->AddComponent<PlayerController>();	

		if (SceneManager::GetInstance()->GetPrevSceneName() == L"RightRoom3Scene")
		{
			player->GetComponent<Transform>()->SetPosition(rightPos->GetWorldPosition());
			player->GetComponent<Transform>()->SetRotation(rightPos->GetWorldRotation());
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

	std::shared_ptr<GameObject> RightPassage2Scene::CreateUI()
	{
		std::shared_ptr<GameObject> ui = CreateEmpty();
		ui->AddComponent<InGameUIController>();
		return ui;
	}

	void RightPassage2Scene::SetScene()
	{
		auto scene = SceneManager::GetInstance();

		auto rightRoom3Portal = scene->FindObjectName(L"RightRoom3Portal");
		auto rightRoom3Point = scene->FindObjectName(L"RightRoom3Point");

		auto room4Portal = scene->FindObjectName(L"Room4Portal");
		auto room4Point = scene->FindObjectName(L"Room4Point");

		rightPos = rightRoom3Point->GetComponent<Transform>();
		room4Pos = room4Point->GetComponent<Transform>();

		rightRoom3Portal->AddComponent<NextSceneTrigger>()->SetNextScene(L"RightRoom3Scene");
		room4Portal->AddComponent<NextSceneTrigger>()->SetNextScene(L"Room4Scene");
	}
}