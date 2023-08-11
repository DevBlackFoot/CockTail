#pragma once
#include "Util/WaitForSeconds.h"

namespace GameEngineSpace
{
	enum class IEnumeratorResult
	{
		Success,
		Failure,
		Running
	};

	class IEnumerator
	{
	public:
		struct promise_type
		{
			std::shared_ptr<WaitForSeconds> waitForSeconds;

			IEnumerator get_return_object() noexcept
			{
				return { std::coroutine_handle<promise_type>::from_promise(*this) };
			}

			std::suspend_never initial_suspend()
			{
				return {};
			}

			std::suspend_always final_suspend() noexcept
			{
				return {};
			}

			std::suspend_always yield_value(std::shared_ptr<WaitForSeconds> _waitForSeconds)
			{
				if (waitForSeconds == nullptr)
				{
					waitForSeconds = _waitForSeconds;

					return {};
				}
			}

			std::shared_ptr<WaitForSeconds>& GetWaitForSeconds() { return waitForSeconds; }

			void return_void() const
			{

			}

			void unhandled_exception() const
			{
				std::terminate();
			}

			promise_type() :
				waitForSeconds(nullptr)
			{

			}

			~promise_type() noexcept
			{
				waitForSeconds.reset();
			}
		};

	public:
		IEnumerator() = default;
		IEnumerator(std::coroutine_handle<promise_type> _handle)
		 : handle(_handle) {}

	public:
		IEnumeratorResult Resume(); 

		bool IsRunning() const;

	public:
		// Promise는 Coroutine의 시작과 끝을 알리는 역할을 한다.
		// Promise 객체는 Coroutine의 실행 결과를 저장하고 Coroutine이 재개될 때 해당 Promise 객체의 상태를 사용하여
		// 실행을 계속할지 여부를 결정합니다.
		std::coroutine_handle<promise_type> handle = nullptr;
	};
}