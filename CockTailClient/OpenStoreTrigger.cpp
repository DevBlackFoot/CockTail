#include "pch.h"
#include "OpenStoreTrigger.h"
#include "UIManager.h"
#include "GameManager.h"

namespace ClientSpace
{
	OpenStoreTrigger::OpenStoreTrigger(std::weak_ptr<GameObject> _obj)
		: InteractionTrigger(_obj)
		, onCanvas(false)
		, onPopup(false)
		, rim(SimpleMath::Vector4{ 2.404f, 1.f, 0.32f, 0.5f })
		, alpha(0.0f)
		, dissolveSpeed(2.0f)
	{
		isPossible = false;
	}

	OpenStoreTrigger::~OpenStoreTrigger()
	{
	}

	void OpenStoreTrigger::Awake()
	{
		InteractionTrigger::Awake();
		storeCanvas = UIManager::GetInstance()->GetCanvas("StoreCanvas");
		popupCanvas = UIManager::GetInstance()->GetCanvas("PopUpCanvas");
		auto cockObj = SceneManager::GetInstance()->FindObjectName(L"Cock");
		if (cockObj != nullptr)
			cockRenderer = cockObj->GetComponent<MeshRenderer>()->GetRenderObj();

		cockRenderer->SetMaterialEmissiveColor("SilhouetteMaterial", rim);

		gameManager = GameManager::GetInstance();

		// 처음 죽었으면 그 때 켜줍니다.
		if (gameManager->isPlayerFirstDead == false)
		{
			isPossible = true;
			cockRenderer->SetMaterialColor("SilhouetteMaterial", { 0.5094f, 0.3431f, 0.1173f, 0.8f });
		}
		else
		{
			cockRenderer->SetMaterialColor("SilhouetteMaterial", { 0.5094f, 0.3431f, 0.1173f, 0.0f });
		}

		if (gameManager->isFirstCraft == false)
		{
			if (gameManager->GetCraftDone() == true)
			{
				// 오늘의 일을 했으면..!
					// 가게 문을 닫아줍니다.
				popupCanvas->GetButtonUI("SelectButton0")->SetEnable(false);
				popupCanvas->GetButtonUI("SelectButton1")->SetEnable(true);
				popupCanvas->GetSpriteUI("PopUp")->SetTexture(ResourceManager::GetInstance()->LoadPrefabTexture(L"PopUpClose"));
			}
			else
			{
				popupCanvas->GetButtonUI("SelectButton0")->SetEnable(true);
				popupCanvas->GetButtonUI("SelectButton1")->SetEnable(true);
				popupCanvas->GetSpriteUI("PopUp")->SetTexture(ResourceManager::GetInstance()->LoadPrefabTexture(L"PopUp"));
			}
		}

		if (SceneManager::GetInstance()->GetPrevSceneName() == L"BarOperationScene" && gameManager->isStoreCutSceneEnd == true)
		{
			isInteracted = true;
		}

		SetButtonEvent();
	}

	void OpenStoreTrigger::Update(float tick)
	{
		if (isPossible == false)
			return;

		InputUpdate();

		/*
		*/
		if (IsSelected())
		{
			alpha = 0.8f;
			cockRenderer->SetMaterialColor("SilhouetteMaterial", { 0.5094f, 0.3431f, 0.1173f, alpha });
			cockRenderer->SetDissolveSpeed(dissolveSpeed);
		}
		else
		{
			alpha -= tick * dissolveSpeed;
			if (alpha < 0.f)
				alpha = 0.f;
			cockRenderer->SetMaterialColor("SilhouetteMaterial", { 0.5094f, 0.3431f, 0.1173f, alpha });
			cockRenderer->SetDissolveSpeed(-dissolveSpeed);
		}

		if (IsInteracted())
		{
			// f를 누를 경우 IsInteracted
			if (onPopup != true && onCanvas == false)
			{
				onPopup = true;

				popupCanvas->SetEnable(onPopup);
				SceneManager::GetInstance()->ChangeTimeScale(0.f);
				mainCanvas->GetCanvasUI("InteractionCanvas")->GetSpriteUI("F")->SetEnable(false);
			}

			if (onCanvas == true)
			{
				// 창이 켜져 있는 상태에서
					// esc를 누르면
				if (input->GetInputState('F', KeyState::DOWN))
				{
					onCanvas = false;
					storeCanvas->SetEnable(onCanvas);

					onPopup = true;
					popupCanvas->SetEnable(onPopup);

					//SceneManager::GetInstance()->SetTimeScaleOrigin();
					//mainCanvas->GetCanvasUI("InteractionCanvas")->GetSpriteUI("F")->SetEnable(true);

					//isInteracted = false;
				}
			}
		}

	}

	void OpenStoreTrigger::LateUpdate(float tick)
	{
		if (isPossible == true)
			return;
	}

	void OpenStoreTrigger::DebugIMGUIRender(float tick)
	{
		/*auto imgui = ImGUIManager::GetInstance();

		if(imgui->Begin("Rim Color"))
		{
			float color[4] = {rim.x, rim.y, rim.z, rim.w};

			imgui->ColorPicker4("Rim", color);

			rim.x = color[0];
			rim.y = color[1];
			rim.z = color[2];
			rim.w = color[3];
		}
		imgui->End();*/
	}

	void OpenStoreTrigger::SetButtonEvent()
	{
		auto tmpButton = popupCanvas->GetButtonUI("SelectButton0");
		tmpButton->SetClickEvent([&]
			{
				SceneManager::GetInstance()->LoadScene(L"BarOperationScene");
				onPopup = false;
				popupCanvas->SetEnable(onPopup);

				// 타임 스케일을 켜 줘야 오류가 안 생깁니다.
				SceneManager::GetInstance()->SetTimeScaleOrigin();
			});

		// store 버튼
		tmpButton = popupCanvas->GetButtonUI("SelectButton1");
		tmpButton->SetClickEvent([&]
			{
				// 스토어를 열면
				onCanvas = true;
				onPopup = false;
				popupCanvas->SetEnable(onPopup);
				storeCanvas->SetEnable(onCanvas);

			});

		tmpButton = popupCanvas->GetButtonUI("SelectButton2");
		tmpButton->SetClickEvent([&]
			{
				// 닫아줍니다.
				onPopup = false;
				popupCanvas->SetEnable(onPopup);

				// 세팅을 원상복귀 해줍니다.
				SceneManager::GetInstance()->SetTimeScaleOrigin();
				mainCanvas->GetCanvasUI("InteractionCanvas")->GetSpriteUI("F")->SetEnable(true);

				isInteracted = false;
			});

		tmpButton = storeCanvas->GetButtonUI("CloseButton");
		tmpButton->SetClickEvent([&]
			{
				onCanvas = false;
				onPopup = true;
				popupCanvas->SetEnable(onPopup);
				storeCanvas->SetEnable(onCanvas);
			});
	}
}
