#pragma once
#include "SceneBase.h"

namespace ClientSpace
{
	struct ElevatorDialog
	{
		int eventIndex;
		std::wstring scriptEvent;
		std::wstring scriptStringEN;
		std::wstring scriptStringKR;
	};

	class ElevatorScene : public GameEngineSpace::SceneBase
	{
	private:
		std::shared_ptr<Transform> playerPos;

		std::unordered_map<int, ElevatorDialog> elevatorDialogMap;

	public:
		ElevatorScene(tstring sceneName);
		virtual ~ElevatorScene();

	public:
		virtual void BuildScene() override;

	private:
		std::shared_ptr<GameObject> CreateUI();
		void SetScene();
	};
}