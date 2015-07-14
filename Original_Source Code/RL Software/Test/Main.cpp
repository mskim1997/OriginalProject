/////////////////////////////////////////////////////////
//Main.cpp
//Defines the Main.
//
//Manuel Madera
//8/8/09
/////////////////////////////////////////////////////////
#ifdef LINUX
#include "mpi++.h"
#endif

#ifdef WIN32
#include <mpi.h>
#endif

#include "Framework\Debug.h"
#include "MLTestProject\World.h"
#include <map>
#include <string>

using namespace std;
using namespace MLTest;

int main(int argc, char *argv[])
{
	int providedThreadLevel;	
	
	MPI_Init_thread(NULL, NULL, MPI_THREAD_SERIALIZED, &providedThreadLevel);

	int rank;
	int size;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);  
  
#ifdef PrintMemoryLeak
	if (rank == 0)
	{
		hLogFile = CreateFile(L"C:\\Users\\Manuel\\Desktop\\Research\\TestProject\\Debug\\log0.txt", GENERIC_WRITE, 
			FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL, NULL);
	}
	else if (rank == 1)
	{
		hLogFile = CreateFile(L"C:\\Users\\Manuel\\Desktop\\Research\\TestProject\\Debug\\log1.txt", GENERIC_WRITE, 
			FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL, NULL);
	}
	else if (rank == 2)
	{
		hLogFile = CreateFile(L"C:\\Users\\Manuel\\Desktop\\Research\\TestProject\\Debug\\log2.txt", GENERIC_WRITE, 
			FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL, NULL);
	}

	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, hLogFile);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ERROR, hLogFile);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT, hLogFile);
#endif

	map<string, string> args;

	for (int i = 1; i < argc; i+=2)
	{
		args[argv[i]] = argv[i + 1];
	}

	MLTest::UserInput userInput;
	userInput.statsFile = args["-o"];
	userInput.agentFile = args["-i"];
	userInput.mazeFile = args["-m"];
	userInput.rank = static_cast<unsigned>(rank);
	userInput.size = size;
	userInput.numAgents = atoi(args["-a"].c_str());
	userInput.mazeXSize = atoi(args["-x"].c_str());
	userInput.mazeYSize = atoi(args["-y"].c_str());

	if (args["-s"] == "false")
	{
		userInput.allowSharing = false;
	}
	else
	{
		userInput.allowSharing = true;
	}

	World world;
	world.Run(userInput);
	//world.Run(static_cast<unsigned>(rank), size);
  
	MPI_Finalize();              

	return 0;
}