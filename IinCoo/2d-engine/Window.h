#pragma once
#include "Event.h"
#include "GLContext.h"
#include <string>
#include <GLFW/glfw3.h>

namespace Engine
{
	//窗口初始化信息结构体
	struct WindowProps
	{
		std::string winTitle;
		unsigned int windowWidth,windowHeight;

		WindowProps(const std::string& title = "Tank War",unsigned int width = 1280,unsigned int height = 720):winTitle(title),windowWidth(width),windowHeight(height){ }
	};


	typedef void (*EventCallback)(Event&);
	class Window
	{
	public:
		Window() = delete;
		static std::unique_ptr<Window> Create(const WindowProps&);
		~Window();

		
	public:
		void OnUpdate();

	public://getter and setter
		unsigned int GetWindowWidth() const{ }
		unsigned int GetWindowHeight() const { }

		void SetWindowVSync(bool);
		bool GetWindowVSync() const;
	
		void SetEventCallback(void (*callback)(Event&)) { mWinData.callback = callback; }
		void* GetNativeWindow() const { return mWindow; }
	private:
		Window(const WindowProps&);
		struct WindowData
		{
			std::string title;
			unsigned int width,height;
			bool isVSync;
			EventCallback callback;
		};

		WindowData mWinData;
		static unsigned int mWindowCount; // 已经创建的窗口计数 
		GLFWwindow* mWindow; // info 使用智能指针报错
		std::unique_ptr<GLContext> mGLContext;
	};

	
}