#pragma once
#include "SceneBase.h"

namespace ClientSpace
{
	struct Stage4Dialog
	{
		int eventIndex;
		std::wstring scriptEvent;
		std::wstring scriptStringEN;
		std::wstring scriptStringKR;
	};

	class Room4Scene : public GameEngineSpace::SceneBase
	{
	private:
		std::shared_ptr<GameObject> enemySpawn;
		std::shared_ptr<Transform> leftPos;
		std::shared_ptr<Transform> rightPos;

		std::unordered_map<int, Stage4Dialog> stage4DialogMap;

	public:
		Room4Scene(tstring sceneName);
		virtual ~Room4Scene();

	public:
		virtual void BuildScene() override;

	private:
		std::shared_ptr<GameObject> CreateUI();
		void SetScene();
	};
}