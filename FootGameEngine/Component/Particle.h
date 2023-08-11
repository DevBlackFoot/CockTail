#pragma once
#include "RendererBase.h"
#include "Define/FootEngineTypedef.h"

namespace GraphicsEngineSpace
{
	class ParticleEffect;
	class Texture;
}

namespace GameEngineSpace
{

	// 파티클을 그릴 수 있도록 만들어주는 파티클 컴포넌트 입니다.
	class Particle : public RendererBase
	{
		// 캐싱용..
		std::shared_ptr<GraphicsEngineSpace::ParticleEffect> particle;

		// 회전 금지용.. 불 변수
		bool lockRotation;

	public:
		Particle(std::weak_ptr<GameObject> object);

		virtual ~Particle() {}

	public:
		// 빌더에 필요한 init.
		void Init(std::string particleName);

		void LoadParticlePrefab(std::string particleName);

		void Start() override;

		void Render() override;
		void Render(std::shared_ptr<GraphicsEngineSpace::IRenderer> renderer) override;
		void YieldRender(std::shared_ptr<GraphicsEngineSpace::IRenderer> renderer) override;
		void PrepareRender(float tick) override;

		// 테스트용 디버그
		void DebugIMGUIRender(float tick) override;

		float GetPlayTime(int meshIdx);
		void Stop();
		void Play();

		void SetTotalTime(float totalTime) override;
		void SetLockRotation(bool lock) { this->lockRotation = lock; }

		// 이미지를 돌려줍니다,
		void SetTexRotation(float radian);
		void SetTexture(int meshIdx, const std::shared_ptr<GraphicsEngineSpace::Texture>& texture);
	};

}
