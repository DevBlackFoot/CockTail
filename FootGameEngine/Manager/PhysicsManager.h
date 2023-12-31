#pragma once
#include "Define/FootEngineTypedef.h"
#include "IMoolyEngine.h"
#include "PhysicsData.h"

namespace GameEngineSpace
{
	class Collider;
	class GameObject;
	class SceneBase;
	class Transform;

	struct RaycastHit
	{
		RaycastHit()
			: isHit(false)
			, distance(0.0f)
			, objectName("")
			, gameObject(nullptr)
			, normal({0, 0, 0})
			, hitPos({0, 0, 0})
		{}

		RaycastHit(const char* name, float dist, bool hit, std::shared_ptr<GameObject> gameObj, MoolyEngine::Vector3 _normal, MoolyEngine::Vector3 _hitPos)
			: objectName(name)
			, distance(dist)
			, isHit(hit)
			, gameObject(gameObj)
			, normal(_normal)
			, hitPos(_hitPos)
		{}

		bool isHit;
		std::string objectName;
		std::shared_ptr<GameObject> gameObject;
		float distance;
		MoolyEngine::Vector3 normal;
		MoolyEngine::Vector3 hitPos;
	};

	struct PhysicsInterpolateData
	{
		Vector3 position;

		Quaternion rotation;

		float time;
	};

	class PhysicsManager final
	{
	private:
		static std::shared_ptr<PhysicsManager> instance;
		std::shared_ptr<MoolyEngine::IMoolyEngine> physicsEngine;

		DirectX::SimpleMath::Quaternion defaultQuat;
		DirectX::SimpleMath::Quaternion reverseQuat;

		std::vector<std::pair<Collider*, Transform*>> colliders;
	
	public:
		static float accumulatePhyXTime;

		float fixedTimeStep = 0.02f;

		float totalTick = 0.f;

		bool isPhysicsUpdate = false;

		std::unordered_map<uint32,
			std::pair<Transform*, std::pair<PhysicsInterpolateData, PhysicsInterpolateData>>> physicsInterpolateDatas;
	
	public:
		PhysicsManager() = default;
		~PhysicsManager() = default;
		PhysicsManager(const PhysicsManager& other) = delete;
		PhysicsManager(const PhysicsManager&& other) = delete;
		PhysicsManager& operator= (const PhysicsManager& other) = delete;
		PhysicsManager& operator= (const PhysicsManager&& other) = delete;

		static std::shared_ptr<PhysicsManager> GetInstance();

		void Init();

		void AddScene(const std::string& sceneName);
		void SetCurrentScene(const std::string& sceneName);
		void ClearScene(const std::string& sceneName);
		void DeleteScene(const std::string& sceneName);

		void AddObject(Collider* collider);
		void DeleteObject(Collider* collider);
		void EnableObject(Collider* collider);
		void DisableObject(Collider* collider);

		void SetTriggerShape(Collider* collider);

		void CreatePlayerController(MoolyEngine::Vector3 pos);

		void AddPhysicsQueue(Collider* collider, Transform* worldTransform);

		void SetTransform(Collider* collider, Transform* transform);
		void UpdateTransform(Collider* collider, Transform* transform, int physicsCount);
		void UpdateInterpolate(int physicsCount);

		void AddForce(Collider* collider, MoolyEngine::Vector3 force);
		void AddImpulse(Collider* collider, MoolyEngine::Vector3 impulse);
		void AddTriggerEvent(Collider* collider, MoolyEngine::IEventTrigger* callback);
		void AddColliderEvent(Collider* collider, MoolyEngine::IEventCollider* callback);
		void MovePlayer(Vector3 disp);
		void MoveKinematicObject(Collider* collider, MoolyEngine::Vector3 position);

		float GetFixedTimeStep() { return fixedTimeStep; }

		void SetAngularDamp(Collider* collider, float damp);
		void SetAxisLock(Collider* collider, MoolyEngine::PhysicsAxisLock lock);
		void SetLinearDamp(Collider* collider, float damp);
		void SetMass(Collider* collider, float mass);
		void SetLinearVelocity(Collider* collider, MoolyEngine::Vector3 velocity);
		void SetAngularVelocity(Collider* collider, MoolyEngine::Vector3 angular);
		void SetMaxLinearVelocity(Collider* collider, float value);
		void SetMaxAngularVelocity(Collider* collider, float value);
		void SetGravity(Collider* collider, bool value);
		void SetKinematic(Collider* collider, bool value);
		void SetInterpolate(Collider* collider, bool value);
		void SetLayer(Collider* collider, std::string layer);

		void SetTrigger(Collider* collider, bool flag = true);

		void SetIsEnableObject(uint32 uid, bool val);

		void IgnoreCollision(Collider* collider1, Collider* collider2);

		RaycastHit Raycast(MoolyEngine::Vector3 origin, MoolyEngine::Vector3 dir, float dist, std::vector<std::string> filterLayers, MoolyEngine::PhysicsType type);

		bool CheckBox(MoolyEngine::Vector3 center, MoolyEngine::Vector3 halfExtents, std::vector<std::string> filteredLayers);

		Vector3 GetAngularVelocity(Collider* collider);
		Vector3 GetLinearVelocity(Collider* collider);

		void Simulate(float tick, std::shared_ptr<SceneBase>& currentScene);

		void Release();

		void CollidersClear();

		// OnTrigger, OnCollision 등록을해준다.
		void RegisterCallBack(std::string objName,
			std::function<void(const std::shared_ptr<MoolyEngine::Collision>&)> colEnter,
			std::function<void(const std::shared_ptr<MoolyEngine::Collision>&)> colStay,
			std::function<void(const std::shared_ptr<MoolyEngine::Collision>&)> colExit,
			std::function<void(const std::shared_ptr<MoolyEngine::Trigger>&)> triEnter,
			std::function<void(const std::shared_ptr<MoolyEngine::Trigger>&)> triStay,
			std::function<void(const std::shared_ptr<MoolyEngine::Trigger>&)> triExit);

		Vector4 EulerToQuat(Vector3 euler)
		{
			Vector4 result;

			float x = XMConvertToRadians(euler.x) * 0.5f;
			float y = XMConvertToRadians(euler.y) * 0.5f;
			float z = XMConvertToRadians(euler.z) * 0.5f;

			float sinX = sinf(x);
			float cosX = cosf(x);
			float sinY = sinf(y);
			float cosY = cosf(y);
			float sinZ = sinf(z);
			float cosZ = cosf(z);

			result.x = (cosY * sinX * cosZ) + (sinY * cosX * sinZ);
			result.y = (sinY * cosX * cosZ) - (cosY * sinX * sinZ);
			result.z = (cosY * cosX * sinZ) - (sinY * sinX * cosZ);
			result.w = (cosY * cosX * cosZ) + (sinY * sinX * sinZ);

			return result;
		}

		Vector3 QuatToEuler(Quaternion value)
		{
			Vector3 rotation;

			double x = value.x;
			double y = value.y;
			double z = value.z;
			double w = value.w;
			double sqx = x * x;
			double sqy = y * y;
			double sqz = z * z;
			double sqw = w * w;
			float exceptCheck = 2.0f * (w * x - y * z);
			float eulerX = 0.f;

			if (abs(exceptCheck) >= 1.f)
				eulerX = copysign(3.141592f / 2, exceptCheck);
			else
				eulerX = asin(2.0f * (w * x - y * z));

			float eulerY = atan2(2.0f * (x * z + w * y), (-sqx - sqy + sqz + sqw));
			float eulerZ = atan2(2.0f * (x * y + z * w), (-sqx + sqy - sqz + sqw));
			rotation = Vector3{ eulerX, eulerY, eulerZ };

			rotation = Vector3{ XMConvertToDegrees(rotation.x),
								XMConvertToDegrees(rotation.y),
								XMConvertToDegrees(rotation.z) };

			return rotation;
		}
	};

}
