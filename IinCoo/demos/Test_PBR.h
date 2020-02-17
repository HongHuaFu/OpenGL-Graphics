#pragma once
#include "api.h"


class Test_PBR : public Application
{
public:
	Shader pbrShader;
	Shader equirectangularToCubemapShader;
	Shader irradianceShader;
	Shader prefilterShader;
	Shader brdfShader;
	Shader backgroundShader;

	Model DM;

	unsigned int captureFBO= 0;
	unsigned int captureRBO= 0;

	unsigned int hdrTexture= 0;
	unsigned int envCubemap= 0;
	unsigned int irradianceMap= 0;
	unsigned int prefilterMap= 0;
	unsigned int brdfLUTTexture= 0;

	unsigned int albedo = 0;
	unsigned int normal = 0;
	unsigned int metal = 0;
	unsigned int roughness = 0;
	unsigned int ao = 0;

	
public:
	Test_PBR():Application(){ }

	virtual void SenceInit() override
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		//消除立方体采样时接缝过渡割裂问题
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		

		pbrShader = Shader("../resources/shaders/pbr/ibl/pbr.vs", "../resources/shaders/pbr/ibl/pbr.fs");
		equirectangularToCubemapShader = Shader("../resources/shaders/pbr/ibl/cubemap.vs", "../resources/shaders/pbr/ibl/equirectangular_to_cubemap.fs");
		irradianceShader = Shader("../resources/shaders/pbr/ibl/cubemap.vs", "../resources/shaders/pbr/ibl/irradiance_convolution.fs");
		prefilterShader = Shader("../resources/shaders/pbr/ibl/cubemap.vs", "../resources/shaders/pbr/ibl/prefilter.fs");
		brdfShader = Shader("../resources/shaders/pbr/ibl/brdf.vs", "../resources/shaders/pbr/ibl/brdf.fs");
		backgroundShader = Shader("../resources/shaders/pbr/ibl/background.vs", "../resources/shaders/pbr/ibl/background.fs");

		pbrShader.Use();
		pbrShader.SetInt("irradianceMap", 0);
		pbrShader.SetInt("prefilterMap", 1);
		pbrShader.SetInt("brdfLUT", 2);
		pbrShader.SetInt("albedoMap", 3);
		pbrShader.SetInt("normalMap", 4);
		pbrShader.SetInt("metallicMap", 5);
		pbrShader.SetInt("roughnessMap", 6);
		pbrShader.SetInt("aoMap", 7);

		backgroundShader.Use();
		backgroundShader.SetInt("environmentMap", 0);

		DM = Model("../resources/models/DamagedHelmet/DM.obj");
		TextureFuncs::LoadTexture("../resources/models/DamagedHelmet/albedo.jpg",&albedo);
		TextureFuncs::LoadTexture("../resources/models/DamagedHelmet/normal.jpg",&normal);
		TextureFuncs::LoadTexture("../resources/models/DamagedHelmet/AO.jpg",&ao);
		TextureFuncs::LoadTexture("../resources/models/DamagedHelmet/metalRoughness.jpg",&metal);
		TextureFuncs::LoadTexture("../resources/models/DamagedHelmet/emissive.jpg",&roughness);
		
		//1.生成预处理的FBO
		glGenFramebuffers(1,&captureFBO);
		glGenRenderbuffers(1,&captureRBO);
		glBindFramebuffer(GL_FRAMEBUFFER,captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER,captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);


		//2. 加载环境hdr贴图
		stbi_set_flip_vertically_on_load(true);
		int width, height, nrComponents;
		float *data = stbi_loadf("../resources/skyboxs/monoLake.hdr", &width, &height, &nrComponents, 0);
		
		if (data)
		{
			glGenTextures(1, &hdrTexture);
			glBindTexture(GL_TEXTURE_2D, hdrTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load HDR image." << std::endl;
		}


		//3.生成立方体纹理
		glGenTextures(1, &envCubemap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//4.设置投影矩阵
		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViews[] =
		{
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};

	
		equirectangularToCubemapShader.Use();
		equirectangularToCubemapShader.SetInt("equirectangularMap", 0);
		equirectangularToCubemapShader.SetMat4("projection", captureProjection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);


		//对cubemap渲染六次
		glViewport(0, 0, 512, 512);
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		for (unsigned int i = 0; i < 6; ++i)
		{
			equirectangularToCubemapShader.SetMat4("view", captureViews[i]);
			//每次渲染正方体一个面
			//然后绑定到Cubemap中
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Geometry::renderCube();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//生成mipmap
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);




		//生成预计算辐照图
		glGenTextures(1, &irradianceMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);



		//开始计算irradiance
		irradianceShader.Use();
		irradianceShader.SetInt("environmentMap", 0);
		irradianceShader.SetMat4("projection", captureProjection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

		glViewport(0, 0, 32, 32);
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		for (unsigned int i = 0; i < 6; ++i)
		{
			irradianceShader.SetMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Geometry::renderCube();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//预滤波环境贴图
		glGenTextures(1, &prefilterMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		//采样mipmap，启用三线性过滤
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//生成预滤波的mipmap
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		prefilterShader.Use();
		prefilterShader.SetInt("environmentMap", 0);
		prefilterShader.SetMat4("projection", captureProjection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		unsigned int maxMipLevels = 5;

		//5层mipmap分别生成
		for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
		{
			unsigned int mipWidth = 128 * std::pow(0.5, mip);
			unsigned int mipHeight = 128 * std::pow(0.5, mip);
			glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
			glViewport(0, 0, mipWidth, mipHeight);

			//5个粗糙度级别
			float roughness = (float)mip / (float)(maxMipLevels - 1);

			prefilterShader.SetFloat("roughness", roughness);
			for (unsigned int i = 0; i < 6; ++i)
			{
				prefilterShader.SetMat4("view", captureViews[i]);
				//绑定不同的mipmap
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				Geometry::renderCube();
			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		//计算brdf卷积贴图
		glGenTextures(1, &brdfLUTTexture);
		glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

		glViewport(0, 0, 512, 512);
		brdfShader.Use();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Geometry::renderQuad();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}


	virtual void DrawLoop(Camera camera) override
	{
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)mWindowWidth / (float)mWindowHeight, 0.1f, 100.0f);
		pbrShader.Use();
		pbrShader.SetMat4("projection", projection);
		backgroundShader.Use();
		backgroundShader.SetMat4("projection", projection);

		// then before rendering, configure the viewport to the original framebuffer's screen dimensions
		int scrWidth, scrHeight;
		glfwGetFramebufferSize(mWindow, &scrWidth, &scrHeight);
		glViewport(0, 0, scrWidth, scrHeight);
		
		

		glm::vec3 lightPositions[] = {
			glm::vec3(-10.0f,  10.0f, 10.0f),
			glm::vec3( 10.0f,  10.0f, 10.0f),
			glm::vec3(-10.0f, -10.0f, 10.0f),
			glm::vec3( 10.0f, -10.0f, 10.0f),
		};
		glm::vec3 lightColors[] = {
			glm::vec3(300.0f, 300.0f, 300.0f),
			glm::vec3(300.0f, 300.0f, 300.0f),
			glm::vec3(300.0f, 300.0f, 300.0f),
			glm::vec3(300.0f, 300.0f, 300.0f)
		};
		int nrRows = 7;
		int nrColumns = 7;
		float spacing = 2.5;

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		pbrShader.Use();
		glm::mat4 model = glm::mat4(1.0f);
		
		glm::mat4 view = camera.GetViewMatrix();
		pbrShader.SetMat4("view", view);
		pbrShader.SetVec3("camPos", camera.Position);

		// bind pre-computed IBL data
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

		// rusted iron
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, albedo);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, normal);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, metal);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, roughness);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, ao);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-5.0, 0.0, 2.0));
		model = glm::scale(model,glm::vec3(5.0f));
		/*model = glm::scale(model, glm::vec3(10.0));*/
		pbrShader.SetMat4("model", model);

		DM.Draw(pbrShader);

		// render skybox (render as last to prevent overdraw)
		backgroundShader.Use();

		backgroundShader.SetMat4("view", view);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap); // display irradiance map
		//glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap); // display prefilter map
		Geometry::renderCube();


	}

	/*unsigned int captureFBO;
	unsigned int captureRBO;

	unsigned int hdrTexture;
	unsigned int envCubemap;
	unsigned int irradianceMap;
	unsigned int prefilterMap;
	unsigned int brdfLUTTexture;*/

	virtual void ImguiFunc() override{
		
	}
};