#include "GamePch.h"
#include "PhysicsManager.h"
#include "SceneManager.h"

#include "Object/GameObject.h"
#include "Component/Transform.h"
#include "Component/Collider.h"
#include "Component/BoxCollider.h"
#include "Component/SphereCollider.h"
#include "Component/CapsuleCollider.h"
#include "Component/RayCollider.h"

#include "Timer.h"

#include "SceneBase.h"

namespace GameEngineSpace
{
	std::shared_ptr<PhysicsManager> PhysicsManager::instance = nullptr;

	float PhysicsManager::accumulatePhyXTime = 0.0f;

	std::shared_ptr<PhysicsManager> PhysicsManager::GetInstance()
	{
		if (instance == nullptr)
			instance = std::make_shared<PhysicsManager>();

		return instance;
	}

	void PhysicsManager::Init()
	{
		physicsEngine = MoolyEngine::MoolyEngineInstance();

		printf("PhysicsManager Instance Clear\n");

		printf("PhysicsManager Initialize Start\n");
		physicsEngine->Initialize();
		printf("PhysicsManager Initialize End\n");

		defaultQuat = EulerToQuat({ 0,0,90.0f });
		reverseQuat = EulerToQuat({ 0,0,-90.0f });
	}

	void PhysicsManager::AddScene(const std::string& sceneName)
	{
		physicsEngine->AddScene(sceneName);
	}

	void PhysicsManager::SetCurrentScene(const std::string& sceneName)
	{
		physicsEngine->SetCurrentScene(sceneName);

		physicsEngine->AddLayer("Default");
		physicsEngine->AddLayer("Wall");
		physicsEngine->AddLayer("Trigger");
		physicsEngine->AddLayer("Slope");
		physicsEngine->AddLayer("Plane");

		physicsEngine->AddLayer("Player");
		physicsEngine->AddLayer("Dash");
		physicsEngine->AddLayer("Parry");
		physicsEngine->AddLayer("PlayerAttack");

		physicsEngine->AddLayer("Enemy");
		physicsEngine->AddLayer("Turret");
		physicsEngine->AddLayer("BossTrigger");
		physicsEngine->AddLayer("EnemyAttack");

		physicsEngine->AddLayer("Item");

		physicsEngine->SetLayerFilterData("Item", { "Enemy" });
		/*physicsEngine->SetLayerFilterData("Player", { "Enemy" });
		physicsEngine->SetLayerFilterData("Enemy", { "Player", "Enemy" });*/
		/*physicsEngine->SetLayerFilterData("CharacterBlocker", { "Default", "Plane", "Wall","Trigger", "Slope" });

		physicsEngine->SetLayerFilterData("CharacterBlocker", { "Default", "Plane", "Wall","Trigger", "Slope", "Player"});
		physicsEngine->SetLayerFilterData("Player", { "CharacterBlocker" });*/

		//physicsEngine->SetLayerFilterData("PlayerAttack", { "Enemy", "Turret"});
		//physicsEngine->SetLayerFilterData("Player", { "PlayerAttack" });

		// physicsEngine->SetLayerFilterData("Player", { "Enemy" });
		// physicsEngine->SetLayerFilterData("Enemy", { "Player" });

		//physicsEngine->Test();`
	}

	void PhysicsManager::ClearScene(const std::string& sceneName)
	{
		physicsEngine->ClearScene(sceneName);
	}

	void PhysicsManager::DeleteScene(const std::string& sceneName)
	{
		physicsEngine->DeleteScene(sceneName);
	}

	void PhysicsManager::AddObject(Collider* collider)
	{
		DirectX::SimpleMath::Vector3 _wPos = collider->GetGameObject()->GetComponent<Transform>()->GetWorldPosition();
		DirectX::SimpleMath::Quaternion _wRot = collider->GetGameObject()->GetComponent<Transform>()->GetWorldRotation();

		DirectX::SimpleMath::Vector3 _offset = collider->GetCenter();

		MoolyEngine::Transform _input;
		_input.quat[0] = _wRot.x;
		_input.quat[1] = _wRot.y;
		_input.quat[2] = _wRot.z;
		_input.quat[3] = _wRot.w;

		_input.pos[0] = _wPos.x + _offset.x;
		_input.pos[1] = _wPos.y + _offset.y;
		_input.pos[2] = _wPos.z + _offset.z;

		switch (collider->GetColliderType())
		{
			case MoolyEngine::GeometryType::BOX:
			{
				collider->GetGameObject()->GetComponent<Transform>()->Update(0.0f);
				BoxCollider* boxCollider = static_cast<BoxCollider*>(collider);
				Vector3 _extend = boxCollider->GetSize();
				Vector3 _wScale = collider->GetGameObject()->GetComponent<Transform>()->GetWorldScale();
				_extend.x *= _wScale.x;
				_extend.y *= _wScale.y;
				_extend.z *= _wScale.z;
				boxCollider->SetSize(_extend);

				physicsEngine->CreateBoxActor(collider, std::to_string(collider->GetGameObject()->GetObjectID()), "Box",
					boxCollider->GetPhysicsType(), _input, { _extend.x / 2.0f, _extend.y / 2.0f, _extend.z / 2.0f }, 1);
			}
			break;

			case MoolyEngine::GeometryType::SPHERE:
			{
				SphereCollider* sphereCollider = static_cast<SphereCollider*>(collider);
				float _radius = sphereCollider->GetRadius();

				physicsEngine->CreateSphereActor(collider, std::to_string(collider->GetGameObject()->GetObjectID()), "Sphere", sphereCollider->GetPhysicsType(),
					_input, _radius, 1);
			}
			break;

			case MoolyEngine::GeometryType::CAPSULE:
			{
				collider->GetGameObject()->GetComponent<Transform>()->Update(0.0f);
				CapsuleCollider* capsuleCollider = static_cast<CapsuleCollider*>(collider);
				Quaternion _defaultRot = { 0, 0, 0.7071068f, 0.7071068f };
				_defaultRot *= _wRot;

				_input.quat[0] = _defaultRot.x;
				_input.quat[1] = _defaultRot.y;
				_input.quat[2] = _defaultRot.z;
				_input.quat[3] = _defaultRot.w;

				float _radius = capsuleCollider->GetRadius();
				float _height = capsuleCollider->GetHeight();

				physicsEngine->CreateCapsuleActor(collider, std::to_string(collider->GetGameObject()->GetObjectID()), "Capsule", capsuleCollider->GetPhysicsType(),
					_input, _radius, _height / 2, 1);
			}
			break;

			default:
				break;
		}

		SetTriggerShape(collider);		

		physicsEngine->SetAngularDamping(std::to_string(collider->GetGameObject()->GetObjectID()), 5.0f);
		physicsEngine->SetLinearDamping(std::to_string(collider->GetGameObject()->GetObjectID()), 1.5f);
	}

	void PhysicsManager::DeleteObject(Collider* collider)
	{
		physicsEngine->DeleteActor(std::to_string(collider->GetGameObject()->GetObjectID()));

		auto objectID = collider->GetGameObject()->GetObjectID();

		// colliders에서 GameObject ID가 일치하는 원소 삭제
		colliders.erase(std::remove_if(colliders.begin(), colliders.end(), [&objectID](const std::pair<Collider*, Transform*>& element)
		{
			return element.first->GetGameObject()->GetObjectID() == objectID;
		}), colliders.end());
	}

	void PhysicsManager::EnableObject(Collider* collider)
	{
		physicsEngine->EnableActor(std::to_string(collider->GetGameObject()->GetObjectID()));
	}

	void PhysicsManager::DisableObject(Collider* collider)
	{
		physicsEngine->DisableActor(std::to_string(collider->GetGameObject()->GetObjectID()));
	}

	void PhysicsManager::SetTrigger(Collider* collider, bool flag)
	{
		physicsEngine->SetTriggerShape(std::to_string(collider->GetGameObject()->GetObjectID()), flag);

		physicsEngine->SetLayer(std::to_string(collider->GetGameObject()->GetObjectID()), "TRIGGER");
	}

	void PhysicsManager::SetIsEnableObject(uint32 uid, bool val)
	{
		if (val)
		{
			physicsEngine->EnableActor(std::to_string(uid));
		}
		else
		{
			physicsEngine->DisableActor(std::to_string(uid));
		}
	}

	void PhysicsManager::SetTriggerShape(Collider* collider)
	{
		if (collider->IsTrigger())
		{
			physicsEngine->SetTriggerShape(std::to_string(collider->GetGameObject()->GetObjectID()), true);
			physicsEngine->SetLayer(std::to_string(collider->GetGameObject()->GetObjectID()), "TRIGGER");
		}
	}

	void PhysicsManager::CreatePlayerController(MoolyEngine::Vector3 pos)
	{
		physicsEngine->CreatePlayerController(pos);
	}

	void PhysicsManager::AddPhysicsQueue(Collider* collider, Transform* worldTransform)
	{
		colliders.emplace_back(collider, worldTransform);
	}

	void PhysicsManager::SetTransform(Collider* collider, Transform* transform)
	{
		if (collider->GetColliderType() == MoolyEngine::GeometryType::CAPSULE)
		{
			Vector3 _offset = collider->GetCenter();

			// 물리Transform으로 변환
			MoolyEngine::Transform _input;

			Quaternion _quat(transform->GetWorldRotation().x, transform->GetWorldRotation().y, transform->GetWorldRotation().z, transform->GetWorldRotation().w);
			Quaternion _defaultRot = defaultQuat;
			_defaultRot *= _quat;

			if (_defaultRot.w > 1.f)
				_defaultRot.w = 1.0f;

			_input.pos[0] = transform->GetWorldPosition().x + _offset.x;
			_input.pos[1] = transform->GetWorldPosition().y + _offset.y;
			_input.pos[2] = transform->GetWorldPosition().z + _offset.z;
			_input.quat[0] = _defaultRot.x;
			_input.quat[1] = _defaultRot.y;
			_input.quat[2] = _defaultRot.z;
			_input.quat[3] = _defaultRot.w;

			physicsEngine->SetTransform(std::to_string(collider->GetGameObject()->GetObjectID()), _input);
		}
		else
		{
			DirectX::SimpleMath::Vector3 _offset = collider->GetCenter();

			Quaternion _quat(transform->GetWorldRotation().x, transform->GetWorldRotation().y, transform->GetWorldRotation().z, transform->GetWorldRotation().w);

			// 물리Transform으로 변환
			MoolyEngine::Transform _input;
			_input.pos[0] = transform->GetWorldPosition().x + _offset.x;
			_input.pos[1] = transform->GetWorldPosition().y + _offset.y;
			_input.pos[2] = transform->GetWorldPosition().z + _offset.z;
			_input.quat[0] = _quat.x;
			_input.quat[1] = _quat.y;
			_input.quat[2] = _quat.z;
			_input.quat[3] = _quat.w;

			physicsEngine->SetTransform(std::to_string(collider->GetGameObject()->GetObjectID()), _input);
		}
	}

	void PhysicsManager::UpdateTransform(Collider* collider, Transform* transform, int physicsCount)
	{
		MoolyEngine::Transform _output;

		if (physicsEngine->GetTransform(std::to_string(collider->GetGameObject()->GetObjectID()), _output))
		{
			if (collider->GetColliderType() == MoolyEngine::GeometryType::CAPSULE)
			{
				Vector3 _offset = collider->GetCenter();
				Vector3 _pos(_output.pos[0] - _offset.x, _output.pos[1] - _offset.y, _output.pos[2] - _offset.z);
				Quaternion _quat(_output.quat[0], _output.quat[1], _output.quat[2], _output.quat[3]);
				Quaternion _defaultRot = reverseQuat;

				_defaultRot *= _quat;
				_defaultRot.Normalize();
				
				transform->SetPosition(_pos, Space::WORLD);
				transform->SetRotation(_defaultRot, Space::WORLD);
				
				if (collider->isInterpolate)
				{
					auto key = collider->GetGameObject()->GetObjectID();
			
					std::pair<PhysicsInterpolateData, PhysicsInterpolateData>& interpolateDatas = physicsInterpolateDatas.at(key).second;

					// 현재 틱의 위치 회전 정보
					interpolateDatas.second.position = _pos;
					interpolateDatas.second.rotation = _defaultRot;
				}
			}
			else
			{
				DirectX::SimpleMath::Vector3 _offset = collider->GetCenter();
				DirectX::SimpleMath::Vector3 _pos(_output.pos[0] - _offset.x, _output.pos[1] - _offset.y, _output.pos[2] - _offset.z);
				DirectX::SimpleMath::Quaternion _rot(_output.quat[0], _output.quat[1], _output.quat[2], _output.quat[3]);

				transform->SetPosition(_pos, Space::WORLD);
				transform->SetRotation(_rot, Space::WORLD);

				auto key = collider->GetGameObject()->GetObjectID();
			}
		}
	}

	/// <summary>
	/// 여기서 Transform의 InterpolateQueue에다가 넣어 줄 것이다.
	/// 현재 틱에 얼마만큼 움직였는지 "예상" 해야한다.
	/// </summary>
	/// <param name="physicsCount"></param>
	void PhysicsManager::UpdateInterpolate(int physicsCount)
	{
		for (auto& [key, value] : physicsInterpolateDatas)
		{
			auto& transform = value.first;

			auto& interpolateData = value.second;
			
			float previousTime = interpolateData.first.time;
			float deltaTime = totalTick - previousTime;
			float t = deltaTime / fixedTimeStep;

			Vector3 smoothedPosition = Vector3::Lerp(interpolateData.first.position, interpolateData.second.position, t);
			Quaternion smoothedRotation = Quaternion::Slerp(interpolateData.first.rotation, interpolateData.second.rotation, t);

			transform->PushInterpolateQueue(smoothedPosition, smoothedRotation, t);

			if(t >= 1.0f)
				t = 1.0f;

			interpolateData.first.position = interpolateData.second.position;

			interpolateData.first.rotation = interpolateData.second.rotation;

			interpolateData.first.time = Timer::GetInstance()->TotalTime();
		}
	}

	void PhysicsManager::AddForce(Collider* collider, MoolyEngine::Vector3 force)
	{
		physicsEngine->AddForce(std::to_string(collider->GetGameObject()->GetObjectID()), force);
	}

	void PhysicsManager::AddImpulse(Collider* collider, MoolyEngine::Vector3 impulse)
	{
		physicsEngine->AddImpulse(std::to_string(collider->GetGameObject()->GetObjectID()), impulse);
	}

	void PhysicsManager::AddTriggerEvent(Collider* collider, MoolyEngine::IEventTrigger* callback)
	{
		physicsEngine->AddTriggerEvent(std::to_string(collider->GetGameObject()->GetObjectID()), callback);
	}

	void PhysicsManager::AddColliderEvent(Collider* collider, MoolyEngine::IEventCollider* callback)
	{
		physicsEngine->AddColliderEvent(std::to_string(collider->GetGameObject()->GetObjectID()), callback);
	}

	void PhysicsManager::MovePlayer(Vector3 disp)
	{
		physicsEngine->MovePCC({disp.x, disp.y, disp.z});
	}

	void PhysicsManager::MoveKinematicObject(Collider* collider, MoolyEngine::Vector3 position)
	{

	}

	void PhysicsManager::SetAngularDamp(Collider* collider, float damp)
	{
		physicsEngine->SetAngularDamping(std::to_string(collider->GetGameObject()->GetObjectID()), damp);
	}

	void PhysicsManager::SetAxisLock(Collider* collider, MoolyEngine::PhysicsAxisLock lock)
	{
		physicsEngine->SetAxisLock(std::to_string(collider->GetGameObject()->GetObjectID()), lock);
	}

	void PhysicsManager::SetLinearDamp(Collider* collider, float damp)
	{
		physicsEngine->SetLinearDamping(std::to_string(collider->GetGameObject()->GetObjectID()), damp);
	}

	void PhysicsManager::SetMass(Collider* collider, float mass)
	{
		physicsEngine->SetMass(std::to_string(collider->GetGameObject()->GetObjectID()), mass);
	}

	void PhysicsManager::SetLinearVelocity(Collider* collider, MoolyEngine::Vector3 velocity)
	{
		physicsEngine->SetLinearVelocity(std::to_string(collider->GetGameObject()->GetObjectID()), velocity);
	}

	void PhysicsManager::SetAngularVelocity(Collider* collider, MoolyEngine::Vector3 angular)
	{
		physicsEngine->SetAngularVelocity(std::to_string(collider->GetGameObject()->GetObjectID()), angular);
	}

	void PhysicsManager::SetMaxLinearVelocity(Collider* collider, float value)
	{
		physicsEngine->SetMaxLinearVelocity(std::to_string(collider->GetGameObject()->GetObjectID()), value);
	}

	void PhysicsManager::SetMaxAngularVelocity(Collider* collider, float value)
	{
		physicsEngine->SetMaxAngularVelocity(std::to_string(collider->GetGameObject()->GetObjectID()), value);
	}

	void PhysicsManager::SetGravity(Collider* collider, bool value)
	{
		physicsEngine->SetGravity(std::to_string(collider->GetGameObject()->GetObjectID()), value);
	}

	void PhysicsManager::SetKinematic(Collider* collider, bool value)
	{
		physicsEngine->SetKinematic(std::to_string(collider->GetGameObject()->GetObjectID()), value);
	}

	void PhysicsManager::SetInterpolate(Collider* collider, bool value)
	{
		if(value)
			physicsEngine->SetInterpolate(std::to_string(collider->GetGameObject()->GetObjectID()));
	}

	void PhysicsManager::SetLayer(Collider* collider, std::string layer)
	{
		physicsEngine->SetLayer(std::to_string(collider->GetGameObject()->GetObjectID()), layer);
	}

	RaycastHit PhysicsManager::Raycast(MoolyEngine::Vector3 origin, MoolyEngine::Vector3 dir, float dist, std::vector<std::string> filterLayers, MoolyEngine::PhysicsType type)
	{
		MoolyEngine::RayCastHit hitInfo;
		bool ret = physicsEngine->Raycast(origin, dir, dist, filterLayers, type, hitInfo);

		if (ret == false)
			return RaycastHit();

		auto gameObject = SceneManager::GetInstance()->FindObject(hitInfo.objectName);

		if (gameObject == nullptr)
			return RaycastHit();

		return RaycastHit(hitInfo.objectName.c_str(), hitInfo.distance, ret, gameObject, hitInfo.normal, hitInfo.hitPos);
	}

	bool PhysicsManager::CheckBox(MoolyEngine::Vector3 center, MoolyEngine::Vector3 halfExtents, std::vector<std::string> filteredLayers)
	{
		return physicsEngine->CheckBox(center, halfExtents, filteredLayers);
	}

	DirectX::SimpleMath::Vector3 PhysicsManager::GetAngularVelocity(Collider* collider)
	{
		MoolyEngine::Vector3 _result;

		if (physicsEngine->GetAngularVelocity(std::to_string(collider->GetGameObject()->GetObjectID()), _result))
		{
			return { _result.vec[0], _result.vec[1], _result.vec[2] };
		}

		else
		{
			return { 0.0f, 0.0f, 0.0f };
		}
	}

	DirectX::SimpleMath::Vector3 PhysicsManager::GetLinearVelocity(Collider* collider)
	{
		MoolyEngine::Vector3 _result;

		if (physicsEngine->GetLinearVelocity(std::to_string(collider->GetGameObject()->GetObjectID()), _result))
		{
			return { _result.vec[0], _result.vec[1], _result.vec[2] };
		}

		else
		{
			return { 0.0f, 0.0f, 0.0f };
		}
	}

	void PhysicsManager::Simulate(float tick, std::shared_ptr<SceneBase>& currentScene)
	{
		// tick이 너무 튀는걸 방지하자
		if(tick <= 0.3f)
			accumulatePhyXTime += tick;

		int physicsUpdateCount = 0;

		if (accumulatePhyXTime > fixedTimeStep)
		{
			// 물리 업데이트가 돌았는지 안돌았는지 체크
			isPhysicsUpdate = true;

			physicsUpdateCount = static_cast<int>(accumulatePhyXTime / fixedTimeStep);

			for (int i = 0; i < physicsUpdateCount; i++)
			{
				currentScene->FixedUpdate(fixedTimeStep);

				for (auto& collider : colliders)
				{
					SetTransform(collider.first, collider.second);
				}

				physicsEngine->Simulate(fixedTimeStep);

				for (auto& collider : colliders)
				{
					UpdateTransform(collider.first, collider.second, physicsUpdateCount);
				}
			}

			accumulatePhyXTime = accumulatePhyXTime - static_cast<float>(physicsUpdateCount) * fixedTimeStep;
		}

		// 물리 업데이트가 돌지 않았다면 카메라 업데이트는 돌기때문에 주기 차이로인한 캐릭터 떨림현상 발생
		// 그래서 보간값을 건네준다. 아니면 안건네줘도됨
		//if (!isPhysicsUpdate)
		//{
		//	UpdateInterpolate(physicsUpdateCount);
		//}

		//// 여기서 다시 false 해준다.
		//isPhysicsUpdate = false;

		totalTick += tick;
	}

	void PhysicsManager::Release()
	{
		physicsInterpolateDatas.clear();
	}

	void PhysicsManager::CollidersClear()
	{
		colliders.clear();

		physicsInterpolateDatas.clear();
	}

	void PhysicsManager::IgnoreCollision(Collider* collider1, Collider* collider2)
	{
		physicsEngine->IgnoreCollision(std::to_string(collider1->GetGameObject()->GetObjectID()), std::to_string(collider2->GetGameObject()->GetObjectID()));
	}

	void PhysicsManager::RegisterCallBack(std::string objName, std::function<void(const std::shared_ptr<MoolyEngine::Collision>&)> colEnter, std::function<void(const std::shared_ptr<MoolyEngine::Collision>&)> colStay, std::function<void(const std::shared_ptr<MoolyEngine::Collision>&)> colExit, std::function<void(const std::shared_ptr<MoolyEngine::Trigger>&)> triEnter, std::function<void(const std::shared_ptr<MoolyEngine::Trigger>&)> triStay, std::function<void(const std::shared_ptr<MoolyEngine::Trigger>&)> triExit)
	{
		physicsEngine->RegisterCallBack(objName, colEnter, colStay, colExit, triEnter, triStay, triExit);
	}
}
