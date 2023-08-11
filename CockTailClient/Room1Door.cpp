#include "pch.h"
#include "Room1Door.h"

#include "UIManager.h"
#include "Component/BoxCollider.h"

namespace ClientSpace
{
	Room1Door::Room1Door(std::weak_ptr<GameObject> _obj)
		: Monobehavior(_obj)
		, goalCount(0)
		, emissive(SimpleMath::Vector4{ 2.404f, 1.f, 0.32f, 1.f })
		, alpha(0.f)
		, isSelected(false)
		, dissolveSpeed(2.0f)
		, displayTick(0.f)
		, isMessage(false)
	{
		collider = GetGameObject()->AddComponent<BoxCollider>();

		auto scene = SceneManager::GetInstance();
		door = scene->FindObjectName(L"Room1Door");
		doorCollider = scene->FindObjectName(L"DoorWall");
	}

	Room1Door::~Room1Door()
	{

	}

	void Room1Door::Awake()
	{
		meshRenderer = door->AddComponent<MeshRenderer>();

		renderObj = meshRenderer->GetRenderObj();

		renderObj->AddAnimationState("Idle", "Room1Door_doorIdle", 1, false);
		renderObj->AddAnimationState("Open", "Room1Door_doorAction", 1, false);

		// Parameter�� �߰��ϴ¹�
		renderObj->AddTrigger("DoorOpenTrigger");
		renderObj->AddTrigger("DoorCloseTrigger");

		// Transition Duration : �����Ҷ� ���δ�.����ִϸ��̼� Ŭ���� ���� �ִϸ��̼� Ŭ���� ����(����) �ε巴�� �̾��ִ� ������ �Ѵ�.
		//	                     0.2�ʷ� �����ϸ� 0.2�� ���� �ٴ� �ִϸ��̼�Ŭ���� �����ϴ� �ִϸ��̼� Ŭ���� ��� �ε巴�� �̾��ش�.
		// bool has Exit Time : Ȱ��ȭ �� -> ���� ������ �����ϰ� �Ǿ� ������ ������ �����ص� ���� ���·� �������� �ʰ� �ȴ�.
		//                      ��Ȱ��ȭ ��->���� ������ �����ϴ� ��� ���̰� ���۵ȴ�.
		//			��) Run->Jump ���� Has Exit Time �� Ȱ��ȭ �Ǿ��ְ� exit time�� 0.5(50 %) �� ��� ���� ������ �����ص�
		//				Run �ִϸ��̼� Ŭ���� �ּ� ������ ����Ǿ�� ���̰� �̷������.
		// float Exit Time : (Has Exit Time �� Ȱ��ȭ�ÿ� ������ �� �ִ�.) ���� ����
		// Trasition�� �߰��� State �̸�(Add �ϰ� �����), ������ state �̸�, ����� state �̸�, has Exit Time, ExitTime, Transition Duration  
		renderObj->SettingAnimationStateTransition("Idle", "Idle", "Open", false, 0.f, 0.f);
		renderObj->SettingAnimationStateTransition("Open", "Open", "Idle", false, 0.f, 0.f);

		renderObj->AddCondition("Idle->Open", "DoorOpenTrigger", "None");
		renderObj->AddCondition("Open->Idle", "DoorCloseTrigger", "None");

		// Entry �ִϸ��̼� State ����
		renderObj->EntryPlayAnimationState("Idle");

		// ��ư�� ã�Ƽ� �޾Ƶӽô�.
		auto buttonGameObj = SceneManager::GetInstance()->FindObjectName(L"ElevatorButton");
		if(buttonGameObj != nullptr)
			this->buttonObj = buttonGameObj->GetComponent<MeshRenderer>()->GetRenderObj();

		buttonObj->SetMaterialColor("SilhouetteMaterial", { 0.5094f, 0.3431f, 0.1173f, 0.8f });
		buttonObj->SetMaterialEmissiveColor("SilhouetteMaterial", emissive);

		messageText = UIManager::GetInstance()->GetCanvas("InGameCanvas")->GetTextUI("Explanation");

		RegisterCallBack();
	}

	void Room1Door::Update(float tick)
	{
		if(isSelected == true)
		{
			alpha = 0.8f;
			buttonObj->SetMaterialColor("SilhouetteMaterial", { 0.5094f, 0.3431f, 0.1173f, alpha });
			buttonObj->SetDissolveSpeed(dissolveSpeed);
		}
		else
		{
			alpha -= tick * dissolveSpeed;
			if (alpha < 0.f)
				alpha = 0.f;
			buttonObj->SetMaterialColor("SilhouetteMaterial", { 0.5094f, 0.3431f, 0.1173f, MathHelper::SmoothStep(alpha) });
			buttonObj->SetDissolveSpeed(-dissolveSpeed);
		}

		if(isMessage == true)
		{
			// ƽ�� �÷��ݴϴ�.
			displayTick += tick;
			if(displayTick >= 2.0f)
			{
				isMessage = false;
				displayTick = 0.f;

				messageText->SetAlpha(0.f);
				messageText->SetEnable(false);
				messageText->SetText(L"");
			}
			if(displayTick >= 1.0f)
			{
				// 1�� ���� ���� ����
				messageText->SetAlpha(MathHelper::SmoothStep(2.0f - displayTick));
			}
		}
	}

	void Room1Door::OnTriggerEnter(const std::shared_ptr<MoolyEngine::Trigger>& trigger)
	{
		auto otherCollider = static_cast<Collider*>(trigger->_other);

		if (otherCollider != nullptr)
		{
			std::string _layer = otherCollider->GetLayer();

			if (_layer == "Player")
			{
				//auto _key = GameManager::GetInstance()->FindItem(0);	
				isSelected = true;

				// Ű�� ������
				if (GameManager::GetInstance()->isGetKey/*_key != nullptr*/)
				{
					renderObj->SetTrigger("DoorOpenTrigger");
					doorCollider->SetGameObjectEnable(false);
					GameManager::GetInstance()->RemoveItem(0);

					SoundManager::GetInstance()->PlaySFXSound("ElevatorSwitchOn");
				}
				// Ű�� ���� ų ī��Ʈ�� �޼��ߴٸ�
				else if(!GameManager::GetInstance()->isGetKey && GameManager::GetInstance()->GetKillCount() >= goalCount)
				{
					alpha = 0.8f;
					buttonObj->SetMaterialColor("SilhouetteMaterial", { 0.5094f, 0.3431f, 0.1173f, alpha });
					buttonObj->SetDissolveSpeed(dissolveSpeed);

					GameManager::GetInstance()->isCutSceneCameraMove = true;

					if (GameManager::isNeedKeyCutScenePlay == true)
					{
						messageText->SetAlpha(1.f);
						messageText->SetEnable(true);
						messageText->SetText(L"Ű�� �ʿ��մϴ�.");
						isMessage = true;
					}
				}
				// Ű�� ���� ų ī��Ʈ�� �޼����� ���ߴ�.
				else
				{
					// Ű�� ��� �ּ���
					if(GameManager::isNeedKeyCutScenePlay == true)
					{
						messageText->SetAlpha(1.f);
						messageText->SetEnable(true);
						messageText->SetText(L"Ű�� �ʿ��մϴ�.");
						isMessage = true;
					}
					// �ƾ��� ������ ���� (��� ���� óġ�� �ּ���)
					else
					{
						messageText->SetAlpha(1.f);
						messageText->SetEnable(true);
						messageText->SetText(L"��� ���� óġ�� �ּ���.");
						isMessage = true;
					}
				}
			}
		}
	}

	void Room1Door::OnTriggerExit(const std::shared_ptr<MoolyEngine::Trigger>& trigger)
	{
		auto otherCollider = static_cast<Collider*>(trigger->_other);

		if (otherCollider != nullptr)
		{
			std::string _layer = otherCollider->GetLayer();

			if (_layer == "Player")
			{
				isSelected = false;
			}
		}
	}
}
