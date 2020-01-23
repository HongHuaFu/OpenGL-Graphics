#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Camera.h"


class Application
{
public:
	unsigned int mWindowHeight = 1024;
	unsigned int mWindowWidth = 720;
	GLFWwindow* mWindow;
	float gamelastframe = 0.0f;


	Application(unsigned int window_width = 1024,unsigned int window_height = 720,const char* title = "OpenGL Application");

	void Run();
	virtual void SenceInit() = 0;
	virtual void DrawLoop(Camera) = 0;
	virtual ~Application();


	static void FrameBufferSizeChangeCallBack(GLFWwindow *_window,int _width,int _height);
	static void MouseCallBack(GLFWwindow* window, double xpos, double ypos);
	static void ScrollCallBack(GLFWwindow* window, double xoffset, double yoffset);
	void ProcessInput(GLFWwindow* _window);
};

