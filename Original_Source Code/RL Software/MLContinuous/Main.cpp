/////////////////////////////////////////////////////////
//Main.cpp
//Defines the Main.
//
//Manuel Madera
//8/8/09
/////////////////////////////////////////////////////////
//#ifdef LINUX
//#include "mpi++.h"
//#endif
//
//#ifdef WIN32
//#include <mpi.h>
//#endif
//
//#include "World.h"
//#include "MLAgent.h"
//
//using namespace MLTest;
//
//int main(int argc, char *argv[])
//{
//	MPI_Init(&argc, &argv);
//
//	int rank;
//	int size;
//
//	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//	MPI_Comm_size(MPI_COMM_WORLD, &size);  
//  
//	World world;
//	world.Run(static_cast<unsigned>(rank), size);
//  
//	MPI_Finalize();              
//
//	return 0;
//}