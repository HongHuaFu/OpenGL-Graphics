#include "GLContext.h"
#include "Log.h"
#include <glad/glad.h>

namespace Engine
{
	std::unique_ptr<GLContext> GLContext::Create(GLFWwindow *window)
	{
		return std::make_unique<GLContext>(GLContext(window));
	}

	GLContext::GLContext(GLFWwindow* window):mWindow(window)
	{
		if(!mWindow)
		{
			LOG_ERROR << "GLContext::null window ptr";
		} 
	}

	void GLContext::Use()
	{
		glfwMakeContextCurrent(mWindow);
		int gladStatus = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		if(!gladStatus) LOG_ERROR << "GLContext::glad init error";

		LOG_TRACE << "Graphics Context Init.";
		LOG_INFO << "Renderer: "<< glGetString(GL_RENDERER);
		LOG_INFO << "Ver: "<<glGetString(GL_VERSION);
	}

	void GLContext::SwapBuffers()
	{
		glfwSwapBuffers(mWindow);
	}


}
