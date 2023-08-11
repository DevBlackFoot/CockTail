#include "GamePch.h"
#include "BoxCollider.h"

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
	BoxCollider::BoxCollider(std::weak_ptr<GameObject> _object) 
		: Collider(_object), bDebug(true)
	{
		colliderType = MoolyEngine::GeometryType::BOX;
	}

	BoxCollider::BoxCollider(std::weak_ptr<GameObject> _object, Vector3 _pivot, Vector3 _size) :
		Collider(_object), bDebug(false)
	{
		colliderType = MoolyEngine::GeometryType::BOX;
		center = _pivot;
		boxSize = _size;
	}

	BoxCollider::~BoxCollider()
	{
	}

	void BoxCollider::Awake()
	{	
		PhysicsManager::GetInstance()->AddObject(this);

		// 부모 Collider 생성자에서 rigidbody 캐싱해줌
		if (rigidbody != nullptr)
		{
			// 처음 Rigidbody 설정하고 Collider를 추가하면서 정보를 넘겨준다.
			PhysicsManager::GetInstance()->SetAxisLock(this, rigidbody->GetPhysicsAxisLock());
			PhysicsManager::GetInstance()->SetMass(this, rigidbody->GetMass());
			PhysicsManager::GetInstance()->SetGravity(this, rigidbody->GetGravity());
			PhysicsManager::GetInstance()->SetKinematic(this, rigidbody->GetKinematic());
			PhysicsManager::GetInstance()->SetInterpolate(this, rigidbody->GetInterpolation());
			isInterpolate = rigidbody->GetInterpolation();
		}

		// 트리거를 켜줬다면! 트리거라고 알려준다.
		if(bTrigger)
			PhysicsManager::GetInstance()->SetTrigger(this);

		PhysicsManager::GetInstance()->SetLayer(this, layer);

		SceneManager::GetInstance()->GetCurrentScene()->AddCollider(this->shared_from_this());

		PhysicsManager::GetInstance()->AddPhysicsQueue(this, transform.get());

		if (rigidbody != nullptr && rigidbody->GetInterpolation())
		{
			const Vector3& position = transform->GetWorldPosition();

			const Quaternion& rotation = transform->GetWorldRotation();

			PhysicsManager::GetInstance()->physicsInterpolateDatas.insert({ GetGameObject()->GetObjectID(),
					{ transform.get(), { {position, rotation, 0.f}, {position, rotation,0.f} } } });
		}

		if (rigidbody != nullptr)
			rigidbody->SetCollider(GetGameObject()->GetComponent<BoxCollider>());

		renderObj = Factory::GetInstance()->CreateDXObject<LineObj>(
			BuilderManager::GetInstance()->GetBuilder("LineBuilder"), "WireCube");

		GraphicsManager::GetInstance()->GetRenderer()->InitObject(renderObj);
	}

	void BoxCollider::DrawDebugRender()
	{
		// TODO 콜라이더렌더
	}

	void BoxCollider::ColliderRender()
	{
		if (bDebug != true)
			return;

		Matrix worldPos = XMMatrixTranslation(transform->GetWorldPosition().x + center.x, transform->GetWorldPosition().y + center.y, transform->GetWorldPosition().z + center.z);
		Matrix worldScale = XMMatrixScaling(boxSize.x, boxSize.y, boxSize.z);
		Matrix worldRot = XMMatrixRotationQuaternion(transform->GetWorldRotation());

		Matrix worldTM = worldScale * worldRot * worldPos;

		auto cam = SceneManager::GetInstance()->GetCurrentScene()->GetMainCamera();

		renderObj->Update(worldTM, cam->GetViewMatrix(), cam->GetProjMatrix());
		renderObj->Render();		
	}
}
