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

		// Parameter를 추가하는법
		renderObj->AddTrigger("DoorOpenTrigger");
		renderObj->AddTrigger("DoorCloseTrigger");

		// Transition Duration : 블렌딩할때 쓰인다.현재애니메이션 클립과 다음 애니메이션 클립을 섞어(블렌딩) 부드럽게 이어주는 역할을 한다.
		//	                     0.2초로 설정하면 0.2초 동안 뛰는 애니메이션클립과 점프하는 애니메이션 클립을 섞어서 부드럽게 이어준다.
		// bool has Exit Time : 활성화 시 -> 종료 시점이 존재하게 되어 전이의 조건을 만족해도 다음 상태로 전이하지 않게 된다.
		//                      비활성화 시->전이 조건을 만족하는 즉시 전이가 시작된다.
		//			예) Run->Jump 에서 Has Exit Time 이 활성화 되어있고 exit time이 0.5(50 %) 인 경우 전이 조건을 만족해도
		//				Run 애니메이션 클립이 최소 절반은 재생되어야 전이가 이루어진다.
		// float Exit Time : (Has Exit Time 이 활성화시에 설정할 수 있다.) 종료 시점
		// Trasition을 추가할 State 이름(Add 하고 써야함), 연결할 state 이름, 연결된 state 이름, has Exit Time, ExitTime, Transition Duration  
		renderObj->SettingAnimationStateTransition("Idle", "Idle", "Open", false, 0.f, 0.f);
		renderObj->SettingAnimationStateTransition("Open", "Open", "Idle", false, 0.f, 0.f);

		renderObj->AddCondition("Idle->Open", "DoorOpenTrigger", "None");
		renderObj->AddCondition("Open->Idle", "DoorCloseTrigger", "None");

		// Entry 애니메이션 State 설정
		renderObj->EntryPlayAnimationState("Idle");

		// 버튼을 찾아서 달아둡시다.
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
			// 틱을 올려줍니다.
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
				// 1초 이후 알파 조절
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

				// 키가 있으면
				if (GameManager::GetInstance()->isGetKey/*_key != nullptr*/)
				{
					renderObj->SetTrigger("DoorOpenTrigger");
					doorCollider->SetGameObjectEnable(false);
					GameManager::GetInstance()->RemoveItem(0);

					SoundManager::GetInstance()->PlaySFXSound("ElevatorSwitchOn");
				}
				// 키가 없고 킬 카운트를 달성했다면
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
						messageText->SetText(L"키가 필요합니다.");
						isMessage = true;
					}
				}
				// 키도 없고 킬 카운트도 달성하지 못했다.
				else
				{
					// 키를 얻어 주세요
					if(GameManager::isNeedKeyCutScenePlay == true)
					{
						messageText->SetAlpha(1.f);
						messageText->SetEnable(true);
						messageText->SetText(L"키가 필요합니다.");
						isMessage = true;
					}
					// 컷씬이 나오기 전에 (모든 적을 처치해 주세요)
					else
					{
						messageText->SetAlpha(1.f);
						messageText->SetEnable(true);
						messageText->SetText(L"모든 적을 처치해 주세요.");
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
