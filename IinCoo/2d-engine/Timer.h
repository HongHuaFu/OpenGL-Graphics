#pragma once


namespace Engine
{
	class Timer
	{
	public:
		Timer(float time = .0f): mTime(time){ }

		operator float() const { return mTime; }
		float GetSeconds() const { return mTime; }
		float GetMilliseconds() const { return mTime * 1000.0f; }

	private:
		float mTime;
	};
}
