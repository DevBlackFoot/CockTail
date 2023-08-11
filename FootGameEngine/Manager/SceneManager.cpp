#include "GamePch.h"
#include "SceneManager.h"

#include "SceneBase.h"
#include "Object/GameObject.h"
#include "Component/Camera.h"
#include "Component/BoxCollider.h"
#include "Component/MeshRenderer.h"
#include "Component/Transform.h"
#include "Component/Light.h"
#include "PhysicsManager.h"
#include "Util/ObjectPool.h"

#include "InputManager.h"
#include "Timer.h"
#include "UIManager.h"
#include "GraphicsManager.h"
#include "WindowManager.h"
#include "NavigationManager.h"
#include "CoroutineManager.h"
#include "LoadingManager.h"
#include "Component/BasicCameraController.h"

namespace GameEngineSpace
{
	std::shared_ptr<SceneManager> SceneManager::instance = nullptr;

	void SceneManager::LoadSceneData(std::string sceneName)
	{
		auto sceneData = ResourceManager::GetInstance()->LoadUnityScene("BinaryFile/Scene/" + sceneName + ".scene");

		auto scene = SceneManager::GetInstance()->GetCurrentScene();


		for (int i = 0; i < sceneData->gameObjects.size(); i++)
		{
			// ���ӿ�����Ʈ ����
			std::shared_ptr<GameObject> gameObject = scene->CreateEmpty();

			std::wstring gameObjectName;

			gameObjectName.assign(sceneData->gameObjects[i].name.begin(), sceneData->gameObjects[i].name.end());

			// ���ӿ�����Ʈ �̸� ����
			gameObject->SetName(gameObjectName);

			// ���ӿ�����Ʈ �±� ����
			if (sceneData->gameObjects[i].tag != "Untagged")
				gameObject->SetTag(sceneData->gameObjects[i].tag);

			// ���ӿ�����Ʈ fileID ����
			gameObject->SetFileID(sceneData->gameObjects[i].gameObjectID);

			if (sceneData->gameObjects[i].transform != nullptr)
			{
				auto tmpTransform = gameObject->GetComponent<Transform>();

				tmpTransform->gameObjectID = sceneData->gameObjects[i].transform->gameObjectID;
				tmpTransform->componentID = sceneData->gameObjects[i].transform->componentID;
				tmpTransform->childIDList = sceneData->gameObjects[i].transform->childIDList;

				tmpTransform->SetPosition(Vector3{
					sceneData->gameObjects[i].transform->localPosition.x,
					sceneData->gameObjects[i].transform->localPosition.y,
					sceneData->gameObjects[i].transform->localPosition.z });

				tmpTransform->SetLocalEulerRotation(Vector3{
					sceneData->gameObjects[i].transform->localRotation.x,
					sceneData->gameObjects[i].transform->localRotation.y,
					sceneData->gameObjects[i].transform->localRotation.z });

				tmpTransform->SetScale(Vector3{
					sceneData->gameObjects[i].transform->localScale.x,
					sceneData->gameObjects[i].transform->localScale.y,
					sceneData->gameObjects[i].transform->localScale.z });
			}

			if (sceneData->gameObjects[i].camera != nullptr)
			{
				scene->SetMainCamera(gameObject->AddComponent<Camera>());

				auto tmpCamera = gameObject->GetComponent<Camera>();

				tmpCamera->gameObjectID = sceneData->gameObjects[i].camera->gameObjectID;
				tmpCamera->componentID = sceneData->gameObjects[i].camera->componentID;

				/*if (sceneData->gameObjects[i].camera->projectionMatrixMode == 0)
					tmpCamera->UpdateProjMatrix(ProjType::PERSPECTIVE);
				else
					tmpCamera->UpdateProjMatrix(ProjType::ORTHOGRAPHIC);*/

				tmpCamera->UpdateProjMatrix(ProjType::PERSPECTIVE);

				tmpCamera->SetNearZ(1.0f);

				tmpCamera->SetFarZ(1000.f);

				//gameObject->AddComponent<BasicCameraController>();

				//gameObject->GetComponent<Transform>()->LookAt(Vector3{ 0.f, 0.f, 0.f });
			}

			if (sceneData->gameObjects[i].light != nullptr)
			{
				/*
				*/
				auto tmpLight = gameObject->AddComponent<Light>();

				if (sceneData->gameObjects[i].light->type == 0)
					tmpLight->ChangeType(LightType::Spot);
				else if (sceneData->gameObjects[i].light->type == 1)
					tmpLight->ChangeType(LightType::Directional);
				else if (sceneData->gameObjects[i].light->type == 2)
					tmpLight->ChangeType(LightType::Point);
				else if (sceneData->gameObjects[i].light->type == 3)
					tmpLight->ChangeType(LightType::Area);

				SimpleMath::Vector3 color = { sceneData->gameObjects[i].light->color.x, sceneData->gameObjects[i].light->color.y, sceneData->gameObjects[i].light->color.z };

				tmpLight->SetColor(color);

				// ��ü���� ���������� ���� ���⸦ �ٿ����ҽ��ϴ�.
				if (sceneName != "tutorialScene" || sceneData->gameObjects[i].light->type == 0)
				{
					float newIntensity = sceneData->gameObjects[i].light->intensity * 0.7f;

					tmpLight->SetPower(newIntensity);
				}
				else
					tmpLight->SetPower(sceneData->gameObjects[i].light->intensity);

				tmpLight->SetRange(sceneData->gameObjects[i].light->range);
				tmpLight->SetOuterSpotAngle(sceneData->gameObjects[i].light->outerSpotAngle);
				tmpLight->SetInnerSpotAngle(sceneData->gameObjects[i].light->innerSpotAngle);

				if (sceneData->gameObjects[i].light->shadow == 0)
					tmpLight->SetIsShadow(false);
				else
					tmpLight->SetIsShadow(true);
			}

			if (sceneData->gameObjects[i].boxCollider != nullptr)
			{
				auto tmpCollider = gameObject->AddComponent<BoxCollider>();

				tmpCollider->gameObjectID = sceneData->gameObjects[i].boxCollider->gameObjectID;
				tmpCollider->componentID = sceneData->gameObjects[i].boxCollider->componentID;

				// �ϴ� 8 ground, 9 wall
				if (sceneData->gameObjects[i].layer == 8)
					tmpCollider->SetLayer("Plane");
				else if (sceneData->gameObjects[i].layer == 9)
					tmpCollider->SetLayer("Wall");

				tmpCollider->SetTrigger(sceneData->gameObjects[i].boxCollider->isTrigger);

				tmpCollider->SetCenter(Vector3{
					sceneData->gameObjects[i].boxCollider->center.x,
					sceneData->gameObjects[i].boxCollider->center.y,
					sceneData->gameObjects[i].boxCollider->center.z, });

				tmpCollider->SetSize(Vector3{
					sceneData->gameObjects[i].boxCollider->size.x,
					sceneData->gameObjects[i].boxCollider->size.y,
					sceneData->gameObjects[i].boxCollider->size.z });
			}

			if (sceneData->gameObjects[i].sphereCollider != nullptr)
			{
				/*gameObject->AddComponent<SphereCollider>();

				gameObject->GetComponent<SphereCollider>()->SetRadius(sceneData->gameObjects[i].sphereCollider->radius);

				gameObject->GetComponent<SphereCollider>()->SetLocalCenter(Vector3{
				sceneData->gameObjects[i].sphereCollider->center.x,
				sceneData->gameObjects[i].sphereCollider->center.y,
				sceneData->gameObjects[i].sphereCollider->center.z });*/
			}

			if (sceneData->gameObjects[i].meshFilter != nullptr)
			{
				if (sceneData->gameObjects[i].meshFilter->meshName == "CubeMesh")
				{
					gameObject->AddComponent<MeshRenderer>()->Init("CubeMesh");
				}
				if (sceneData->gameObjects[i].meshFilter->meshName == "SphereMesh")
				{
					gameObject->AddComponent<MeshRenderer>()->Init("SphereMesh");
				}
				if (sceneData->gameObjects[i].meshFilter->meshName == "PlaneMesh")
				{
					gameObject->AddComponent<MeshRenderer>()->Init("PlaneMesh");
				}
				if (sceneData->gameObjects[i].meshFilter->meshName == "PlaneMesh")
				{
					gameObject->AddComponent<MeshRenderer>()->Init("PlaneMesh");
				}

				auto tmpMeshRenderer = gameObject->GetComponent<MeshRenderer>();

				// �ϴ� ���°� �������� ex) Cylinder..
				if (tmpMeshRenderer == nullptr)
					continue;

				// meshrenderer �� meshfilter�κ��� ��������..
				tmpMeshRenderer->gameObjectID = sceneData->gameObjects[i].meshFilter->gameObjectID;
				tmpMeshRenderer->componentID = sceneData->gameObjects[i].meshFilter->componentID;
			}

			// gameObject�� Light�� ����־��ٸ�..
			// unity������ ȸ�������� ���� ������ �����ϰ� �ֱ⶧���� �̷��� �����ش�.
			if (gameObject->GetComponent<Light>() != nullptr)
			{
				gameObject->GetComponent<Light>()->SetDirection(gameObject->GetComponent<Transform>()->GetLook());
			}
		}

		/// <summary>
		///	�θ� �ִٸ� ���� ������..
		/// </summary>
		auto gameObjectInScene = scene->GetGameObjectInScene();

		for (int i = 0; i < gameObjectInScene.size(); i++)
		{
			if (!gameObjectInScene[i]->GetComponent<Transform>()->childIDList.empty())
			{
				for (auto& childID : gameObjectInScene[i]->GetComponent<Transform>()->childIDList)
				{
					// �ڽ��� �ִ��� ã�´�.
					auto it = find_if(gameObjectInScene.begin(), gameObjectInScene.end(), [&id = childID](const std::shared_ptr<GameObject>& obj)->bool {return (obj->GetComponent<Transform>()->componentID == id); });

					// �ڽ��� ã�Ҵٸ�
					if (it != gameObjectInScene.end())
					{
						gameObjectInScene[i]->SetChild(*it);
					}
				}
			}
		}

		for (int i = 0; i < sceneData->prefabs.size(); i++)
		{
			// ���ӿ�����Ʈ ����
			std::shared_ptr<GameObject> gameObject = scene->CreateEmpty();

			std::wstring gameObjectName;

			gameObjectName.assign(sceneData->prefabs[i].name.begin(), sceneData->prefabs[i].name.end());

			// ���ӿ�����Ʈ �̸� ����
			gameObject->SetName(gameObjectName);

			auto tmpTransform = gameObject->AddComponent<Transform>();

			tmpTransform->SetPosition(Vector3{
				sceneData->prefabs[i].transform.localPosition.x,
				sceneData->prefabs[i].transform.localPosition.y,
				sceneData->prefabs[i].transform.localPosition.z });

			tmpTransform->SetLocalEulerRotation(Vector3{
				sceneData->prefabs[i].transform.localRotation.x,
				sceneData->prefabs[i].transform.localRotation.y,
				sceneData->prefabs[i].transform.localRotation.z });

			tmpTransform->SetScale(Vector3{
				sceneData->prefabs[i].transform.localScale.x,
				sceneData->prefabs[i].transform.localScale.y,
				sceneData->prefabs[i].transform.localScale.z });

			if (sceneData->prefabs[i].boxCollider != nullptr)
			{
				auto tmpCollider = gameObject->AddComponent<BoxCollider>();

				tmpCollider->gameObjectID = sceneData->prefabs[i].boxCollider->gameObjectID;
				tmpCollider->componentID = sceneData->prefabs[i].boxCollider->componentID;

				tmpCollider->SetTrigger(sceneData->prefabs[i].boxCollider->isTrigger);

				tmpCollider->SetCenter(Vector3{
					sceneData->prefabs[i].boxCollider->center.x,
					sceneData->prefabs[i].boxCollider->center.y,
					sceneData->prefabs[i].boxCollider->center.z, });

				tmpCollider->SetSize(Vector3{
					sceneData->prefabs[i].boxCollider->size.x,
					sceneData->prefabs[i].boxCollider->size.y,
					sceneData->prefabs[i].boxCollider->size.z });
			}

			if (sceneData->prefabs[i].sphereCollider != nullptr)
			{
				/*gameObject->AddComponent<SphereCollider>();

				gameObject->GetComponent<SphereCollider>()->SetRadius(sceneData->gameObjects[i].sphereCollider->radius);

				gameObject->GetComponent<SphereCollider>()->SetLocalCenter(Vector3{
				sceneData->gameObjects[i].sphereCollider->center.x,
				sceneData->gameObjects[i].sphereCollider->center.y,
				sceneData->gameObjects[i].sphereCollider->center.z });*/
			}

			string delimiter = " ";
			string token;

			if (sceneData->prefabs[i].name.find(delimiter) != string::npos)
			{
				token = sceneData->prefabs[i].name.substr(0, sceneData->prefabs[i].name.find(delimiter));
			}
			else
			{
				token = sceneData->prefabs[i].name.substr(0, sceneData->prefabs[i].name.length());
			}

			gameObject->AddComponent<MeshRenderer>()->LoadPrefab(token + ".model");

			/*if (token == "RedStatue" || token == "Room1Door" || token == "Room4Door" || token == "Room4Hole")
			{
				gameObject->GetComponent<MeshRenderer>()->DeleteMaterial("black_Shadow");
			}*/

			if (token == "lobby" || token == "Room1" || token == "Room2" || token == "Room3" || token == "Room4"
				|| token == "LeftPassage1" || token == "LeftPassage2" || token == "StraightPassage"
				|| token == "Room4Door" || token == "Elevator" || token == "Elevator2")
			{
				//gameObject->GetComponent<MeshRenderer>()->LightOff();
					// �׸��� ����� �Ѱܺ��ô�.
				GraphicsManager::GetInstance()->PassAmbientSkyColor({ sceneData->ambientSkyColor.x,
																	 sceneData->ambientSkyColor.y,
																	 sceneData->ambientSkyColor.z,
																	 0.2f});


				GraphicsManager::GetInstance()->PassIndoor(true);
			}
			else
			{
				// �׸��� ����� �Ѱܺ��ô�.
				GraphicsManager::GetInstance()->PassAmbientSkyColor({ sceneData->ambientSkyColor.x,
																	 sceneData->ambientSkyColor.y,
																	 sceneData->ambientSkyColor.z,
																	 sceneData->ambientSkyColor.w });

				GraphicsManager::GetInstance()->PassIndoor(false);

			}
		}
	}

	std::shared_ptr<GameObject> SceneManager::FindObject(std::string uid)
	{
		int _uid = std::stoi(uid);
		auto objectVector = GetCurrentScene()->GetGameObjectInScene();
		for (int i = 0; i < objectVector.size(); i++)
		{
			if (objectVector[i]->GetObjectID() == _uid)
				return objectVector[i];
		}

		return nullptr;
	}

	std::shared_ptr<GameEngineSpace::GameObject> SceneManager::FindObjectName(tstring name)
	{
		auto objectVector = GetCurrentScene()->GetGameObjectInScene();
		for (int i = 0; i < objectVector.size(); i++)
		{
			if (objectVector[i]->GetName() == name)
				return objectVector[i];
		}

		return nullptr;
	}

	void SceneManager::ChangeScene()
	{
		if (currentScene != nullptr)
		{
			prevSceneName = currentScene->GetSceneName();
			// ���� Scene�� ������ �����ش�..
			currentScene->ClearScene();
			std::string name;
			tstring tname = currentScene->sceneName;
			name.assign(tname.begin(), tname.end());
			PhysicsManager::GetInstance()->DeleteScene(name);
			PhysicsManager::GetInstance()->CollidersClear();
		}

		// ���⼭ ����Ʈ ����Ʈ�� ���� �۾� => Reset Data
		GraphicsManager::GetInstance()->GetRenderer()->ResetShadowPass();
		isBakedShadow = false;

		// Navmesh reset
		NavigationManager::GetInstance()->Release();

		// ObjectPool reset
		ObjectPool::GetInstance()->Release();

		// Coroutine rest
		CoroutinManager::GetInstance()->Release();

		currentScene = scenes.at(reservedLoadSceneName);
		std::string name;
		tstring tname = currentScene->sceneName;
		name.assign(tname.begin(), tname.end());

		reservedLoadScene = false;

		isChangeScene = true;

		// �� �ʱ�ȭ.
		PhysicsManager::GetInstance()->AddScene(name);
		PhysicsManager::GetInstance()->SetCurrentScene(name);
		currentScene->BuildScene();
		// �� �� ������Ʈ�� init ������
		//GraphicsManager::GetInstance()->GetRenderer()->InitObject();
		currentScene->Awake();
		currentScene->Start();

		//// ��ŸƮ�� ������ ���ݽô�.
		//auto sceneCanvas = UIManager::GetInstance()->GetMainCanvas()->GetCanvasUI("SceneCanvas");
		//sceneCanvas->GetSpriteUI("LoadingSpriteUI")->SetEnable(false);
		//sceneCanvas->GetSpriteUI("LoadingAnim")->SetEnable(false);
		//sceneCanvas->SetEnable(false);
	}

	std::shared_ptr<SceneManager> SceneManager::GetInstance()
	{
		// �ν��Ͻ��� ������ ������ְ� ������ �ν��Ͻ� ����
		if (instance == nullptr)
		{
			instance = std::make_shared<SceneManager>();
		}

		return instance;
	}

	void SceneManager::Init()
	{
		printf("SceneManager Init Clear\n");

		// ���Ŀ� ������ ������ ���⼭ ������ Init���ش�.
		physicsManager = PhysicsManager::GetInstance();

		physicsManager->Init();

		printf("PhysicsManager Init Clear\n");

	}

	void SceneManager::Release()
	{
		currentScene.reset();

		physicsManager->Release();

		// �� ��ü ������
		for (auto& scene : scenes)
		{
			scene.second->ClearScene();
		}
	}

	void SceneManager::Update()
	{
		auto mousePos = InputManager::GetInstance()->GetMousePos();
		auto scale = UIManager::GetInstance()->GetCanvas("CursorCanvas")->GetScreenScale();

		UIManager::GetInstance()->GetCanvas("CursorCanvas")->GetSpriteUI("Info")->SetPosition({ mousePos.x + (27.5f * scale.x), mousePos.y + (30.f * scale.y), 0.0002f });
		UIManager::GetInstance()->GetCanvas("CursorCanvas")->GetSpriteUI("Cursor")->SetPosition({ mousePos.x, mousePos.y, 0.0001f });

		if (LoadingManager::isLoadingEnd)
		{
			// ������ �ξ��� LoadData �����带 ������.
			LoadingManager::GetInstance()->StopLoadDataThread();

			// �ε��̹����� ����.
			auto sceneCanvas = UIManager::GetInstance()->GetMainCanvas()->GetCanvasUI("SceneCanvas");
			sceneCanvas->GetSpriteUI("LoadingSpriteUI")->SetEnable(false);
			sceneCanvas->GetSpriteUI("LoadingAnim")->SetEnable(false);
			sceneCanvas->SetEnable(false);

			// �ٽ� false�� �ٲ۴�.
			LoadingManager::isLoadingEnd = false;
		}

		// ���� ���� �ִٸ�..
		if (currentScene != nullptr)
		{
			// ����� TimeScale�� �ִٸ� �ҷ��ɴϴ�.
			ReservedTimeScale();

			// ���� ����� ������Ʈ�� ������ ����
			currentScene->ReservedInstantiateGameObject();

			// �ı� ����� ������Ʈ�� ������ �ı�
			currentScene->ReservedDestroyGameObject();

			float deltaTime = Timer::GetInstance()->DeltaTime();

			deltaTime *= timeScale;

			// deltaTime�� 0���� �۾����� ��� ����
			if (deltaTime < 0.0f)
				deltaTime = 0.0f;

			// tick�� Ƣ�°� ����
			if (isChangeScene == true)
			{
				isChangeScene = false;
				deltaTime = 0.0166f;
			}

			// ���� Simulate�� ���� ������ tick �ֱ��� FixedUpdate
			// FixedUpdate�� PhysicsManager->Simulate ���ʿ� �־��־���.
			physicsManager->Simulate(deltaTime, currentScene);

			// ������� ������Ʈ ���ش�.
			currentScene->Update(deltaTime);

			CoroutinManager::GetInstance()->Update(deltaTime);

			UIManager::GetInstance()->Update();

			currentScene->LateUpdate(deltaTime);

			if (isBakedShadow != true)
			{
				currentScene->BakeShadow(deltaTime);

				isBakedShadow = true;
			}

			// EyePos ����
			auto mainCam = currentScene->GetMainCamera();
			if (mainCam != nullptr)
			{
				auto mainCamPos = mainCam->GetGameObject()->GetComponent<Transform>()->GetWorldPosition();
				GraphicsManager::GetInstance()->GetRenderer()->PassEyePos(mainCamPos);
			}

			// ���� ���� => ������ �̵�
			currentScene->Render(deltaTime);

			if (InputManager::GetInstance()->GetInputState(VK_F1, KeyState::DOWN))
			{
				bool selectMode = UIManager::GetInstance()->GetMainCanvas()->GetSelectMode();
				UIManager::GetInstance()->GetMainCanvas()->SetSelectMode(!selectMode);
			}

			// �����
			if (InputManager::GetInstance()->GetInputState(VK_TAB, KeyState::DOWN))
			{
				bool debug = currentScene->GetDebugRender();
				currentScene->SetDebugRender(!debug);
			}

			// �׸��� OnEnable..
			currentScene->OnEnable();
		}

		// �� ������ �����Ǿ� ������ �ٲ��ش�.
		if (reservedLoadScene)
		{
			ChangeScene();
		}

		// �׷��� ��� ���콺 Ŀ���� ����ݴϴ�.
	}

	void SceneManager::ReservedTimeScale()
	{
		if (!timeScaleList.empty())
		{
			timeScale = timeScaleList.front();

			timeScaleList.pop();
		}
	}

	void SceneManager::AddGameScene(std::shared_ptr<SceneBase> _scene)
	{
		if (_scene != nullptr)
		{
			scenes.insert({ _scene->GetSceneName(), _scene });
		}
	}

	void SceneManager::LoadScene(tstring _sceneName)
	{
		// ���� Map�� �����ϴ°�.
		assert(scenes.find(_sceneName) != scenes.end());

		// ���� �����Ѵ�.
		reservedLoadScene = true;
		reservedLoadSceneName = _sceneName;
	}

	void SceneManager::UpdateMainCameraAspectRatio(uint32 _screenWidth, uint32 _screenHeight)
	{
		if (currentScene != nullptr)
		{
			std::shared_ptr<Camera> _mainCamera = currentScene->GetMainCamera();

			if (_mainCamera != nullptr)
			{
				currentScene->GetMainCamera()->SetAspectRatio(static_cast<float>(_screenWidth) / _screenHeight);
			}
		}
	}

	void SceneManager::ChangeTimeScale(float val)
	{
		timeScaleList.push(val);
	}

	void SceneManager::SetTimeScaleOrigin()
	{
		while (!timeScaleList.empty())
		{
			timeScaleList.pop();
		}

		timeScaleList.push(1.f);
	}
}
