#include "GamePch.h"
#include "UIManager.h"

#include "GraphicsManager.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "WindowManager.h"
#include "Timer.h"
#include "UICreator.h"

namespace GameEngineSpace
{
	std::shared_ptr<UIManager> UIManager::instance = nullptr;

	std::shared_ptr<UIManager> UIManager::GetInstance()
	{
		if (instance == nullptr)
			instance = std::make_shared<UIManager>();

		return instance;
	}

	void UIManager::Init()
	{
		graphicsInstance = GraphicsManager::GetInstance();
		inputInstance = InputManager::GetInstance();

		auto renderer = graphicsInstance->GetRenderer();
		auto width = WindowManager::GetInstance()->GetClientWidth();
		auto height = WindowManager::GetInstance()->GetClientHeight();

		nowWidth = width;
		nowHeight = height;

		mainCanvas = renderer->CreateCanvas("MainCanvas", nowWidth, nowHeight);

		// 각종 캔버스들을 이 아래에서 미리 생성해둔다.
		mainCanvas->CreateCanvasUI("TitleCanvas", width / 2, height / 2)->SetPosition({ static_cast<float>(width / 2), static_cast<float>(height / 2), 0.1f });
		mainCanvas->CreateCanvasUI("CombinationCanvas", width / 2, height / 2)->SetPosition({ static_cast<float>(width / 2), static_cast<float>(height / 2), 0.1f });
		mainCanvas->CreateCanvasUI("DialogCanvas", width / 2, height / 2)->SetPosition({ static_cast<float>(width / 2), static_cast<float>(height / 2), 0.1f });
		mainCanvas->CreateCanvasUI("InGameCanvas", width / 2, height / 2)->SetPosition({ static_cast<float>(width / 2), static_cast<float>(height / 2), 0.1f });
		mainCanvas->CreateCanvasUI("SceneTestCanvas", width / 2, height / 2)->SetPosition({ static_cast<float>(width / 2), static_cast<float>(height / 2), 0.1f });
		mainCanvas->CreateCanvasUI("SceneCanvas", width / 2, height / 2)->SetPosition({ static_cast<float>(width / 2), static_cast<float>(height / 2), 0.1f });
		mainCanvas->CreateCanvasUI("InteractionCanvas", width / 2, height / 2)->SetPosition({ static_cast<float>(width / 2), static_cast<float>(height / 2), 0.1f });
		mainCanvas->CreateCanvasUI("MiniGameCanvas", width / 2, height / 2)->SetPosition({ static_cast<float>(width / 2), static_cast<float>(height / 2), 0.1f });
		mainCanvas->CreateCanvasUI("CreditCanvas", width / 2, height / 2)->SetPosition({ static_cast<float>(width / 2), static_cast<float>(height / 2), 0.1f });
		mainCanvas->CreateCanvasUI("JWCreditCanvas", width / 2, height / 2)->SetPosition({ static_cast<float>(width / 2), static_cast<float>(height / 2), 0.1f });
		mainCanvas->CreateCanvasUI("BarDialogCanvas", width / 2, height / 2)->SetPosition({ static_cast<float>(width / 2), static_cast<float>(height / 2), 0.1f });
		mainCanvas->CreateCanvasUI("ItemResultCanvas", width / 2, height / 2)->SetPosition({ static_cast<float>(width / 2), static_cast<float>(height / 2), 0.1f });
		mainCanvas->CreateCanvasUI("StoreCanvas", width / 2, height / 2)->SetPosition({ static_cast<float>(width / 2), static_cast<float>(height / 2), 0.1f });
		mainCanvas->CreateCanvasUI("PopUpCanvas", width / 2, height / 2)->SetPosition({ static_cast<float>(width / 2), static_cast<float>(height / 2), 0.1f });
		// 새로 생성할 바 및 미니게임 캔버스
		mainCanvas->CreateCanvasUI("BarCanvas", width / 2, height / 2)->SetPosition({ static_cast<float>(width / 2), static_cast<float>(height / 2), 0.1f });
		mainCanvas->CreateCanvasUI("ShakeCanvas", width / 2, height / 2)->SetPosition({ static_cast<float>(width / 2), static_cast<float>(height / 2), 0.1f });
		mainCanvas->CreateCanvasUI("ResultCanvas", width / 2, height / 2)->SetPosition({ static_cast<float>(width / 2), static_cast<float>(height / 2), 0.1f });
		
		mainCanvas->CreateCanvasUI("InGameMenuCanvas", width / 2, height / 2)->SetPosition({ static_cast<float>(width / 2), static_cast<float>(height / 2), 0.1f });
		mainCanvas->CreateCanvasUI("SettingCanvase", width / 2, height / 2)->SetPosition({ static_cast<float>(width / 2), static_cast<float>(height / 2), 0.1f });
		
		// 끄지 않을 캔버스
		mainCanvas->CreateCanvasUI("CursorCanvas", width / 2, height / 2)->SetPosition({ static_cast<float>(width / 2), static_cast<float>(height / 2), 0.1f });

		// ImGUI포인터를 미리 보관해둔다.
		imgui = ImGUIManager::GetInstance();

		creator = std::make_shared<UICreator>();

		printf("UIManager Init Clear\n");
	}

	// WIC 텍스쳐 로드 가능한 시점에 호출
	void UIManager::CreateCanvasAll()
	{
		creator->CreateCanvasAll();
	}

	void UIManager::Render(float tick)
	{
		GraphicsManager::GetInstance()->GetRenderer()->GraphicsDebugBeginEvent("Canvas");
		mainCanvas->Render(GraphicsManager::GetInstance()->GetRenderer(), tick);
		GraphicsManager::GetInstance()->GetRenderer()->GraphicsDebugEndEvent();
	}

	void UIManager::Update()
	{
		auto mousePos = inputInstance->GetMousePos();

		if (mainCanvas->GetSelectMode() == true)
			mainCanvas->CheckPickedUI(mousePos.x, mousePos.y, inputInstance->GetInputState(VK_LBUTTON, KeyState::UP));
		else
			mainCanvas->CheckCollidedButton(mousePos.x, mousePos.y, inputInstance->GetInputState(VK_LBUTTON, KeyState::UP));
	}

	void UIManager::LateUpdate()
	{
	}

	void UIManager::Release()
	{
		mainCanvas->Release();
	}

	std::shared_ptr<Canvas> UIManager::CreateCanvas(const std::string& UIIDstr)
	{
		std::shared_ptr<Canvas> newCanvas = mainCanvas->CreateCanvasUI(UIIDstr, nowWidth, nowHeight);
		newCanvas->SetScale({ mainCanvas->GetWidth() / static_cast<float>(nowWidth), mainCanvas->GetHeight() / static_cast<float>(nowHeight) });

		return newCanvas;
	}

	std::shared_ptr<Canvas> UIManager::GetCanvas(const std::string& canvasIDStr)
	{
		return mainCanvas->GetCanvasUI(canvasIDStr);
	}

	void UIManager::OnResize(uint32 width, uint32 height)
	{
		mainCanvas->SetWidth(width);
		mainCanvas->SetHeight(height);

		// 비율 계산 후 모든 캔버스 Scale 변경
		float xScale = static_cast<float>(width) / nowWidth;
		float yScale = static_cast<float>(height) / nowHeight;

		mainCanvas->SetScaleAllCanvas({ xScale, yScale });
	}

	void UIManager::ClearUI()
	{
	}

	void UIManager::SelectGUIRender()
	{
		auto pickedUI = mainCanvas->GetSelectedUI();

		if (imgui->UISelectBegin())
		{
			if (pickedUI == nullptr)
				imgui->Text("None");
			else
			{
				// picked UI가 있는 경우.	
				imgui->Text(pickedUI->GetName());

				auto pickedPos = pickedUI->GetPosition();
				float UIPos[2] = { pickedPos.x, pickedPos.y };
				float UIPosZ = pickedPos.z;
				float UIWidth = pickedUI->GetWidth();
				float UIHeight = pickedUI->GetHeight();

				imgui->DragFloat2("UI Position", UIPos);
				imgui->DragFloat("UI Float Z", &UIPosZ, 0.05f, 0.0f, 1.0f);
				imgui->DragFloat("UI Width", &UIWidth);
				imgui->DragFloat("UI Height", &UIHeight);

				pickedUI->SetPosition({ UIPos[0], UIPos[1], UIPosZ });
				pickedUI->SetWidth(UIWidth);
				pickedUI->SetHeight(UIHeight);

				if (std::dynamic_pointer_cast<ProgressBarUI>(pickedUI) != nullptr)
				{
					float fill = std::dynamic_pointer_cast<ProgressBarUI>(pickedUI)->GetFillPercent();

					imgui->DragFloat("Fill Percent", &fill, 0.01f, 0.0f, 1.0f);

					std::dynamic_pointer_cast<ProgressBarUI>(pickedUI)->SetFillPercent(fill);
				}

				if (std::dynamic_pointer_cast<TextUI>(pickedUI))
				{
					/*
					float fontSize = std::dynamic_pointer_cast<TextUI>(pickedUI)->GetFontSize();

					imgui->DragFloat("Font Size", &fontSize);

					static int alignment = 1;
					imgui->DragInt("Alignment", &alignment, 1, 0, 4);

					std::dynamic_pointer_cast<TextUI>(pickedUI)->SetFontSize(fontSize);

					// 입력 받을 곳의 Text
					static char msgBox[128] = "";
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
						std::dynamic_pointer_cast<TextUI>(pickedUI)->SetText(msgBox);
						// 옮김과 동시에 input text 영역을 지워줍니다.
						memset(&msgBox[0], 0, sizeof(msgBox));
					}

					switch (alignment)
					{
					case 0:
						std::dynamic_pointer_cast<TextUI>(pickedUI)->SetAlignment(TextAlignment::CENTER);
						break;

					case 1:
						std::dynamic_pointer_cast<TextUI>(pickedUI)->SetAlignment(TextAlignment::LEFT);
						break;

					case 2:
						std::dynamic_pointer_cast<TextUI>(pickedUI)->SetAlignment(TextAlignment::RIGHT);
						break;

					case 3:
						std::dynamic_pointer_cast<TextUI>(pickedUI)->SetAlignment(TextAlignment::TOP);
						break;

					case 4:
						std::dynamic_pointer_cast<TextUI>(pickedUI)->SetAlignment(TextAlignment::BOTTOM);
						break;

					default:
						std::dynamic_pointer_cast<TextUI>(pickedUI)->SetAlignment(TextAlignment::LEFT);
						break;
					}
					*/

				}

				if (std::dynamic_pointer_cast<SpriteUI>(pickedUI))
				{
					auto sprite = std::dynamic_pointer_cast<SpriteUI>(pickedUI);
					auto maskColor = sprite->GetMaskColor();
					float color[4] = { maskColor.x, maskColor.y, maskColor.z, maskColor.w };

					imgui->ColorPicker4("Sprite Mask Color", color);
					
					sprite->SetMaskColor(color[0], color[1], color[2], color[3]);
				}
			}
		}
		imgui->End();
	}

}
