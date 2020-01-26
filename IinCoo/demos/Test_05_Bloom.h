#pragma once
#include "api.h"

class Test_05_Bloom:public Application
{
private:
	Model sponza;
	Shader geometryShader;
public:
	virtual void SenceInit() override
	{
		sponza = Model("../resources/models/Sponza/Sponza.gltf");
		geometryShader = Shader();
	}

	virtual void DrawLoop(Camera camera) override
	{

	}

	virtual void ImguiFunc() override
	{

	}
};