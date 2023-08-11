#include "GraphicsPch.h"

#include "Resources/Texture.h"
#include "SpriteUI.h"
#include "Manager/UIUtilsManager.h"

namespace GraphicsEngineSpace
{
	SpriteUI::SpriteUI()
		: texture(nullptr)
		, maskColor(Colors::White)
		, isAnim(false)
		, isCursor(false)
		, usingRotation(false)
		, srcOffset(RECT{ 0, 0, 1, 1 })
		, rotation(0.f)
	{
	}

	SpriteUI::~SpriteUI()
	{

	}

	void SpriteUI::SetTexture(std::shared_ptr<Texture> texture)
	{
		this->texture = texture;
		this->srcOffset.bottom = texture->GetHeight();
		this->srcOffset.right = texture->GetWidth();
	}

	void SpriteUI::SetMaskColor(SimpleMath::Vector4 color)
	{
		maskColor = color;
	}

	void SpriteUI::SetMaskColor(float r, float g, float b, float a)
	{
		maskColor = SimpleMath::Vector4{ r, g, b, a };
	}

	void SpriteUI::SetMaskAlpha(float a)
	{
		maskColor.w = a;
	}

	const std::shared_ptr<Texture>& SpriteUI::GetTexture()
	{
		return texture;
	}

	void SpriteUI::SetSrcOffset(SimpleMath::Vector4 offset)
	{
		srcOffset.left = offset.x;
		srcOffset.top = offset.y;
		srcOffset.right = offset.z;
		srcOffset.bottom = offset.w;

	}

	void SpriteUI::SetSrcOffset(RECT offset)
	{
		srcOffset.left = offset.left;
		srcOffset.top = offset.top;
		srcOffset.right = offset.right;
		srcOffset.bottom = offset.bottom;
	}

	void SpriteUI::SetRotation(float eulerAngle)
	{
		// ui 컨트롤러에서 변경해주기 위해서..
		//usingRotation = true;

		// 들어온 값을 라디안으로 변경해줍니다.
		rotation = eulerAngle * (MathHelper::Pi / 180.f);
	}

	float SpriteUI::GetRotationDegree()
	{
		return rotation * (180.f / MathHelper::Pi);
	}

	void SpriteUI::Render(std::shared_ptr<IRenderer> renderer, float tick)
	{
		if (isEnable != true)
			return;

		if (isCursor == true)
		{
			DrawCursor(renderer, tick);
			return;
		}

		// 그림 그리기
		if (texture != nullptr)
		{
			auto screenPos = GetScreenPosition();
			auto screenScale = GetScreenScale();

			// Batch로 그려주기..
				// UIUtilManager에 있는 batch를 사용합니다.
			std::shared_ptr<SpriteBatch> batch = UIUtilsManager::GetInstance()->GetSpriteBatch();

			batch->Begin(DirectX::SpriteSortMode_Texture, UIUtilsManager::GetInstance()->GetBlendState().Get(), nullptr, UIUtilsManager::GetInstance()->GetDepthState().Get());

			if (isAnim != true)
			{
				if (usingRotation == true)
				{
					// 로테이션을 사용할 때 오리진 값 역시 변경해줍니다.
					/*float texMidX = texture->GetWidth() / 2.f;
					float texMidY = texture->GetHeight() / 2.f;*/

					float texMidX = width / 2.f;
					float texMidY = height / 2.f;

					/// 오리진이 이상해지면 고칩니다. texture의 width, Height가 아니라. UI의 넓이 높이로.

					// 로테이션 값은 라디안 값이 들어갑니다.
					// 기본적으로 텍스쳐의 가운데로 오리진을 설정해서 돌려줍니다.
					batch->Draw(texture->GetMapSRV().Get(), RECT{
					static_cast<long>(screenPos.x + texMidX * screenScale.x),
					static_cast<long>(screenPos.y + texMidY * screenScale.y),
					static_cast<long>(screenPos.x + ((texMidX + width) * screenScale.x)),
					static_cast<long>(screenPos.y + ((texMidY + height) * screenScale.y)) }, nullptr
					, maskColor, rotation, { texMidX, texMidY },
						DirectX::SpriteEffects_None, screenPos.z);
				}
				else
				{
					batch->Draw(texture->GetMapSRV().Get(), RECT{
					static_cast<long>(screenPos.x),
					static_cast<long>(screenPos.y),
					static_cast<long>(screenPos.x + (width * screenScale.x)),
					static_cast<long>(screenPos.y + (height * screenScale.y)) }, nullptr,
					maskColor, 0.0f, { 0.f, 0.f },
					DirectX::SpriteEffects_None, screenPos.z);
				}
			}
			else
			{
				batch->Draw(texture->GetMapSRV().Get(), RECT{
				static_cast<long>(screenPos.x),
				static_cast<long>(screenPos.y),
				static_cast<long>(screenPos.x + (width * screenScale.x)),
				static_cast<long>(screenPos.y + (height * screenScale.y)) }, &srcOffset
				, maskColor, 0.0f, { 0.f, 0.f },
					DirectX::SpriteEffects_None, screenPos.z);
			}
			batch->End();

			for (auto iter : children)
			{
				if (iter != nullptr)
					iter->Render(renderer, tick);
			}

		}
	}

	void SpriteUI::DrawCursor(std::shared_ptr<IRenderer> renderer, float tick)
	{
		// 얘는 그냥 위치로 그려주면..?
		auto screenScale = GetScreenScale();

		std::shared_ptr<SpriteBatch> batch = UIUtilsManager::GetInstance()->GetSpriteBatch();
		batch->Begin(DirectX::SpriteSortMode_Immediate, UIUtilsManager::GetInstance()->GetBlendState().Get(), nullptr, UIUtilsManager::GetInstance()->GetDepthState().Get());
		batch->Draw(texture->GetMapSRV().Get(), RECT(position.x, position.y, position.x + (width * screenScale.x), position.y + (height * screenScale.y)),
			nullptr, maskColor, 0.0f, { 0.f, 0.f }, DirectX::SpriteEffects_None, position.z);
		batch->End();

		for (auto iter : children)
		{
			if (iter != nullptr)
				iter->Render(renderer, tick);
		}
	}
}
