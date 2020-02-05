#pragma once
#include <string>

namespace Engine
{
	enum EventFlag
	{
		EventFlagApp    =  1<<0x00,
		EventFlagInput  =  1<<0x01,
		EventFlagKeyBoard  = 1<< 0x02,
		EventFlagMouse     = 1<<0x03,
		EventFlagMouseButton = 1<< 0x04
	};

	enum class EventType
	{
		//�����¼�
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,

		//�����¼�
		AppTick, AppUpdate, AppRender,

		//�����¼�
		KeyPressed, KeyReleased, KeyChar,

		//����¼�
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};
	
	class Event
	{
	public:
		bool isHandle = false;
		
		virtual EventType GetEventType() const = 0;
		virtual const char* GetEventName() const = 0;
		virtual int GetEventFlags() const = 0;


		virtual std::string ToString() const { return GetEventName(); }
		inline bool IsSameFlag(EventFlag category)
		{
			return GetEventFlags() & category;
		}
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
	
	//�¼��ַ�
	class EventDispatcher
	{
	public:
		EventDispatcher(Event& event): mEvent(event){}

		template<typename T, typename F>
		bool Dispatch(const F& func);
		
	private:
		Event& mEvent;
	};
	
}

