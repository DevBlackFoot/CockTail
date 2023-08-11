#pragma once

namespace GameEngineSpace
{
	class Particle;
}

namespace ClientSpace
{

	// 현재 씬 상황에 따라 포탈 이펙트를 재생 시켜주는 클래스
	class PortalEffect : public ComponentBase
	{
	private:
		// 포탈 이펙트는 이 컴포넌트를 가진 게임 오브젝트가 가지고 있어야 합니다.
		std::shared_ptr<GameEngineSpace::Particle> portalParticle;
		std::shared_ptr<GameManager> gameManager;

		int goalCount;
		bool playEffect;
		bool stopEffect;

	public:
		PortalEffect(std::weak_ptr<GameObject> _obj);
		virtual ~PortalEffect();

		void Init(int goalCount);
		void Play();
		void Stop();

		virtual void Awake() override;
		virtual void Update(float tick) override;
	};

}
