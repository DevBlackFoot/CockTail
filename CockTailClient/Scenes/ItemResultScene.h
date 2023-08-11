#pragma once
#include "SceneBase.h"

namespace ClientSpace
{

	class ItemResultScene : public GameEngineSpace::SceneBase
	{
	public:
		ItemResultScene(tstring sceneName);
		virtual ~ItemResultScene();

	public:
		virtual void BuildScene() override;
	};

}
