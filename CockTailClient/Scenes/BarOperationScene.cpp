#include "pch.h"
#include "BarOperationScene.h"

#include "InGameUIController.h"
#include "Util/JsonReader.h"
#include "BarCoreController.h"

namespace ClientSpace
{
	BarOperationScene::BarOperationScene(tstring sceneName)
		:SceneBase(sceneName)
	{
	}

	BarOperationScene::~BarOperationScene()
	{
	}

	void BarOperationScene::BuildScene()
	{
		CreateUI();
	}

	std::shared_ptr<GameObject> ClientSpace::BarOperationScene::CreateUI()
	{
		std::shared_ptr<GameObject> ui = CreateEmpty();
		ui->AddComponent<BarCoreController>();
		ui->AddComponent<InGameUIController>();
		return ui;
	}
}