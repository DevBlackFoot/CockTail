#include "pch.h"
#include "ElevatorScene.h"

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

#include "BossController.h"
#include "BossTile.h"
#include "BossTurret.h"

#include "BossTurretAttackObject.h"

#include "SmileTurret.h"

//UI
#include "InGameUIController.h"
#include "SettingUIController.h"
#include "ItemGetUI.h"
#include "CutScenePlay.h"

#include "RedController.h"

namespace ClientSpace
{
	ElevatorScene::ElevatorScene(tstring sceneName)
		: SceneBase(sceneName)
	{
	}

	ElevatorScene::~ElevatorScene()
	{
	}

	void ElevatorScene::BuildScene()
	{
		//SoundManager::GetInstance()->StopAllSound();
		//SoundManager::GetInstance()->PlayBackgroundSound("ElevatorBGM", 0.5f);
		
		// 각종 오브젝트들 생성
		SceneManager::GetInstance()->LoadSceneData("ElevatorScene");
		
		auto scene = SceneManager::GetInstance();

		SetScene();

		// 스크립트 가져오기
		std::vector <JsonObject> scriptJsons = GameManager::GetInstance()->GetJsonObject("lobbyScriptData");
		for (auto scriptJsonObject : scriptJsons)
		{
			ElevatorDialog elevatorDialogStruct;

			elevatorDialogStruct.eventIndex = scriptJsonObject.GetInt(L"EventIndex");
			elevatorDialogStruct.scriptEvent = scriptJsonObject.GetString(L"ScriptEvent");
			//elevatorDialogStruct.scriptStringEN = scriptJsonObject.GetString(L"ScriptStringEN");
			elevatorDialogStruct.scriptStringKR = scriptJsonObject.GetString(L"ScriptStringKR");

			elevatorDialogMap[elevatorDialogStruct.eventIndex] = elevatorDialogStruct;
		}

		// player 생성
		std::shared_ptr<GameObject> player = CreateEmpty();
		// player 위치 정하기
		auto entryPoint = scene->FindObjectName(L"EntryPoint");
		playerPos = entryPoint->GetComponent<Transform>();

		player->AddComponent<PlayerController>();
		player->GetComponent<Transform>()->SetPosition(playerPos->GetWorldPosition());
		player->GetComponent<Transform>()->SetRotation(playerPos->GetWorldRotation());
		player->AddComponent<PlayerAttackController>();
		

		SceneManager::GetInstance()->GetCurrentScene()->GetMainCamera()->GetGameObject()->AddComponent<CameraController>();

		// 이펙트 생성
		std::shared_ptr<GameObject> leftEffect = CreateEmpty();
		leftEffect->AddComponent<GameEngineSpace::SpriteEffect>()->Init("Resources/Effect/WallEffect.png");
		leftEffect->GetComponent<Transform>()->SetPosition({-11.5f, -1.3f, 3.f});
		leftEffect->GetComponent<Transform>()->SetScale({20.f, 40.f, 1.0f});
		leftEffect->GetComponent<Transform>()->SetLocalEulerRotation({0.f, 315.f, 0.f});
		leftEffect->GetComponent<GameEngineSpace::SpriteEffect>()->SetYPlay(false);
		leftEffect->GetComponent<GameEngineSpace::SpriteEffect>()->SetMaskColor({0.3f, 0.3f, 0.3f, 1.0f});

		std::shared_ptr<GameObject> leftEffectGradient = CreateEmpty();
		leftEffectGradient->AddComponent<GameEngineSpace::SpriteEffect>()->Init("Resources/Effect/WallGradient.png");
		leftEffectGradient->GetComponent<Transform>()->SetPosition({ -5.37f, -5.51f, 7.67f });
		leftEffectGradient->GetComponent<Transform>()->SetScale({ 40.f, 25.f, 1.0f });
		leftEffectGradient->GetComponent<Transform>()->SetLocalEulerRotation({ 0.f, 315.f, 0.f });
		leftEffectGradient->GetComponent<GameEngineSpace::SpriteEffect>()->SetMaskColor({ 1.0f, 1.0f, 1.0f, 1.0f });

		std::shared_ptr<GameObject> rightEffect = CreateEmpty();
		rightEffect->AddComponent<GameEngineSpace::SpriteEffect>()->Init("Resources/Effect/WallEffect.png");
		rightEffect->GetComponent<Transform>()->SetPosition({ 22.43f, -3.16f, 15.77f });
		rightEffect->GetComponent<Transform>()->SetScale({ 20.f, 40.f, 1.0f });
		rightEffect->GetComponent<Transform>()->SetLocalEulerRotation({ 0.f, 45.f, 0.f });
		rightEffect->GetComponent<GameEngineSpace::SpriteEffect>()->SetYPlay(false);
		rightEffect->GetComponent<GameEngineSpace::SpriteEffect>()->SetMaskColor({ 0.1f, 0.1f, 0.1f, 1.0f });

		std::shared_ptr<GameObject> rightEffectGradient = CreateEmpty();
		rightEffectGradient->AddComponent<GameEngineSpace::SpriteEffect>()->Init("Resources/Effect/WallGradient.png");
		rightEffectGradient->GetComponent<Transform>()->SetPosition({ 21.29f, -6.49f, 11.87f });
		rightEffectGradient->GetComponent<Transform>()->SetScale({ 40.f, 25.f, 1.0f });
		rightEffectGradient->GetComponent<Transform>()->SetLocalEulerRotation({ 0.f, 45.f, 0.f });
		rightEffectGradient->GetComponent<GameEngineSpace::SpriteEffect>()->SetMaskColor({ 1.0f, 1.0f, 1.0f, 1.0f });

		std::shared_ptr<GameObject> floor = CreateEmpty();
		floor->AddComponent<GameEngineSpace::SpriteEffect>()->Init("Resources/Effect/ElevatorFloor.png");
		floor->GetComponent<Transform>()->SetPosition({ 0.f, -30.f, 0.f });
		floor->GetComponent<Transform>()->SetLocalEulerRotation({ 90.f, 0.f, 0.f });
		floor->GetComponent<Transform>()->SetScale({ 40.f, 40.f, 1.0f });



		auto ui = CreateUI();	

		std::shared_ptr<GameObject> bossCutScene = CreateEmpty();
		bossCutScene->AddComponent<CutScenePlay>()->Init(ui->GetComponent<InGameUIController>().get(), CutScenePlayType::Elevator);

		// 플레이어 멈춰놓기
		player->GetComponent<PlayerController>()->SetIsPlayerControl(false);

		// Red 생성하기
		auto red = CreateEmpty();
		red->AddComponent<MeshRenderer>()->LoadPrefab("redani.model");
		red->AddComponent<RedController>();
		red->GetComponent<RedController>()->SetSpawnEnemy(true);
		red->GetComponent<Transform>()->SetPosition({0.77, 1.93, 0.38});
		red->GetComponent<Transform>()->Rotate(Vector3::Up, 225);
		red->SetGameObjectEnable(false);		// 꺼둔다.
		
		bossCutScene->GetComponent<CutScenePlay>()->SetRed(red.get());

		/// <summary>
		/// 포탑 보스 
		/// </summary>
		std::shared_ptr<GameObject> boss = CreateEmpty();
		boss->AddComponent<SmileTurret>()->SetSpriteEffectObject(leftEffect, rightEffect);

		bossCutScene->GetComponent<CutScenePlay>()->SetBoss(boss.get());
		bossCutScene->GetComponent<CutScenePlay>()->SetSpriteEffect(leftEffect.get(), rightEffect.get());
	}

	std::shared_ptr<GameObject> ElevatorScene::CreateUI()
	{
		std::shared_ptr<GameObject> ui = CreateEmpty();
		ui->AddComponent<InGameUIController>();
		ui->GetComponent<InGameUIController>()->SetElevatorDialogMap(elevatorDialogMap);
		ui->AddComponent<ItemGetUI>()->Init();
		return ui;
	}

	void ElevatorScene::SetScene()
	{
		
	}
}