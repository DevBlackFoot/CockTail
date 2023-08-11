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
		// ��ǻ� UI �ϳ��� ������ �ִ� ���̴�.
		std::shared_ptr<GameObject> ui = CreateEmpty();
		ui->AddComponent<ResultUIController>();
	}
}
