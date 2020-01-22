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
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);


	Shader diffuseShader("./shaders/l.vs","./shaders/l.fs");
	Shader robotShader("./shaders/li.vs","./shaders/li.fs");
	Model robot("./resources/robot/robot.obj");
	Model floor("./resources/floor/floor.obj");
	Skybox skybox("./resources/skybox/gn_env/");
	
	
		unsigned int amount = 100;
		glm::mat4* modelMatrices;
		modelMatrices = new glm::mat4[amount];


		for (unsigned int i = 0; i < 10; i++)
		{
			for(unsigned int j = 0; j<10; j++)
			{
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
				model = glm::translate(model, glm::vec3(i * 0.4f , 0, j*0.4f));
				model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));

				
				//model = glm::scale(model, glm::vec3(0.5f));

				modelMatrices[i * 10 + j] = model;
			}
			
		}

		// configure instanced array
		// -------------------------
		unsigned int buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

		//绑定实例变换矩阵属性
		for (unsigned int i = 0; i < robot.meshes.size(); i++)
		{
			unsigned int VAO = robot.meshes[i].VAO;
			glBindVertexArray(VAO);
			// set attribute pointers for matrix (4 times vec4)
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

			glVertexAttribDivisor(3, 1);
			glVertexAttribDivisor(4, 1);
			glVertexAttribDivisor(5, 1);
			glVertexAttribDivisor(6, 1);

			glBindVertexArray(0);
		}
	

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
		robotShader.Use();
		robotShader.SetMat4("projection", projection);
		robotShader.SetMat4("view", view);

		diffuseShader.Use();
		// render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

		auto floorModel = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
		diffuseShader.SetMat4("model", floorModel);
		//draw floor
		floor.Draw(diffuseShader);



		//我应该加一个实例化渲染模型方法吗？
		robotShader.Use();
		robotShader.SetInt("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, robot.texture_loaded[0].id);
		for (unsigned int i = 0; i < robot.meshes.size(); i++)
		{
			glBindVertexArray(robot.meshes[i].VAO);
			glDrawElementsInstanced(GL_TRIANGLES, robot.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
			glBindVertexArray(0);
		}

		skybox.Draw(view,projection);
		
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




