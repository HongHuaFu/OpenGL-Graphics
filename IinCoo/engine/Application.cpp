#include "Application.h"

Application::Application(bool& success,unsigned int window_width,unsigned int window_height,const char* title)
{
	if(window_width!=0 && window_height!=0)
	{
		mWindowHeight = window_height;
		mWindowWidth = window_width;
	}
	
	//GLFW初始化配置
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	mWindow = glfwCreateWindow(mWindowWidth,mWindowHeight,title,nullptr,nullptr);
	if(!mWindow)
	{
		std::cout << "ERROR::Failed to create GLFW window" << std::endl;
		glfwTerminate();
		success = false;
		return;
	}

	//创建当前窗口背景
	glfwMakeContextCurrent(mWindow);

	//绑定回调函数
	glfwSetFramebufferSizeCallback(mWindow,FrameBufferSizeChangeCallBack);
	glfwSetCursorPosCallback(mWindow,MouseCallBack);
	glfwSetScrollCallback(mWindow,ScrollCallBack);

	//GLFW捕捉鼠标
	/*glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);*/

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		success = false;
		return;
	}

	success = true;
	if (success) {
		std::cout << "GLFW window init success!" << std::endl;
	}
}

void Application::Run()
{
	while (!glfwWindowShouldClose(mWindow))
	{
		ProcessInput(mWindow);


		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(mWindow);
		glfwPollEvents();
	}
}

Application::~Application()
{
	glfwTerminate();
}

void Application::FrameBufferSizeChangeCallBack(GLFWwindow *_window,int _width,int _height)
{
	//设置视口
	glViewport(0, 0, _width, _height);
	return;
}

void Application::MouseCallBack(GLFWwindow* _window,double xpos,double ypos)
{
	
}

void Application::ScrollCallBack(GLFWwindow * _window,double xoffset,double yoffset)
{
}

void Application::ProcessInput(GLFWwindow* _window)
{
	if(glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(_window, true);
}




