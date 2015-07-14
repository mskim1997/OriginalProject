/////////////////////////////////////////////////////////
//FileLogger.h
//Defines the FileLogger class.  This object is responsible
//for storing log messages into a file.
//
//Manuel Madera
//1/25/10
/////////////////////////////////////////////////////////

#ifndef FileLogger_h
#define FileLogger_h

#include "Framework.h"
#include "ILogger.h"
#include <string>
#include <fstream>

using namespace std;

namespace Framework
{
	namespace Logging
	{
		class FRAMEWORK_API FileLogger : public ILogger
		{
		private:
			ofstream outStream;

		public:
			FileLogger(string fileName);
			virtual ~FileLogger();

			virtual void Log(string message);
		};
	}
}

#endif  //FileLogger_h