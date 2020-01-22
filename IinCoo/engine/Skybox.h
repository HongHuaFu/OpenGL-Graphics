#pragma once
#include <vector>
#include <string>
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Skybox
{
public:
	Skybox(const char* path);
	~Skybox();

	void Draw(const glm::mat4& v,const glm::mat4& p);
private:
	std::vector<std::string> texturesPath;
	Shader skyboxShader;
	unsigned int cubemapTexture;
	unsigned int skyboxVAO, skyboxVBO;
};

