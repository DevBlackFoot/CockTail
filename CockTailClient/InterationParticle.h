#pragma once

namespace GameEngineSpace
{
	class Particle;
}

namespace ClientSpace
{
	// �κ� �� ���ͷ����� �ʿ��� �κп��� Play��Ű�� ���ͷ��� ���� ��ƼŬ �Դϴ�.
		// �ϴ��� �κ񿡼��� �����ϵ��� �ϰ�, �ʿ��ϴٸ� �������̿����� �߰��մϴ�.
	class InteractionParticle : public ComponentBase
	{
		// Ư�� ���ǿ��� �÷��̸� �ѹ��� �����ֱ� ���ؼ�
		bool isPlay;
		// �� ���� ������Ʈ�� ������ �ִ� ��ƼŬ ������Ʈ�� ĳ�����ݴϴ�.
		std::shared_ptr<GameEngineSpace::Particle> particle;
		std::shared_ptr<GameManager> gameManager;

		// �������� ��ƼŬ�� �����ϰ� ������ݴϴ�.
	public:
		InteractionParticle(std::weak_ptr<GameObject> _obj);
		virtual ~InteractionParticle() {};

		virtual void Start() override;

		virtual void Update(float tick) override;

	};

}
