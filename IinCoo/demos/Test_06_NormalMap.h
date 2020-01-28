#pragma once
#include "api.h"

class Test_06_NormalMap : public Application
{
private:
	Skybox skybox;
	Model robot;
	Shader robotShader;
	unsigned int diffuseMap,normalMap,aoMap,hlMap;
public:
	Test_06_NormalMap():Application(){ }

	virtual void SenceInit() override
	{
		skybox = Skybox("../resources/skyboxs/gn_env/");
		robot = Model("../resources/models/robot/robot.obj");
		robotShader = Shader("../resources/shaders/normalmap/normalMap.vs", "../resources/shaders/normalmap/normalMap.fs");
		TextureFuncs::LoadTexture("../resources/models/robot/diffuse.jpg",&diffuseMap);
		TextureFuncs::LoadTexture("../resources/models/robot/normal.jpg",&normalMap);
		TextureFuncs::LoadTexture("../resources/models/robot/ao.jpg",&aoMap);
		TextureFuncs::LoadTexture("../resources/models/robot/highlight.jpg",&hlMap);
		robotShader.Use();
		robotShader.SetInt("diffuseMap", 0);
		robotShader.SetInt("normalMap", 1);
		robotShader.SetInt("aoMap", 2);
		robotShader.SetInt("hlMap", 3);
	}

	
	virtual void DrawLoop(Camera camera) override
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)mWindowWidth / (float)mWindowHeight, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		robotShader.Use();
		robotShader.SetMat4("projection", projection);
		robotShader.SetMat4("view", view);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model,glm::vec3(0.1f));

		robotShader.SetMat4("model", model);
		robotShader.SetVec3("viewPos", camera.Position);
		glm::vec3 lightPos(x,y,z);
		robotShader.SetVec3("lightPos", lightPos);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalMap);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, aoMap);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, hlMap);
		glEnable(GL_DEPTH_TEST);

		robot.Draw(robotShader);
		
		model = glm::translate(model,lightPos);
		robotShader.SetMat4("model", model);
		Geometry::renderCube();
		skybox.Draw(view,projection);
	}
	float x,y,z;
	virtual void ImguiFunc() override{
		ImGui::Begin("LightPos Y");
		ImGui::Text("Press E to open/close mouse.");
		ImGui::SliderFloat("x", &x, -10.f, 10.0f);
		ImGui::SliderFloat("y", &y, -10.f, 10.0f);
		ImGui::SliderFloat("z", &z, -10.f, 10.0f);
		ImGui::Text(" %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();

		ImGui::Begin("Tex");
		ImGui::Text("Tex Diffuse");
		ImGui::Image((void*)(intptr_t)diffuseMap, ImVec2(150, 150));
		ImGui::Text("Tex Normal");
		ImGui::Image((void*)(intptr_t)normalMap, ImVec2(150, 150));
		ImGui::Text("Tex AO");
		ImGui::Image((void*)(intptr_t)aoMap, ImVec2(150, 150));
		ImGui::Text("Tex High Light");
		ImGui::Image((void*)(intptr_t)hlMap, ImVec2(150, 150));
		ImGui::End();
	}
};