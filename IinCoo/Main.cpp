#define demos_test
#ifdef demos_test
#include "demos/Test.h"
#define Test_08
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

#ifdef Test_07
	Test_07_3DClickPick demo = Test_07_3DClickPick();
	demo.Run();
#endif // Test_07

#ifdef Test_08
	Test_PBR pbr = Test_PBR();
	pbr.Run();
#endif // Test_08



}

#endif

#ifdef _2dgameenignetest
#include "Test_Enigne2d.h"


int main()
{
	Test_Logger();
	system("pause");
}
#endif // _2dgameenignetest
