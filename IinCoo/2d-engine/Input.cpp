#include "Input.h"
#include "Application.h"
#include <GLFW/glfw3.h>

namespace Engine
{
	std::unique_ptr<Input> Input::sInstance = std::make_unique<Input>();

	bool Input::IsKeyPressed(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}
}
