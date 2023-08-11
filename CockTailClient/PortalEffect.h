#pragma once

namespace GameEngineSpace
{
	class Particle;
}

namespace ClientSpace
{

	// ���� �� ��Ȳ�� ���� ��Ż ����Ʈ�� ��� �����ִ� Ŭ����
	class PortalEffect : public ComponentBase
	{
	private:
		// ��Ż ����Ʈ�� �� ������Ʈ�� ���� ���� ������Ʈ�� ������ �־�� �մϴ�.
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
