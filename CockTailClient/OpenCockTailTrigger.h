#pragma once
#include "InteractionTrigger.h"
#include "InGameUIController.h"

namespace ClientSpace
{

	class OpenCockTailTrigger : public InteractionTrigger
	{
	public:
		OpenCockTailTrigger(std::weak_ptr<GameObject> _obj);
		virtual ~OpenCockTailTrigger();

		virtual void Update(float tick) override;
		virtual void LateUpdate(float tick) override;

	};

}
