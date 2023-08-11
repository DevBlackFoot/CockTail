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

		// ��ƼŬ�� ���� Start�� �Ҹ� ���̶� ����
		particle->Stop();
	}

	void InteractionParticle::Update(float tick)
	{
		// �װ� ������ ���� �����ݽô�.
		if(gameManager->playerDeadCnt < 1)
			return;

		if(isPlay != true)
		{
			// ��ƼŬ ���ุ �����ݴϴ�.
			particle->Play();
			isPlay = true;
		}
	}
}
