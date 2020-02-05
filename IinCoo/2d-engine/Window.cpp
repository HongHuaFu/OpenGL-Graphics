#include "Window.h"
#include "Log.h"
#include "EventSubs.h"

namespace Engine
{
	std::unique_ptr<Window> Window::Create(const WindowProps &props)
	{
		return std::make_unique<Window>(props);
	}

	Window::~Window()
	{
		glfwDestroyWindow(mWindow);

		mWindowCount -= 1;
		if (mWindowCount == 0)
		{
			glfwTerminate();
		}
	}

	
	void Window::OnUpdate()
	{
		//事件弹出
		glfwPollEvents();
		//交换缓存
		mGLContext->SwapBuffers();
	}

	unsigned int Window::mWindowCount = 0;

	void Window::SetWindowVSync(bool open)
	{
		open ? glfwSwapInterval(1) : glfwSwapInterval(0);
		mWinData.isVSync = open;
	}

	bool Window::GetWindowVSync() const
	{
		return mWinData.isVSync;
	}

	Window::Window(const WindowProps &props)
	{
		mWinData.width = props.windowWidth;
		mWinData.height = props.windowHeight;
		mWinData.title = props.winTitle;

		LOG_INFO <<mWinData.title<< " Window Info: width: "<<mWinData.width<< " height: "<<mWinData.height;
		if(mWindowCount==0)
		{
			if(!glfwInit()) LOG_ERROR << "GLFW::Init error";
			glfwSetErrorCallback([](int error, const char* description){
				LOG_ERROR<<"GLFW::ERROR CODE: "<< error << "Detail: "<<description;
			});
		}

		mWindow = glfwCreateWindow(mWinData.width,mWinData.height,mWinData.title.c_str(),nullptr,nullptr);
		mWindowCount += 1;

		mGLContext = GLContext::Create(mWindow);
		mGLContext->Use();

		glfwSetWindowUserPointer(mWindow,&mWinData);
		SetWindowVSync(true);//开启垂直同步

		//回调事件绑定
		//1. 窗口尺寸修改回调函数
		glfwSetWindowSizeCallback(mWindow, [](GLFWwindow* window, int width, int height){
			WindowData* ptr = (WindowData*)glfwGetWindowUserPointer(window);
			ptr->width = width;
			ptr->height = height;
			EventWindowResize event(width,height);
			ptr->callback(event);
		});

		//2.窗口关闭事件
		glfwSetWindowCloseCallback(mWindow, [](GLFWwindow* window)
		{
			WindowData* ptr = (WindowData*)glfwGetWindowUserPointer(window);
			EventWindowClose event;
			ptr->callback(event);
		});

		//3.键盘事件
		glfwSetKeyCallback(mWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData* ptr = (WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				EventKeyPressed event(key, 0);
				ptr->callback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				EventKeyRelease event(key);
				ptr->callback(event);
				break;
			}
			case GLFW_REPEAT:
			{
				EventKeyPressed event(key, 1);
				ptr->callback(event);
				break;
			}
			}
		});

		//4.键盘输入事件
		glfwSetCharCallback(mWindow, [](GLFWwindow* window, unsigned int keycode)
		{
			WindowData* ptr = (WindowData*)glfwGetWindowUserPointer(window);

			EventKeyChar event(keycode);
			ptr->callback(event);
		});

		//5.鼠标事件
		glfwSetMouseButtonCallback(mWindow, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData* ptr = (WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				EventMouseButtonPressed event(button);
				ptr->callback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				EventMouseButtonReleased event(button);
				ptr->callback(event);
				break;
			}
			}
		});


		//6.滚轮事件
		glfwSetScrollCallback(mWindow, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData* ptr = (WindowData*)glfwGetWindowUserPointer(window);

			EventMouseScroll event((float)xOffset, (float)yOffset);
			ptr->callback(event);
		});

		//7.鼠标位置事件
		glfwSetCursorPosCallback(mWindow, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData* ptr = (WindowData*)glfwGetWindowUserPointer(window);

			EventMouseMoved event((float)xPos, (float)yPos);
			ptr->callback(event);
		});

	}


}

