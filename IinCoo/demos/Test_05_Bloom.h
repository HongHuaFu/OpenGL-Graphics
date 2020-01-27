#pragma once
#include "api.h"

class Test_05_Bloom : public Application
{
private:
	GLuint gBuffer = 0;
	GLuint gPosition = 0,gNormal = 0,gAbedoSpec = 0;
	GLuint rboDepth = 0;//渲染缓冲对象


	unsigned int hdrFBO;
	unsigned int hdrBuffers[2];

	unsigned int blurFBO[2];
	unsigned int blurBuffers[2];

	Skybox skybox;


	//点光
	const GLuint NUM_LIGHTS = 100;
	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;

	Model cyborg;
	Model plane;
	Shader shaderGeometryPassShader;
	Shader shaderLightingPassShader;
	Shader blurPassShader;
	Shader mergeShader;

	std::vector<glm::vec3> modelPositions;
public:
	Test_05_Bloom():Application(){ }

	virtual void SenceInit() override
	{
#pragma region Shader and model set
		skybox = Skybox("../resources/skyboxs/gn_env/");
		shaderGeometryPassShader = Shader("../resources/shaders/bloom/gBuffer.vs","../resources/shaders/bloom/gBuffer.fs");
		shaderLightingPassShader = Shader("../resources/shaders/bloom/light.vs", "../resources/shaders/bloom/light.fs");
		blurPassShader = Shader("../resources/shaders/bloom/blur.vs", "../resources/shaders/bloom/blur.fs");
		mergeShader = Shader("../resources/shaders/bloom/merge.vs", "../resources/shaders/bloom/merge.fs");

		blurPassShader.Use();
		blurPassShader.SetInt("image",0);
		mergeShader.Use();
		mergeShader.SetInt("scene", 0);
		mergeShader.SetInt("bloomBlur", 1);

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

#pragma region HDR Buffer Set
		glGenFramebuffers(1,&hdrFBO);
		glBindFramebuffer(GL_FRAMEBUFFER,hdrFBO);
		glGenTextures(2,hdrBuffers);
		for (unsigned int i = 0; i < 2; i++)
		{
			glBindTexture(GL_TEXTURE_2D, hdrBuffers[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mWindowWidth, mWindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, hdrBuffers[i], 0);
		}
		unsigned int hdrRboDepth;
		glGenRenderbuffers(1, &hdrRboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, hdrRboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mWindowWidth, mWindowHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, hdrRboDepth);
		unsigned int attachmentsX[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, attachmentsX);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
#pragma endregion

#pragma region Blur Buffer Set
		glGenFramebuffers(2, blurFBO);
		glGenTextures(2, blurBuffers);
		for (unsigned int i = 0; i < 2; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, blurFBO[i]);
			glBindTexture(GL_TEXTURE_2D, blurBuffers[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mWindowWidth, mWindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurBuffers[i], 0);
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "Framebuffer not complete!" << std::endl;
		}

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
			GLfloat rColor = (((rand() % 100) / 200.0f) + 0.5)* 0.8f; 
			GLfloat gColor = (((rand() % 100) / 200.0f) + 0.5) * 0.3f; 
			GLfloat bColor = ((rand() % 100) / 200.0f) + 0.5; 
			lightColors.push_back(glm::vec3(rColor, gColor, bColor));
		}

#pragma endregion
	}

	float threshold = 1.0f;
	bool horizontal = true;
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


		// HDR FBO 
		glBindFramebuffer(GL_FRAMEBUFFER,hdrFBO);
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

			const float constant = 1.0; 
			const float linear = 0.7;
			const float quadratic = 1.8;
			shaderLightingPassShader.SetFloat("lights[" + std::to_string(i) + "].Linear", linear);
			shaderLightingPassShader.SetFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
		}
		shaderLightingPassShader.SetVec3("viewPos", camera.Position);
		shaderLightingPassShader.SetFloat("threshold",threshold);
		glEnable(GL_DEPTH_TEST);
		Geometry::renderQuad();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//Blur Pass
		blurPassShader.Use();
		bool first_iteration = true;
		unsigned int amount = 10;
		for (unsigned int i = 0; i < amount; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, blurFBO[horizontal]);
			blurPassShader.SetInt("horizontal", horizontal);
			glActiveTexture(GL_TEXTURE0);
			if(first_iteration)
			{
				//第一迭代时将光贴图传入
				glBindTexture(GL_TEXTURE_2D,hdrBuffers[1]);
			}
			else
			{
				//将上次生成的模糊贴图传入处理
				//生成水平高斯模糊和垂直高斯模糊贴图
				
				glBindTexture(GL_TEXTURE_2D, blurBuffers[!horizontal]);
			}
			
			Geometry::renderQuad();
			horizontal = !horizontal;//最后一次的贴图将会是horizontal
			if (first_iteration)
				first_iteration = false;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// merge pass
		mergeShader.Use();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hdrBuffers[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, blurBuffers[!horizontal]);
		mergeShader.SetInt("bloom", isBloom);
		mergeShader.SetFloat("exposure", exposure);
		Geometry::renderQuad();


		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); //绑定到主FBO中
		glBlitFramebuffer(0, 0, mWindowWidth, mWindowHeight, 0, 0, mWindowWidth, mWindowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST); //复制深度缓冲
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		skybox.Draw(view,projection);

	}



	bool isBloom = true;
	float exposure = 1.0f;
	float lightx = 0.0f;
	float lighty = 0.0f;
	virtual void ImguiFunc() override{
		ImGui::Begin("LightPos Y");
		ImGui::Text("Press E to open/close mouse.");
		ImGui::SliderFloat("exposure", &exposure, 0.5f, 2.0f);
		ImGui::SliderFloat("threshold", &threshold, 0.5f, 1.0f);
		ImGui::Text(" %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Light Bloom");
		ImGui::Image((void*)(intptr_t)hdrBuffers[1], ImVec2(200, 200));
		ImGui::Text("Blur Tex");
		ImGui::Image((void*)(intptr_t)blurBuffers[horizontal], ImVec2(200, 200));
		
		ImGui::Text("Abedo");
		ImGui::Image((void*)(intptr_t)hdrBuffers[0], ImVec2(200, 200));
		ImGui::End();
		
		
	}
};