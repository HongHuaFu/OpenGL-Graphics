#pragma once

#include <vector>
#include <string>

namespace TextureFuncs
{
	unsigned int LoadTexture(const char* path);
	unsigned int LoadCubemap(std::vector<std::string> faces);
}

