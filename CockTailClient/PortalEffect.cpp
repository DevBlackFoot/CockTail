#include "pch.h"
#include "PortalEffect.h"

#include "Component/Particle.h"
#include "GameManager.h"

namespace ClientSpace
{
	PortalEffect::PortalEffect(std::weak_ptr<GameObject> _obj)
		: ComponentBase(_obj)
		, goalCount(0)
		, playEffect(false)
		, stopEffect(false)
	{
	}

	PortalEffect::~PortalEffect()
	{
	}

	void PortalEffect::Init(int goalCount)
	{
		gameManager = GameManager::GetInstance();
		this->goalCount = goalCount;
	}

	void PortalEffect::Play()
	{
		if (playEffect != true)
		{
			playEffect = true;
			portalParticle->Start();
			SoundManager::GetInstance()->PlaySFXSound("PortalOpen");
		}
	}

	void PortalEffect::Stop()
	{
		if (stopEffect != true)
		{
			portalParticle->Stop();
			stopEffect = true;
		}
	}

	void PortalEffect::Awake()
	{
		portalParticle = GetGameObject()->GetComponent<Particle>();
	}

	void PortalEffect::Update(float tick)
	{
		if (gameManager->GetKillCount() < goalCount)
		{
			if (stopEffect != true)
			{
				portalParticle->Stop();
				stopEffect = true;
			}
			return;
		}
		else if (SceneManager::GetInstance()->GetCurrentScene()->GetSceneName() == L"Room1Scene" && GameManager::isNeedKeyCutScenePlay == false)
		{
			if (playEffect != true)
				return;
		}

		if (playEffect != true)
		{
			playEffect = true;
			portalParticle->Start();
			SoundManager::GetInstance()->PlaySFXSound("PortalOpen");
		}
	}
}
