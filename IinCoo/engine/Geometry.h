#pragma once
#include <glm/glm.hpp>

namespace Geometry
{
	// 0- 1
	void renderQuad(const glm::vec2& leftDownPoint = glm::vec2(-1.0f,-1.0f),const glm::vec2& rightUpPoint= glm::vec2(1.0f,1.0f));


	void renderCube();

	void renderSphere();
}