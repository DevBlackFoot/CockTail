#pragma once

namespace GameEngineSpace
{
	class Particle;
}

namespace ClientSpace
{
	// 로비 등 인터렉션이 필요한 부분에서 Play시키는 인터렉션 관련 파티클 입니다.
		// 일단은 로비에서만 동작하도록 하고, 필요하다면 엘레베이에서도 추가합니다.
	class InteractionParticle : public ComponentBase
	{
		// 특정 조건에서 플레이를 한번만 시켜주기 위해서
		bool isPlay;
		// 이 게임 오브젝트가 가지고 있는 파티클 컴포넌트를 캐싱해줍니다.
		std::shared_ptr<GameEngineSpace::Particle> particle;
		std::shared_ptr<GameManager> gameManager;

		// 나머지는 파티클과 동일하게 사용해줍니다.
	public:
		InteractionParticle(std::weak_ptr<GameObject> _obj);
		virtual ~InteractionParticle() {};

		virtual void Start() override;

		virtual void Update(float tick) override;

	};

}
