#include "GamePch.h"
#include "SpriteEffect.h"

#include "../../FootGraphicsEngine/GraphicsEngineFramework.h"

#include "Camera.h"
#include "Object/GameObject.h"
#include "SceneBase.h"
#include "SceneManager.h"
#include "GraphicsManager.h"

using namespace GraphicsEngineSpace;

namespace GameEngineSpace
{
	SpriteEffect::SpriteEffect(std::weak_ptr<GameObject> object)
		: RendererBase(object)
		, yPlay(false)
		, playSpeed(0.5f)
	{
	}

	void SpriteEffect::Init(std::string objName)
	{
		renderObj = Factory::GetInstance()->CreateDXObject<GraphicsEngineSpace::SpriteEffect>(
			BuilderManager::GetInstance()->GetBuilder("SpriteEffectBuilder"), objName);

		GraphicsManager::GetInstance()->GetRenderer()->InitObject(renderObj);

		// 이펙트 바인딩
		effect = std::dynamic_pointer_cast<GraphicsEngineSpace::SpriteEffect>(renderObj);

	}

	void SpriteEffect::Render()
	{
		if (isRender == true)
			renderObj->Render();
	}

	void SpriteEffect::Render(std::shared_ptr<GraphicsEngineSpace::IRenderer> renderer)
	{
		if (isRender == true)
			renderObj->Render(renderer);
	}

	void SpriteEffect::YieldRender(std::shared_ptr<GraphicsEngineSpace::IRenderer> renderer)
	{
		if (isRender == true)
			renderObj->YieldRender(renderer);
	}

	void SpriteEffect::PrepareRender(float tick)
	{
		if (isRender != true)
			return;

		std::shared_ptr<Camera> mainCam = SceneManager::GetInstance()->GetCurrentScene()->GetMainCamera();

		renderObj->Update(transform->GetWorldTM(), mainCam->GetViewMatrix(), mainCam->GetProjMatrix());

		// y 오프셋의 지속적인 변화
		if (yPlay == true)
		{
			if (effect != nullptr)
			{
				SimpleMath::Vector2 offset;

				if (effect != nullptr)
					offset = effect->GetTexOffset();

				effect->SetTexOffset(offset.x, offset.y - playSpeed * tick);

				if (offset.y < 0.0f)
					effect->SetTexOffset(offset.x, 1.0f);
			}
		}
		/*

		*/
	}

	void SpriteEffect::DebugIMGUIRender(float tick)
	{
		auto imgui = ImGUIManager::GetInstance();

		// offset을 변경해봅니다.
		/*
		 */
		auto effect = std::dynamic_pointer_cast<GraphicsEngineSpace::SpriteEffect>(renderObj);
		SimpleMath::Vector2 offset;

		if (effect != nullptr)
			offset = effect->GetTexOffset();

		auto gameObjName = GetGameObject()->GetObjectID();

		if (imgui->TreeNode("Sprite Effect Info", "Sprite Effect" + std::to_string(gameObjName)))
		{
			auto pos = transform->GetLocalPosition();
			float posf[3] = { pos.x, pos.y, pos.z };

			auto scale = transform->GetWorldScale();
			float scalef[3] = { scale.x, scale.y, scale.z };

			auto rot = transform->GetLocalEulerRotation();
			float rotf[3] = { rot.x, rot.y, rot.z };

			bool isWrap = effect->GetWrap();
			SimpleMath::Vector4 src = effect->GetTexSrc();

			imgui->DragFloat3("Position", posf, 0.5f);
			imgui->DragFloat3("Scale", scalef, 0.1f, 0.1f, 20.f);
			imgui->DragFloat3("Rotation", rotf, 5.0f, -360.f, 360.f);

			float offsetArr[2] = { offset.x, offset.y };
			imgui->DragFloat2("Offset", offsetArr, 0.01f, -1.f, 1.f);

			float srcArr[4] = { src.x, src.y, src.z, src.w };
			imgui->DragFloat4("Src", srcArr, 0.01f, 0.f, 1.f);

			float speed = playSpeed;
			imgui->DragFloat("Speed", &speed, 0.1f, 0.f, 10.f);

			imgui->CheckBox("IsWrap", &isWrap);

			transform->SetPosition(Vector3{ posf[0], posf[1], posf[2] }, Space::LOCAL);
			transform->SetScale(Vector3{ scalef[0], scalef[1], scalef[2] });
			transform->SetLocalEulerRotation(Vector3{ rotf[0], rotf[1], rotf[2] });
			effect->SetTexOffset(offsetArr[0], offsetArr[1]);
			effect->SetTexSrc(srcArr[0], srcArr[1], srcArr[2], srcArr[3]);
			effect->SetBool("Wrap", isWrap);
			playSpeed = speed;

			imgui->TreePop();
			imgui->Separator();
		}
	}

	void SpriteEffect::SetMaskColor(const SimpleMath::Vector4& maskColor)
	{
		// 스프라이트 이펙트의 SetRimColor는 내부적으로 Mask Color로 동작합니다.
		renderObj->SetRimColor(maskColor);
	}

	void SpriteEffect::SetEmissiveColor(const SimpleMath::Vector4& emissiveColor)
	{
		renderObj->SetMaterialEmissiveColor("spawn", emissiveColor);
	}

	void SpriteEffect::SetEffectSrc(const SimpleMath::Vector4& src)
	{
		if(effect != nullptr)
		{
			// 0.f ~ 1.0f;
				// x, y, z, w;
				// x - minX, y - minY, z - maxX, w - maxY;
				// 0, 0, fill, 1.f;
			effect->SetTexSrc(src);
		}
	}

	void SpriteEffect::SetWrap(bool wrap)
	{
		renderObj->SetBool("Wrap", wrap);
	}

	void SpriteEffect::SetSRC(bool src)
	{
		renderObj->SetBool("SRC", src);
	}
}
