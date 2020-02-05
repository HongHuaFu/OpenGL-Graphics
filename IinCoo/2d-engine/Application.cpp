#include "Application.h"
#include "Log.h"
#include <functional>

namespace Engine
{
	Application* Application::sInstance = nullptr;

	Application::Application()
	{
		if(!sInstance) LOG_ERROR<<"application has inited before.";
		sInstance = this;
		mWindow = Window::Create(WindowProps());
		
		mWindow->SetEventCallback(Application::OnEvent);
	}

}

