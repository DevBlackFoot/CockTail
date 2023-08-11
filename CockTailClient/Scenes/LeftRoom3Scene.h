#pragma once
#include "SceneBase.h"

namespace ClientSpace
{
	class LeftRoom3Scene : public GameEngineSpace::SceneBase
	{
	private:
		std::shared_ptr<GameObject> enemySpawn;
		std::shared_ptr<Transform> leftPos;
		std::shared_ptr<Transform> straightPos;


	public:
		LeftRoom3Scene(tstring sceneName);
		virtual ~LeftRoom3Scene();

	public:
		virtual void BuildScene() override;

	private:
		std::shared_ptr<GameObject> CreateUI();
		void SetScene();
	};
}