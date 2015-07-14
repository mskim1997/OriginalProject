#pragma once

#pragma unmanaged
#include "UnmanagedSubscriber.h"
#include "MLTestProject\World.h"

#pragma managed

#include <msclr\marshal_cppstd.h>

using namespace System;
using namespace System::ServiceModel;
using namespace System::Runtime::Serialization;
using namespace System::Collections::Generic;
using namespace System::ComponentModel;
using namespace System::Threading;

[DataContract]
ref class AgentPosition : EventArgs
{
public:
	[DataMember]
    property char Object;

	[DataMember]
    property int XPos;

	[DataMember]
    property int YPos;
};

interface class IAgentServiceCallback
{
	[OperationContract]
	void Inform(AgentPosition^ pos);
};

[ServiceContract(CallbackContract=IAgentServiceCallback::typeid)]
interface class IAgentService
{
	[OperationContract]
	void RegisterClient();
};

[ServiceBehavior(InstanceContextMode=InstanceContextMode::PerCall)]
ref class AgentService : IAgentService
{
private:
	static IList<IAgentServiceCallback^>^ callback;
	UnmanagedSubscriber* subscriber;
	AutoResetEvent^ doneEvent;

private:
	AgentService()
	{
		callback = gcnew List<IAgentServiceCallback^>();
		subscriber = new UnmanagedSubscriber();

		IsStopped = false;
		doneEvent = gcnew AutoResetEvent(false);
	}

public:
	static AgentService()
	{
		if (AgentService::Instance == nullptr)
		{
			Instance = gcnew AgentService();
		}
	}

	~AgentService()
	{
		delete subscriber;
	}

	virtual void RegisterClient()
	{
		IAgentServiceCallback^ clientCallback = OperationContext::Current->GetCallbackChannel<IAgentServiceCallback^>();

		if (!AgentService::callback->Contains(clientCallback))
		{
			AgentService::callback->Add(clientCallback);
		}
	}

	static property AgentService^ Instance;
	property bool IsStopped;
	property int rank;
	property int size;

	void Wait()
	{
		doneEvent->WaitOne();
	}

	void Set()
	{
		doneEvent->Set();
	}

	void Stop()
	{
		IsStopped = true;
		subscriber->Terminate();
	}

	void Run(Dictionary<System::String^, System::String^>^ argList)
	{
		BackgroundWorker^ bckg = gcnew BackgroundWorker();

		bool hostService = System::Convert::ToBoolean(argList["-h"]);

		bckg->DoWork += gcnew DoWorkEventHandler(this, &AgentService::Work);

		bckg->RunWorkerAsync(argList);		

		if (hostService)
		{
			subscriber->Run();

			while (!IsStopped)
			{
				Inform(subscriber->GetPackage());
			}
		}

		//AgentPosition^ pos;
		//int x = 0;
		//int y = 0;

		//for (int i = 0; i < 50; i++)
		//{
		//	pos = gcnew AgentPosition();
		//	pos->Object = '1';
		//	pos->XPos = x;
		//	pos->YPos = y;

		//	Dispatch(pos);

		//	x++;

		//	if (x > 7)
		//	{
		//		x = 0;
		//		y++;

		//		if (y > 7)
		//		{
		//			y = 0;
		//		}
		//	}
		//}
	}

private:
	void Work(Object^ sender, DoWorkEventArgs^ e)
	{
		Dictionary<System::String^, System::String^>^ argList = (Dictionary<System::String^, System::String^>^)e->Argument;
		
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

		AgentService::Instance->Set();
	}

	void Inform(Framework::Packages::MazePositionEventPackage *positionPackage)
	{
		AgentPosition^ pos = gcnew AgentPosition();

		if (positionPackage != NULL)
		{
			Framework::Packages::MazePositionPackage *posPackage = positionPackage->GetPositionPackage();

			if (posPackage != NULL)
			{
				pos->Object = posPackage->GetObject();
				pos->XPos = posPackage->GetPosition().GetXCoor();
				pos->YPos = posPackage->GetPosition().GetYCoor();

				Dispatch(pos);				
			}

			delete posPackage;
			delete positionPackage;
		}
	}

	void Dispatch(AgentPosition^ pos)
	{
		for (int i = 0; i < AgentService::callback->Count; i++)
		{
			AgentService::callback[i]->Inform(pos);
		}
	}
};