/////////////////////////////////////////////////////////
//ILog.h
//Defines the ILog interface.
//
//Manuel Madera
//1/25/10
/////////////////////////////////////////////////////////

#ifndef ILog_h
#define ILog_h

#include "Framework.h"
#include <typeinfo>  //for 'typeid' to work
#include <string>

using namespace std;

namespace Framework
{
	namespace Logging
	{
		class FRAMEWORK_API ILog
		{
		public:
			virtual ~ILog() {}

			//virtual void Log(string componentName, string formattedMessage, ...) = 0;

			//virtual void Log(string componentName, char *format, ...) = 0;

			virtual void Log(const type_info *component, string formattedMessage, ...) = 0;

			virtual void Log(const type_info *component, char *format, ...) = 0;
		};
	}
}

#endif  //ILog_h