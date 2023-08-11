#pragma once
#include "SceneBase.h"

namespace ClientSpace
{
	class StraightPassageScene : public GameEngineSpace::SceneBase
	{
	private:
		std::shared_ptr<GameObject> enemySpawn;
		std::shared_ptr<Transform> room2Pos;
		std::shared_ptr<Transform> room3Pos;


	public:
		StraightPassageScene(tstring sceneName);
		virtual ~StraightPassageScene();

	public:
		virtual void BuildScene() override;

	private:
		std::shared_ptr<GameObject> CreateUI();
		void SetScene();
	};
}