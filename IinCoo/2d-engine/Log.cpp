#include "Log.h"
#include <Windows.h>


namespace Engine
{
	Log* Log::logger;
	Log::Log(){ std::cout<<"[trace]: logger init."; }

	Log& Log::Print(LogLevel const &type,const char* fileName,int lines,const char* funcName)
	{
		switch(type)
		{
		case LogLevel::Trace: //white
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
				FOREGROUND_INTENSITY | FOREGROUND_RED |FOREGROUND_GREEN | FOREGROUND_BLUE);
			std::cout<<std::endl<<"[trace]: ";
			break;
		case LogLevel::Info: //green
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
				FOREGROUND_INTENSITY | FOREGROUND_GREEN);
			std::cout<<std::endl<<"[info]: ";
			break;
		case LogLevel::Warn: //yellow
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
				FOREGROUND_INTENSITY | FOREGROUND_RED |FOREGROUND_GREEN);
			std::cout<<std::endl<<"[warn]: position in "<<fileName<<"::"<<funcName<<"::line "<<lines<<" [end]"<<std::endl;
			std::cout<<"  <detail>: ";

			break;
		case LogLevel::Error://red
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
				FOREGROUND_INTENSITY | FOREGROUND_RED);
			std::cout<<std::endl<<"[error]: position in "<<fileName<<"::"<<funcName<<"::line "<<lines<<" [end]"<<std::endl;
			std::cout<<"  <detail>: ";
			break;
		}

		return *this;
	}

}