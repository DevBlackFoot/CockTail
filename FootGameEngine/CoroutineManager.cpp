#include "GamePch.h"
#include "CoroutineManager.h"
#include "Util/IEnumerator.h"

namespace GameEngineSpace
{
	std::shared_ptr<CoroutinManager> CoroutinManager::instance = nullptr;

	std::shared_ptr<CoroutinManager> CoroutinManager::GetInstance()
	{
		if (instance == nullptr)
			instance = std::make_shared<CoroutinManager>();

		return instance;
	}

	void CoroutinManager::StartCoroutine(IEnumerator coroutine)
	{
		coroutines.push_back(std::move(coroutine));
	}

	void CoroutinManager::StopCoroutine(IEnumerator coroutine)
	{
		for (auto it = coroutines.begin(); it != coroutines.end();)
		{
			if (it->handle == coroutine.handle)
			{
				it->handle.destroy();
				it = coroutines.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	void CoroutinManager::Update(float tick)
	{
		for (auto it = coroutines.begin(); it != coroutines.end();)
		{
			auto waitForSeconds = it->handle.promise().GetWaitForSeconds();
			
			if (waitForSeconds != nullptr)
				waitForSeconds->Update(tick);

			auto result = it->Resume();

			if (result == IEnumeratorResult::Success)
			{
				it = coroutines.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	void CoroutinManager::Release()
	{
		coroutines.clear();
	}
}