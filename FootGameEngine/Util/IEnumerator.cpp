#include "GamePch.h"
#include "IEnumerator.h"

namespace GameEngineSpace
{
	// 이 함수가 호출되면 코루틴이 실행되고 일시 중지된 지점에서부터 실행이 재개됩니다.
	IEnumeratorResult IEnumerator::Resume()
	{
		if (handle)
		{
			// WaitForSecond가 yield된 상태에서는 resume을 호출하지 않습니다.
			if (handle.promise().waitForSeconds && !handle.promise().waitForSeconds->CanResume())
			{
				return IEnumeratorResult::Running;		 // 아직 대기 중이므로 Running을 반환합니다.
			}
			else if (handle.promise().waitForSeconds && handle.promise().waitForSeconds->CanResume())
			{
				handle.promise().waitForSeconds.reset(); // nullptr로 초기화
			}

			handle.resume();

			// 코루틴이 완료되면 handle.done()이 true가 되고, handle은 nullptr로 초기화됩니다. 
			if (handle.done())
			{
				handle = nullptr;
				
				return IEnumeratorResult::Success;
			}

			return IEnumeratorResult::Running;
		}

		// 실행할 코루틴이 없으면 Failure를 반환합니다.
		return IEnumeratorResult::Failure;
	}

	bool IEnumerator::IsRunning() const
	{
		return handle != nullptr;
	}
}