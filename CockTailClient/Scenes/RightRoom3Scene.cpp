#include "pch.h"
#include "RightRoom3Scene.h"

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
	RightRoom3Scene::RightRoom3Scene(tstring sceneName)
		: SceneBase(sceneName)
	{
	}

	RightRoom3Scene::~RightRoom3Scene()
	{
	}

	void RightRoom3Scene::BuildScene()
	{
		// 각종 오브젝트들 생성
		SceneManager::GetInstance()->LoadSceneData("RightRoom3Scene");

		// NavMesh 생성 
		NavigationManager::GetInstance()->MakeNavMesh("Room3NavMesh");

		SetScene();

		// player 생성
		std::shared_ptr<GameObject> player = CreateEmpty();
		player->AddComponent<PlayerController>();
		if (SceneManager::GetInstance()->GetPrevSceneName() == L"RightPassage1Scene")
		{
			player->GetComponent<Transform>()->SetPosition(right1Pos->GetWorldPosition());
			player->GetComponent<Transform>()->SetRotation(right1Pos->GetWorldRotation());
		}

		else
		{
			player->GetComponent<Transform>()->SetPosition(right2Pos->GetWorldPosition());
			player->GetComponent<Transform>()->SetRotation(right2Pos->GetWorldRotation());
		}
		player->AddComponent<PlayerAttackController>();

		SceneManager::GetInstance()->GetCurrentScene()->GetMainCamera()->GetGameObject()->AddComponent<CameraController>();

		CreateUI();
	}

	std::shared_ptr<GameObject> RightRoom3Scene::CreateUI()
	{
		std::shared_ptr<GameObject> ui = CreateEmpty();
		ui->AddComponent<InGameUIController>();
		return ui;
	}

	void RightRoom3Scene::SetScene()
	{
		auto scene = SceneManager::GetInstance();

		auto rightPassage1Portal = scene->FindObjectName(L"RightPassage1Portal");
		auto rightPassage1Point = scene->FindObjectName(L"RightPassage1Point");

		auto rightPassage2Portal = scene->FindObjectName(L"RightPassage2Portal");
		auto rightPassage2Point = scene->FindObjectName(L"RightPassage2Point");

		right1Pos = rightPassage1Point->GetComponent<Transform>();
		right2Pos = rightPassage2Point->GetComponent<Transform>();

		rightPassage1Portal->AddComponent<NextSceneTrigger>()->SetNextScene(L"RightPassage1Scene");
		rightPassage2Portal->AddComponent<NextSceneTrigger>()->SetNextScene(L"RightPassage2Scene");
	}
}