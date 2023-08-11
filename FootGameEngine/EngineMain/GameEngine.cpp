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
		// ��� �Ŵ����� Init �մϴ�.
		WindowManager::GetInstance()->InitWindow(gameTitle, hInst, screenWidth, screenHeight);
		Timer::GetInstance()->Init();
		InputManager::GetInstance();
		GraphicsManager::GetInstance()->Init(WindowManager::GetInstance()->GetHWND(), screenWidth, screenHeight);
		UIManager::GetInstance()->Init();

		SceneManager::GetInstance()->Init();
		NavigationManager::GetInstance();
		SoundManager::GetInstance()->Initialize();

		// ��� �Ŵ����� Init�� ������.. �����츦 ����ݴϴ�.
		WindowManager::GetInstance()->StartWindow();

		// UI�� �׸����ְ���
		LoadingManager::GetInstance()->Init();
		GraphicsManager::GetInstance()->UIUtilInit();
		UIManager::GetInstance()->CreateCanvasAll();

		// �ε嵥���� ������ ����
		LoadingManager::GetInstance()->StartLoadDataThread();
	}

	INT GameEngine::Tick()
	{
		// �� ƽ ��������..
			// ������ �Ŵ��� ������ update�� �����ϴ�
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
