#pragma once

namespace ClientSpace
{
	class NextSceneTrigger : public Monobehavior
	{
	private:
		std::shared_ptr<Collider> collider;
		tstring nextScene;
		int goalCount;
		bool ectOption;

		// 메시지 표시를 위한..
		std::shared_ptr<TextUI> messageText;
		float displayTick;
		bool isMessage;

	public:
		NextSceneTrigger(std::weak_ptr<GameObject> _obj);
		virtual ~NextSceneTrigger();

		virtual void Awake() override;
		virtual void Update(float tick) override;
		virtual void OnTriggerEnter(const std::shared_ptr<MoolyEngine::Trigger>& trigger) override;

		void SetKillCount(int _value) { goalCount = _value; }
		void SetNextScene(tstring value) { nextScene = value; }
		tstring GetNextScene() { return nextScene; }

		void SetEctOption(bool val) { ectOption = val; }
	};
}