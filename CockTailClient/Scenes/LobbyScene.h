#pragma once
#include "SceneBase.h"

namespace ClientSpace
{

	struct LobbyDialog
	{
		int eventIndex;
		std::wstring scriptEvent;
		std::wstring scriptStringEN;
		std::wstring scriptStringKR;
	};


	class LobbyScene : public GameEngineSpace::SceneBase
	{
	private:
		Vector3 playerEntryPos;
		Vector3 playerEntryRot;

		std::shared_ptr<GameObject> uiController;

	public:
		LobbyScene(tstring sceneName);
		virtual ~LobbyScene();

	public:
		virtual void BuildScene() override;

	private:
		void SetScene();
		std::shared_ptr<GameObject> CreateUI();
		unordered_map<int, LobbyDialog> lobbydialogMap;
	};
}