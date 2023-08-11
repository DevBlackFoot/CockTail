#include "GamePch.h"
#include "Monobehavior.h"
#include "Collider.h"
#include "Object/GameObject.h"

#include "Manager/PhysicsManager.h"

namespace GameEngineSpace
{
	Monobehavior::Monobehavior(std::weak_ptr<GameObject> _gameObject) :
		ComponentBase(_gameObject, ComponentType::MONOBEHAVIOR)
	{	}

	Monobehavior::~Monobehavior()
	{}

	void Monobehavior::Release()
	{
		__super::Release();
	}

	void Monobehavior::RegisterCallBack()
	{
		const std::function<void(const std::shared_ptr<MoolyEngine::Collision>&)> colEnter = std::bind(&Monobehavior::OnCollisionEnter,
			this, std::placeholders::_1);

		const std::function<void(const std::shared_ptr<MoolyEngine::Collision>&)> colStay = std::bind(&Monobehavior::OnCollisionStay,
			this, std::placeholders::_1);

		const std::function<void(const std::shared_ptr<MoolyEngine::Collision>&)> colExit = std::bind(&Monobehavior::OnCollisionExit,
			this, std::placeholders::_1);

		const std::function<void(const std::shared_ptr<MoolyEngine::Trigger>&)> triEnter = std::bind(&Monobehavior::OnTriggerEnter,
			this, std::placeholders::_1);

		const std::function<void(const std::shared_ptr<MoolyEngine::Trigger>&)> triStay = std::bind(&Monobehavior::OnTriggerStay,
			this, std::placeholders::_1);

		const std::function<void(const std::shared_ptr<MoolyEngine::Trigger>&)> triExit = std::bind(&Monobehavior::OnTriggerExit,
			this, std::placeholders::_1);

		PhysicsManager::GetInstance()->RegisterCallBack(std::to_string(GetGameObject()->GetObjectID()), colEnter, colStay, colExit, triEnter, triStay, triExit);
	}
}