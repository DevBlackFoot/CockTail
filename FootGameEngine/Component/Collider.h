#pragma once
#include "Object/ComponentBase.h"
#include "IMoolyEngine.h"

namespace GraphicsEngineSpace
{
	class IDXObject;
}

namespace GameEngineSpace
{
	class Transform;
	class Rigidbody;

	class Collider : public ComponentBase, public std::enable_shared_from_this<Collider>
	{
	protected:
		std::shared_ptr<Transform> transform;
		std::shared_ptr<Rigidbody> rigidbody;
		MoolyEngine::GeometryType colliderType;
		MoolyEngine::PhysicsType physicsType;
		std::string layer;
		bool bTrigger;

		DirectX::SimpleMath::Vector3 center;

		std::shared_ptr<GraphicsEngineSpace::IDXObject> renderObj;

	public:
		bool isInterpolate = false;

	public:
		Collider(std::weak_ptr<GameObject> _object);
		~Collider();

		void Awake() override;
		void Start() override;
		void FixedUpdate(float tick) override;
		void Update(float tick) override;
		void LateUpdate(float tick) override;
		void OnEnable() override;
		void OnDisable() override;

		void Release() override;

		MoolyEngine::GeometryType GetColliderType() const { return colliderType; }
		MoolyEngine::PhysicsType GetPhysicsType() const { return physicsType; }
		std::shared_ptr<Transform> GetTransform() { return transform; }
		std::string GetLayer() const { return layer; }
		DirectX::SimpleMath::Vector3 GetCenter() const { return center; }

		void SetPhysicsType(MoolyEngine::PhysicsType value) { physicsType = value; }
		void SetTrigger(bool value);
		void SetCenter(Vector3 _value) { center = _value; }

		void SetLayer(std::string value);

		bool IsTrigger() const { return bTrigger; }

		void AddTriggerEvent(MoolyEngine::IEventTrigger* callbackClass);
		void AddColliderEvent(MoolyEngine::IEventCollider* callbackClass);

		void ChangeTriggerShape(bool value);
		virtual void ColliderRender() abstract;

		void DeleteCollider();

		void SetRigidbody(std::shared_ptr<Rigidbody> _rigidbody);
	};
}