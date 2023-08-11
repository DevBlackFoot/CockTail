#pragma once
#include "SceneBase.h"

namespace ClientSpace
{
	class Elevator2Scene : public GameEngineSpace::SceneBase
	{
	private:
		std::shared_ptr<Transform> playerPos;

	public:
		Elevator2Scene(tstring sceneName);
		virtual ~Elevator2Scene();

	public:
		virtual void BuildScene() override;

	private:
		std::shared_ptr<GameObject> CreateUI();
		void SetScene();
	};
}