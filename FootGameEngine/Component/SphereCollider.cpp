#include "GamePch.h"
#include "SphereCollider.h"

#include "Object/GameObject.h"
#include "GraphicsManager.h"
#include "PhysicsManager.h"

#include "SceneManager.h"
#include "SceneBase.h"
#include "Camera.h"
#include "Timer.h"

#include "../../FootGraphicsEngine/GraphicsEngineFramework.h"

namespace GameEngineSpace
{
	SphereCollider::SphereCollider(std::weak_ptr<GameObject> _object) :
		Collider(_object), bDebug(true)
	{
		colliderType = MoolyEngine::GeometryType::SPHERE;
	}

	SphereCollider::SphereCollider(std::weak_ptr<GameObject> _object, Vector3 _pivot, float _radius) :
		Collider(_object)
	{
		colliderType = MoolyEngine::GeometryType::SPHERE;
		center = _pivot;
		radius = _radius;
	}

	SphereCollider::~SphereCollider()
	{
	}

	void SphereCollider::Awake()
	{
		PhysicsManager::GetInstance()->AddObject(this);

		// �θ� Collider �����ڿ��� rigidbody ĳ������
		if (rigidbody != nullptr)
		{
			// ó�� Rigidbody �����ϰ� Collider�� �߰��ϸ鼭 ������ �Ѱ��ش�.
			PhysicsManager::GetInstance()->SetAxisLock(this, rigidbody->GetPhysicsAxisLock());
			PhysicsManager::GetInstance()->SetMass(this, rigidbody->GetMass());
			PhysicsManager::GetInstance()->SetGravity(this, rigidbody->GetGravity());
			PhysicsManager::GetInstance()->SetKinematic(this, rigidbody->GetKinematic());
			PhysicsManager::GetInstance()->SetInterpolate(this, rigidbody->GetInterpolation());
			isInterpolate = rigidbody->GetInterpolation();
		}

		// Ʈ���Ÿ� ����ٸ�! Ʈ���Ŷ�� �˷��ش�.
		if (bTrigger)
			PhysicsManager::GetInstance()->SetTrigger(this);

		PhysicsManager::GetInstance()->SetLayer(this, layer);

		SceneManager::GetInstance()->GetCurrentScene()->AddCollider(this->shared_from_this());

		PhysicsManager::GetInstance()->AddPhysicsQueue(this, transform.get());

		if (rigidbody != nullptr && rigidbody->GetInterpolation())
		{
			const Vector3& position = transform->GetWorldPosition();

			const Quaternion& rotation = transform->GetWorldRotation();

			PhysicsManager::GetInstance()->physicsInterpolateDatas.insert({ GetGameObject()->GetObjectID(),
					{ transform.get(), { {position, rotation, 0.f}, {position, rotation, 0.f} } } });
		}

		if (rigidbody != nullptr)
			rigidbody->SetCollider(GetGameObject()->GetComponent<SphereCollider>());

		renderObj = Factory::GetInstance()->CreateDXObject<LineObj>(
			BuilderManager::GetInstance()->GetBuilder("LineBuilder"), "WireSphere");

		GraphicsManager::GetInstance()->GetRenderer()->InitObject(renderObj);
	}

	
	void SphereCollider::Update(float tick)
	{
		if (bDebug)
			DrawDebugRender();
	}

	void SphereCollider::DrawDebugRender()
	{

	}

	void SphereCollider::ColliderRender()
	{
		if (bDebug != true)
			return;

		Matrix worldPos = XMMatrixTranslation(transform->GetWorldPosition().x, transform->GetWorldPosition().y, transform->GetWorldPosition().z);
		Matrix worldScale = XMMatrixScaling(radius, radius, radius);
		Matrix worldRot = XMMatrixRotationQuaternion(transform->GetWorldRotation());

		Matrix worldTM = worldScale * worldRot * worldPos;

		auto cam = SceneManager::GetInstance()->GetCurrentScene()->GetMainCamera();

		renderObj->Update(worldTM, cam->GetViewMatrix(), cam->GetProjMatrix());
		renderObj->Render();
	}
}
