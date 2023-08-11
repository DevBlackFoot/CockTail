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

	// ���� �ؽ�Ʈ�� ���� ���ο� SetText �Լ� �Դϴ�.
	void TextUI::SetNText(const std::string& text)
	{
		// ���������� text�� ����� ������ �� ���� �� ����� ã�Ƽ� width�� �������ݴϴ�.
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
				// \n�� ������ �ʱ�ȭ.
				length = 0;
			}
			else
				length++;
		}
		// �������� ������ �ƴ� �� ����
		if (length > maxLength)
		{
			maxLength = length;
			length = 0;
		}

		// ���⼭ ������ �ƽ� Length�� width�� �������ݴϴ�.
		if (maxLength == 0)
			maxLength = text.size();

		if (isFixedWidth != true)
			width = maxLength * (fontSize - 1.0f);

		height = fontSize;

		if (isStyle == true)
			isStyle = false;
	}

	// ���� �ؽ�Ʈ�� ���� ���ο� SetText �Լ� �Դϴ�.
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
				// \n�� ������ �ʱ�ȭ.
				length = 0;
			}
			else
				length++;
		}
		// �������� ������ �ƴ� �� ����
		if (length > maxLength)
		{
			maxLength = length;
			length = 0;
		}

		// ���⼭ ������ �ƽ� Length�� width�� �������ݴϴ�.
		if (maxLength == 0)
			maxLength = text.size();

		if (isFixedWidth != true)
			width = maxLength * (fontSize - 1.0f);

		height = fontSize;

		// ��Ÿ�� ������ �Ǿ��־��ٸ� �������ش�.
		if(isStyle == true)
			isStyle = false;
	}

	void TextUI::SetStyleText(const std::string& text)
	{
		// maxLength�� ���� ����.
		this->text.assign(text.begin(), text.end());
		isStyle = true;

		int maxLength = 0;
		int length = 0;
		// ��ɾ �ش��ϴ� /, c, ��..
		int opLength = 0;
		for (int i = 0; i < text.size(); i++)
		{
			if (text[i] == '/')
			{
				// �����ø� ������ ���� ����..
				if (i + 1 < text.size())
				{
					// c�� ��� 8�� /c123456
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
					// �������� �ΰ�
					opLength += 2;
				}
				// ������ ����� ���� ���⼭�� Length�� �÷��ݴϴ�.
				length++;
			}
			else
				length++;
		}
		// �������� ������ �ƴ� ���
		if (length > maxLength)
		{
			maxLength = length;
			length = 0;
		}

		// ���⼭ ������ �ƽ� Length�� width�� �������ݴϴ�.
		if (maxLength == 0)
			maxLength = text.size();

		if (isFixedWidth != true)
			width = maxLength * (fontSize - 1.0f);

		height = fontSize;

		// ���⼭ �߶��ݴϴ�.
			// ������ ���͸� Ŭ���� �� �߶� �־��ݴϴ�.
		sliceText.clear();
		sliceText = resplit(this->text, std::wregex(L"[/]"));

	}

	void TextUI::SetStyleText(const std::wstring& text)
	{
		// maxLength�� ���� ����.
		// maxLength�� ���� ����.
		this->text = text;
		isStyle = true;

		int maxLength = 0;
		int length = 0;
		// ��ɾ �ش��ϴ� /, c, ��..
		int opLength = 0;
		for (int i = 0; i < text.size(); i++)
		{
			if (text[i] == '/')
			{
				// �����ø� ������ ���� ����..
				if (i + 1 < text.size())
				{
					// c�� ��� 8�� /c123456
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
					// �������� �ΰ�
					opLength += 2;
				}
				// ������ ����� ���� ���⼭�� Length�� �÷��ݴϴ�.
				length++;
			}
			else
				length++;
		}
		// �������� ������ �ƴ� ���
		if (length > maxLength)
		{
			maxLength = length;
			length = 0;
		}

		// ���⼭ ������ �ƽ� Length�� width�� �������ݴϴ�.
		if (maxLength == 0)
			maxLength = text.size();

		if (isFixedWidth != true)
			width = maxLength * (fontSize - 1.0f);

		height = fontSize;

		// ���⼭ �߶��ݴϴ�.
			// ������ ���͸� Ŭ���� �� �߶� �־��ݴϴ�.
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

		// �׸� �׸���.
		std::shared_ptr<SpriteBatch> batch = uiUtilManager->GetSpriteBatch();
		auto sfont = uiUtilManager->GetFont(font);

		// alignment�� ���� �ٸ��� ��Ÿ�����ϴ�.
		SimpleMath::Vector2 origin = SimpleMath::Vector2::Zero;
		SimpleMath::Vector2 size = sfont->MeasureString(text.c_str());

		switch (alignment)
		{

		case TextAlignment::CENTER:
		{
			origin.x += this->GetWidth() / 2;
		}
		break;

		// Left�� ����(zero)�� �ٸ��� �����ϴ�.
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
		// �ش��ϴ� ���� ������ �������� ���� ����, ��Ʈ�� �׷��ݴϴ�.

		// �����̽��� ������ ����
		if (sliceText.size() < 1)
			return;

		// �����̽� �ڿ� ��Ÿ���� ù char�� �������� ��Ÿ���� �������ݴϴ�.
		// slice ������� �������شٰ� �����մϴ�.
			// 0xRRGGBB�� ����� �ص�. �־��ݽô�.

		// ��ġ�� �����ɽô�.
		auto uiUtilManager = UIUtilsManager::GetInstance();

		// �׸� �׸���.
		std::shared_ptr<SpriteBatch> batch = uiUtilManager->GetSpriteBatch();
		auto sfont = uiUtilManager->GetFont(font);
		auto ifont = uiUtilManager->GetFont(font + "I");
		auto bfont = uiUtilManager->GetFont(font + "B");

		SimpleMath::Vector2 origin = SimpleMath::Vector2::Zero;

		batch->Begin(DirectX::SpriteSortMode_Deferred, uiUtilManager->GetBlendState().Get(), nullptr, uiUtilManager->GetDepthState().Get());
		// ���ο��� �ѹ��� �� ��ο� �غ��ô�..
		std::wstring tmpText = L"";
		SimpleMath::Vector3 screenPos = GetScreenPosition();
		SimpleMath::Vector2 screenScale = GetScreenScale();
		std::shared_ptr<SpriteFont> tmpFont = sfont;
		SimpleMath::Vector4 tmpColor = color;

		bool firstDraw = false;
		for (int i = 0; i < sliceText.size(); i++)
		{
			// �߸� �κ��� ù��° ģ���� op
			auto op = sliceText[i][0];

			switch (op)
			{
			case 'c':
			case 'C':
			{
				// rgb�÷��� �̾Ƴ��ݴϴ�.
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

			// b i o n�� op�� ������ => 1���� �ں��� �־��ݴϴ�.
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
				// ���� ����. x�� �ʱ�ȭ y�� �÷���
					// ���⼭�� ������ \�� �ƴ� /�� ������մϴ�..
				screenPos.x = GetScreenPosition().x;
				screenPos.y += fontSize * 1.7f * screenScale.y;

				tmpText.assign(sliceText[i].begin() + 1, sliceText[i].end());
				break;
			}

			default:
			{
				// �տ� �ƹ��͵� ���� ���۵Ǵ� ��츦 �����ؾ���.
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

			// ���� �ؽ�Ʈ ũ�⸦ üũ
				// IgnoreWhite ������ ���Ⱑ �Ⱥ��� �Ŷ��..?
			auto tmpRect = tmpFont->MeasureDrawBounds(tmpText.c_str(), SimpleMath::Vector2{0.f, 0.f}, false);

			// ��Ʈ ũ�⿡ ���� ó���� ������Ѵ�.
				// ��Ʈ�� ũ�� �� �ٰ� ������ �� �ٴ�.
			screenPos.x += (tmpRect.right - tmpRect.left) * (fontSize / 50.0f) * screenScale.x;
			// 1�ȼ��� ����
			//screenPos.x += 2.0f * screenScale.x;
		}

		batch->End();
	}

	void TextUI::DrawCursor(std::shared_ptr<IRenderer> renderer, float tick)
	{
		// ��� �׳� ��ġ�� �׷��ش�.
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
