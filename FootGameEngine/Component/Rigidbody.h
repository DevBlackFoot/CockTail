#pragma once
#include "Object/ComponentBase.h"
#include "Define/FootEngineTypedef.h"
#include "MoolyInfo.h"

namespace GameEngineSpace
{
	class Collider;

	class Transform;

	class Rigidbody : public ComponentBase
	{
	private:
		std::shared_ptr<Collider> collider;
		std::shared_ptr<Transform> transform;
		MoolyEngine::PhysicsAxisLock physAxisLock;
		float mass;
		float damp;
		float angularDamp;
		bool bGravity;
		bool bKinematic;

		bool isInterpolation;

	public:
		float currentTime = 0.f;

		float fixedTimeStep = 0.01666f;

	public:
		Rigidbody(std::weak_ptr<GameObject> _object);
		virtual ~Rigidbody() {}

		void Awake() override;

		void FixedUpdate(float tick) override;

		void AddForce(MoolyEngine::Vector3 force);
		void AddImpulse(MoolyEngine::Vector3 impulse);

		void MoveKinematic(MoolyEngine::Vector3 position);

		void SetMass(float _mass);
		void SetAngularDamp(float damp);
		void SetLinearDamp(float damp);
		
		void SetMaxAngularVelocity(float veloicty);
		void SetMaxLinearVelocity(float velocity);
		void SetAxisLock(MoolyEngine::PhysicsAxisLock axisLock);
		void SetPhysAxisLock(bool _lx, bool _ly, bool _lz, bool _ax, bool _ay, bool _az);

		void SetLinearVelocity(Vector3 velocity);
		void SetAngularVelocity(Vector3 angular);

		void SetMaxAngularDampVelocity(float velocity);
		void SetMaxLinearDampVelocity(float velocity);

		void SetGravity(bool value);
		void SetKinematic(bool value);

		void SetCollider(std::shared_ptr<Collider> _collider) { collider = _collider; }

		MoolyEngine::PhysicsAxisLock GetPhysicsAxisLock() { return physAxisLock; }

		float GetMass() { return mass;  }
		bool GetGravity() { return bGravity;}
		bool GetKinematic() { return bKinematic; }

		Vector3 GetLinearVelocity();

		Vector3 GetAngularVelocity();

		bool GetInterpolation() { return isInterpolation; }

		void SetInterpolation(bool val);

		float GetCurTime() { return currentTime; }
	};
}