#pragma once

// simple logger
// 用法：
//	 LOG_TRACE<<"TRACE TEST.";
//	 LOG_INFO<<"INFO TEST.";
//	 LOG_WARN<<"WARN TEST.";
//	 LOG_ERROR<<"ERROR TEST.";

#include <iostream>
namespace Engine
{
	enum class LogLevel{ Trace,Info,Warn,Error };

	// Singleton
	class Log
	{
	public:
		inline static Log* GetInstance()
		{
			if(!logger) logger = new Log();
			return logger;
		}

		Log& Print(LogLevel const&,const char*,int,const char*);
		template<typename T> Log& operator<< (T const&);
	private:
		static Log* logger;
		Log();
	};

	template<typename T> Log& Log::operator<<(T const &t)
	{
		std::cout << t;
		return *this;
	}
}

#define LOG_TRACE	Engine::Log::GetInstance()->Print(Engine::LogLevel::Trace,__FILE__,__LINE__,__FUNCTION__)
#define LOG_INFO	Engine::Log::GetInstance()->Print(Engine::LogLevel::Info,__FILE__,__LINE__,__FUNCTION__)
#define LOG_WARN	Engine::Log::GetInstance()->Print(Engine::LogLevel::Warn,__FILE__,__LINE__,__FUNCTION__)
#define LOG_ERROR	Engine::Log::GetInstance()->Print(Engine::LogLevel::Error,__FILE__,__LINE__,__FUNCTION__)

