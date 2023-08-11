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
		// Promise�� Coroutine�� ���۰� ���� �˸��� ������ �Ѵ�.
		// Promise ��ü�� Coroutine�� ���� ����� �����ϰ� Coroutine�� �簳�� �� �ش� Promise ��ü�� ���¸� ����Ͽ�
		// ������ ������� ���θ� �����մϴ�.
		std::coroutine_handle<promise_type> handle = nullptr;
	};
}