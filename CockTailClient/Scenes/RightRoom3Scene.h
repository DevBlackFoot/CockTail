#pragma once
#include "SceneBase.h"

namespace ClientSpace
{

	class RightRoom3Scene : public GameEngineSpace::SceneBase
	{
	private:
		std::shared_ptr<Transform> right1Pos;
		std::shared_ptr<Transform> right2Pos;

	public:
		RightRoom3Scene(tstring sceneName);
		virtual ~RightRoom3Scene();

	public:
		virtual void BuildScene() override;

	private:
		std::shared_ptr<GameObject> CreateUI();
		void SetScene();
	};
}