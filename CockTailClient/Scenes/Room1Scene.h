#pragma once
#include "SceneBase.h"
#include "Room4Scene.h"

namespace ClientSpace
{
	struct Stage1Dialog
	{
		int eventIndex;
		std::wstring scriptEvent;
		std::wstring scriptStringEN;
		std::wstring scriptStringKR;
	};

	class Room1Scene : public GameEngineSpace::SceneBase
	{
	private:
		Vector3 playerEntryPos;
		Vector3 playerEntryRot;
		std::shared_ptr<GameObject> enemySpawn;

		std::unordered_map<int, Stage1Dialog> stage1DialogMap;
		std::unordered_map<int, Stage4Dialog> stage4DialogMap;

	public:
		Room1Scene(tstring sceneName);
		virtual ~Room1Scene();

	public:
		virtual void BuildScene() override;

	private:
		std::shared_ptr<GameObject> CreateUI();
		void SetScene();
	};
}