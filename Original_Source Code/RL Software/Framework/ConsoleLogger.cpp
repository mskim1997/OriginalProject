/////////////////////////////////////////////////////////
//ConsoleLogger.cpp
//Defines the ConsoleLogger class.  This object is responsible
//for printing log messages to the console.
//
//Manuel Madera
//1/25/10
/////////////////////////////////////////////////////////

#include "ConsoleLogger.h"
#include <iostream>

using namespace std;

namespace Framework
{
	namespace Logging
	{
		ConsoleLogger::ConsoleLogger()
		{
		}

		ConsoleLogger::~ConsoleLogger()
		{
		}

		void ConsoleLogger::Log(string message)
		{
			cout << message << endl;
			//printf(message.c_str());
		}
	}
}