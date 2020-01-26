#pragma once
#include "api.h"


class Test_01_SkyBox : public Application
{
public:
	Test_01_SkyBox():Application(){ }
	virtual void SenceInit() override
	{
		skybox = Skybox("../resources/skyboxs/gn_env/");
	}


	virtual void DrawLoop(Camera camera) override
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT| GL_STENCIL_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)mWindowWidth/(float)mWindowHeight,0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		skybox.Draw(view,projection);
	}

	virtual void ImguiFunc() override
	{

	}

private:

	Skybox skybox;
};

