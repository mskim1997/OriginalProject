/////////////////////////////////////////////////////////
//ConsoleLogger.h
//Defines the ConsoleLogger class.  This object is responsible
//for printing log messages to the console.
//
//Manuel Madera
//1/25/10
/////////////////////////////////////////////////////////

#ifndef ConsoleLogger_h
#define ConsoleLogger_h

#include "Framework.h"
#include "ILogger.h"
#include <string>

using namespace std;

namespace Framework
{
	namespace Logging
	{
		class FRAMEWORK_API ConsoleLogger : public ILogger
		{
		public:
			ConsoleLogger();
			virtual ~ConsoleLogger();

			virtual void Log(string message);
		};
	}
}

#endif  //ConsoleLogger_h