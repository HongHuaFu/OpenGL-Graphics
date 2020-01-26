#include "Geometry.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>


unsigned int geo__CustomQuadVAO = 0;
unsigned int geo__CustomQuadVBO;
void Geometry::renderQuad(const glm::vec2& leftDownPoint,const glm::vec2& rightUpPoint)
{
	if (geo__CustomQuadVAO == 0)
	{
		
		float quadVertices[] = {
			// positions        // texture Coords
			leftDownPoint.x,  rightUpPoint.y, 0.0f, 0.0f, 1.0f,
			leftDownPoint.x, leftDownPoint.y, 0.0f, 0.0f, 0.0f,
			rightUpPoint.x,  rightUpPoint.y, 0.0f, 1.0f, 1.0f,
			rightUpPoint.x, leftDownPoint.y, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &geo__CustomQuadVAO);
		glGenBuffers(1, &geo__CustomQuadVBO);
		glBindVertexArray(geo__CustomQuadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, geo__CustomQuadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(geo__CustomQuadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}