#include "GamePch.h"
#include "GameEngine.h"

#include "InputManager.h"
#include "Timer.h"
#include "WindowManager.h"
#include "GraphicsManager.h"
#include "SceneManager.h"
#include "UIManager.h"
#include "NavigationManager.h"
#include "SoundManager.h"
#include "LoadingManager.h"
#include "CoroutineManager.h"
#include "Util/ObjectPool.h"

namespace GameEngineSpace
{
	std::shared_ptr<GameEngine> GameEngine::gameEngineInstance = nullptr;

	void GameEngine::Init(tstring gameTitle, HINSTANCE hInst, int screenWidth, int screenHeight)
	{
		// 모든 매니저를 Init 합니다.
		WindowManager::GetInstance()->InitWindow(gameTitle, hInst, screenWidth, screenHeight);
		Timer::GetInstance()->Init();
		InputManager::GetInstance();
		GraphicsManager::GetInstance()->Init(WindowManager::GetInstance()->GetHWND(), screenWidth, screenHeight);
		UIManager::GetInstance()->Init();

		SceneManager::GetInstance()->Init();
		NavigationManager::GetInstance();
		SoundManager::GetInstance()->Initialize();

		// 모든 매니저의 Init이 끝난뒤.. 윈도우를 띄워줍니다.
		WindowManager::GetInstance()->StartWindow();

		// UI를 그릴수있게함
		LoadingManager::GetInstance()->Init();
		GraphicsManager::GetInstance()->UIUtilInit();
		UIManager::GetInstance()->CreateCanvasAll();

		// 로드데이터 스레드 생성
		LoadingManager::GetInstance()->StartLoadDataThread();
	}

	INT GameEngine::Tick()
	{
		// 매 틱 돌때마다..
			// 윈도우 매니저 내부의 update를 돌립니다
		return WindowManager::GetInstance()->Update();
	}

	void GameEngine::Release()
	{
		Timer::GetInstance()->Release();
		SceneManager::GetInstance()->Release();
		NavigationManager::GetInstance()->Release();
		SoundManager::GetInstance()->Release();
		GraphicsManager::GetInstance()->Release();
		WindowManager::GetInstance()->Release();
		CoroutinManager::GetInstance()->Release();
		ObjectPool::GetInstance()->Release();
	}
}
