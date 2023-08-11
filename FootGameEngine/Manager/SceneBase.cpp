#include "GamePch.h"
#include "SceneBase.h"

#include "Object/GameObject.h"
#include "InputManager.h"
#include "GraphicsManager.h"
#include "Timer.h"
#include "UIManager.h"

#include "Component/RendererBase.h"
#include "Component/Light.h"
#include "Component/Collider.h"
#include "Component/Camera.h"
#include "NavigationManager.h"
#include "PhysicsManager.h"
#include "SceneManager.h"

namespace GameEngineSpace
{
#ifdef _DEBUG
	bool SceneBase::debugRender = true;
#else
	bool SceneBase::debugRender = false;
#endif

	SceneBase::SceneBase(tstring sceneName) :
		sceneName(sceneName)
		, mainCamera(nullptr)
		, gameObjectInScene(std::vector<std::shared_ptr<GameObject>>())
		, renderObjInScene(std::vector<std::shared_ptr<RendererBase>>())
		, colliderObjInScene(std::vector<std::shared_ptr<Collider>>())
		, mainLight(nullptr)
		, pointLightInScene(std::vector<std::shared_ptr<Light>>())
		, spotLightInScene(std::vector<std::shared_ptr<Light>>())
		, isInside(true)
		, insideSceneMaskColor(Vector4{ powf((116.f / 255.f), 2.2f), powf((115.f / 255.f), 2.2f), powf((115.f / 255.f), 2.2f), 1.0f })
		, rimLightColor(Vector4{ 232.f / 255.f, 234.f / 255.f, 241.f / 255.f, 1.f })
	{

	}

	SceneBase::~SceneBase()
	{
	}

	void SceneBase::Awake()
	{
		for (auto& gameObj : gameObjectInScene)
		{
			gameObj->Awake();
		}
	}

	void SceneBase::Start()
	{
		for (auto& gameObj : gameObjectInScene)
		{
			gameObj->Start();
		}
	}

	void SceneBase::FixedUpdate(float tick)
	{
		for (auto& gameObj : gameObjectInScene)
		{
			gameObj->FixedUpdate(tick);
		}
	}

	void SceneBase::Update(float tick)
	{
		for (auto& gameObj : gameObjectInScene)
		{
			gameObj->Update(tick);
		}
	}

	void SceneBase::LateUpdate(float tick)
	{
		for (auto& gameObj : gameObjectInScene)
		{
			gameObj->LateUpdate(tick);
		}
	}

	void SceneBase::BakeShadow(float tick)
	{
		for (auto& renderObj : renderObjInScene)
		{
			if (renderObj->GetIsEnable() == true)
			{
				renderObj->PrepareRender(tick);
			}
		}

		auto renderer = GraphicsManager::GetInstance()->GetRenderer();

		// ShadowRender
		// PointLight ����ŭ �������ô�..
		for (int i = 0; i < pointLightInScene.size(); i++)
		{
			if (pointLightInScene[i]->GetIsShadow() != true)
				continue;

			renderer->ShadowBakeStart(i, true);
			for (auto& renderObj : renderObjInScene)
			{
				if (renderObj->GetIsEnable() == true)
				{
					renderer->ShadowBake(renderObj->GetRenderObj(), true);
				}
			}
			renderer->ShadowRenderEnd();
		}

		// SpotLight Shadow
		for (int i = 0; i < spotLightInScene.size(); i++)
		{
			if (spotLightInScene[i]->GetIsShadow() != true)
				continue;

			renderer->ShadowBakeStart(i, false);
			for (auto& renderObj : renderObjInScene)
			{
				if (renderObj->GetIsEnable() == true)
				{
					renderer->ShadowBake(renderObj->GetRenderObj(), false);
				}
			}
			renderer->ShadowRenderEnd();
		}

		// DirLight Shadow
		if (mainLight != nullptr)
		{
			renderer->ShadowRenderStart();
			for (auto& renderObj : renderObjInScene)
			{
				if (renderObj->GetIsEnable() == true)
				{
					renderer->ShadowBake(renderObj->GetRenderObj(), false);
				}
			}
			renderer->ShadowRenderEnd();
		}
	}

	void SceneBase::Render(float tick)
	{
		// �� ������Ʈ���� ������ �۾�
		for (auto& renderObj : renderObjInScene)
		{
			if (renderObj->GetIsEnable() == true)
			{
				renderObj->PrepareRender(tick);
				//renderObj->SetRimLightColor(rimLightColor);
			}
		}

		auto renderer = GraphicsManager::GetInstance()->GetRenderer();

		// ShadowRender
		// PointLight ����ŭ �������ô�..
		for (int i = 0; i < pointLightInScene.size(); i++)
		{
			if (pointLightInScene[i]->GetIsShadow() != true)
				continue;

			renderer->ShadowRenderStart(i, true);
			for (auto& renderObj : renderObjInScene)
			{
				if (renderObj->GetIsEnable() == true)
				{
					renderer->ShadowRender(renderObj->GetRenderObj(), true);
				}
			}
			renderer->ShadowRenderEnd();
		}

		// SpotLight Shadow
		for (int i = 0; i < spotLightInScene.size(); i++)
		{
			if (spotLightInScene[i]->GetIsShadow() != true)
				continue;

			renderer->ShadowRenderStart(i, false);
			for (auto& renderObj : renderObjInScene)
			{
				if (renderObj->GetIsEnable() == true)
				{
					renderer->ShadowRender(renderObj->GetRenderObj(), false);
				}
			}
			renderer->ShadowRenderEnd();
		}

		renderer->GraphicsDebugBeginEvent("BeginRender");
		renderer->BeginRender();
		renderer->GraphicsDebugEndEvent();

		// ��� ���� �غ��ô�..
			// �ϴ��� �׷�����, ���߿��� ������� ������� �ľ��ؼ� �ִ°ɷ�..

		/*
		if (mainCamera != nullptr)
			mainCamera->Render();
		*/

		auto sceneCanvas = UIManager::GetInstance()->GetMainCanvas()->GetCanvasUI("SceneCanvas");
		sceneCanvas->SetEnable(true);
		sceneCanvas->GetSpriteUI("SceneBackGroundSpriteUI")->SetEnable(true);
		sceneCanvas->GetSpriteUI("SceneBackGroundSpriteUI")->SetMaskColor(insideSceneMaskColor);
		sceneCanvas->Render(renderer, tick);
		sceneCanvas->SetEnable(false);

		if (debugRender == true)
		{
			// �׷��� ���� Collider Render
			renderer->GraphicsDebugBeginEvent("ColliderRender");
			for (auto& collider : colliderObjInScene)
			{
				if (collider->GetIsEnable() == true)
					collider->ColliderRender();
			}
			renderer->GraphicsDebugEndEvent();
		}

		// Object Render
		renderer->GraphicsDebugBeginEvent("ObjectRender");
		for (auto& renderObj : renderObjInScene)
		{
			if (renderObj->GetIsEnable() == true)
			{
				std::string objName;
				objName.assign(renderObj->GetGameObject()->GetName().begin(), renderObj->GetGameObject()->GetName().end());
				renderer->GraphicsDebugBeginEvent(objName);

				// ���࿡ �÷��̿���.. ����ŷ�� ���ݴϴ�.
				/*if(objName == "Player")
					renderer->SetSilhouetteMaskDSState();*/

				renderObj->Render(renderer);

				// ���� ���� �� ���� ���� �����ݴϴ�.
				/*if (objName == "Player")
					renderer->SetDefaultDSState();*/

				renderer->GraphicsDebugEndEvent();
			}
		}
		renderer->GraphicsDebugEndEvent();

		// �ش� �κп��� depth state ����
		renderer->BeginTransparencyRender();

		renderer->GraphicsDebugBeginEvent("TransparencyMeshRender");
		for (auto& renderObj : renderObjInScene)
		{
			if (renderObj->GetIsEnable() == true)
			{
				std::string objName;
				objName.assign(renderObj->GetGameObject()->GetName().begin(), renderObj->GetGameObject()->GetName().end());
				renderer->GraphicsDebugBeginEvent(objName);
				renderObj->YieldRender(renderer);
				renderer->GraphicsDebugEndEvent();
			}
		}
		renderer->GraphicsDebugEndEvent();

		// LightPass
		renderer->Render();

		// ���⼭ ��ƼŬ ���� ����..
		renderer->BeginParticleRender();

		// ƽ�� 0���� Ŭ����..
		if (tick > 0.f)
		{
		// ������Ʈ�� ���鼭 ��ƼŬ���� �׷����ϴ�.
			// ��ƼŬ�� ���⼭ prepare..
			renderer->GraphicsDebugBeginEvent("ParticleRender");
			for (auto& particle : particleInScene)
			{
				if (particle->GetIsEnable() == true)
				{
					particle->SetTotalTime(Timer::GetInstance()->TotalTime());
					particle->PrepareRender(tick);
					particle->Render(renderer);
				}
			}
			renderer->GraphicsDebugEndEvent();
			renderer->GraphicsDebugBeginEvent("ParticleMerge");
		}
		renderer->OITParticleMerge();
		renderer->GraphicsDebugEndEvent();

		// ���������� post Processing pass�� �����մϴ�.
		renderer->PostProcessing();

		// DebugRender(Deferred Ȯ�ο� + ���������� ImGUI ����)
		DebugRender(tick);

		//UIManager::GetInstance()->SelectGUIRender();
		// Scene�� �� �׷�����, UIManager ����
		UIManager::GetInstance()->Render(tick);

		renderer->EndRender();
	}

	void SceneBase::OnEnable()
	{
		for (auto& gameObj : gameObjectInScene)
		{
			gameObj->OnEnable();
		}
	}

	void SceneBase::ClearScene()
	{
		// ���������� �����ش�.
		// ������� �ϳ��� ���� �� ���������� ������
		for (auto& gameObj : gameObjectInScene)
		{
			gameObj->Release();
		}

		gameObjectInScene.clear();
		renderObjInScene.clear();
		particleInScene.clear();

		colliderObjInScene.clear();

		pointLightInScene.clear();
		spotLightInScene.clear();

		mainLight = nullptr;

		// UI �����ֱ�
		//UIManager::GetInstance()->ClearUI();
	}

	void SceneBase::AddGameObject(std::shared_ptr<GameObject> newGameObj)
	{
		gameObjectInScene.push_back(newGameObj);

		// �ڽĵ��� ������ �ڽĵ鵵 �־��ش�.
		const std::vector<std::shared_ptr<GameObject>> children = newGameObj->GetChildren();

		for (auto child : children)
		{
			this->AddGameObject(child);
		}
	}

	void SceneBase::InstantiateGameObject(std::shared_ptr<GameObject> _newGameObj)
	{
		instantiateGameObjectList.push(_newGameObj);
	}

	void SceneBase::ReservedInstantiateGameObject()
	{
		if (!instantiateGameObjectList.empty())
		{
			AddGameObject(instantiateGameObjectList.front());

			instantiateGameObjectList.front()->Awake();

			instantiateGameObjectList.front()->Start();

			instantiateGameObjectList.pop();
		}
	}

	void SceneBase::DestroyGameObject(std::shared_ptr<GameObject> _gameObj)
	{
		destroyGameObjectList.push(_gameObj);

		std::vector<std::shared_ptr<GameObject>> children = _gameObj->GetChildren();

		for (auto& child : children)
		{
			DestroyGameObject(child);
		}
	}

	void SceneBase::ReservedDestroyGameObject()
	{
		while (!destroyGameObjectList.empty())
		{
			auto objectID = destroyGameObjectList.front()->GetObjectID();

			// ColliderObjInScene���� Object ID�� ���� �༮�� ã��
			auto colliderIt = find_if(colliderObjInScene.begin(), colliderObjInScene.end(), [&id = objectID](const std::shared_ptr<Collider> c)
			{ return (c->GetGameObject()->GetObjectID() == id); });

			// �ִٸ� PhysX���� �����ϰ� ���⼭�� �������ش�. 
			if (colliderIt != colliderObjInScene.end())
			{
				PhysicsManager::GetInstance()->DeleteObject((*colliderIt).get());

				colliderObjInScene.erase(colliderIt);
			}

			// RenderObjInScene���� Object ID�� ���� �༮�� ã��
			auto renderIt = find_if(renderObjInScene.begin(), renderObjInScene.end(), [&id = objectID](const std::shared_ptr<RendererBase>& r)
			{ return (r->GetGameObject()->GetObjectID() == id); });

			// �ִٸ� ���� 
			if (renderIt != renderObjInScene.end())
			{
				renderObjInScene.erase(renderIt);
			}

			auto particleIt = find_if(particleInScene.begin(), particleInScene.end(), [&id = objectID](const std::shared_ptr<RendererBase>& r)
			{ return (r->GetGameObject()->GetObjectID() == id); });

			// �ִٸ� ���� 
			if (particleIt != particleInScene.end())
			{
				particleInScene.erase(particleIt);
			}

			// ���� ������Ʈ�� ã�Ƽ� ����
			auto it = std::find(gameObjectInScene.begin(), gameObjectInScene.end(), destroyGameObjectList.front());

			if (it != gameObjectInScene.end())
			{
				(*it)->Release();

				gameObjectInScene.erase(it);
			}

			// �θ�-�ڽ� ���踦 �����ϸ鼭 �ڽ� ������Ʈ�鵵 ����
			/*std::vector<std::shared_ptr<GameObject>> children = destroyGameObjectList.front()->GetChildren();

			for (auto& child : children)
			{
				DestroyGameObject(child);
			}*/

			// �ı��� ���� ������Ʈ�� ���ҽ��� ��� ����
			destroyGameObjectList.front()->OnDestroy();
			destroyGameObjectList.front().reset();

			destroyGameObjectList.pop();
		}
	}

	void SceneBase::DebugRender(float tick)
	{
		auto renderer = GraphicsManager::GetInstance()->GetRenderer();

		// Debug Render
		if (debugRender == true)
		{
			renderer->DebugRender();

			// Debug ��� �ӽ÷� FPS ����
			static float frameTime = 0.0f;
			static int fps = 0;
			static int frameCount = 0;

			frameTime += tick;
			frameCount++;

			if (frameTime >= 1.0f)
			{
				fps = frameCount;
				frameCount = 0;
				frameTime = 0.0f;
			}

			auto imgui = ImGUIManager::GetInstance();

			if (imgui->Begin("Main Debug Menu"))
			{
				if (imgui->CollapsingHeader("FPS"))
				{
					imgui->Text("FPS : " + std::to_string(fps));
					imgui->Text("DeltaTime : " + std::to_string(tick));
				}
				imgui->Spacing();

				imgui->CollapsingHeader("Light In Scene");
				imgui->Spacing();

				imgui->CollapsingHeader("Sprite Effect Info");
				imgui->Spacing();

				/*if (imgui->CollapsingHeader("Cursor Info"))
				{
					auto hcursor = ::GetCursor();

					imgui->Text("Now Cursor : " + std::to_string((int)(hcursor)));
				}
				imgui->Spacing();*/

				// IMGUI ����
				for (auto& gameObj : gameObjectInScene)
				{
					gameObj->DebugIMGUIRender(tick);
				}
			}
			imgui->End();

			/*
			// �Է� ���� ���� Text
			static char msgBox[128] = "";
			// �ش� �ؽ�Ʈ�� ������ ���ϰ� ���� string
			static std::string outputText = "None";
			if (imgui->Begin("Test Text Input Area"))
			{
				// ������ ���� �����ϰ�.. ������ ���� ���·� ���ݴϴ�.
					// ��ǲ�� ������ msgBox�� ������ ���մϴ�.
					// �߰��� hint�� �ƹ��͵� �Է����� �ʾ��� �� ������ �ؽ�Ʈ�Դϴ�.
				imgui->InputText(" ", "input Text", msgBox, sizeof(msgBox));
				// ��ư�� Input ������ �����ٿ� �Ӵϴ�.
				imgui->SameLine();

				// ��ư�� ������ => if�� ���� ������ �����մϴ�.
				if (imgui->Button("Input"))
				{
					// msgBox���� ������ outputText�� �ű�ϴ�.
					outputText.assign(msgBox, 128);
					// �ű�� ���ÿ� input text ������ �����ݴϴ�.
					memset(&msgBox[0], 0, sizeof(msgBox));
				}
			}
			imgui->End();

			// �� ���Դ��� Ȯ�����ִ� �뵵�� imgui �������Դϴ�. => ��� ��..!
			if (imgui->Begin("Test Text Output Area"))
			{
				// output Text�� �׳� ����ݴϴ�.
				imgui->Text(outputText);
			}
			imgui->End();*/

			// navmesh�� �ִ� scene �̶�� �׷��ش�.
			//NavigationManager::GetInstance()->DebugRenderNavMesh();

			UIManager::GetInstance()->SelectGUIRender();
		}
	}

	std::shared_ptr<GameObject> SceneBase::CreateEmpty()
	{
		// ����� ������Ʈ�� �����.. Ʈ�������� ���� �� �־��ش�.
		std::shared_ptr<GameObject> newGameObj = std::make_shared<GameObject>();

		newGameObj->AddComponent<Transform>();

		AddGameObject(newGameObj);

		// ��ȯ.
		return newGameObj;
	}

	void SceneBase::AddRenderer(std::shared_ptr<RendererBase> renderObj)
	{
		// ���� �� �κ���, �׷��Ƚ����� ����� ��.
		renderObjInScene.push_back(renderObj);
	}

	void SceneBase::AddParticle(std::shared_ptr<RendererBase> particleObj)
	{
		particleInScene.push_back(particleObj);
	}

	void SceneBase::AddCollider(std::shared_ptr<Collider> colliderObj)
	{
		colliderObjInScene.push_back(colliderObj);
	}

	void SceneBase::AddPointLight(std::shared_ptr<Light> pLight)
	{
		pointLightInScene.push_back(pLight);
	}

	void SceneBase::AddSpotLight(std::shared_ptr<Light> sLight)
	{
		spotLightInScene.push_back(sLight);
	}
}
