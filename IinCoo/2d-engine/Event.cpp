#include "Event.h"

namespace Engine
{
	template<typename T, typename F>
	bool Engine::EventDispatcher::Dispatch(const F& func)
	{
		if (mEvent.GetEventType() == T::GetStaticType())
		{
			mEvent.isHandle = func(static_cast<T&>(mEvent));
			return true;
		}
		return false;
	}
}
