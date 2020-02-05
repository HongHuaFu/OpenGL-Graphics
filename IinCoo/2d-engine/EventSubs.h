#pragma once
#include "Event.h"
#include <sstream>

namespace Engine
{
	class EventWindowResize : public Event
	{
	public:
		EventWindowResize(unsigned int width, unsigned int height):mWidth(width),mHeight(height) {}

		inline unsigned int GetWidth() const { return mWidth; }
		inline unsigned int GetHeight() const { return mHeight; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Event::Window::Resize: " << mWidth << ", " << mHeight;
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::WindowResize; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetEventName() const override { return "WindowResize"; }
		virtual int GetEventFlags() const override { return EventFlagApp; }
	private:
		unsigned int mWidth, mHeight;
	};

	class EventWindowClose : public Event
	{
	public:
		EventWindowClose() = default;

		static EventType GetStaticType() { return EventType::WindowClose; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetEventName() const override { return "WindowClose"; }
		virtual int GetEventFlags() const override { return EventFlagApp; }
	};

	class EventAppTick : public Event
	{
	public:
		EventAppTick() = default;

		static EventType GetStaticType() { return EventType::AppTick; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetEventName() const override { return "AppTick"; }
		virtual int GetEventFlags() const override { return EventFlagApp; }
	};

	class EventAppUpdate : public Event
	{
	public:
		EventAppUpdate() = default;

		static EventType GetStaticType() { return EventType::AppUpdate; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetEventName() const override { return "AppUpdate"; }
		virtual int GetEventFlags() const override { return EventFlagApp; }
	};

	class EventAppRender : public Event
	{
	public:
		EventAppRender() = default;

		static EventType GetStaticType() { return EventType::AppRender; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetEventName() const override { return "AppRender"; }
		virtual int GetEventFlags() const override { return EventFlagApp; }
	};

	class EventKey : public Event
	{
	public:
		inline int GetKeyCode() const { return mKeyCode; }
		virtual int GetEventFlags() const override { return EventFlagKeyBoard | EventFlagInput; }

	protected:
		EventKey(int keycode) : mKeyCode(keycode) { }
		int mKeyCode;
	};

	class EventKeyPressed : public EventKey
	{
	public:
		EventKeyPressed(int keycode, int repeatCount): EventKey(keycode),mRepeatCount(repeatCount) {}
		inline int GetRepeatCount() const { return mRepeatCount; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Event::Key::Pressed: " << mKeyCode << " (" << mRepeatCount << " repeats)";
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::KeyPressed; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetEventName() const override { return "KeyPressed"; }

	private:
		int mRepeatCount;
	};

	class EventKeyRelease : public EventKey
	{
	public:
		EventKeyRelease(int keycode): EventKey(keycode){}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Event::Key::Release: " << mKeyCode;
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::KeyReleased; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetEventName() const override { return "KeyReleased"; }
	};

	class EventKeyChar : public EventKey
	{
	public:
		EventKeyChar(int keycode): EventKey(keycode){}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Event::Key::Char: " << mKeyCode;
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::KeyChar; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetEventName() const override { return "KeyChar"; }
	};

	class EventMouseMoved : public Event
	{
	public:
		EventMouseMoved(float x, float y): mMouseX(x), mMouseY(y) {}

		inline float GetX() const { return mMouseX; }
		inline float GetY() const { return mMouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Event::Mouse::Moved: " << mMouseX << ", " << mMouseY;
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::MouseMoved; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetEventName() const override { return "MouseMoved"; }
		virtual int GetEventFlags() const override { return EventFlagMouse | EventFlagInput; }
	private:
		float mMouseX, mMouseY;
	};

	class EventMouseScroll : public Event
	{
	public:
		EventMouseScroll(float xOffset, float yOffset):mXOffset(xOffset), mYOffset(yOffset) {}

		inline float GetXOffset() const { return mXOffset; }
		inline float GetYOffset() const { return mYOffset; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Event::Mouse::Scroll: " << mXOffset << ", " << mYOffset;
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::MouseScrolled; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetEventName() const override { return "MouseScrolled"; }
		virtual int GetEventFlags() const override { return EventFlagMouse | EventFlagInput; }
	private:
		float mXOffset, mYOffset;
	};



	class EventMouseButton : public Event
	{
	public:
		inline int GetMouseButton() const { return mButton; }
		virtual int GetEventFlags() const override { return EventFlagMouse | EventFlagInput; }
	protected:
		EventMouseButton(int button):mButton(button) {}
		int mButton;
	};

	class EventMouseButtonPressed : public EventMouseButton
	{
	public:
		EventMouseButtonPressed(int button)	: EventMouseButton(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Event::Mouse::Button::Pressed: " << mButton;
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::MouseButtonPressed; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetEventName() const override { return "MouseButtonPressed"; }
	};

	class EventMouseButtonReleased : public EventMouseButton
	{
	public:
		EventMouseButtonReleased(int button):EventMouseButton(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Event::Mouse::Button::Released: " << mButton;
			return ss.str();
		}

		static EventType GetStaticType() { return EventType::MouseButtonReleased; }
		virtual EventType GetEventType() const override { return GetStaticType(); }
		virtual const char* GetEventName() const override { return "MouseButtonReleased"; }
	};
}
