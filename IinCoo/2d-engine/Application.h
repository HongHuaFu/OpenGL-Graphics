#pragma once

#include "EventSubs.h"
#include "Layer.h"
#include "Window.h"
#include "UILayer.h"

namespace Engine
{
	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		static void OnEvent(Event&);

		void PushLayer(Layer*);
		void PushOverlay(Layer*);

		inline Window& GetWindow() { return *mWindow; }
		inline static Application& Get() { return *sInstance; }
	private:
		bool OnWindowClose(EventWindowClose&);
		bool OnWindowResize(EventWindowResize&);
	private:
		std::unique_ptr<Window> mWindow;
		UILayer* mImGuiLayer;
		bool mRunning = true;
		bool mMinimized = false;
		LayerStack mLayerStack;
		float mLastFrameTime = 0.0f;
	private:
		static Application* sInstance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();

}
