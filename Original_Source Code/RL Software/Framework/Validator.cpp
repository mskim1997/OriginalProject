/////////////////////////////////////////////////////////
//Validator.cpp
//Defines the Validator class used to validate pointers
//and data types.
//
//Manuel Madera
//4/10/2010
/////////////////////////////////////////////////////////

#include "Validator.h"
#include "LogManager.h"

using namespace Framework::Logging;

namespace Framework
{
	Validator::Validator()
	{
	}

	Validator::~Validator()
	{
	}

	/*static*/ bool Validator::IsNull(void *ptr, string message)
	{
		bool isNull = false;

		if (ptr == NULL)
		{
			isNull = true;

			LogManager::Instance()->Log(&typeid(Validator), message);
		}

		return isNull;
	}

	/*static*/ bool Validator::IsNull(const void *ptr, string message)
	{
		bool isNull = false;

		if (ptr == NULL)
		{
			isNull = true;

			LogManager::Instance()->Log(&typeid(Validator), message);
		}

		return isNull;
	}
}