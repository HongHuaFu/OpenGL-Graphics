#pragma once
#include "api.h"




//直射光，shadowMap
class Test_02_ShadowMap : public Application
{
public:
	Test_02_ShadowMap():Application(){ }


	virtual void SenceInit() override
	{
		skybox = Skybox("../resources/skyboxs/gn_env/");
		floor = Model("../resources/models/floor/floor.obj");
		robot = Model("../resources/models/robot/robot.obj");
		modelshader = Shader("../resources/shaders/shadowmap/shadowMap.vs","../resources/shaders/shadowmap/shadowMap.fs");
		depthshader = Shader("../resources/shaders/shadowmap/shadowDepth.vs","../resources/shaders/shadowmap/shadowDepth.fs");
		debugshader = Shader("../resources/shaders/shadowmap/debugshadow.vs","../resources/shaders/shadowmap/debugshadow.fs");
		lightPos = glm::vec3(-4.0f, 4.0f, 4.0f);


		
		glGenFramebuffers(1, &depthMapFBO);
		// 创建深度贴图
		
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glEnable(GL_DEPTH_TEST);
		
	}


	virtual void DrawLoop(Camera camera) override
	{
		float aa =  sqrt(1.0f - LightPosY*LightPosY);
		lightPos.x = aa* sin(gamelastframe);
		lightPos.z = aa *cos(gamelastframe);
		lightPos.y = LightPosY;


		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)mWindowWidth/(float)mWindowHeight,0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		GLfloat near_plane = 0.1f, far_plane = 10.0f;
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		// - now render scene from light's point of view
		depthshader.Use();
		depthshader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);
	
		glCullFace(GL_FRONT);
		
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO); //绑定FBO
			glClear(GL_DEPTH_BUFFER_BIT);
			RenderSence(depthshader); //渲染到FBO
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glCullFace(GL_BACK);

		glViewport(0,0,mWindowWidth,mWindowHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		modelshader.Use();
		modelshader.SetMat4("projection", projection);
		modelshader.SetMat4("view", view);
		// set light uniforms
		modelshader.SetVec3("viewPos", camera.Position);
		modelshader.SetVec3("lightPos", lightPos);
		modelshader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

		//从10开始的贴图槽被占用了
		modelshader.SetInt("shadowMap", 0);//绑定贴图
		glActiveTexture(GL_TEXTURE0);
		
		glBindTexture(GL_TEXTURE_2D, depthMap);
		RenderSence(modelshader);

		debugshader.Use();
		debugshader.SetInt("depthMap", 0);
		debugshader.SetFloat("near_plane", near_plane);
		debugshader.SetFloat("far_plane", far_plane);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		renderQuad();
		skybox.Draw(view,projection);
		glClear(GL_DEPTH_BUFFER_BIT);

	}

	virtual void ImguiFunc() override
	{
		ImGui::Begin("LightPos Y");
		ImGui::Text("Press E to open/close mouse catch.");
		ImGui::SliderFloat("float", &LightPosY, 0.01f, 0.99f);
		ImGui::Text("LightPos Y = %f", LightPosY);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

private:
	float LightPosY = sqrt(2.0f)/2.0f;
	void RenderSence(Shader mshader)
	{
		mshader.Use();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(1.0f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		auto floorModel = glm::scale(model, glm::vec3(100.0f, 50.0f, 100.0f));
		mshader.SetMat4("model",floorModel);
		floor.Draw(mshader);

		//model = glm::translate(model, glm::vec3(0.0f, 1.75f, 0.0f));
		mshader.SetMat4("model",model);
		robot.Draw(mshader);
	}

	Skybox skybox;
	glm::vec3 lightPos; //直射光位置
	Model floor;
	Model robot;
	const GLuint SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;//阴影贴图大小
	Shader modelshader;
	Shader depthshader;
	Shader debugshader;
	GLuint depthMapFBO = 0;
	GLuint depthMap = 0;

	unsigned int quadVAO = 0;
	unsigned int quadVBO;

	void renderQuad()
	{
		if (quadVAO == 0)
		{
			float quadVertices[] = {
				// positions        // texture Coords
				-1.0f,  -0.5f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				-0.5f,  -0.5f, 0.0f, 1.0f, 1.0f,
				-0.5f, -1.0f, 0.0f, 1.0f, 0.0f,
			};
			// setup plane VAO
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		}
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}
};

