#pragma once

namespace ClientSpace
{
	class Room1Door : public Monobehavior
	{
	private:
		// 버튼에 달아줘야 하는 컴퍼넌트 (OnTriggerEnter를 버튼이 받아야하니까)
		// door
		std::shared_ptr<GameObject> door;
		std::shared_ptr<GameObject> doorCollider;
		std::shared_ptr<IDXObject> renderObj;
		std::shared_ptr<IDXObject> buttonObj;
		std::shared_ptr<MeshRenderer> meshRenderer;

		int goalCount;
		float alpha;
		SimpleMath::Vector4 emissive;

		bool isSelected;
		float dissolveSpeed;

		// button
		std::shared_ptr<Collider> collider;

		// 메시지 표시를 위한..
		std::shared_ptr<TextUI> messageText;
		float displayTick;
		bool isMessage;

	public:
		Room1Door(std::weak_ptr<GameObject> _obj);
		virtual ~Room1Door();

	public:
		virtual void Awake() override;
		virtual void Update(float tick) override;
		virtual void OnTriggerEnter(const std::shared_ptr<MoolyEngine::Trigger>& trigger) override;
		virtual void OnTriggerExit(const std::shared_ptr<MoolyEngine::Trigger>& trigger) override;

		
	
		void SetKillCount(int _value) { goalCount = _value; }
	};
}