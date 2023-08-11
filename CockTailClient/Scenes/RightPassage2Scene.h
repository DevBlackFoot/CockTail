#pragma once
#include "SceneBase.h"

namespace ClientSpace
{
	class RightPassage2Scene : public GameEngineSpace::SceneBase
	{
	private:
		std::shared_ptr<Transform> rightPos;
		std::shared_ptr<Transform> room4Pos;

	public:
		RightPassage2Scene(tstring sceneName);
		virtual ~RightPassage2Scene();

	public:
		virtual void BuildScene() override;

	private:
		std::shared_ptr<GameObject> CreateUI();
		void SetScene();
	};
}