#include "pch.h"
#include "Elevator2Scene.h"

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

#include "MiniTurret.h"
#include "MiniTurretController.h"
#include "MissileController.h"

//UI
#include "InGameUIController.h"
#include "SettingUIController.h"

namespace ClientSpace
{
	Elevator2Scene::Elevator2Scene(tstring sceneName)
		: SceneBase(sceneName)
	{
	}

	Elevator2Scene::~Elevator2Scene()
	{
	}

	void Elevator2Scene::BuildScene()
	{
		SoundManager::GetInstance()->StopAllSound();
		SoundManager::GetInstance()->PlayBackgroundSound("ElevatorBGM", 0.7f);

		// 각종 오브젝트들 생성
		SceneManager::GetInstance()->LoadSceneData("Elevator2Scene");

		SetScene();

		// player 생성
		std::shared_ptr<GameObject> player = CreateEmpty();
		player->AddComponent<PlayerController>();
		player->GetComponent<Transform>()->SetPosition(playerPos->GetWorldPosition());
		player->GetComponent<Transform>()->SetRotation(playerPos->GetWorldRotation());
		player->AddComponent<PlayerAttackController>();

		SceneManager::GetInstance()->GetCurrentScene()->GetMainCamera()->GetGameObject()->AddComponent<CameraController>();

		CreateUI();
	}

	std::shared_ptr<GameObject> Elevator2Scene::CreateUI()
	{
		std::shared_ptr<GameObject> ui = CreateEmpty();
		ui->AddComponent<InGameUIController>();
		return ui;
	}

	void Elevator2Scene::SetScene()
	{
		auto scene = SceneManager::GetInstance();

		auto entryPoint = scene->FindObjectName(L"EntryPoint");
		playerPos = entryPoint->GetComponent<Transform>();

		// MiniTurret 1 ~ 12
		auto turret1 = scene->FindObjectName(L"MiniTurret 1");
		auto turret2 = scene->FindObjectName(L"MiniTurret 2");
		auto turret3 = scene->FindObjectName(L"MiniTurret 3");
		auto turret4 = scene->FindObjectName(L"MiniTurret 4");
		auto turret5 = scene->FindObjectName(L"MiniTurret 5");
		auto turret6 = scene->FindObjectName(L"MiniTurret 6");
		auto turret7 = scene->FindObjectName(L"MiniTurret 7");
		auto turret8 = scene->FindObjectName(L"MiniTurret 8");
		auto turret9 = scene->FindObjectName(L"MiniTurret 9");
		auto turret10 = scene->FindObjectName(L"MiniTurret 10");
		auto turret11 = scene->FindObjectName(L"MiniTurret 11");
		auto turret12 = scene->FindObjectName(L"MiniTurret 12");

		auto mtt1 = scene->FindObjectName(L"MiniTurretTrigger 1");
		auto mtt2 = scene->FindObjectName(L"MiniTurretTrigger 2");
		auto mtt3 = scene->FindObjectName(L"MiniTurretTrigger 3");
		auto mtt4 = scene->FindObjectName(L"MiniTurretTrigger 4");
		auto mtt5 = scene->FindObjectName(L"MiniTurretTrigger 5");
		auto mtt6 = scene->FindObjectName(L"MiniTurretTrigger 6");
		auto mtt7 = scene->FindObjectName(L"MiniTurretTrigger 7");
		auto mtt8 = scene->FindObjectName(L"MiniTurretTrigger 8");
		auto mtt9 = scene->FindObjectName(L"MiniTurretTrigger 9");
		auto mtt10 = scene->FindObjectName(L"MiniTurretTrigger 10");
		auto mtt11 = scene->FindObjectName(L"MiniTurretTrigger 11");
		auto mtt12 = scene->FindObjectName(L"MiniTurretTrigger 12");

		turret1->AddComponent<MiniTurret>()->SetTriggerObject(mtt1);
		turret2->AddComponent<MiniTurret>()->SetTriggerObject(mtt2);
		turret3->AddComponent<MiniTurret>()->SetTriggerObject(mtt3);
		turret4->AddComponent<MiniTurret>()->SetTriggerObject(mtt4);
		turret5->AddComponent<MiniTurret>()->SetTriggerObject(mtt5);
		turret6->AddComponent<MiniTurret>()->SetTriggerObject(mtt6);
		turret7->AddComponent<MiniTurret>()->SetTriggerObject(mtt7);
		turret8->AddComponent<MiniTurret>()->SetTriggerObject(mtt8);
		turret9->AddComponent<MiniTurret>()->SetTriggerObject(mtt9);
		turret10->AddComponent<MiniTurret>()->SetTriggerObject(mtt10);
		turret11->AddComponent<MiniTurret>()->SetTriggerObject(mtt11);
		turret12->AddComponent<MiniTurret>()->SetTriggerObject(mtt12);

		auto missileCon = CreateEmpty();
		auto temp = missileCon->AddComponent<MissileController>();

		turret1->GetComponent<MiniTurret>()->SetMissiles(temp);
		turret2->GetComponent<MiniTurret>()->SetMissiles(temp);
		turret3->GetComponent<MiniTurret>()->SetMissiles(temp);
		turret4->GetComponent<MiniTurret>()->SetMissiles(temp);
		turret5->GetComponent<MiniTurret>()->SetMissiles(temp);
		turret6->GetComponent<MiniTurret>()->SetMissiles(temp);
		turret7->GetComponent<MiniTurret>()->SetMissiles(temp);
		turret8->GetComponent<MiniTurret>()->SetMissiles(temp);
		turret9->GetComponent<MiniTurret>()->SetMissiles(temp);
		turret10->GetComponent<MiniTurret>()->SetMissiles(temp);
		turret11->GetComponent<MiniTurret>()->SetMissiles(temp);
		turret12->GetComponent<MiniTurret>()->SetMissiles(temp);

		auto turretManager = CreateEmpty();
		auto turretCon = turretManager->AddComponent<MiniTurretController>();
		turretCon->PushTurret(turret1);
		turretCon->PushTurret(turret2);
		turretCon->PushTurret(turret3);
		turretCon->PushTurret(turret4);
		turretCon->PushTurret(turret5);
		turretCon->PushTurret(turret6);
		turretCon->PushTurret(turret7);
		turretCon->PushTurret(turret8);
		turretCon->PushTurret(turret9);
		turretCon->PushTurret(turret10);
		turretCon->PushTurret(turret11);
		turretCon->PushTurret(turret12);
	}
}