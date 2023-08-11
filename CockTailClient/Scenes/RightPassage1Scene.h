#pragma once
#include "SceneBase.h"

namespace ClientSpace
{
	class RightPassage1Scene : public GameEngineSpace::SceneBase
	{
	private:
		std::shared_ptr<GameObject> enemySpawn;
		std::shared_ptr<Transform> room1Pos;
		std::shared_ptr<Transform> room3Pos;


	public:
		RightPassage1Scene(tstring sceneName);
		virtual ~RightPassage1Scene();

	public:
		virtual void BuildScene() override;

	private:
		std::shared_ptr<GameObject> CreateUI();
		void SetScene();
	};
}