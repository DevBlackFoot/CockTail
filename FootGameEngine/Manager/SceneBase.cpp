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
		// PointLight 수만큼 돌려봅시다..
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
		// 각 오브젝트들의 렌더전 작업
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
		// PointLight 수만큼 돌려봅시다..
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

		// 배경 렌더 해봅시다..
			// 일단은 그려보고, 나중에는 현재씬이 어떤씬인지 파악해서 넣는걸로..

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
			// 그려진 다음 Collider Render
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

				// 만약에 플레이여면.. 마스킹을 켜줍니다.
				/*if(objName == "Player")
					renderer->SetSilhouetteMaskDSState();*/

				renderObj->Render(renderer);

				// 렌더 끝난 뒤 원상 복귀 시켜줍니다.
				/*if (objName == "Player")
					renderer->SetDefaultDSState();*/

				renderer->GraphicsDebugEndEvent();
			}
		}
		renderer->GraphicsDebugEndEvent();

		// 해당 부분에서 depth state 변경
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

		// 여기서 파티클 렌더 시작..
		renderer->BeginParticleRender();

		// 틱이 0보다 클때만..
		if (tick > 0.f)
		{
		// 오브젝트를 돌면서 파티클들을 그려봅니다.
			// 파티클은 여기서 prepare..
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

		// 최종적으로 post Processing pass를 진행합니다.
		renderer->PostProcessing();

		// DebugRender(Deferred 확인용 + 내부적으로 ImGUI 렌더)
		DebugRender(tick);

		//UIManager::GetInstance()->SelectGUIRender();
		// Scene이 다 그려지고, UIManager 렌더
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
		// 내부적으로 지워준다.
		// 순서대로 하나씩 리셋 후 마지막으로 릴리즈
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

		// UI 지워주기
		//UIManager::GetInstance()->ClearUI();
	}

	void SceneBase::AddGameObject(std::shared_ptr<GameObject> newGameObj)
	{
		gameObjectInScene.push_back(newGameObj);

		// 자식들이 있으면 자식들도 넣어준다.
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

			// ColliderObjInScene에서 Object ID가 같은 녀석을 찾자
			auto colliderIt = find_if(colliderObjInScene.begin(), colliderObjInScene.end(), [&id = objectID](const std::shared_ptr<Collider> c)
			{ return (c->GetGameObject()->GetObjectID() == id); });

			// 있다면 PhysX에서 제거하고 여기서도 제거해준다. 
			if (colliderIt != colliderObjInScene.end())
			{
				PhysicsManager::GetInstance()->DeleteObject((*colliderIt).get());

				colliderObjInScene.erase(colliderIt);
			}

			// RenderObjInScene에서 Object ID가 같은 녀석을 찾자
			auto renderIt = find_if(renderObjInScene.begin(), renderObjInScene.end(), [&id = objectID](const std::shared_ptr<RendererBase>& r)
			{ return (r->GetGameObject()->GetObjectID() == id); });

			// 있다면 제거 
			if (renderIt != renderObjInScene.end())
			{
				renderObjInScene.erase(renderIt);
			}

			auto particleIt = find_if(particleInScene.begin(), particleInScene.end(), [&id = objectID](const std::shared_ptr<RendererBase>& r)
			{ return (r->GetGameObject()->GetObjectID() == id); });

			// 있다면 제거 
			if (particleIt != particleInScene.end())
			{
				particleInScene.erase(particleIt);
			}

			// 게임 오브젝트를 찾아서 삭제
			auto it = std::find(gameObjectInScene.begin(), gameObjectInScene.end(), destroyGameObjectList.front());

			if (it != gameObjectInScene.end())
			{
				(*it)->Release();

				gameObjectInScene.erase(it);
			}

			// 부모-자식 관계를 유지하면서 자식 오브젝트들도 삭제
			/*std::vector<std::shared_ptr<GameObject>> children = destroyGameObjectList.front()->GetChildren();

			for (auto& child : children)
			{
				DestroyGameObject(child);
			}*/

			// 파괴된 게임 오브젝트의 리소스를 모두 해제
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

			// Debug 출력 임시로 FPS 측정
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

				// IMGUI 렌더
				for (auto& gameObj : gameObjectInScene)
				{
					gameObj->DebugIMGUIRender(tick);
				}
			}
			imgui->End();

			/*
			// 입력 받을 곳의 Text
			static char msgBox[128] = "";
			// 해당 텍스트를 가져서 지니고 있을 string
			static std::string outputText = "None";
			if (imgui->Begin("Test Text Input Area"))
			{
				// 제목은 없다 가정하고.. 다음과 같은 형태로 해줍니다.
					// 인풋이 들어오면 msgBox의 내용이 변합니다.
					// 중간의 hint는 아무것도 입력하지 않았을 때 나오는 텍스트입니다.
				imgui->InputText(" ", "input Text", msgBox, sizeof(msgBox));
				// 버튼과 Input 영역을 같은줄에 둡니다.
				imgui->SameLine();

				// 버튼을 누르면 => if문 안의 내용을 실행합니다.
				if (imgui->Button("Input"))
				{
					// msgBox내의 내용을 outputText로 옮깁니다.
					outputText.assign(msgBox, 128);
					// 옮김과 동시에 input text 영역을 지워줍니다.
					memset(&msgBox[0], 0, sizeof(msgBox));
				}
			}
			imgui->End();

			// 잘 들어왔는지 확인해주는 용도의 imgui 윈도우입니다. => 없어도 됨..!
			if (imgui->Begin("Test Text Output Area"))
			{
				// output Text를 그냥 띄워줍니다.
				imgui->Text(outputText);
			}
			imgui->End();*/

			// navmesh가 있는 scene 이라면 그려준다.
			//NavigationManager::GetInstance()->DebugRenderNavMesh();

			UIManager::GetInstance()->SelectGUIRender();
		}
	}

	std::shared_ptr<GameObject> SceneBase::CreateEmpty()
	{
		// 빈게임 오브젝트를 만들고.. 트랜스폼을 붙인 뒤 넣어준다.
		std::shared_ptr<GameObject> newGameObj = std::make_shared<GameObject>();

		newGameObj->AddComponent<Transform>();

		AddGameObject(newGameObj);

		// 반환.
		return newGameObj;
	}

	void SceneBase::AddRenderer(std::shared_ptr<RendererBase> renderObj)
	{
		// 지금 이 부분이, 그래픽스에도 존재는 함.
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
