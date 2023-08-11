#pragma once

#include "UIBase.h"

namespace GraphicsEngineSpace
{
	class Texture;
	class Canvas;

	// 2D SpriteUI를 화면에 띄우기 위한 클래스
	class SpriteUI : public UIBase
	{
		// 사용할 텍스쳐 정보..리소스 매니저에 생성된 텍스쳐를 사용한다.
		std::shared_ptr<Texture> texture;
		SimpleMath::Vector4 maskColor;

		float rotation;

		bool isAnim;
		bool isCursor;
		bool usingRotation;
		RECT srcOffset;

	public:
		SpriteUI();

		/*SpriteUI(const SpriteUI& other)
		{
			texture = std::make_shared<Texture>(*other.texture);

			maskColor = other.maskColor;

			isAnim = other.isAnim;

			srcOffset = other.srcOffset;

			isEnable = other.isEnable;

			isSelected = other.isSelected;

			parent = std::weak_ptr<UIBase>();

			for (const auto& child : other.children)
			{
				children.push_back(std::make_shared<SpriteUI>(*std::static_pointer_cast<SpriteUI>(child)));
			}

			position = other.position;
			rotation = other.rotation;
			scale = other.scale;

			name = other.name;
		}*/

		~SpriteUI() override;

		virtual void SetTexture(std::shared_ptr<Texture> texture);
		virtual SimpleMath::Vector4 GetMaskColor() { return maskColor; }
		virtual void SetMaskColor(SimpleMath::Vector4 color);
		virtual void SetMaskColor(float r, float g, float b, float a);
		virtual void SetMaskAlpha(float a);
		virtual const std::shared_ptr<Texture>& GetTexture();
		virtual void SetIsAnim(bool anim) { this->isAnim = anim; }
		virtual void SetSrcOffset(SimpleMath::Vector4 offset);
		virtual void SetSrcOffset(RECT offset);
		virtual void SetCursor(bool cursor) { isCursor = cursor; }
		virtual void SetUsingRotation(bool isRot) { usingRotation = isRot; }
		virtual bool GetUsingRotation() { return usingRotation; }
		virtual void SetRotation(float eulerAngle);
		virtual float GetRotationDegree();

	protected:
		void Render(std::shared_ptr<IRenderer> renderer, float tick) override;
		void DrawCursor(std::shared_ptr<IRenderer> renderer, float tick);

		friend Canvas;

	};

}