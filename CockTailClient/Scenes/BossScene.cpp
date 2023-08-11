#include "pch.h"
#include "BossScene.h"

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

//UI
#include "InGameUIController.h"
#include "SettingUIController.h"

namespace ClientSpace
{
	BossScene::BossScene(tstring sceneName)
		: SceneBase(sceneName)
	{
	}

	BossScene::~BossScene()
	{
	}

	void BossScene::BuildScene()
	{
		// 각종 오브젝트들 생성
		SceneManager::GetInstance()->LoadSceneData("BossScene");

		SetScene();

		// player 생성
		std::shared_ptr<GameObject> player = CreateEmpty();
		player->AddComponent<PlayerController>();
		player->GetComponent<Transform>()->SetPosition({ 1.34f, 2.f, -3.05f });
		player->AddComponent<PlayerAttackController>();

		SceneManager::GetInstance()->GetCurrentScene()->GetMainCamera()->GetGameObject()->AddComponent<CameraController>();

		CreateUI();
	}

	std::shared_ptr<GameObject> BossScene::CreateUI()
	{
		std::shared_ptr<GameObject> ui = CreateEmpty();
		ui->AddComponent<InGameUIController>();
		return ui;
	}

	void BossScene::SetScene()
	{
		auto scene = SceneManager::GetInstance();

		//auto entryPoint = scene->FindObjectName(L"EntryPoint");
	}
}