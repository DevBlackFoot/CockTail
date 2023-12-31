#include "GraphicsPch.h"
#include "ButtonUI.h"

#include "Manager/UIUtilsManager.h"

namespace GraphicsEngineSpace
{
	ButtonUI::ButtonUI()
		: defaultTexture(nullptr)
		, defaultMaskColor(Colors::White)
		, hoverTexture(nullptr)
		, hoverMaskColor(Colors::LightGray)
		, pressTexture(nullptr)
		, pressMaskColor(Colors::DarkGray)
		, state(ButtonState::DEFAULT)
		, clickEvent(nullptr)
		, pressEvent(nullptr)
		, enterEvent(nullptr)
		, hoverEvent(nullptr)
		, outEvent(nullptr)
	{
	}

	ButtonUI::~ButtonUI()
	{
	}

	void ButtonUI::SetDefaultTexture(std::shared_ptr<Texture> texture)
	{
		this->defaultTexture = texture;
	}

	void ButtonUI::SetDefaultMaskColor(float r, float g, float b, float a)
	{
		defaultMaskColor = SimpleMath::Vector4{ r, g, b, a };
	}

	void ButtonUI::SetHoverTexture(std::shared_ptr<Texture> texture)
	{
		this->hoverTexture = texture;
	}

	void ButtonUI::SetHoverMaskColor(float r, float g, float b, float a)
	{
		hoverMaskColor = SimpleMath::Vector4{ r, g, b, a };
	}

	void ButtonUI::SetPressTexture(std::shared_ptr<Texture> texture)
	{
		this->pressTexture = texture;
	}

	void ButtonUI::SetPressMaskColor(float r, float g, float b, float a)
	{
		pressMaskColor = SimpleMath::Vector4{ r, g, b, a };
	}

	// 매 틱 돌면서 ButtonState를 갱신해줍니다.
		// 각 이벤트가 존재하면 실행시킵니다.
	void ButtonUI::SetButtonState(ButtonState state)
	{
		// DISABLE State는 SetButtonEnable 함수로만 변경 가능하도록 합니다.
		if (this->state == ButtonState::DISABLE)
			return;

		// 현재 스테이트가 hover면서 들어온 State가 Default면
			// 호버에서 Default로 바뀌는 순간이 나가는 순간이기 때문
			// 버튼 밖으로 나감
		if (this->state == ButtonState::HOVER && state == ButtonState::DEFAULT)
		{
			// out Event 실행
			if (outEvent != nullptr)
				outEvent();
		}

		switch (state)
		{
		case ButtonState::ENTER:
		{
			if (enterEvent != nullptr)
				enterEvent();

			break;
		}

		case ButtonState::PRESS:
		{
			if (pressEvent != nullptr)
				pressEvent();

			break;
		}

		// 버튼을 눌렀다 떼는 순간 => click Event 실행
		case ButtonState::UP:
		{
			if (clickEvent != nullptr)
				clickEvent();

			break;
		}

		case ButtonState::HOVER:
		{
			if (hoverEvent != nullptr)
				hoverEvent();

			break;
		}

		case ButtonState::DEFAULT:
		case ButtonState::DOWN:
		default:
			break;
		}

		// Button State 갱신
			// 어느 이벤트든 state가 DisAble로 갱신 되었으면 state를 변경하지 않습니다.
		if(this->state != ButtonState::DISABLE)
			this->state = state;
	}

	// 버튼의 선택 가능 여부를 결정하는 함수, True면 선택 가능, False면 선택 불가능 하도록.
	void ButtonUI::SetButtonEnable(bool enable)
	{
		if (enable == true)
			this->state = ButtonState::DEFAULT;
		else
			this->state = ButtonState::DISABLE;
	}

	void ButtonUI::SetClickEvent(std::function<void()> clickEvent)
	{
		this->clickEvent = clickEvent;
	}

	void ButtonUI::SetPressEvent(std::function<void()> pressEvent)
	{
		this->pressEvent = pressEvent;
	}

	void ButtonUI::SetEnterEvent(std::function<void()> enterEvent)
	{
		this->enterEvent = enterEvent;
	}

	void ButtonUI::SetHoverEvent(std::function<void()> hoverEvent)
	{
		this->hoverEvent = hoverEvent;
	}

	void ButtonUI::SetOutEvent(std::function<void()> outEvent)
	{
		this->outEvent = outEvent;
	}

	ButtonState ButtonUI::GetButtonState()
	{
		return state;
	}

	void ButtonUI::Render(std::shared_ptr<IRenderer> renderer, float tick)
	{
		// 현재의 버튼 상태에 따라 달라지는..
		if (isEnable != true)
			return;

		// 그림 그리기.
			// 각 텍스쳐의 여부에 따라..
		auto defaultTex = defaultTexture;
		auto hoverTex = hoverTexture;
		auto pressTex = pressTexture;

		// 투명이더라도 DefaultTexture는 반드시 존재해야함..
		if (defaultTex != nullptr)
		{
			auto screenPos = GetScreenPosition();
			auto screenScale = GetScreenScale();
			RECT buttonSize = { static_cast<long>(screenPos.x), static_cast<long>(screenPos.y),
			static_cast<long>(screenPos.x + (width * screenScale.x)), static_cast<long>(screenPos.y + (height * screenScale.y)) };
			auto batch = UIUtilsManager::GetInstance()->GetSpriteBatch();

			// 상황에 따른 그리기..
			batch->Begin(DirectX::SpriteSortMode_Texture, UIUtilsManager::GetInstance()->GetBlendState().Get(), nullptr, UIUtilsManager::GetInstance()->GetDepthState().Get());

			// 실제로 Draw 함수를 호출하는 Switch 문
			switch (state)
			{
				// Button이 DisAble 일 때
			case ButtonState::DISABLE:
			{
					// 무조건 회색으로
				batch->Draw(defaultTex->GetMapSRV().Get(), buttonSize, nullptr,
					Colors::DimGray, 0.0f, { 0.f, 0.f }, DirectX::SpriteEffects_None, screenPos.z);

				break;
			}

			// 누르고 있을 때.
			case ButtonState::DOWN:
			case ButtonState::PRESS:
			{

				if (pressTex != nullptr)
				{
					batch->Draw(pressTex->GetMapSRV().Get(), buttonSize, nullptr,
						DirectX::Colors::White, 0.0f, { 0.f, 0.f }, DirectX::SpriteEffects_None, screenPos.z);
				}
				else
				{
					// 텍스쳐가 없으면.. color 마스크를 바꿔보자..
					batch->Draw(defaultTex->GetMapSRV().Get(), buttonSize, nullptr,
						pressMaskColor, 0.0f, { 0.f, 0.f }, DirectX::SpriteEffects_None, screenPos.z);
				}

				break;
			}

			// hover에 해당하는 State
			case ButtonState::UP:
			case ButtonState::HOVER:
			case ButtonState::ENTER:
			{
				if (hoverTex != nullptr)
				{
					batch->Draw(hoverTex->GetMapSRV().Get(), buttonSize, nullptr,
						DirectX::Colors::White, 0.0f, { 0.f, 0.f }, DirectX::SpriteEffects_None, screenPos.z);
				}
				else
				{
					// 텍스쳐가 없으면.. color 마스크를 바꿔보자..
					batch->Draw(defaultTex->GetMapSRV().Get(), buttonSize, nullptr,
						hoverMaskColor, 0.0f, { 0.f, 0.f }, DirectX::SpriteEffects_None, screenPos.z);
				}

				break;
			}

			// 나머지 경우.. 그냥 그려줍니다.
			case ButtonState::DEFAULT:
			default:
			{
				batch->Draw(defaultTex->GetMapSRV().Get(), buttonSize, nullptr,
					defaultMaskColor, 0.0f, { 0.f, 0.f }, DirectX::SpriteEffects_None, screenPos.z);

				break;
			}
			}

			batch->End();
		}

		for (auto iter : children)
		{
			if (iter != nullptr)
				iter->Render(renderer, tick);
		}
	}
}
