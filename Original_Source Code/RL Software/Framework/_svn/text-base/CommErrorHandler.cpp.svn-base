/////////////////////////////////////////////////////////
//CommErrorHandler.cpp
//Defines an MPI Comm Error handler.
//
//Manuel Madera
//3/23/2010
/////////////////////////////////////////////////////////

#include "CommErrorHandler.h"
#include "LogManager.h"

namespace Framework
{
	namespace Communications
	{
		CommErrorHandler::CommErrorHandler()
		{
		}

		CommErrorHandler::~CommErrorHandler()
		{
		}

		/*static*/ void CommErrorHandler::ErrorHandler(MPI_Comm *comm, int *err, ...)
		{
			char error_string[BUFSIZ];
			int length_of_error_string;

			MPI_Error_string(*err, error_string, &length_of_error_string);

			printf("MPI Error %s", error_string);
			Logging::ILog *log = Logging::LogManager::Instance();

			if (!Validator::IsNull(log, NAME("log")))
			{
				log->Log(&typeid(CommErrorHandler), "MPI error %s", error_string);
			}
		}
	}
}