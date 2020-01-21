#pragma once

//Shader loader 
//author: @JoeyDeVries LearnOpenGL


#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	unsigned int ID;
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);

public:
	inline void Use(){ glUseProgram(ID);  }

#pragma region set value
public://set value

	inline void SetBool(const std::string &name, bool value) const{ glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); }

	inline void SetInt(const std::string &name, int value) const { glUniform1f(glGetUniformLocation(ID, name.c_str()), value);  }

	inline void SetFloat(const std::string &name, float value) const
	{ 
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
	}

	inline void SetVec2(const std::string &name, const glm::vec2 &value) const
	{ 
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
	}

	inline void SetVec2(const std::string &name, float x, float y) const
	{ 
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); 
	}

	
	inline void SetVec3(const std::string &name, const glm::vec3 &value) const
	{ 
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
	}

	inline void SetVec3(const std::string &name, float x, float y, float z) const
	{ 
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); 
	}

	inline void SetVec4(const std::string &name, const glm::vec4 &value) const
	{ 
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
	}

	inline void SetVec4(const std::string &name, float x, float y, float z, float w) 
	{ 
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); 
	}
	
	inline void SetMat2(const std::string &name, const glm::mat2 &mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	
	inline void SetMat3(const std::string &name, const glm::mat3 &mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	
	inline void SetMat4(const std::string &name, const glm::mat4 &mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

#pragma endregion

private:
	void checkCompileErrors(GLuint shader, std::string type);
	
};

