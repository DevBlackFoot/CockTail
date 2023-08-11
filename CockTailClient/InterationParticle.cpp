#include "pch.h"
#include "InterationParticle.h"

#include "Component/Particle.h"

namespace ClientSpace
{
	InteractionParticle::InteractionParticle(std::weak_ptr<GameObject> _obj)
		: ComponentBase(_obj)
		, isPlay(false)
	{
	}

	void InteractionParticle::Start()
	{
		gameManager = GameManager::GetInstance();
		particle = GetGameObject()->GetComponent<Particle>();

		// 파티클이 먼저 Start가 불릴 것이라 가정
		particle->Stop();
	}

	void InteractionParticle::Update(float tick)
	{
		// 죽고 나서만 실행 시켜줍시다.
		if(gameManager->playerDeadCnt < 1)
			return;

		if(isPlay != true)
		{
			// 파티클 실행만 시켜줍니다.
			particle->Play();
			isPlay = true;
		}
	}
}
