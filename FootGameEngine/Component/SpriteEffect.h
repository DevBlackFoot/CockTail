#pragma once
#include "RendererBase.h"
#include "Define/FootEngineTypedef.h"

namespace GraphicsEngineSpace
{
	class SpriteEffect;
}

namespace GameEngineSpace
{

	// 2D 스프라이트 이펙트를 뽑아내기 위한 렌더러 컴포넌트 입니다.
	class SpriteEffect : public RendererBase
	{
	public:
		SpriteEffect(std::weak_ptr<GameObject> object);
		virtual ~SpriteEffect() {}

		std::shared_ptr<GraphicsEngineSpace::SpriteEffect> effect;

		bool yPlay;
		float playSpeed;

	public:
		// 여기서 초기화 할 때 이름을 넣습니다
		void Init(std::string objName);

		void Render() override;
		void Render(std::shared_ptr<GraphicsEngineSpace::IRenderer> renderer) override;
		void YieldRender(std::shared_ptr<GraphicsEngineSpace::IRenderer> renderer) override;
		// Update
		void PrepareRender(float tick) override;

		void DebugIMGUIRender(float tick) override;

		void SetMaskColor(const SimpleMath::Vector4& maskColor);
		void SetEmissiveColor(const SimpleMath::Vector4& emissiveColor);
		void SetEffectSrc(const SimpleMath::Vector4& src);

		void SetYPlay(bool yPlay) { this->yPlay = yPlay; }
		void SetPlaySpeed(float speed) { this->playSpeed = speed; }
		void SetWrap(bool wrap);
		void SetSRC(bool src);

		float GetPlaySpeed() { return this->playSpeed; }

	};

}
