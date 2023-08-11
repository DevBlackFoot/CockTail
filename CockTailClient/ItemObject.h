#pragma once
#include "Component/Particle.h"

namespace ClientSpace
{
	class ItemObject : public Monobehavior
	{
	protected:
		std::shared_ptr<Transform> transform;
		std::shared_ptr<Collider> collider;
		std::shared_ptr<Rigidbody> rigid;
		std::shared_ptr<MeshRenderer> mesh;
		//std::shared_ptr<GameObject> trailObject;
		std::shared_ptr<Transform> playerTransform;

		std::shared_ptr<Particle> m_particle;
		//std::shared_ptr<Particle> trail_particle;
		// ÆÄÆ¼Å¬
		//bool isCollided;
		int itemIndex;
		wstring itemString;
		//float timer;

		bool isLaunched;
		bool isFollow = false;

	protected:
		void CalDist();
		void ChasePlayer(float tick);

	public:
		ItemObject(std::weak_ptr<GameObject> _obj);
		~ItemObject();

		virtual void Awake() override;
		virtual void Start() override;
		virtual void Update(float tick) override;

		virtual void OnCollisionEnter(const std::shared_ptr<MoolyEngine::Collision>& trigger) override;

		void SetItemInfo(int _index) { itemIndex = _index; }
		void PushObjectRandomVector(float minAngle, float maxAngle);
		void SetPlayer();
	};
}