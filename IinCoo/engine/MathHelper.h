#pragma once

namespace MathHelp
{
	float lerp(float a,float b,float f)
	{
		return a + f * (b - a);
	}
}