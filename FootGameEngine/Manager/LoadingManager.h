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

		// �ִϸ��̼��� ���� �ӽ� Ÿ�̸Ӹ� ������ݽô�
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

		// �̵�, ���� ������ ����
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
