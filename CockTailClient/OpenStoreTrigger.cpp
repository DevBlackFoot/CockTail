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

		// ó�� �׾����� �� �� ���ݴϴ�.
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
				// ������ ���� ������..!
					// ���� ���� �ݾ��ݴϴ�.
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
			// f�� ���� ��� IsInteracted
			if (onPopup != true && onCanvas == false)
			{
				onPopup = true;

				popupCanvas->SetEnable(onPopup);
				SceneManager::GetInstance()->ChangeTimeScale(0.f);
				mainCanvas->GetCanvasUI("InteractionCanvas")->GetSpriteUI("F")->SetEnable(false);
			}

			if (onCanvas == true)
			{
				// â�� ���� �ִ� ���¿���
					// esc�� ������
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

				// Ÿ�� �������� �� ��� ������ �� ����ϴ�.
				SceneManager::GetInstance()->SetTimeScaleOrigin();
			});

		// store ��ư
		tmpButton = popupCanvas->GetButtonUI("SelectButton1");
		tmpButton->SetClickEvent([&]
			{
				// ���� ����
				onCanvas = true;
				onPopup = false;
				popupCanvas->SetEnable(onPopup);
				storeCanvas->SetEnable(onCanvas);

			});

		tmpButton = popupCanvas->GetButtonUI("SelectButton2");
		tmpButton->SetClickEvent([&]
			{
				// �ݾ��ݴϴ�.
				onPopup = false;
				popupCanvas->SetEnable(onPopup);

				// ������ ���󺹱� ���ݴϴ�.
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
