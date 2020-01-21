#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../third-party/stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma);

class Model
{
public:
	std::vector<Texture> texture_loaded;//存储已加载的纹理
	std::vector<Mesh> meshes;
	std::string directory;
	bool gammaCorrect;

	Model(std::string const &path,bool gamma = false) : gammaCorrect(gamma)
	{
		LoadModel(path);
	}


	void Draw(Shader shader)
	{
		for(unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].DrawMesh(shader);
	}

private:
	void LoadModel(std::string const &path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
	
};

