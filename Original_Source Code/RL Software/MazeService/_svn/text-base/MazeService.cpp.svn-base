// MazeService.cpp : main project file.
#include "stdafx.h"

//#ifdef _DEBUG
//#define _CRTDBG_MAP_ALLOC
//#include <crtdbg.h>
//#define new new(_CLIENT_BLOCK, __FILE__, __LINE__)
//#endif

//#ifndef	_CRTDBG_MAP_ALLOC
//#define _CRTDBG_MAP_ALLOC
//#endif
//#include <stdlib.h>
//#include <crtdbg.h>

#pragma unmanaged
#include <mpi.h>
//#include "MLTestProject\World.h"
#include "MLContinuous\World.h"
#include "Framework\Debug.h"
#include "Framework\CommErrorHandler.h"
#pragma managed
#include "MazeService.h"

using namespace System::Threading;

static void MainThread(System::Object^ arg)
{
	AgentService::Instance->Run((Dictionary<System::String^, System::String^>^)arg);
	//service->Run();
}

int main(array<System::String ^> ^args)
{
#ifdef PrintMemoryLeak
	HANDLE hLogFile;
#endif

	//MPI_Init(NULL, NULL);
	int providedThreadLevel;
	
	MPI_Init_thread(NULL, NULL, MPI_THREAD_SERIALIZED, &providedThreadLevel);
	//MPI_Errhandler errorHandler;

	//MPI_Comm_create_errhandler( Framework::Communications::CommErrorHandler::ErrorHandler, &errorHandler );
 //   MPI_Comm_set_errhandler( MPI_COMM_WORLD, errorHandler );   

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

	Dictionary<System::String^, System::String^>^ argList = gcnew Dictionary<System::String^, System::String^>();

	for (int i = 0; i < args->Length; i += 2)
	{
		argList->Add(args[i], args[i + 1]);
	}

	bool hostService = System::Convert::ToBoolean(argList["-h"]);

	if (rank == 0)
	{
		ServiceHost^ host = nullptr;
		AgentService^ service = AgentService::Instance;

		if (hostService)
		{
			host = gcnew ServiceHost(service->GetType());
			host->Open();
			Console::WriteLine("service is up...");
			Console::ReadLine();
		}

		service->rank = rank;
		service->size = size;

		System::Threading::ThreadPool::QueueUserWorkItem(gcnew System::Threading::WaitCallback(MainThread), argList);

		//service->Run();
		
		if (hostService)
		{
			Console::WriteLine("Hit Enter to exit ...");		
			Console::ReadLine();

			service->Stop();

			host->Close();
		}
		else
		{
			service->Wait();
		}		
	}
	else
	{
		Console::WriteLine("Running Nodes ...");

		msclr::interop::marshal_context context;    
		std::string statsFile = context.marshal_as<std::string>(argList["-o"]);
		std::string agentFile = context.marshal_as<std::string>(argList["-i"]);
		std::string mazeFile = context.marshal_as<std::string>(argList["-m"]);

		MLContinuous::UserInput userInput;
		userInput.rank = static_cast<unsigned>(rank);
		userInput.size = size;
		userInput.statsFile = statsFile;
		userInput.agentFile = agentFile;
		userInput.mazeFile = mazeFile;
		userInput.numAgents = System::Convert::ToInt32(argList["-a"]);
		userInput.mazeXSize = System::Convert::ToInt32(argList["-x"]);
		userInput.mazeYSize = System::Convert::ToInt32(argList["-y"]);
		userInput.allowSharing = System::Convert::ToBoolean(argList["-s"]);
		userInput.slidingWindowLookAhead = System::Convert::ToDouble(argList["-wi"]);
		userInput.minRadius = System::Convert::ToDouble(argList["-r"]);
		userInput.minTurningAngle = System::Convert::ToDouble(argList["-t"]);
		userInput.immediateMoves = System::Convert::ToInt32(argList["-im"]);
		userInput.runNumber = System::Convert::ToInt32(argList["-run"]);

		MLContinuous::World world;
		world.Run(userInput);

		if (hostService)
		{
			Console::WriteLine("Hit Enter to exit ...");		
			Console::ReadLine();
		}
	}

	//MPI_Barrier(MPI_COMM_WORLD);

	//MPI_Errhandler_free( &errorHandler );

	MPI_Finalize();

#ifdef PrintMemoryLeak
	CHECK_HEAP();
	_CrtDumpMemoryLeaks();
	CloseHandle(hLogFile);	
#endif

    return 0;
}
