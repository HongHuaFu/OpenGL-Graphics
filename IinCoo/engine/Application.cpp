#include "Application.h"
#include "Shader.h"
#include "Model.h"
#include "Camera.h"


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
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);//开启模板测试
	glStencilFunc(GL_NOTEQUAL,1,0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);//在模板测试和深度测试都通过时替换像素

	Shader diffuseShader("./shaders/l.vs","./shaders/l.fs");
	Shader shellShader("./shaders/shell.vs","./shaders/shell.fs");
	Model robot("./resources/robot/robot.obj");
	Model floor("./resources/floor/floor.obj");
	
	
	
	while (!glfwWindowShouldClose(mWindow))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		ProcessInput(mWindow);

		

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT| GL_STENCIL_BUFFER_BIT);

		diffuseShader.Use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)mWindowWidth / (float)mWindowHeight, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		diffuseShader.SetMat4("projection", projection);
		diffuseShader.SetMat4("view", view);

		// render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

		auto floorModel = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
		diffuseShader.SetMat4("model", floorModel);
		glStencilMask(0x00);
		floor.Draw(diffuseShader);
		diffuseShader.SetMat4("model", model);
		shellShader.Use();
		shellShader.SetMat4("projection", projection);
		shellShader.SetMat4("view", view);
		

		diffuseShader.Use();

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF); //开启模板缓存写入
		robot.Draw(diffuseShader);
		float scale = 1.03f;
		float offset = -0.3f;
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);//未绘制区域描线
		glStencilMask(0x00);//关闭模板写入
		glDisable(GL_DEPTH_TEST);
		
		shellShader.Use();
		model = glm::translate(model, glm::vec3(offset, offset, offset));
		model = glm::scale(model, glm::vec3(scale, scale, scale));
		shellShader.SetMat4("model", model);
		robot.Draw(shellShader);
		glStencilMask(0xFF);
		glEnable(GL_DEPTH_TEST);
		
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




