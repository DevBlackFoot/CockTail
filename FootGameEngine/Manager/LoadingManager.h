#pragma once
#include "Define/FootEngineTypedef.h"

namespace GameEngineSpace
{
	class LoadingManager final
	{
		static std::shared_ptr<LoadingManager> instance;

	private:
		std::thread loadingThread;

		std::thread loadDataThread;

		bool isLoadingImageThreadOn = true;

		// 애니메이션을 위한 임시 타이머를 만들어줍시다
		float totalAnimTime;
		int64 currTime;
		int64 prevTime;
		float deltaTime;
		float secondPerCount;

	public:
		static bool isLoading;

		static bool isLoadingEnd;
	
	public:
		LoadingManager() = default;

		~LoadingManager();

		// 이동, 복사 생성자 제거
		LoadingManager(const LoadingManager& _other) = delete;
		LoadingManager& operator= (const LoadingManager& _other) = delete;
		LoadingManager(LoadingManager&& _other) = delete;
		LoadingManager& operator= (LoadingManager&& _other) = delete;

		static std::shared_ptr<LoadingManager> GetInstance();

		void Init();

		void ShowLoadingImage();

		void StartLoadDataThread();

		void StopLoadDataThread();

		void LoadData();
	};
}
