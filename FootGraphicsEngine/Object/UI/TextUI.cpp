#include "GraphicsPch.h"

#include "Resources/Texture.h"
#include "TextUI.h"
#include "Manager/UIUtilsManager.h"


namespace GraphicsEngineSpace
{
	TextUI::TextUI()
		: text(L"Temp Text")
		, font{ "NanumSquareRoundRegular" }
		, color{ 1.0f, 1.0f, 1.0f, 1.0f }
		, fontSize(16.0f)
		, alignment(TextAlignment::LEFT)
		, isStyle(false)
		, isFixedWidth(false)
		, isCursor(false)
	{
	}

	TextUI::~TextUI()
	{

	}

	void TextUI::SetText(const std::string& text)
	{
		this->text.assign(text.begin(), text.end());

		if (isFixedWidth != true)
			width = text.size() * (fontSize - 1.0f);

		height = fontSize;

		if (isStyle == true)
			isStyle = false;
	}

	void TextUI::SetText(const std::wstring& text)
	{
		this->text = text;

		if (isFixedWidth != true)
			width = text.size() * (fontSize - 1.0f);

		height = fontSize;

		if (isStyle == true)
			isStyle = false;
	}

	// 개행 텍스트를 위한 새로운 SetText 함수 입니다.
	void TextUI::SetNText(const std::string& text)
	{
		// 내부적으로 text의 개행된 사이즈 중 가장 긴 사이즈를 찾아서 width에 지정해줍니다.
		this->text.assign(text.begin(), text.end());

		int maxLength = 0;
		int length = 0;
		for (auto chr : text)
		{
			if (chr == '\n')
			{
				if (length > maxLength)
				{
					maxLength = length;
				}
				// \n을 만나면 초기화.
				length = 0;
			}
			else
				length++;
		}
		// 마지막이 개행이 아닐 수 있음
		if (length > maxLength)
		{
			maxLength = length;
			length = 0;
		}

		// 여기서 세팅한 맥스 Length로 width를 설정해줍니다.
		if (maxLength == 0)
			maxLength = text.size();

		if (isFixedWidth != true)
			width = maxLength * (fontSize - 1.0f);

		height = fontSize;

		if (isStyle == true)
			isStyle = false;
	}

	// 개행 텍스트를 위한 새로운 SetText 함수 입니다.
	void TextUI::SetNText(const std::wstring& text)
	{
		this->text = text;

		int maxLength = 0;
		int length = 0;
		for (auto chr : text)
		{
			if (chr == '\n')
			{
				if (length > maxLength)
				{
					maxLength = length;
				}
				// \n을 만나면 초기화.
				length = 0;
			}
			else
				length++;
		}
		// 마지막이 개행이 아닐 수 있음
		if (length > maxLength)
		{
			maxLength = length;
			length = 0;
		}

		// 여기서 세팅한 맥스 Length로 width를 설정해줍니다.
		if (maxLength == 0)
			maxLength = text.size();

		if (isFixedWidth != true)
			width = maxLength * (fontSize - 1.0f);

		height = fontSize;

		// 스타일 지정이 되어있었다면 해제해준다.
		if(isStyle == true)
			isStyle = false;
	}

	void TextUI::SetStyleText(const std::string& text)
	{
		// maxLength를 새로 세팅.
		this->text.assign(text.begin(), text.end());
		isStyle = true;

		int maxLength = 0;
		int length = 0;
		// 명령어에 해당하는 /, c, 등..
		int opLength = 0;
		for (int i = 0; i < text.size(); i++)
		{
			if (text[i] == '/')
			{
				// 슬래시를 만나면 뒤의 것이..
				if (i + 1 < text.size())
				{
					// c일 경우 8개 /c123456
					if (text[i + 1] == 'c')
						opLength += 8;
					else if (text[i + 1] == 'n')
					{
						opLength += 2;

						length -= opLength;
						if (length > maxLength)
							maxLength = length;

						length = 0;
						opLength = 0;

						continue;
					}
				}
				else
				{
					// 나머지는 두개
					opLength += 2;
				}
				// 적절한 계산을 위해 여기서도 Length를 올려줍니다.
				length++;
			}
			else
				length++;
		}
		// 마지막이 개행이 아닐 경우
		if (length > maxLength)
		{
			maxLength = length;
			length = 0;
		}

		// 여기서 세팅한 맥스 Length로 width를 설정해줍니다.
		if (maxLength == 0)
			maxLength = text.size();

		if (isFixedWidth != true)
			width = maxLength * (fontSize - 1.0f);

		height = fontSize;

		// 여기서 잘라줍니다.
			// 기존의 벡터를 클리어 후 잘라서 넣어줍니다.
		sliceText.clear();
		sliceText = resplit(this->text, std::wregex(L"[/]"));

	}

	void TextUI::SetStyleText(const std::wstring& text)
	{
		// maxLength를 새로 세팅.
		// maxLength를 새로 세팅.
		this->text = text;
		isStyle = true;

		int maxLength = 0;
		int length = 0;
		// 명령어에 해당하는 /, c, 등..
		int opLength = 0;
		for (int i = 0; i < text.size(); i++)
		{
			if (text[i] == '/')
			{
				// 슬래시를 만나면 뒤의 것이..
				if (i + 1 < text.size())
				{
					// c일 경우 8개 /c123456
					if(text[i + 1] == 'c')
						opLength += 8;
					else if(text[i + 1] == 'n')
					{
						length -= opLength;
						if (length > maxLength)
							maxLength = length;

						length = 0;
						opLength = 0;

						continue;
					}
				}
				else
				{
					// 나머지는 두개
					opLength += 2;
				}
				// 적절한 계산을 위해 여기서도 Length를 올려줍니다.
				length++;
			}
			else
				length++;
		}
		// 마지막이 개행이 아닐 경우
		if (length > maxLength)
		{
			maxLength = length;
			length = 0;
		}

		// 여기서 세팅한 맥스 Length로 width를 설정해줍니다.
		if (maxLength == 0)
			maxLength = text.size();

		if (isFixedWidth != true)
			width = maxLength * (fontSize - 1.0f);

		height = fontSize;

		// 여기서 잘라줍니다.
			// 기존의 벡터를 클리어 후 잘라서 넣어줍니다.
		sliceText.clear();
		sliceText = resplit(this->text, std::wregex(L"[/]"));
	}

	void TextUI::SetColor(const SimpleMath::Vector4& color)
	{
		this->color = color;
	}

	void TextUI::SetAlpha(float alpha)
	{
		this->color.w = alpha;
	}

	void TextUI::SetFont(const std::string& fontName)
	{
		font = fontName;
	}

	void TextUI::SetStyle(bool isStyle)
	{
		this->isStyle = isStyle;
	}

	void TextUI::SetFixedWidth(float width)
	{
		isFixedWidth = true;
		this->width = width;
	}

	void TextUI::SetFontSize(float fontSize)
	{
		if (fontSize < 0.0f)
			this->fontSize = 0.0f;
		else
			this->fontSize = fontSize;

		if (isFixedWidth != true && isStyle != true)
			width = text.size() * (this->fontSize - 1.0f);

		height = this->fontSize;
	}

	void TextUI::SetAlignment(TextAlignment alignment)
	{
		this->alignment = alignment;
	}

	std::string TextUI::GetText()
	{
		std::string temp;
		temp.assign(text.begin(), text.end());

		return temp;
	}

	void TextUI::Render(std::shared_ptr<IRenderer> renderer, float tick)
	{
		if (isEnable != true)
			return;

		if (isStyle == true)
		{
			StyleRender(renderer, tick);
			return;
		}

		if(isCursor == true)
		{
			DrawCursor(renderer, tick);
			return;
		}

		auto uiUtilManager = UIUtilsManager::GetInstance();

		// 그림 그리기.
		std::shared_ptr<SpriteBatch> batch = uiUtilManager->GetSpriteBatch();
		auto sfont = uiUtilManager->GetFont(font);

		// alignment에 따라 다르게 나타나집니다.
		SimpleMath::Vector2 origin = SimpleMath::Vector2::Zero;
		SimpleMath::Vector2 size = sfont->MeasureString(text.c_str());

		switch (alignment)
		{

		case TextAlignment::CENTER:
		{
			origin.x += this->GetWidth() / 2;
		}
		break;

		// Left는 현재(zero)와 다를바 없습니다.
		case TextAlignment::LEFT:
			break;

		case TextAlignment::RIGHT:
		{
			origin.x -= this->GetWidth() / 2 - size.x / 2;
		}
		break;


		case TextAlignment::TOP:
		{
			origin.y += this->GetHeight() / 2 - size.y / 2;
		}
		break;

		case TextAlignment::BOTTOM:
		{
			origin.y -= this->GetHeight() / 2 - size.y / 2;
		}
		break;

		default:
			break;

		}

		batch->Begin(DirectX::SpriteSortMode_Deferred, uiUtilManager->GetBlendState().Get(), nullptr, uiUtilManager->GetDepthState().Get());
		sfont->DrawString(batch.get(), text.c_str(), GetScreenPosition(), color, 0.0f,
			origin, SimpleMath::Vector2{ fontSize / 50.0f, fontSize / 50.0f } *GetScreenScale(),
			DirectX::SpriteEffects_None, GetScreenPosition().z);
		batch->End();

		for (auto iter : children)
		{
			if (iter != nullptr)
				iter->Render(renderer, tick);
		}
	}

	void TextUI::StyleRender(std::shared_ptr<IRenderer> renderer, float tick)
	{
		// 해당하는 식을 만나기 전까지는 기존 색상, 폰트로 그려줍니다.

		// 슬라이스가 없으면 리턴
		if (sliceText.size() < 1)
			return;

		// 슬라이스 뒤에 나타나는 첫 char를 바탕으로 스타일을 변경해줍니다.
		// slice 순서대로 렌더해준다고 가정합니다.
			// 0xRRGGBB로 만들어 준뒤. 넣어줍시다.

		// 배치를 가져옵시다.
		auto uiUtilManager = UIUtilsManager::GetInstance();

		// 그림 그리기.
		std::shared_ptr<SpriteBatch> batch = uiUtilManager->GetSpriteBatch();
		auto sfont = uiUtilManager->GetFont(font);
		auto ifont = uiUtilManager->GetFont(font + "I");
		auto bfont = uiUtilManager->GetFont(font + "B");

		SimpleMath::Vector2 origin = SimpleMath::Vector2::Zero;

		batch->Begin(DirectX::SpriteSortMode_Deferred, uiUtilManager->GetBlendState().Get(), nullptr, uiUtilManager->GetDepthState().Get());
		// 내부에서 한번에 다 드로우 해봅시다..
		std::wstring tmpText = L"";
		SimpleMath::Vector3 screenPos = GetScreenPosition();
		SimpleMath::Vector2 screenScale = GetScreenScale();
		std::shared_ptr<SpriteFont> tmpFont = sfont;
		SimpleMath::Vector4 tmpColor = color;

		bool firstDraw = false;
		for (int i = 0; i < sliceText.size(); i++)
		{
			// 잘린 부분의 첫번째 친구가 op
			auto op = sliceText[i][0];

			switch (op)
			{
			case 'c':
			case 'C':
			{
				// rgb컬러를 뽑아내줍니다.
				std::wstring colorStr(sliceText[i].begin() + 1, sliceText[i].begin() + 7);
				std::wstring hexColor = L"0x";
				hexColor += colorStr;
				int hex = std::stoi(hexColor.c_str(), nullptr, 16);
				float r = ((hex >> 16) & 0xFF) / 255.f;
				float g = ((hex >> 8) & 0xFF) / 255.f;
				float b = (hex & 0xFF) / 255.f;
				tmpColor.x = r;
				tmpColor.y = g;
				tmpColor.z = b;

				tmpText.assign(sliceText[i].begin() + 7, sliceText[i].end());

				break;
			}

			// b i o n은 op를 뺀글자 => 1글자 뒤부터 넣어줍니다.
			case 'b':
			case 'B':
			{
				if (bfont != nullptr)
					tmpFont = bfont;

				tmpText.assign(sliceText[i].begin() + 1, sliceText[i].end());
				break;
			}

			case 'i':
			case 'I':
			{
				if (ifont != nullptr)
					tmpFont = ifont;

				tmpText.assign(sliceText[i].begin() + 1, sliceText[i].end());

				break;
			}

			case 'o':
			case 'O':
			{
				tmpFont = sfont;

				tmpText.assign(sliceText[i].begin() + 1, sliceText[i].end());

				break;
			}

			case 'n':
			case 'N':
			{
				// 개행 문자. x를 초기화 y를 늘려줌
					// 여기서의 개행은 \이 아닌 /을 써줘야합니다..
				screenPos.x = GetScreenPosition().x;
				screenPos.y += fontSize * 1.7f * screenScale.y;

				tmpText.assign(sliceText[i].begin() + 1, sliceText[i].end());
				break;
			}

			default:
			{
				// 앞에 아무것도 없이 시작되는 경우를 생각해야함.
				if (i == 0)
					tmpText.assign(sliceText[i].begin(), sliceText[i].end());
				else
					tmpText = L"";
				break;
			}
			}

			if (tmpText == L"")
				continue;

			tmpFont->DrawString(batch.get(), tmpText.c_str(), screenPos, tmpColor, 0.0f,
				origin, SimpleMath::Vector2{ fontSize / 50.0f, fontSize / 50.0f } * screenScale,
				DirectX::SpriteEffects_None, screenPos.z);

			// 현재 텍스트 크기를 체크
				// IgnoreWhite 때문에 띄어쓰기가 안보인 거라면..?
			auto tmpRect = tmpFont->MeasureDrawBounds(tmpText.c_str(), SimpleMath::Vector2{0.f, 0.f}, false);

			// 폰트 크기에 따른 처리를 해줘야한다.
				// 폰트가 크면 더 붙고 작으면 덜 붙는.
			screenPos.x += (tmpRect.right - tmpRect.left) * (fontSize / 50.0f) * screenScale.x;
			// 1픽셀의 여유
			//screenPos.x += 2.0f * screenScale.x;
		}

		batch->End();
	}

	void TextUI::DrawCursor(std::shared_ptr<IRenderer> renderer, float tick)
	{
		// 얘는 그냥 위치로 그려준다.
		auto screenScale = GetScreenScale();

		auto uiUtilManager = UIUtilsManager::GetInstance();
		auto sfont = uiUtilManager->GetFont(font);

		std::shared_ptr<SpriteBatch> batch = uiUtilManager->GetSpriteBatch();
		batch->Begin(DirectX::SpriteSortMode_Immediate, uiUtilManager->GetBlendState().Get(), nullptr, uiUtilManager->GetDepthState().Get());
		sfont->DrawString(batch.get(), text.c_str(), SimpleMath::Vector2{position.x, position.y},
			color, 0.0f, { 0.f, 0.f }, SimpleMath::Vector2{ fontSize / 50.0f, fontSize / 50.0f } * GetScreenScale(), DirectX::SpriteEffects_None, position.z);
		batch->End();

		for (auto iter : children)
		{
			if (iter != nullptr)
				iter->Render(renderer, tick);
		}
	}

	std::vector<std::wstring> TextUI::resplit(const std::wstring& s, const std::wregex& sep_regex)
	{
		std::wsregex_token_iterator iter(s.begin(), s.end(), sep_regex, -1);
		std::wsregex_token_iterator end;
		return { iter, end };
	}
}
