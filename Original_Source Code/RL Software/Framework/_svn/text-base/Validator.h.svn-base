/////////////////////////////////////////////////////////
//Validator.h
//Defines the Validator class used to validate pointers
//and data types.
//
//Manuel Madera
//4/10/2010
/////////////////////////////////////////////////////////

#ifndef Validator_h
#define Validator_h

#include "Framework.h"
#include <string>
#include "Debug.h"

using namespace std;

#ifdef NullPointerVerbose
#define STR(x) #x
#define XSTR(x) STR(x)

#define NAME(x) string("{Null pointer: ") + x + " at: " + string(__FILE__) + string(":") + string(XSTR(__LINE__)) + "} "
#else
#define NAME(x) x
#endif

namespace Framework
{
	class FRAMEWORK_API Validator
	{
	public:
		Validator();
		virtual ~Validator();

		static bool IsNull(void *ptr, string message);
		static bool IsNull(const void *ptr, string message);
	};
}

#endif  //Validator_h