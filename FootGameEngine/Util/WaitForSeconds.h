#pragma once

namespace GameEngineSpace
{
	// ����� �ð��� �Է¹޾� �ش� �ð���ŭ ����ϴ� ���
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