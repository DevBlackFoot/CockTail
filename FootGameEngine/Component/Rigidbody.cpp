#include "GamePch.h"
#include "Rigidbody.h"

#include "PhysicsManager.h"
#include "Object/GameObject.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"

#include "Timer.h"

namespace GameEngineSpace
{
	Rigidbody::Rigidbody(std::weak_ptr<GameObject> _object)
		: ComponentBase(_object, ComponentType::RIGIDBODY)
		, bGravity(true)
		, bKinematic(false)
		, mass(2)
		, damp(0)
		, angularDamp(0)
		, isInterpolation(false)
	{
		transform = GetGameObject()->GetComponent<Transform>();
	}

	void Rigidbody::Awake()
	{
	}

	void Rigidbody::FixedUpdate(float tick)
	{
		
	}

	void Rigidbody::AddForce(MoolyEngine::Vector3 force)
	{
		PhysicsManager::GetInstance()->AddForce(collider.get(), force);
	}

	void Rigidbody::AddImpulse(MoolyEngine::Vector3 impulse)
	{
		PhysicsManager::GetInstance()->AddImpulse(collider.get(), impulse);
	}

	void Rigidbody::MoveKinematic(MoolyEngine::Vector3 position)
	{
		PhysicsManager::GetInstance()->MoveKinematicObject(collider.get(), position);
	}

	void Rigidbody::SetMass(float _mass)
	{
		mass = _mass;

		if (collider != nullptr)
		{
			PhysicsManager::GetInstance()->SetMass(collider.get(), mass);
		}
	}

	void Rigidbody::SetAngularDamp(float damp)
	{
		PhysicsManager::GetInstance()->SetAngularDamp(collider.get(), damp);
	}

	void Rigidbody::SetLinearDamp(float damp)
	{
		PhysicsManager::GetInstance()->SetLinearDamp(collider.get(), damp);
	}

	void Rigidbody::SetMaxAngularVelocity(float veloicty)
	{
		PhysicsManager::GetInstance()->SetMaxAngularVelocity(collider.get(), veloicty);
	}

	void Rigidbody::SetMaxLinearVelocity(float velocity)
	{
		PhysicsManager::GetInstance()->SetMaxLinearVelocity(collider.get(), velocity);
	}

	void Rigidbody::SetAxisLock(MoolyEngine::PhysicsAxisLock axisLock)
	{
		PhysicsManager::GetInstance()->SetAxisLock(collider.get(), axisLock);
	}

	void Rigidbody::SetPhysAxisLock(bool _lx, bool _ly, bool _lz, bool _ax, bool _ay, bool _az)
	{
		physAxisLock = { _lx, _ly, _lz, _ax, _ay, _az };
		if (collider != nullptr)
		{
			SetAxisLock(physAxisLock);
		}
	}

	void Rigidbody::SetLinearVelocity(Vector3 velocity)
	{
		PhysicsManager::GetInstance()->SetLinearVelocity(collider.get(), {velocity.x, velocity.y, velocity.z});
	}

	void Rigidbody::SetAngularVelocity(Vector3 angular)
	{
		PhysicsManager::GetInstance()->SetAngularVelocity(collider.get(), { angular.x, angular.y, angular.z });
	}

	void Rigidbody::SetMaxAngularDampVelocity(float velocity)
	{
		PhysicsManager::GetInstance()->SetMaxAngularVelocity(collider.get(), velocity);
	}

	void Rigidbody::SetMaxLinearDampVelocity(float velocity)
	{
		PhysicsManager::GetInstance()->SetMaxLinearVelocity(collider.get(), velocity);
	}

	void Rigidbody::SetGravity(bool value)
	{
		bGravity = value;

		if (collider != nullptr)
		{
			PhysicsManager::GetInstance()->SetGravity(collider.get(), value);
		}
	}

	void Rigidbody::SetKinematic(bool value)
	{
		bKinematic = value;

		if (collider != nullptr)
		{
			PhysicsManager::GetInstance()->SetKinematic(collider.get(), value);
		}
	}

	DirectX::SimpleMath::Vector3 Rigidbody::GetLinearVelocity()
	{
		return PhysicsManager::GetInstance()->GetLinearVelocity(collider.get());
	}

	DirectX::SimpleMath::Vector3 Rigidbody::GetAngularVelocity()
	{
		return PhysicsManager::GetInstance()->GetAngularVelocity(collider.get());
	}

	void GameEngineSpace::Rigidbody::SetInterpolation(bool val)
	{
		isInterpolation = val;

		if (isInterpolation)
		{
			currentTime = Timer::GetInstance()->TotalTime();

			fixedTimeStep = PhysicsManager::GetInstance()->GetFixedTimeStep();

			transform->isInterpolate = true;
		}
	}
}