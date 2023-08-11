#include "GamePch.h"
#include "WaitForSeconds.h"
#include "Timer.h"

namespace GameEngineSpace
{
	void WaitForSeconds::Update(float tick)
	{
		seconds -= tick;
	}

	bool WaitForSeconds::CanResume()
	{
		return seconds <= 0.f ? true : false;
	}
}