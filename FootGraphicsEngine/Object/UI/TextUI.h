#pragma once

// 텍스트 UI를 화면에 띄우기 위해..

#include <string>
#include <regex>

#include "UIBase.h"

namespace GraphicsEngineSpace
{
	class Canvas;

	// 텍스트에서만 사용해줄 Enum 값
	enum class TextAlignment
	{
		CENTER,
		LEFT,
		RIGHT,
		TOP,
		BOTTOM
	};

	class TextUI : public UIBase
	{
		std::wstring text;
		std::vector<std::wstring> sliceText;
		std::string font;
		// 알파값까지 한 color
		SimpleMath::Vector4 color;
		float fontSize;
		TextAlignment alignment;
		bool isStyle;
		bool isFixedWidth;
		bool isCursor;

	public:
		TextUI();
		~TextUI() override;
		virtual void SetText(const std::string& text);
		virtual void SetText(const std::wstring& text);
		virtual void SetNText(const std::string& text);
		virtual void SetNText(const std::wstring& text);
		virtual void SetStyleText(const std::string& text);
		virtual void SetStyleText(const std::wstring& text);
		virtual void SetColor(const SimpleMath::Vector4& color);
		virtual void SetAlpha(float alpha);
		virtual void SetFont(const std::string& fontName);
		virtual void SetStyle(bool isStyle);
		virtual void SetFixedWidth(float width);
		virtual void SetUsingFixedWidth(bool isFixedWidth) { this->isFixedWidth = isFixedWidth; }
		virtual void SetFontSize(float fontSize);
		virtual void SetAlignment(TextAlignment alignment);
		virtual void SetCursor(bool cursor) { isCursor = cursor; }

		virtual std::string GetText();
		virtual float GetFontSize() { return fontSize; }

	protected:
		void Render(std::shared_ptr<IRenderer> renderer, float tick) override;

		friend Canvas;
	private:
		void StyleRender(std::shared_ptr<IRenderer> renderer, float tick);
		void DrawCursor(std::shared_ptr<IRenderer> renderer, float tick);
		std::vector<std::wstring> resplit(const std::wstring& s, const std::wregex& sep_regex = std::wregex{L"\\s+"});
	};
}
