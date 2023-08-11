#include "pch.h"
#include "FloatingUI.h"

#include "../../FootGraphicsEngine/GraphicsEngineFramework.h"

#include "Component/SpriteEffect.h"
#include "GameManager.h"

namespace ClientSpace
{
	FloatingUI::FloatingUI(std::weak_ptr<GameObject> _object)
		: ComponentBase(_object)
		, alphaSpeed(0.5f)
		, isPlay(false)
		, alpha(0.f)
		, maskColor(1.0f, 1.0f, 1.0f, 1.0f)
		, nowUIName(FloatingUIName::NONE)
	{

	}

	FloatingUI::~FloatingUI()
	{
	}

	void FloatingUI::Init(std::string objName, FloatingUIName UI)
	{
		// 각종 정보 캐싱 및 transform등 세팅.
		auto gameObj = GetGameObject();

		transform = gameObj->GetComponent<Transform>();
		gameManager = GameManager::GetInstance();

		// 추가
		spriteEffect = gameObj->AddComponent<GameEngineSpace::SpriteEffect>();
		spriteEffect->Init(objName);

		maskColor.w = alpha;
		spriteEffect->SetMaskColor(maskColor);

		switch (UI)
		{
		case FloatingUIName::MOVE:
		{
			transform->SetPosition({ -8.0f, 2.0f, -23.0f });
			transform->SetLocalEulerRotation({ 90.f, 45.f, 0.f });
			transform->SetScale({ 4.f, 1.5f, 1.f });

			nowUIName = FloatingUIName::MOVE;
		}
		break;

		case FloatingUIName::ATTACK:
		{
			transform->SetPosition({ -6.0f, 1.0f, -35.5f });
			transform->SetLocalEulerRotation({ 90.f, 45.f, 0.f });
			transform->SetScale({ 3.5f, 1.2f, 1.f });

			nowUIName = FloatingUIName::ATTACK;

		}
		break;

		case FloatingUIName::DODGE:
		{
			transform->SetPosition({ 0.3f, 1.0f, -41.7f });
			transform->SetLocalEulerRotation({ 90.f, 45.f, 0.f });
			transform->SetScale({ 3.5f, 1.2f, 1.f });

			nowUIName = FloatingUIName::DODGE;
		}
		break;

		default:
			break;
		}
	}

	void FloatingUI::Update(float tick)
	{
		// isPlay를 가져옵니다.
		switch (nowUIName)
		{
		case FloatingUIName::MOVE:
			isPlay = gameManager->GetLobbyPlay();
			break;

		case FloatingUIName::ATTACK:
		case FloatingUIName::DODGE:
			isPlay = gameManager->GetTutorialPlay();
			break;

		case FloatingUIName::NONE:
		default:
			isPlay = false;
			break;
		}

		if (tick <= 0.f || isPlay == false)
			return;

		if(alpha < 0.1f)
			alpha += tick * alphaSpeed * 0.5f;
		else
			alpha += tick * alphaSpeed;

		if (alpha > 1.0f)
			alpha = 1.0f;

		maskColor.w = alpha;
		spriteEffect->SetMaskColor(maskColor);
	}
}
