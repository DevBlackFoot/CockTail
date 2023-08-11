#include "pch.h"
#include "OpenCockTailTrigger.h"




ClientSpace::OpenCockTailTrigger::OpenCockTailTrigger(std::weak_ptr<GameObject> _obj)
	: InteractionTrigger(_obj)
{
	isPossible = false;
}

ClientSpace::OpenCockTailTrigger::~OpenCockTailTrigger()
{

}

void ClientSpace::OpenCockTailTrigger::Update(float tick)
{
	if (isPossible == false)
		return;

	InputUpdate();

	if (IsInteracted())
	{
		//ClientSpace::InGameUIController::isfirstInBar = false;
		SceneManager::GetInstance()->LoadScene(L"BarOperationScene");
	}
}

void ClientSpace::OpenCockTailTrigger::LateUpdate(float tick)
{
	if (isPossible == true)
		return;

	// ���⿡ ���� Ȯ�εǸ� true �� �ٲ��ֱ�
	if (ClientSpace::InGameUIController::isfirstInBar == true)
	{
		isPossible = true;
	}
}
