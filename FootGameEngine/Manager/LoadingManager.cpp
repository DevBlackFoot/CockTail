#include "GamePch.h"
#include "LoadingManager.h"
#include "UIManager.h"
#include "GraphicsManager.h"
#include "Timer.h"

namespace GameEngineSpace
{
	std::shared_ptr<LoadingManager> LoadingManager::instance = nullptr;

	bool LoadingManager::isLoading = true;

	bool LoadingManager::isLoadingEnd = false;

	LoadingManager::~LoadingManager()
	{
		if (loadDataThread.joinable())
		{
			StopLoadDataThread();
		}
	}

	std::shared_ptr<LoadingManager> LoadingManager::GetInstance()
	{
		// 인스턴스가 없으면 만들어주고 있으면 인스턴스 리턴
		if (instance == nullptr)
		{
			instance = std::make_shared<LoadingManager>();
		}

		return instance;
	}

	void LoadingManager::Init()
	{
		totalAnimTime = 0.f;
		currTime = 0;
		prevTime = 0;

		int64 initTime;
		QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&initTime));

		secondPerCount = 1.0f / static_cast<float>(initTime);
	}

	void LoadingManager::ShowLoadingImage()
	{
		// 델타를 구해줍시다.
		int64 _currTime;
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&_currTime));
		currTime = _currTime;
		
		float deltaTime = static_cast<float>(currTime - prevTime) * secondPerCount;
		prevTime = currTime;

		if(totalAnimTime > 7.0f)
			totalAnimTime = 0.f;

		int loadingIdx = static_cast<int>(floor(totalAnimTime));

		auto sceneCanvas = UIManager::GetInstance()->GetMainCanvas()->GetCanvasUI("SceneCanvas");
		sceneCanvas->SetEnable(true);
		sceneCanvas->GetSpriteUI("LoadingSpriteUI")->SetEnable(true);
		sceneCanvas->GetSpriteUI("LoadingAnim")->SetEnable(true);
		sceneCanvas->GetSpriteUI("LoadingAnim")->SetSrcOffset(RECT{ 94 * loadingIdx, 0, 94 * (loadingIdx + 1), 119 });

		totalAnimTime += deltaTime * 7.0f;

		sceneCanvas->GetSpriteUI("SceneBackGroundSpriteUI")->SetEnable(false);

		// 렌더 필수
		auto renderer = GraphicsManager::GetInstance()->GetRenderer();
		renderer->BeginMainRender();
		//renderer->PostProcessing();

		sceneCanvas->Render(renderer, 0.0f);

		renderer->EndRender();
	}

	void LoadingManager::StartLoadDataThread()
	{
		loadDataThread = std::thread(&LoadingManager::LoadData, this);
	}

	void LoadingManager::StopLoadDataThread()
	{
		if (loadDataThread.joinable())
		{
			loadDataThread.join();
		}
	}

	void LoadingManager::LoadData()
	{
		GraphicsManager::GetInstance()->LateInit();

		isLoading = false;

		isLoadingEnd = true;
	}
}
