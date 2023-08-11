#include "pch.h"
#include "ItemObject.h"
#include "Component/SphereCollider.h"
#include "Scripts/PlayerController.h"

#include "../../FootGraphicsEngine/GraphicsEngineFramework.h"

namespace ClientSpace
{
	void ItemObject::PushObjectRandomVector(float minAngle, float maxAngle)
	{
		auto gameManager = GameManager::GetInstance();
		Vector3 _vector = gameManager->RotateVector(gameManager->CreateRandomFloat(minAngle, maxAngle), 0.0f, gameManager->CreateRandomFloat(minAngle, maxAngle));
		_vector *= 50.0f;
		rigid->AddForce({ _vector.x, _vector.y, _vector.z });
	}

	void ItemObject::SetPlayer()
	{
		auto _player = SceneManager::GetInstance()->FindObjectName(L"Player");
		if (_player == nullptr)
			return;

		playerTransform = _player->GetComponent<Transform>();
	}

	void ItemObject::CalDist()
	{
		Vector3 _pos = playerTransform->GetWorldPosition();
		float dist = Vector3::Distance(_pos, transform->GetWorldPosition());
		if (dist <= 3.0f)
			isFollow = true;;

		if (dist < 1.3f)
			transform->SetPosition(_pos);
	}

	void ItemObject::ChasePlayer(float tick)
	{
		if (isFollow == false)
			return;

		Vector3 _pos = playerTransform->GetWorldPosition();
		Vector3 _targetPos = Vector3::Lerp(transform->GetWorldPosition(), _pos, tick * 6.0f);
		transform->SetPosition(_targetPos);
	}

	ItemObject::ItemObject(std::weak_ptr<GameObject> _obj)
		: Monobehavior(_obj)
		, isLaunched(false)
	{
		transform = GetGameObject()->GetComponent<Transform>();
		rigid = GetGameObject()->AddComponent<Rigidbody>();
		collider = GetGameObject()->AddComponent<SphereCollider>();

		//transform->SetScale({ 0.2f,0.2f,0.2f });

		GetGameObject()->GetComponent<SphereCollider>()->SetRadius(0.2f);
		/*mesh = GetGameObject()->AddComponent<MeshRenderer>();
		mesh->Init("Warning.model");*/
		collider->SetLayer("Item");

		/*trailObject = make_shared<GameObject>();
		trailObject->AddComponent<Transform>()->SetPosition({ 0.f,0.f,0.f }, Space::LOCAL);*/

		//trail_particle = trailObject->AddComponent<Particle>();
		//trail_particle->LoadParticlePrefab("ItemTrail");

		//GetGameObject()->SetChild(trailObject);


		m_particle = GetGameObject()->AddComponent<Particle>();
		m_particle->LoadParticlePrefab("Item");
		m_particle->SetLockRotation(true);
	}

	ItemObject::~ItemObject()
	{

	}

	void ItemObject::Awake()
	{
		SetPlayer();
		rigid->SetMass(0.2f);
		//trailObject->Awake();
		RegisterCallBack();
	}

	void ItemObject::Start()
	{
		//trailObject->Start();
	}

	void ItemObject::Update(float tick)
	{
		//if (isCollided)
		//{
			//timer -= tick;

			//if (timer <= 0)
			//{
				//trail_particle->Stop();
				//isCollided = false;
			//}
		//}

		if (isLaunched == false)
		{
			isLaunched = true;
			// 생성되는 시점에서 아이템 인덱스를 바탕으로 텍스쳐를 찾아줍니다.
			wstring idx = std::to_wstring(itemIndex - 8);

			m_particle->SetTexture(3, ResourceManager::GetInstance()->LoadPrefabTexture(L"Ingredient" + idx));

			PushObjectRandomVector(-45.0f, 45.0f);
		}

		ChasePlayer(tick);
		CalDist();
	}

	void ItemObject::OnCollisionEnter(const std::shared_ptr<MoolyEngine::Collision>& collision)
	{
		auto otherCollider = static_cast<Collider*>(collision->_other);

		if (otherCollider == nullptr)
			return;

		if (otherCollider->GetLayer() == "Player")
		{
			auto _attackBox = otherCollider->GetGameObject()->GetComponent<PlayerController>();

			if (_attackBox == nullptr)
				return;

			GameManager::GetInstance()->PushItem(itemIndex);
			//printf("Get Item : %d \n", itemIndex);
			GameManager::GetInstance()->ResetItemGetCounter();

			//SceneManager::GetInstance()->GetCurrentScene()->DestroyGameObject(GetGameObject());

			//isCollided = true;
			PhysicsManager::GetInstance()->DisableObject(collider.get());
			ObjectPool::GetInstance()->ReturnGameObject(GetGameObject());
			SoundManager::GetInstance()->PlaySFXSound("GetItem");
			isLaunched = false;
			//m_particle->Stop();
			//timer = 10.0f;
			//ObjectPool::GetInstance()->ReturnGameObject(GetGameObject());
		}
	}
}
