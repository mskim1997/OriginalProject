/////////////////////////////////////////////////////////
//CommErrorHandler.h
//Defines an MPI Comm Error handler.
//
//Manuel Madera
//3/23/2010
/////////////////////////////////////////////////////////

#ifndef CommErrorHandler_h
#define CommErrorHandler_h

#include "Packages.h"
#include "Framework.h"

#ifdef LINUX
#include "mpi++.h"
#endif

#ifdef WIN32
#include <mpi.h>
#endif

namespace Framework
{
	namespace Communications
	{
		class FRAMEWORK_API CommErrorHandler
		{
		public:
			CommErrorHandler();
			virtual ~CommErrorHandler();
	                                
			static void ErrorHandler(MPI_Comm *comm, int *err, ...);
		};
	}
}

#endif  //CommErrorHandler_h