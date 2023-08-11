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

	// ��ƼŬ�� �׸� �� �ֵ��� ������ִ� ��ƼŬ ������Ʈ �Դϴ�.
	class Particle : public RendererBase
	{
		// ĳ�̿�..
		std::shared_ptr<GraphicsEngineSpace::ParticleEffect> particle;

		// ȸ�� ������.. �� ����
		bool lockRotation;

	public:
		Particle(std::weak_ptr<GameObject> object);

		virtual ~Particle() {}

	public:
		// ������ �ʿ��� init.
		void Init(std::string particleName);

		void LoadParticlePrefab(std::string particleName);

		void Start() override;

		void Render() override;
		void Render(std::shared_ptr<GraphicsEngineSpace::IRenderer> renderer) override;
		void YieldRender(std::shared_ptr<GraphicsEngineSpace::IRenderer> renderer) override;
		void PrepareRender(float tick) override;

		// �׽�Ʈ�� �����
		void DebugIMGUIRender(float tick) override;

		float GetPlayTime(int meshIdx);
		void Stop();
		void Play();

		void SetTotalTime(float totalTime) override;
		void SetLockRotation(bool lock) { this->lockRotation = lock; }

		// �̹����� �����ݴϴ�,
		void SetTexRotation(float radian);
		void SetTexture(int meshIdx, const std::shared_ptr<GraphicsEngineSpace::Texture>& texture);
	};

}
