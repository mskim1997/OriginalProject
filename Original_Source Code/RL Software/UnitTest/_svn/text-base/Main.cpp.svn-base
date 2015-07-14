#ifdef LINUX
#include "mpi++.h"
#endif

#ifdef WIN32
#include <mpi.h>
#endif

#include "Framework\CommErrorHandler.h"
#include "UnitTest.h"
#include <iostream>

using namespace std;
using namespace Testing;

int main(int argc, char *argv[])
{
	//MPI_Init(&argc, &argv);
	int providedThreadLevel;
	MPI_Init_thread(&argc, &argv, MPI_THREAD_SERIALIZED, &providedThreadLevel);

	printf("The thread level is %d\n", providedThreadLevel);

	int rank;
	int size;
	int x;

	MPI_Errhandler errorHandler;

	MPI_Comm_create_errhandler( Framework::Communications::CommErrorHandler::ErrorHandler, &errorHandler );
    MPI_Comm_set_errhandler( MPI_COMM_WORLD, errorHandler ); 

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);  

	UnitTest unitTest;
	unitTest.Run(static_cast<unsigned>(rank), size);

	printf("Press Enter to exit ... \n");
	cin >> x;

	printf("This is before MPI Finalize\n");

	MPI_Errhandler_free( &errorHandler );

	MPI_Finalize();

	printf("This is after MPI Finalize\n");
}