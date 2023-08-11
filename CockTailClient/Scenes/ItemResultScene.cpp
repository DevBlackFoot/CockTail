#include "pch.h"
#include "ItemResultScene.h"

#include "ResultUIController.h"

namespace ClientSpace
{
	ItemResultScene::ItemResultScene(tstring sceneName)
		: SceneBase(sceneName)
	{
	}

	ItemResultScene::~ItemResultScene()
	{
	}

	void ItemResultScene::BuildScene()
	{
		// 사실상 UI 하나만 가지고 있는 씬이다.
		std::shared_ptr<GameObject> ui = CreateEmpty();
		ui->AddComponent<ResultUIController>();
	}
}
