#ifdef LINUX
#include "mpi++.h"
#endif

#ifdef WIN32
#include <mpi.h>
#endif

#include "Framework\CommErrorHandler.h"
#include "World.h"
#include <iostream>

using namespace std;
using namespace WorldTest;

int main(int argc, char *argv[])
{
	//MPI_Init(&argc, &argv);
	int providedThreadLevel;
	int mpiError;

	MPI_Init_thread(NULL, NULL, MPI_THREAD_SERIALIZED, &providedThreadLevel);

	MPI_Errhandler errorHandler;

	mpiError = MPI_Comm_create_errhandler( Framework::Communications::CommErrorHandler::ErrorHandler, &errorHandler );

	if (mpiError == MPI_SUCCESS)
	{		
		//MPI_Errhandler_set(MPI_COMM_WORLD, MPI_ERRORS_RETURN);	
		mpiError = MPI_Comm_set_errhandler( MPI_COMM_WORLD, errorHandler ); 

		if (mpiError == MPI_SUCCESS)
		{
			int rank;
			int size;

			MPI_Comm_rank(MPI_COMM_WORLD, &rank);
			MPI_Comm_size(MPI_COMM_WORLD, &size);	

			World world;
			world.Run(static_cast<unsigned>(rank), size);

			printf("This is before MPI Finalize\n");

			MPI_Errhandler_free( &errorHandler );
		}

		printf("This is after MPI Finalize\n");
	}

	MPI_Finalize();
}
 