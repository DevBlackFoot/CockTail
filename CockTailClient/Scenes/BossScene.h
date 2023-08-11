#pragma once
#include "SceneBase.h"

namespace ClientSpace
{
	class BossScene : public GameEngineSpace::SceneBase
	{
	private:
		std::shared_ptr<GameObject> enemySpawn;

	public:
		BossScene(tstring sceneName);
		virtual ~BossScene();

	public:
		virtual void BuildScene() override;

	private:
		std::shared_ptr<GameObject> CreateUI();
		void SetScene();
	};
}