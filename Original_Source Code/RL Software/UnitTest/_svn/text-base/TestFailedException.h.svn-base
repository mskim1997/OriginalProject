/////////////////////////////////////////////////////////
//TestFailedException.h
//Defines the TestFailedException class, thrown when a test fails.
//
//Manuel Madera
//2/20/10
/////////////////////////////////////////////////////////

#ifndef TestFailedException_h
#define TestFailedException_h

#include "Framework/LogManager.h"
#include <exception>
#include <string>

using namespace std;

#define STR(x) #x
#define XSTR(x) STR(x)

#define MESSAGE(x) string("{Exception occured at: ") + string(__FILE__) + string(":") + string(XSTR(__LINE__)) + "} " + x

namespace Testing
{
	class TestFailedException : public exception
    {     
	private:
		Framework::Logging::ILog *log;

    public:
		TestFailedException(string message) : exception(message.c_str())
		{
			this->log = Framework::Logging::LogManager::Instance();			

			this->log->Log(&typeid(this), this->what());
		}

        virtual ~TestFailedException()
		{
		}

    };
}
    
#endif  //TestFailedException