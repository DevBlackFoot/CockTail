#pragma once
#include "SceneBase.h"

namespace ClientSpace
{
	class Room2Scene : public GameEngineSpace::SceneBase
	{
	private:
		std::shared_ptr<GameObject> enemySpawn;
		std::shared_ptr<Transform> leftPos;
		std::shared_ptr<Transform> straightPos;

	public:
		Room2Scene(tstring sceneName);
		virtual ~Room2Scene();

	public:
		virtual void BuildScene() override;

	private:
		std::shared_ptr<GameObject> CreateUI();
		void SetScene();
	};
}