#include "GamePch.h"
#include "IEnumerator.h"

namespace GameEngineSpace
{
	// �� �Լ��� ȣ��Ǹ� �ڷ�ƾ�� ����ǰ� �Ͻ� ������ ������������ ������ �簳�˴ϴ�.
	IEnumeratorResult IEnumerator::Resume()
	{
		if (handle)
		{
			// WaitForSecond�� yield�� ���¿����� resume�� ȣ������ �ʽ��ϴ�.
			if (handle.promise().waitForSeconds && !handle.promise().waitForSeconds->CanResume())
			{
				return IEnumeratorResult::Running;		 // ���� ��� ���̹Ƿ� Running�� ��ȯ�մϴ�.
			}
			else if (handle.promise().waitForSeconds && handle.promise().waitForSeconds->CanResume())
			{
				handle.promise().waitForSeconds.reset(); // nullptr�� �ʱ�ȭ
			}

			handle.resume();

			// �ڷ�ƾ�� �Ϸ�Ǹ� handle.done()�� true�� �ǰ�, handle�� nullptr�� �ʱ�ȭ�˴ϴ�. 
			if (handle.done())
			{
				handle = nullptr;
				
				return IEnumeratorResult::Success;
			}

			return IEnumeratorResult::Running;
		}

		// ������ �ڷ�ƾ�� ������ Failure�� ��ȯ�մϴ�.
		return IEnumeratorResult::Failure;
	}

	bool IEnumerator::IsRunning() const
	{
		return handle != nullptr;
	}
}