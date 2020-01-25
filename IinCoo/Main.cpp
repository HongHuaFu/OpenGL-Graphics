#include "demos/Test.h"
#define Test_03

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

}

