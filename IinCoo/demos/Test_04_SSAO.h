#pragma once
#include "api.h"
#include <random>
#include <cmath>

class Test_04_SSAO:public Application
{
private:
	Model sponza;
	Shader geometryShader;
	Shader lightShader;
	Shader ssaoShader;
	Shader blurShader;

	glm::vec3 lightPos;
	glm::vec3 lightColor;
public:
	virtual void SenceInit() override
	{
		InitSence();
		InitGbuffer();
		InitSSAO();
		InitSampleKernel();
	}

	float ssaoRadius = 0.5f;
	float ssaoBias = 0.025f;
	virtual void DrawLoop(Camera camera) override
	{
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//���δ���׶�
#pragma region Geometry Pass
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)mWindowWidth / (float)mWindowHeight, 0.1f, 50.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.25f));
		geometryShader.Use();
		geometryShader.SetMat4("projection", projection);
		geometryShader.SetMat4("view", view);

		auto kModel = glm::translate(model,glm::vec3(-1.0f,.0f,0.0f));
		geometryShader.SetMat4("model", kModel);
		sponza.Draw(geometryShader);
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
#pragma endregion

#pragma region SSAO
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
		glClear(GL_COLOR_BUFFER_BIT);
		ssaoShader.Use();
		for (unsigned int i = 0; i < 64; ++i)
			ssaoShader.SetVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
		ssaoShader.SetMat4("projection", projection);
		ssaoShader.SetFloat("radius",ssaoRadius);
		ssaoShader.SetFloat("bias",ssaoBias);
		ssaoShader.SetVec2("WindowSize",glm::vec2(mWindowWidth,mWindowHeight));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);
		Geometry::renderQuad();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//blur
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
		glClear(GL_COLOR_BUFFER_BIT);
		blurShader.Use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
		Geometry::renderQuad();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

#pragma endregion

#pragma region Light Pass
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		lightShader.Use();
		//���õƹ���Ϣ
		glm::vec3 lightPosView = glm::vec3(camera.GetViewMatrix() * glm::vec4(lightPos, 1.0));
		lightShader.SetVec3("light.Position", lightPosView);
		lightShader.SetVec3("light.Color", lightColor);
		// ����˥��ֵ
		const float constant  = 1.0; 
		const float linear    = 0.09;
		const float quadratic = 0.032;
		lightShader.SetFloat("light.Linear", linear);
		lightShader.SetFloat("light.Quadratic", quadratic);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlbedo);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
		glEnable(GL_DEPTH_TEST);
		Geometry::renderQuad();
#pragma endregion
		
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); //�󶨵���FBO��
		glBlitFramebuffer(0, 0, mWindowWidth, mWindowHeight, 0, 0, mWindowWidth, mWindowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST); //������Ȼ���
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		skybox.Draw(view,projection);

	}

	bool openSSAO = true;
	virtual void ImguiFunc() override
	{
		ImGui::Begin("LightPos Y");
		ImGui::Text("Press E to open/close mouse.");
		ImGui::Text(" %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::SliderFloat("float ssaoBias", &ssaoBias, 0.01f, 0.04f);
		ImGui::SliderFloat("float ssaoRadius", &ssaoRadius, 0.25f, 3.0f);
		ImGui::Text("SSAO");
		ImGui::Image((void*)(intptr_t)ssaoColorBuffer, ImVec2(200, 200));
		ImGui::Text("Positon");
		ImGui::Image((void*)(intptr_t)gPosition, ImVec2(200, 200));
		ImGui::Text("Normal");
		ImGui::Image((void*)(intptr_t)gNormal, ImVec2(200, 200));
		ImGui::End();
	}
	Skybox skybox = Skybox("../resources/skyboxs/gn_env/");
private:
	void InitSence()
	{
		geometryShader = Shader("../resources/shaders/ssao/gBuffer.vs","../resources/shaders/ssao/gBuffer.fs");
		lightShader = Shader("../resources/shaders/ssao/light.vs","../resources/shaders/ssao/light.fs");
		blurShader = Shader("../resources/shaders/ssao/blur.vs","../resources/shaders/ssao/blur.fs");
		ssaoShader = Shader("../resources/shaders/ssao/ssao.vs","../resources/shaders/ssao/ssao.fs");

		sponza = Model("../resources/models/sponza/spz.obj");

		lightPos = glm::vec3(2.0, 4.0, -2.0);
		lightColor = glm::vec3(1.0f, 0.98f, .96f);

		//�趨��ͼ���
		lightShader.Use();
		lightShader.SetInt("gPosition", 0);
		lightShader.SetInt("gNormal", 1);
		lightShader.SetInt("gAlbedo", 2);
		lightShader.SetInt("ssao", 3);

		ssaoShader.Use();
		ssaoShader.SetInt("gPosition", 0);
		ssaoShader.SetInt("gNormal", 1);
		ssaoShader.SetInt("texNoise", 2);

		blurShader.Use();
		blurShader.SetInt("ssaoInput", 0);

		
		
	}

	unsigned int gBuffer = 0,gPosition =0, gNormal = 0, gAlbedo =0;
	void InitGbuffer()
	{
		//����֡���沢��  //�ó�д����ϵĴ���orz
		glGenFramebuffers(1, &gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);


		glGenTextures(1, &gPosition);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mWindowWidth, mWindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

		glGenTextures(1, &gNormal);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mWindowWidth, mWindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);


		unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);
		unsigned int rboDepth;
		glGenRenderbuffers(1, &rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mWindowWidth, mWindowHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	unsigned int ssaoFBO= 0, ssaoBlurFBO=0,ssaoColorBuffer=0, ssaoColorBufferBlur=0;
	void InitSSAO()
	{
		glGenFramebuffers(1, &ssaoFBO);  
		glGenFramebuffers(1, &ssaoBlurFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);


		//SSAO
		glGenTextures(1, &ssaoColorBuffer);
		glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mWindowWidth, mWindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "SSAO Framebuffer not complete!" << std::endl;

		//Blur
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
		glGenTextures(1, &ssaoColorBufferBlur);
		glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mWindowWidth, mWindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	std::vector<glm::vec3> ssaoKernel;
	unsigned int noiseTexture;
	void InitSampleKernel()
	{
		//uniform_real_distribution �����ֲ��ڴ� С������ 0-1
		std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); 
		std::default_random_engine generator; //�����������
		

		for (unsigned int i = 0; i < 64; ++i)
		{
			glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));//  x -1,+1  y -1,+1  z 0,+1 ��������� ȡz������

			sample = glm::normalize(sample); //��λ���������� 

			sample *= randomFloats(generator);//����� ������һ���̶��Ͽ���Ϊ���Ȼ�

			float scale = float(i) / 64.0;
			//�� x*x ƽ����ֵ��samaple�������ӽӽ�ԭ��
			scale = MathHelp::lerp(0.1f, 1.0f, scale * scale);
			sample *= scale;
			ssaoKernel.push_back(sample);
		}

		//�����ת
		std::vector<glm::vec3> ssaoNoise;
		for (unsigned int i = 0; i < 16; i++)
		{
			glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // ��z����ת
			ssaoNoise.push_back(noise);
		}
		glGenTextures(1, &noiseTexture);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//ƽ����ͼ 4x4
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	
};