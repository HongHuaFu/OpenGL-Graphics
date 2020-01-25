#pragma once
#include "api.h"

//#define Shader_debug

class Test_03_DefferedShading : public Application
{
private:
	GLuint gBuffer = 0;
	GLuint gPosition = 0,gNormal = 0,gAbedoSpec = 0;
	GLuint rboDepth = 0;//渲染缓冲对象
	Skybox skybox;
	//点光
	const GLuint NUM_LIGHTS = 100;
	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;

	Model cyborg;
	Model plane;
	Shader shaderGeometryPassShader;
	Shader shaderLightingPassShader;
#ifdef Shader_debug
	Shader debugShader;
#endif
	std::vector<glm::vec3> modelPositions;
public:
	Test_03_DefferedShading():Application(){ }

	virtual void SenceInit() override
	{
#pragma region Shader and model set
		skybox = Skybox("../resources/skyboxs/gn_env/");
		shaderGeometryPassShader = Shader("../resources/shaders/deferredshading/gBuffer.vs","../resources/shaders/deferredshading/gBuffer.fs");
		shaderLightingPassShader = Shader("../resources/shaders/deferredshading/light.vs", "../resources/shaders/deferredshading/light.fs");
#ifdef Shader_debug
		debugShader = Shader("../resources/shaders/deferredshading/debug.vs", "../resources/shaders/deferredshading/debug.fs");
#endif // 1

		

		
		
		//Model
		cyborg = Model("../resources/models/robot_spec/robot.obj");
		plane = Model("../resources/models/floor/floor.obj");
		modelPositions.push_back(glm::vec3(-3.0, -3.0, -3.0));
		modelPositions.push_back(glm::vec3(0.0, -3.0, -6.0));
		


#pragma endregion


#pragma region GBuffer
		//绑定gBuffer到帧缓存中
		glGenFramebuffers(1,&gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

		// 1 - 位置颜色缓冲
		glGenTextures(1, &gPosition);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mWindowWidth, mWindowHeight, 0, GL_RGB, GL_FLOAT, NULL);//16位
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

		// 2 - 法线信息缓冲
		glGenTextures(1, &gNormal);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mWindowWidth, mWindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

		// 3 - color + specular 缓冲
		glGenTextures(1, &gAbedoSpec);
		glBindTexture(GL_TEXTURE_2D, gAbedoSpec);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWindowWidth, mWindowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAbedoSpec, 0);

		//告诉openGL渲染到3个缓冲中
		unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);

		//创建渲染缓冲对象
		glGenRenderbuffers(1, &rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mWindowWidth, mWindowHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

		//检查帧缓存是否完备
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
#pragma endregion
		
#pragma region Lighting set
		
		srand(13);
		for (GLuint i = 0; i < NUM_LIGHTS; i++)
		{
			//随机灯光位置
			GLfloat xPos = ((rand() % 100) / 100.0) * -1.0f;
			GLfloat yPos = ((rand() % 100) / 100.0)* 0.5f;
			GLfloat zPos = ((rand() % 100) / 100.0) * -1.0f;
			lightPositions.push_back(glm::vec3(xPos, yPos, zPos));

			// 随机灯光颜色
			GLfloat rColor = (((rand() % 100) / 200.0f) + 0.5); 
			GLfloat gColor = (((rand() % 100) / 200.0f) + 0.5); 
			GLfloat bColor = ((rand() % 100) / 200.0f) + 0.5; 
			lightColors.push_back(glm::vec3(rColor, gColor, bColor));
		}

#pragma endregion

	
	}


	virtual void DrawLoop(Camera camera) override{
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//1. 几何处理阶段
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer); //绑定Gbuffer fBo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)mWindowWidth / (GLfloat)mWindowHeight, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model;


		shaderGeometryPassShader.Use();
		shaderGeometryPassShader.SetMat4("projection",projection);
		shaderGeometryPassShader.SetMat4("view",view);

		for (GLuint i = 0; i < modelPositions.size(); i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, modelPositions[i]);
			
			model = glm::scale(model, glm::vec3(0.25f));
			shaderGeometryPassShader.SetMat4("model",model);
		
			cyborg.Draw(shaderGeometryPassShader);
			
		}
		auto floorModel = glm::scale(model, glm::vec3(100.0f, 50.0f, 100.0f));
		shaderGeometryPassShader.SetMat4("model",floorModel);
		plane.Draw(shaderGeometryPassShader);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
	
		// 2. Lighting Pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderLightingPassShader.Use();
		
		glUniform1i(glGetUniformLocation(shaderLightingPassShader.ID, "gPosition"), 0);
		glUniform1i(glGetUniformLocation(shaderLightingPassShader.ID, "gNormal"), 1);
		glUniform1i(glGetUniformLocation(shaderLightingPassShader.ID, "gAlbedoSpec"), 2);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAbedoSpec);
		for (unsigned int i = 0; i < lightPositions.size(); i++)
		{
			glm::vec3 ppos;

			ppos.x = lightPositions[i].x + ( lightx+  0.35f*sin(gamelastframe * sin(i*17.9874 + 8.433324)));
			ppos.y =lighty+ lightPositions[i].y - 0.5* sin(gamelastframe / 10.0f) * cos(gamelastframe*gamelastframe);
			ppos.z =lightPositions[i].z +  0.3f*cos(gamelastframe * cos(i*14.9874 + 1.32421));
			shaderLightingPassShader.SetVec3("lights[" + std::to_string(i) + "].Position", ppos);
			shaderLightingPassShader.SetVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
			// update attenuation parameters and calculate radius
			const float constant = 1.0; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
			const float linear = 0.7;
			const float quadratic = 1.8;
			shaderLightingPassShader.SetFloat("lights[" + std::to_string(i) + "].Linear", linear);
			shaderLightingPassShader.SetFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
		}
		shaderLightingPassShader.SetVec3("viewPos", camera.Position);
		
		

		

		
#ifdef Shader_debug
		debugShader.Use();
		debugShader.SetInt("fboAttachment",0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gAbedoSpec);
#endif // Shader_debug
		glEnable(GL_DEPTH_TEST);
		renderQuad();

		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); //绑定到主FBO中
		glBlitFramebuffer(0, 0, mWindowWidth, mWindowHeight, 0, 0, mWindowWidth, mWindowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST); //复制深度缓冲
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		skybox.Draw(view,projection);
		
	}

	float lightx = 0.0f;
	float lighty = 0.0f;
	virtual void ImguiFunc() override{
		ImGui::Begin("LightPos Y");
		ImGui::Text("Press E to open/close mouse.");
		ImGui::SliderFloat("float x", &lightx, -2.0f, 2.0f);
		ImGui::SliderFloat("float y", &lighty, -2.0f, 2.0f);
		ImGui::Text(" %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Positon");
		ImGui::Image((void*)(intptr_t)gPosition, ImVec2(200, 200));
		ImGui::Text("Normal");
		ImGui::Image((void*)(intptr_t)gNormal, ImVec2(200, 200));
		ImGui::Text("Abedo && Spec");
		ImGui::Image((void*)(intptr_t)gAbedoSpec, ImVec2(200, 200));
		ImGui::End();
	}

	unsigned int quadVAO = 0;
	unsigned int quadVBO;
	void renderQuad()
	{
		if (quadVAO == 0)
		{
			float quadVertices[] = {
				// positions        // texture Coords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
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