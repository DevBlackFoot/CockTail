#include "GamePch.h"
#include "Particle.h"

#include "../../FootGraphicsEngine/Interface/IDXObject.h"
#include "../../FootGraphicsEngine/GraphicsEngineFramework.h"

#include "Component/Camera.h"
#include "Object/GameObject.h"
#include "SceneBase.h"
#include "SceneManager.h"
#include "GraphicsManager.h"
#include "InputManager.h"

namespace GameEngineSpace
{
	Particle::Particle(std::weak_ptr<GameObject> object)
		: RendererBase(object)
		, particle(nullptr)
		, lockRotation(false)
	{
		// Awake ���� particle�� true�� �ص־��մϴ�.
		isParticle = true;
	}

	void Particle::Init(std::string particleName)
	{
		renderObj = Factory::GetInstance()->CreateDXObject<ParticleEffect>(
			BuilderManager::GetInstance()->GetBuilder("ParticleEffectBuilder"), particleName);

		GraphicsManager::GetInstance()->GetRenderer()->InitObject(renderObj);
	}

	void Particle::LoadParticlePrefab(std::string particleName)
	{
		renderObj = ResourceManager::GetInstance()->GetParticlePrefab(particleName);
	}

	void Particle::Start()
	{
		particle = std::dynamic_pointer_cast<ParticleEffect>(renderObj);

		if (particle != nullptr)
		{
			particle->Play();
		}
	}

	void Particle::Render()
	{
		renderObj->Render();
	}

	void Particle::Render(std::shared_ptr<GraphicsEngineSpace::IRenderer> renderer)
	{
		renderObj->Render(renderer);
	}

	void Particle::YieldRender(std::shared_ptr<GraphicsEngineSpace::IRenderer> renderer)
	{
		renderObj->YieldRender(renderer);
	}

	void Particle::PrepareRender(float tick)
	{
		// ���⼭ ������Ʈ ����� ���ݴϴ�.
		std::shared_ptr<Camera> mainCam = SceneManager::GetInstance()->GetCurrentScene()->GetMainCamera();

		// ������Ʈ�� ���� ���ݴϴ�.
			// ���⼭ Perspective �������� ����� �־��ݴϴ�.
		auto pos = this->transform->GetWorldPosition();
		if (lockRotation == true)
			renderObj->Update(Matrix::CreateTranslation(pos), mainCam->GetViewMatrix(), mainCam->GetProjMatrix());
		else
		{
			auto rot = this->transform->GetWorldRotation();

			// ���⼭ �����̼��� �־ �����ݴϴ�.
			renderObj->Update(Matrix::CreateFromQuaternion(rot) * Matrix::CreateTranslation(pos), mainCam->GetViewMatrix(), mainCam->GetProjMatrix());
		}

		renderObj->PreRender(tick);

		/*if(InputManager::GetInstance()->GetInputState('N', KeyState::DOWN))
			particle->Play();*/
	}

	void Particle::DebugIMGUIRender(float tick)
	{
		/*

		auto imgui = ImGUIManager::GetInstance();

		auto gameObjName = GetGameObject()->GetObjectID();

		if (imgui->TreeNode("Sprite Effect Info", "Particle Effect" + std::to_string(gameObjName)))
		{
			auto pos = transform->GetWorldPosition();
			float posf[3] = { pos.x, pos.y, pos.z };

			auto scale = transform->GetWorldScale();
			float scalef[3] = { scale.x, scale.y, scale.z };

			static float rotf = 0.f;

			imgui->DragFloat3("Position", posf, 0.5f);
			imgui->DragFloat3("Scale", scalef, 0.1f, 1.f, 20.f);
			imgui->DragFloat("Rotation", &rotf, 1.f, 0.f, 360.f);

			transform->SetPosition(Vector3{ posf[0], posf[1], posf[2] });
			transform->SetScale(Vector3{ scalef[0], scalef[1], scalef[2] });

			float rad = XMConvertToRadians(rotf);

			particle->SetTexRotation(rad);

			imgui->TreePop();
			imgui->Separator();
		}
		 */
	}

	float Particle::GetPlayTime(int meshIdx)
	{
		float ret = 0.f;

		if (particle != nullptr)
		{
			ret = particle->GetPlayTime(meshIdx);
		}

		return ret;
	}

	void Particle::Stop()
	{
		if (particle != nullptr)
			particle->Stop();
	}

	void Particle::Play()
	{
		if (particle != nullptr)
		{
			particle->Play();
		}
	}

	void Particle::SetTotalTime(float totalTime)
	{
		renderObj->SetTotalTime(totalTime);
	}

	void Particle::SetTexRotation(float radian)
	{
		if (particle != nullptr)
			particle->SetTexRotation(radian);
	}

	void Particle::SetTexture(int meshIdx, const std::shared_ptr<GraphicsEngineSpace::Texture>& texture)
	{
		if(particle != nullptr)
			particle->SetParticleTexture(meshIdx, texture);
	}
}
