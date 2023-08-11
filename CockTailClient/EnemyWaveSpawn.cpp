#include "pch.h"
#include "EnemyWaveSpawn.h"

#include "GameManager.h"
#include "SceneManager.h"
#include "InputManager.h"

#include "MeleeDogController.h"
#include "ArchorDogController.h"
#include "EnemyNavAgent.h"
#include "EnemySpawn.h"
#include "ToadController.h"
#include "Component/MeshRenderer.h"
#include "Scripts/PlayerController.h"

namespace ClientSpace
{
	EnemyWaveSpawn::EnemyWaveSpawn(std::weak_ptr<GameObject> _object)
		: ComponentBase(_object)
		, waveIdx(1)
		, waveKillGoal(4)
		, wave1Play(false)
		, wave2Play(false)
		, wave3Play(false)
		, isWaveStart(false)
	{
	}

	EnemyWaveSpawn::~EnemyWaveSpawn()
	{
	}

	void EnemyWaveSpawn::Init()
	{
		// 각종 매니저 캐싱
		gameManager = GameManager::GetInstance();
		sceneManager = SceneManager::GetInstance();

		auto wave = gameManager->GetJsonObject("waveBattle");

		// 데이터를 넣어봅시다.
		for (auto data : wave)
		{
			// Spawn Order에 따라 다르게 넣어줍니다.
			int nowIdx = data.GetInt(L"SpawnOrder:Integer");

			int tmpId = data.GetInt(L"id");

			wstring tmpStr = data.GetString(L"SpawnType");
			int tmpType = 0;

			if (tmpStr == L"Melee")
				tmpType = 1;
			else if (tmpStr == L"Range")
				tmpType = 2;
			else if (tmpStr == L"Frog")
				tmpType = 3;

			int tmpPosX = data.GetInt(L"SpawnPositionx:Integer");
			int tmpPosY = data.GetInt(L"SpawnPositiony:Integer");
			int tmpPosZ = data.GetInt(L"SpawnPositionz:Integer");
			int tmpRotY = data.GetInt(L"SpawnRotationY:Integer");

			// 벡터를 하나 만듭니다.
			std::vector<int> tmpVec = { tmpId, tmpType, tmpPosX, tmpPosY, tmpPosZ, tmpRotY };

			switch (nowIdx)
			{
			case 1:
			{
				wave1EnemyData.push_back(tmpVec);
			}
			break;

			case 2:
			{
				wave2EnemyData.push_back(tmpVec);
			}
			break;

			case 3:
			{
				wave3EnemyData.push_back(tmpVec);
			}
			break;

			default:
				break;
			}
		}

		// 첫 웨이브를 가정하고 Idx와 Kill Goal을 세팅해줍니다.
		waveIdx = 1;
		waveKillGoal = wave1EnemyData.size();

		// 게임 매니저의 KillCount를 초기화합니다.
		gameManager->ResetKillCount();
	}

	void EnemyWaveSpawn::SetAllEnemy()
	{
		auto scene = SceneManager::GetInstance()->GetCurrentScene();

		// 각 벡터를 돌면서 모든 Enemy를 세팅해줍니다.
			// 몬스터를 만들고..
		// Info 정보 -> 
		for (auto enemyInfo : wave1EnemyData)
		{
			auto enemy = scene->CreateEmpty();

			int id = enemyInfo[0];
			int type = enemyInfo[1];
			int posx = enemyInfo[2];
			int posy = enemyInfo[3];
			int posz = enemyInfo[4];
			int roty = enemyInfo[5];

			// 기본적인 포지션 값을 잡아줍니다.
			enemy->GetComponent<Transform>()->SetPosition({ static_cast<float>(posx), static_cast<float>(posy), static_cast<float>(posz) });
			enemy->GetComponent<Transform>()->SetLocalEulerRotation({ 0.f, static_cast<float>(roty), 0.f });

			// 타입에 따라서 추가해주는 model과 컨트롤러를 올려줍니다.
			switch (type)
			{
			case 1:
			{
				enemy->AddComponent<MeshRenderer>()->LoadPrefab("dogani.model");
				enemy->AddComponent<MeleeDogController>()->SetSpawnEnemy(true);
				break;
			}

			case 2:
			{
				enemy->AddComponent<MeshRenderer>()->LoadPrefab("dogani_throw.model");
				enemy->AddComponent<ArchorDogController>()->SetSpawnEnemy(true);
				break;
			}

			case 3:
			{
				enemy->AddComponent<MeshRenderer>()->LoadPrefab("toadani.model");
				enemy->AddComponent<ToadController>()->SetSpawnEnemy(true);
				enemy->GetComponent<Transform>()->SetScale(SimpleMath::Vector3{ 1.3f, 1.3f, 1.3f });
				break;
			}

			default:
				break;
			}

			// 그리고 오브젝트를 Enable 해둡니다.
			// id만 가지고 이름을 만들어봅니다.
			enemy->SetName(L"Enemy " + to_wstring(id));
			enemy->SetGameObjectEnable(false);
		}

		for (auto enemyInfo : wave2EnemyData)
		{
			auto enemy = scene->CreateEmpty();

			int id = enemyInfo[0];
			int type = enemyInfo[1];
			int posx = enemyInfo[2];
			int posy = enemyInfo[3];
			int posz = enemyInfo[4];
			int roty = enemyInfo[5];

			// 기본적인 포지션 값을 잡아줍니다.
			enemy->GetComponent<Transform>()->SetPosition({ static_cast<float>(posx), static_cast<float>(posy), static_cast<float>(posz) });
			enemy->GetComponent<Transform>()->SetLocalEulerRotation({ 0.f, static_cast<float>(roty), 0.f });

			// 타입에 따라서 추가해주는 model과 컨트롤러를 올려줍니다.
			switch (type)
			{
			case 1:
			{
				enemy->AddComponent<MeshRenderer>()->LoadPrefab("dogani.model");
				enemy->AddComponent<MeleeDogController>()->SetSpawnEnemy(true);
				break;
			}

			case 2:
			{
				enemy->AddComponent<MeshRenderer>()->LoadPrefab("dogani_throw.model");
				enemy->AddComponent<ArchorDogController>()->SetSpawnEnemy(true);
				break;
			}

			case 3:
			{
				enemy->AddComponent<MeshRenderer>()->LoadPrefab("toadani.model");
				enemy->AddComponent<ToadController>()->SetSpawnEnemy(true);
				enemy->GetComponent<Transform>()->SetScale(SimpleMath::Vector3{ 1.3f, 1.3f, 1.3f });
				break;
			}

			default:
				break;
			}

			// 그리고 오브젝트를 Enable 해둡니다.
			// id만 가지고 이름을 만들어봅니다.
			enemy->SetName(L"Enemy " + to_wstring(id));
			enemy->SetGameObjectEnable(false);
		}

		for (auto enemyInfo : wave3EnemyData)
		{
			auto enemy = scene->CreateEmpty();

			int id = enemyInfo[0];
			int type = enemyInfo[1];
			int posx = enemyInfo[2];
			int posy = enemyInfo[3];
			int posz = enemyInfo[4];
			int roty = enemyInfo[5];

			// 기본적인 포지션 값을 잡아줍니다.
			enemy->GetComponent<Transform>()->SetPosition({ static_cast<float>(posx), static_cast<float>(posy), static_cast<float>(posz) });
			enemy->GetComponent<Transform>()->SetLocalEulerRotation({ 0.f, static_cast<float>(roty), 0.f });

			// 타입에 따라서 추가해주는 model과 컨트롤러를 올려줍니다.
			switch (type)
			{
			case 1:
			{
				enemy->AddComponent<MeshRenderer>()->LoadPrefab("dogani.model");
				enemy->AddComponent<MeleeDogController>()->SetSpawnEnemy(true);
				break;
			}

			case 2:
			{
				enemy->AddComponent<MeshRenderer>()->LoadPrefab("dogani_throw.model");
				enemy->AddComponent<ArchorDogController>()->SetSpawnEnemy(true);
				break;
			}

			case 3:
			{
				enemy->AddComponent<MeshRenderer>()->LoadPrefab("toadani.model");
				enemy->AddComponent<ToadController>()->SetSpawnEnemy(true);
				enemy->GetComponent<Transform>()->SetScale(SimpleMath::Vector3{ 1.3f, 1.3f, 1.3f });
				auto enemyEffect = enemy->GetComponent<ToadController>()->GetEnemySpawn();
				enemyEffect->GetComponent<EnemySpawn>()->SetGoalScale(2.0f);
				break;
			}

			default:
				break;
			}

			// 그리고 오브젝트를 Enable 해둡니다.
			// id만 가지고 이름을 만들어봅니다.
			enemy->SetName(L"Enemy " + to_wstring(id));
			enemy->SetGameObjectEnable(false);
		}

	}

	void EnemyWaveSpawn::Update(float tick)
	{
		// 테스트용 InputManager
		if (isWaveStart)
		{
			// 첫 웨이브 실행 for문을 돌면서.. 찾아줍니다.
			if (wave1Play != true)
			{
				wave1Play = true;

				for (int i = 0; i < waveKillGoal; i++)
				{
					// wave1은 0부터.
					auto enemy = sceneManager->FindObjectName(L"Enemy " + to_wstring(i));
					enemy->SetGameObjectEnable(true);

					// Start와 Awake를 모두 불러줍니다..?
					enemy->Start();
				}
			}

			isWaveStart = false;
		}

		// 킬 카운트를 달성하면.. 다음 웨이브로 넘어가줍니다.
		if (gameManager->GetKillCount() >= waveKillGoal)
		{
			if (waveIdx == 1 && wave2Play != true)
			{
				// 킬카운트 리셋
				gameManager->ResetKillCount();

				// 인데스와 골을 바꿔줍니다.
				waveIdx = 2;
				wave2Play = true;
				waveKillGoal = wave2EnemyData.size();

				int idOffset = wave1EnemyData.size();
				for (int i = 0; i < waveKillGoal; i++)
				{
					// wave1은 0부터.
					auto enemy = sceneManager->FindObjectName(L"Enemy " + to_wstring(i + idOffset));
					enemy->SetGameObjectEnable(true);

					// Start와 Awake를 모두 불러줍니다..?
					enemy->Start();
				}

				// 아래것 실행 안되게 리턴해줍니다.
				return;
			}

			if (waveIdx == 2 && wave3Play != true)
			{
				// 킬카운트 리셋
				gameManager->ResetKillCount();

				// 인데스와 골을 바꿔줍니다.
				waveIdx = 3;
				wave3Play = true;
				waveKillGoal = wave3EnemyData.size();

				int idOffset = wave1EnemyData.size() + wave2EnemyData.size();
				for (int i = 0; i < waveKillGoal; i++)
				{
					// wave1은 0부터.
					auto enemy = sceneManager->FindObjectName(L"Enemy " + to_wstring(i + idOffset));
					enemy->SetGameObjectEnable(true);

					// Start와 Awake를 모두 불러줍니다..?
					enemy->Start();
				}

				return;
			}

			if (waveIdx == 3)
			{
				// 킬카운트 리셋
				gameManager->ResetKillCount();

				gameManager->isGetKey = true;

				// 컷씬플레이 업데이트 돌리기
				GameManager::GetInstance()->isCutSceneCameraMove = true;

				playerController->GetRenderObj()->SetTrigger("IdleTrigger");
				playerController->GetRenderObj()->SetFloat("Move", 0);
				playerController->SetStatus(PlayerStatus::IDLE);

				playerController->SetTarget({ 6.56, playerController->GetGameObject()->GetComponent<Transform>()->GetWorldPosition().y, 8.18 });
			}
		}
	}
}
