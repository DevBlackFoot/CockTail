#include "pch.h"
#include "NextSceneTrigger.h"
#include "UIManager.h"
#include "GameManager.h"

ClientSpace::NextSceneTrigger::NextSceneTrigger(std::weak_ptr<GameObject> _obj)
	: Monobehavior(_obj)
	, goalCount(0)
	, ectOption(true)
	, displayTick(0.f)
	, isMessage(false)
{

}

ClientSpace::NextSceneTrigger::~NextSceneTrigger()
{

}

void ClientSpace::NextSceneTrigger::Awake()
{
	collider = GetGameObject()->GetComponent<Collider>();
	RegisterCallBack();
	//collider->AddTriggerEvent(this);

	messageText = UIManager::GetInstance()->GetCanvas("InGameCanvas")->GetTextUI("Explanation");
}

void ClientSpace::NextSceneTrigger::Update(float tick)
{
	if (isMessage == true)
	{
		// 틱을 올려줍니다.
		displayTick += tick;
		if (displayTick >= 2.0f)
		{
			isMessage = false;
			displayTick = 0.f;

			messageText->SetAlpha(0.f);
			messageText->SetEnable(false);
			messageText->SetText(L"");
		}
		if (displayTick >= 1.0f)
		{
			// 1초 이후 알파 조절
			messageText->SetAlpha(MathHelper::SmoothStep(2.0f - displayTick));
		}
	}
}

void ClientSpace::NextSceneTrigger::OnTriggerEnter(const std::shared_ptr<MoolyEngine::Trigger>& trigger)
{
	if (ectOption == false)
		return;

	auto otherCollider = static_cast<Collider*>(trigger->_other);
	if (otherCollider->GetLayer() == "Player")
	{
		if (GameManager::GetInstance()->GetKillCount() < goalCount)
		{
			messageText->SetAlpha(1.f);
			messageText->SetEnable(true);
			messageText->SetText(L"모든 적을 처치해 주세요.");
			isMessage = true;

			return;
		}
		else if (SceneManager::GetInstance()->GetCurrentScene()->GetSceneName() == L"Room1Scene" && GameManager::isNeedKeyCutScenePlay == false)
		{
			// 룸 1이고 isNeed키가 재생 되지 않았다면..
			messageText->SetAlpha(1.f);
			messageText->SetEnable(true);
			messageText->SetText(L"엘리베이터 버튼을 조사해주세요.");
			isMessage = true;

			return;
		}

		GameManager::GetInstance()->ResetKillCount();
		GameManager::GetInstance()->SetHPs();
		SceneManager::GetInstance()->LoadScene(nextScene);
	}
}
