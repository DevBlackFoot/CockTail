#include "GamePch.h"
#include "Collider.h"

#include "Object/GameObject.h"
#include "Transform.h"

#include "PhysicsManager.h"
#include "SceneManager.h"
#include "SceneBase.h"

namespace GameEngineSpace
{
	Collider::Collider(std::weak_ptr<GameObject> _object)
		: ComponentBase(_object, ComponentType::COLLIDER)
		, colliderType(MoolyEngine::GeometryType::INVALID)
		, bTrigger(false)
		, renderObj(nullptr)
	{
		transform = GetGameObject()->GetComponent<Transform>();
		rigidbody = GetGameObject()->GetComponent<Rigidbody>();
		layer = GetGameObject()->GetTag();

		// Rigidbody를 추가할 일이 있다면 먼저 Rigidbody부터 Add할 것
		if (rigidbody == nullptr)
		{
			physicsType = MoolyEngine::PhysicsType::STATIC;
		}
		else
		{
			physicsType = MoolyEngine::PhysicsType::DYNAMIC;
		}
	}

	Collider::~Collider()
	{

	}

	void Collider::Awake()
	{
		
	}

	void Collider::Start()
	{
		
	}

	void Collider::FixedUpdate(float tick)
	{
		
	}

	void Collider::Update(float tick)
	{

	}

	// 모든 transform이 연산이 되고 이녀석의 위치 정보를 physics에 넘겨준다.
	void Collider::LateUpdate(float tick)
	{
		
	}

	void Collider::OnEnable()
	{
		PhysicsManager::GetInstance()->EnableObject(this);
	}

	void Collider::OnDisable()
	{
		PhysicsManager::GetInstance()->DisableObject(this);
	}

	void Collider::Release()
	{
		// PhysicsManager::GetInstance()->DeleteObject(this);
	}

	void Collider::SetTrigger(bool value)
	{
		bTrigger = value;
	}

	void Collider::SetLayer(std::string value)
	{
		layer = value;

		PhysicsManager::GetInstance()->SetLayer(this, value);
	}

	void Collider::AddTriggerEvent(MoolyEngine::IEventTrigger* callbackClass)
	{
		PhysicsManager::GetInstance()->AddTriggerEvent(this, callbackClass);
	}

	void Collider::AddColliderEvent(MoolyEngine::IEventCollider* callbackClass)
	{
		PhysicsManager::GetInstance()->AddColliderEvent(this, callbackClass);
	}
	void Collider::ChangeTriggerShape(bool value)
	{
		SetTrigger(value);

		PhysicsManager::GetInstance()->SetTriggerShape(this);
	}

	void Collider::DeleteCollider()
	{
		PhysicsManager::GetInstance()->DeleteObject(this);
	}


	void Collider::SetRigidbody(std::shared_ptr<Rigidbody> _rigidbody)
	{
		rigidbody = _rigidbody;

		physicsType = MoolyEngine::PhysicsType::DYNAMIC;
	}
}
