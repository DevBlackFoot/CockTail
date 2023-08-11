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

	// 게임 매니저를 내부에서 캐싱하도록 합니다.
	class GameManager;

	// Tutorial과 Lobby에서 렌더되는 Floating UI를 위한 컴포넌트 입니다.
	class FloatingUI : public ComponentBase
	{
	private:
		std::shared_ptr<GameEngineSpace::SpriteEffect> spriteEffect;
		std::shared_ptr<GameManager> gameManager;
		std::shared_ptr<Transform> transform;

		// 얼마나 빨리 밝아질 것인가.
		float alphaSpeed;
		float alpha;
		// 마스크 컬러.
		SimpleMath::Vector4 maskColor;
		// 이 스프라이트가 재생이 되었는가..
		bool isPlay;
		FloatingUIName nowUIName;

	public:
		FloatingUI(std::weak_ptr<GameObject> _object);
		virtual ~FloatingUI();

		// UI 종류를 지정하도록 합니다.
		void Init(std::string objName, FloatingUIName UI);
		
		// 사실상 업데이트에서 게임 매니저에서 특정 정보를 획득해서
		// 알파값을 변경해 띄워주면 됩니다..
		void Update(float tick) override;
		void SetPlay(bool isPlay) { this->isPlay = isPlay; }
	};

}
