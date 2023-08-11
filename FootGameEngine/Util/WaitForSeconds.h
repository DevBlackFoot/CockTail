#pragma once

namespace GameEngineSpace
{
	// 대기할 시간을 입력받아 해당 시간만큼 대기하는 기능
	class WaitForSeconds
	{
	private:
		float seconds;

	public:
		WaitForSeconds(float _seconds) : seconds(_seconds) {}

		void Update(float tick);

		bool CanResume();
	};
}