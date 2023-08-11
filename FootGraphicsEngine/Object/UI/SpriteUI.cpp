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
		// ui ��Ʈ�ѷ����� �������ֱ� ���ؼ�..
		//usingRotation = true;

		// ���� ���� �������� �������ݴϴ�.
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

		// �׸� �׸���
		if (texture != nullptr)
		{
			auto screenPos = GetScreenPosition();
			auto screenScale = GetScreenScale();

			// Batch�� �׷��ֱ�..
				// UIUtilManager�� �ִ� batch�� ����մϴ�.
			std::shared_ptr<SpriteBatch> batch = UIUtilsManager::GetInstance()->GetSpriteBatch();

			batch->Begin(DirectX::SpriteSortMode_Texture, UIUtilsManager::GetInstance()->GetBlendState().Get(), nullptr, UIUtilsManager::GetInstance()->GetDepthState().Get());

			if (isAnim != true)
			{
				if (usingRotation == true)
				{
					// �����̼��� ����� �� ������ �� ���� �������ݴϴ�.
					/*float texMidX = texture->GetWidth() / 2.f;
					float texMidY = texture->GetHeight() / 2.f;*/

					float texMidX = width / 2.f;
					float texMidY = height / 2.f;

					/// �������� �̻������� ��Ĩ�ϴ�. texture�� width, Height�� �ƴ϶�. UI�� ���� ���̷�.

					// �����̼� ���� ���� ���� ���ϴ�.
					// �⺻������ �ؽ����� ����� �������� �����ؼ� �����ݴϴ�.
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
		// ��� �׳� ��ġ�� �׷��ָ�..?
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
