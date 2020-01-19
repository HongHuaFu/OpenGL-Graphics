#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>


class Application
{
private:
	unsigned int mWindowHeight = 1024;
	unsigned int mWindowWidth = 720;
	GLFWwindow* mWindow;


public:
	Application(bool& success,unsigned int window_width = 1024,unsigned int window_height = 720,const char* title = "OpenGL Application");

	void Run();

	virtual ~Application();

private: //»Øµ÷º¯Êý
	static void FrameBufferSizeChangeCallBack(GLFWwindow *_window,int _width,int _height);
	static void MouseCallBack(GLFWwindow* window, double xpos, double ypos);
	static void ScrollCallBack(GLFWwindow* window, double xoffset, double yoffset);
	void ProcessInput(GLFWwindow* _window);
};

