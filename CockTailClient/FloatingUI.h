#pragma once

namespace GameEngineSpace
{
	class SpriteEffect;
}

namespace ClientSpace
{
	enum class FloatingUIName
	{
		MOVE,
		ATTACK,
		DODGE,
		NONE,
	};

	// ���� �Ŵ����� ���ο��� ĳ���ϵ��� �մϴ�.
	class GameManager;

	// Tutorial�� Lobby���� �����Ǵ� Floating UI�� ���� ������Ʈ �Դϴ�.
	class FloatingUI : public ComponentBase
	{
	private:
		std::shared_ptr<GameEngineSpace::SpriteEffect> spriteEffect;
		std::shared_ptr<GameManager> gameManager;
		std::shared_ptr<Transform> transform;

		// �󸶳� ���� ����� ���ΰ�.
		float alphaSpeed;
		float alpha;
		// ����ũ �÷�.
		SimpleMath::Vector4 maskColor;
		// �� ��������Ʈ�� ����� �Ǿ��°�..
		bool isPlay;
		FloatingUIName nowUIName;

	public:
		FloatingUI(std::weak_ptr<GameObject> _object);
		virtual ~FloatingUI();

		// UI ������ �����ϵ��� �մϴ�.
		void Init(std::string objName, FloatingUIName UI);
		
		// ��ǻ� ������Ʈ���� ���� �Ŵ������� Ư�� ������ ȹ���ؼ�
		// ���İ��� ������ ����ָ� �˴ϴ�..
		void Update(float tick) override;
		void SetPlay(bool isPlay) { this->isPlay = isPlay; }
	};

}
