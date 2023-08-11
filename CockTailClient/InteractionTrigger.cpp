#include "pch.h"
#include "InteractionTrigger.h"
#include "InputManager.h"
#include "UIManager.h"

ClientSpace::InteractionTrigger::InteractionTrigger(std::weak_ptr<GameObject> _obj)
	: Monobehavior(_obj)
	, isInteracted(false)
	, isSelected(false)
{
	collider = GetGameObject()->GetComponent<Collider>();
	input = GameEngineSpace::InputManager::GetInstance();
	mainCanvas = UIManager::GetInstance()->GetMainCanvas();
	
}

ClientSpace::InteractionTrigger::~InteractionTrigger()
{

}

void ClientSpace::InteractionTrigger::Awake()
{
	//collider->AddTriggerEvent(this);
	
	RegisterCallBack();
}

void ClientSpace::InteractionTrigger::Update(float tick)
{
	InputUpdate();
}

void ClientSpace::InteractionTrigger::OnTriggerEnter(const std::shared_ptr<MoolyEngine::Trigger>& trigger)
{
	if (isPossible == false)
		return;

	auto otherCollider = static_cast<Collider*>(trigger->_other);

	if (otherCollider->GetLayer() == "Player")
	{
		// ��Ʈ���ſ� ������� �÷��̾� Ʈ������ �޾ƿ���
		// transform = target->GetComponent<Transform>();
		isSelected = true;
		mainCanvas->GetCanvasUI("InteractionCanvas")->GetSpriteUI("F")->SetEnable(true);
	}
}

void ClientSpace::InteractionTrigger::OnTriggerStay(const std::shared_ptr<MoolyEngine::Trigger>& trigger)
{
	if (isPossible == false)
		return;

	//SimpleMath::Vector4 transformVector4 = { playerTransform.GetPosition().x, way.y + ���ߵ�, way.z, 1.0f };

	//XMMATRIX viewProj = XMMatrixMultiply(camera->GetViewMatrix(), camera->GetProjMatrix());

	//Vector4 screenPos = XMVector4Transform(XMLoadFloat4(&transformVector4), viewProj);

	//// Divide the x and y values of the screen space positions by their w value
	//screenPos = XMVectorDivide(screenPos, XMVectorSplatW(screenPos));

	//// -1 ~ 1 ������ 0 ~ 1 �������� ��ȯ
	//screenPos += {1.f, 1.f, 1.f, 1.f};

	//// y�� ��������
	//screenPos = screenPos * 0.5f;
	//screenPos.y = 1.f - screenPos.y;

	//// Multiply the x and y values by the size of the viewport in pixels
	//int width, height;
	//width = WindowManager::GetInstance()->GetClientWidth();
	//height = WindowManager::GetInstance()->GetClientHeight();

	//screenPos = XMVectorMultiply(screenPos, XMVectorSet(width, height, 0.0f, 0.0f));

	// ui->SetPosition(screenPos);
}

void ClientSpace::InteractionTrigger::OnTriggerExit(const std::shared_ptr<MoolyEngine::Trigger>& trigger)
{
	if (isPossible == false)
		return;

	auto otherCollider = static_cast<Collider*>(trigger->_other);

	if (otherCollider->GetLayer() == "Player")
	{
		isSelected = false;
		mainCanvas->GetCanvasUI("InteractionCanvas")->GetSpriteUI("F")->SetEnable(false);
	}
}

void ClientSpace::InteractionTrigger::InputUpdate()
{
	if (isSelected == false)
		return;

	if (input->GetInputState('F', KeyState::DOWN))
	{
		isInteracted = true;
	}
}
