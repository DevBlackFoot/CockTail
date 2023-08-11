#pragma once
#include "SceneBase.h"

namespace ClientSpace
{
	class LeftPassage1Scene : public GameEngineSpace::SceneBase
	{
	private:
		std::shared_ptr<GameObject> enemySpawn;
		std::shared_ptr<Transform> room1Pos;
		std::shared_ptr<Transform> left2Pos;

	public:
		LeftPassage1Scene(tstring sceneName);
		virtual ~LeftPassage1Scene();

	public:
		virtual void BuildScene() override;

	private:
		std::shared_ptr<GameObject> CreateUI();
		void SetScene();
	};
}