#pragma once
#include <GLFW/glfw3.h>
#include <memory>

namespace Engine
{
	class GLContext
	{
	public:
		static std::unique_ptr<GLContext> Create(GLFWwindow*); 
		void Use();
		void SwapBuffers();

	private:
		GLContext(GLFWwindow*);
		GLFWwindow* mWindow;
	};
}

