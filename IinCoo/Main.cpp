#include "demos/Test.h"
#define Test_06

int main()
{
#ifdef Test_01
	Test_01_SkyBox demo = Test_01_SkyBox();
	demo.Run();
#endif // Test_01

#ifdef Test_02
	Test_02_ShadowMap demo = Test_02_ShadowMap();
	demo.Run();
#endif // Test_02

#ifdef Test_03
	Test_03_DefferedShading demo = Test_03_DefferedShading();
	demo.Run();
#endif // Test_03

#ifdef Test_04
	Test_04_SSAO demo = Test_04_SSAO();
	demo.Run();
#endif // Test_04

#ifdef Test_05
	Test_05_Bloom demo = Test_05_Bloom();
	demo.Run();
#endif // Test_05

#ifdef Test_06
	Test_06_NormalMap demo = Test_06_NormalMap();
	demo.Run();
#endif // Test_06
}

