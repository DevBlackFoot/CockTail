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
		// ���� �Ŵ��� ĳ��
		gameManager = GameManager::GetInstance();
		sceneManager = SceneManager::GetInstance();

		auto wave = gameManager->GetJsonObject("waveBattle");

		// �����͸� �־�ô�.
		for (auto data : wave)
		{
			// Spawn Order�� ���� �ٸ��� �־��ݴϴ�.
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

			// ���͸� �ϳ� ����ϴ�.
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

		// ù ���̺긦 �����ϰ� Idx�� Kill Goal�� �������ݴϴ�.
		waveIdx = 1;
		waveKillGoal = wave1EnemyData.size();

		// ���� �Ŵ����� KillCount�� �ʱ�ȭ�մϴ�.
		gameManager->ResetKillCount();
	}

	void EnemyWaveSpawn::SetAllEnemy()
	{
		auto scene = SceneManager::GetInstance()->GetCurrentScene();

		// �� ���͸� ���鼭 ��� Enemy�� �������ݴϴ�.
			// ���͸� �����..
		// Info ���� -> 
		for (auto enemyInfo : wave1EnemyData)
		{
			auto enemy = scene->CreateEmpty();

			int id = enemyInfo[0];
			int type = enemyInfo[1];
			int posx = enemyInfo[2];
			int posy = enemyInfo[3];
			int posz = enemyInfo[4];
			int roty = enemyInfo[5];

			// �⺻���� ������ ���� ����ݴϴ�.
			enemy->GetComponent<Transform>()->SetPosition({ static_cast<float>(posx), static_cast<float>(posy), static_cast<float>(posz) });
			enemy->GetComponent<Transform>()->SetLocalEulerRotation({ 0.f, static_cast<float>(roty), 0.f });

			// Ÿ�Կ� ���� �߰����ִ� model�� ��Ʈ�ѷ��� �÷��ݴϴ�.
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

			// �׸��� ������Ʈ�� Enable �صӴϴ�.
			// id�� ������ �̸��� �����ϴ�.
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

			// �⺻���� ������ ���� ����ݴϴ�.
			enemy->GetComponent<Transform>()->SetPosition({ static_cast<float>(posx), static_cast<float>(posy), static_cast<float>(posz) });
			enemy->GetComponent<Transform>()->SetLocalEulerRotation({ 0.f, static_cast<float>(roty), 0.f });

			// Ÿ�Կ� ���� �߰����ִ� model�� ��Ʈ�ѷ��� �÷��ݴϴ�.
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

			// �׸��� ������Ʈ�� Enable �صӴϴ�.
			// id�� ������ �̸��� �����ϴ�.
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

			// �⺻���� ������ ���� ����ݴϴ�.
			enemy->GetComponent<Transform>()->SetPosition({ static_cast<float>(posx), static_cast<float>(posy), static_cast<float>(posz) });
			enemy->GetComponent<Transform>()->SetLocalEulerRotation({ 0.f, static_cast<float>(roty), 0.f });

			// Ÿ�Կ� ���� �߰����ִ� model�� ��Ʈ�ѷ��� �÷��ݴϴ�.
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

			// �׸��� ������Ʈ�� Enable �صӴϴ�.
			// id�� ������ �̸��� �����ϴ�.
			enemy->SetName(L"Enemy " + to_wstring(id));
			enemy->SetGameObjectEnable(false);
		}

	}

	void EnemyWaveSpawn::Update(float tick)
	{
		// �׽�Ʈ�� InputManager
		if (isWaveStart)
		{
			// ù ���̺� ���� for���� ���鼭.. ã���ݴϴ�.
			if (wave1Play != true)
			{
				wave1Play = true;

				for (int i = 0; i < waveKillGoal; i++)
				{
					// wave1�� 0����.
					auto enemy = sceneManager->FindObjectName(L"Enemy " + to_wstring(i));
					enemy->SetGameObjectEnable(true);

					// Start�� Awake�� ��� �ҷ��ݴϴ�..?
					enemy->Start();
				}
			}

			isWaveStart = false;
		}

		// ų ī��Ʈ�� �޼��ϸ�.. ���� ���̺�� �Ѿ�ݴϴ�.
		if (gameManager->GetKillCount() >= waveKillGoal)
		{
			if (waveIdx == 1 && wave2Play != true)
			{
				// ųī��Ʈ ����
				gameManager->ResetKillCount();

				// �ε����� ���� �ٲ��ݴϴ�.
				waveIdx = 2;
				wave2Play = true;
				waveKillGoal = wave2EnemyData.size();

				int idOffset = wave1EnemyData.size();
				for (int i = 0; i < waveKillGoal; i++)
				{
					// wave1�� 0����.
					auto enemy = sceneManager->FindObjectName(L"Enemy " + to_wstring(i + idOffset));
					enemy->SetGameObjectEnable(true);

					// Start�� Awake�� ��� �ҷ��ݴϴ�..?
					enemy->Start();
				}

				// �Ʒ��� ���� �ȵǰ� �������ݴϴ�.
				return;
			}

			if (waveIdx == 2 && wave3Play != true)
			{
				// ųī��Ʈ ����
				gameManager->ResetKillCount();

				// �ε����� ���� �ٲ��ݴϴ�.
				waveIdx = 3;
				wave3Play = true;
				waveKillGoal = wave3EnemyData.size();

				int idOffset = wave1EnemyData.size() + wave2EnemyData.size();
				for (int i = 0; i < waveKillGoal; i++)
				{
					// wave1�� 0����.
					auto enemy = sceneManager->FindObjectName(L"Enemy " + to_wstring(i + idOffset));
					enemy->SetGameObjectEnable(true);

					// Start�� Awake�� ��� �ҷ��ݴϴ�..?
					enemy->Start();
				}

				return;
			}

			if (waveIdx == 3)
			{
				// ųī��Ʈ ����
				gameManager->ResetKillCount();

				gameManager->isGetKey = true;

				// �ƾ��÷��� ������Ʈ ������
				GameManager::GetInstance()->isCutSceneCameraMove = true;

				playerController->GetRenderObj()->SetTrigger("IdleTrigger");
				playerController->GetRenderObj()->SetFloat("Move", 0);
				playerController->SetStatus(PlayerStatus::IDLE);

				playerController->SetTarget({ 6.56, playerController->GetGameObject()->GetComponent<Transform>()->GetWorldPosition().y, 8.18 });
			}
		}
	}
}
