#pragma once
#include "../third-party/stb_image.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace TextureFuncs
{
	unsigned int LoadTexture(const char* path);
	unsigned int LoadCubemap(std::vector<std::string> faces);
	
}

