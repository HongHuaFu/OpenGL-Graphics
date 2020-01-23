#include "Application.h"
#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include "Skybox.h"


float lastX = 0.0f;
float lastY = 0.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));


Application::Application(unsigned int window_width,unsigned int window_height,const char* title)
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
		return;
	}

	//创建当前窗口背景
	glfwMakeContextCurrent(mWindow);

	//绑定回调函数
	glfwSetFramebufferSizeCallback(mWindow,FrameBufferSizeChangeCallBack);
	glfwSetCursorPosCallback(mWindow,MouseCallBack);
	glfwSetScrollCallback(mWindow,ScrollCallBack);

	//GLFW捕捉鼠标
	glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	std::cout << "GLFW window init success!" << std::endl;

	lastX = mWindowWidth / 2.0f;
	lastY = mWindowHeight / 2.0f;
	return;
}

void Application::Run()
{
	SenceInit();
	while (!glfwWindowShouldClose(mWindow))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		gamelastframe = lastFrame;
		ProcessInput(mWindow);

		DrawLoop(camera);

		
		
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
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void Application::ScrollCallBack(GLFWwindow * _window,double xoffset,double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void Application::ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}




