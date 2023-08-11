#pragma once
#include "SceneBase.h"

namespace ClientSpace
{
	class LeftPassage2Scene : public GameEngineSpace::SceneBase
	{
	private:
		std::shared_ptr<GameObject> enemySpawn;
		std::shared_ptr<Transform> room3Pos;
		std::shared_ptr<Transform> room4Pos;


	public:
		LeftPassage2Scene(tstring sceneName);
		virtual ~LeftPassage2Scene();

	public:
		virtual void BuildScene() override;

	private:
		std::shared_ptr<GameObject> CreateUI();
		void SetScene();
	};
}