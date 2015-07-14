// WCFTest.cpp : main project file.

#include "stdafx.h"

using namespace System;
using namespace System::ServiceModel;
using namespace System::Runtime::Serialization;
using namespace System::Collections::Generic;
using namespace System::ComponentModel;

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
	AgentPosition^ GetPosition();

	[OperationContract]
	void RegisterClient();
};

[ServiceBehavior(InstanceContextMode=InstanceContextMode::PerCall)]
ref class AgentService : IAgentService
{
private:
	static IList<IAgentServiceCallback^>^ callback;
public:
	AgentService()
	{
		//mutex = gcnew object();
		callback = gcnew List<IAgentServiceCallback^>();
	}

	virtual void RegisterClient()
	{
		IAgentServiceCallback^ clientCallback = OperationContext::Current->GetCallbackChannel<IAgentServiceCallback^>();

		if (!AgentService::callback->Contains(clientCallback))
		{
			AgentService::callback->Add(clientCallback);
			//this->callback->Add(clientCallback);
		}
		//BackgroundWorker^ bckg = gcnew BackgroundWorker();

		//bckg->DoWork += gcnew DoWorkEventHandler(this, &AgentService::Work);

		//bckg->RunWorkerAsync();
	}

	//Not needed, delete
	virtual AgentPosition^ GetPosition()
	{
		return gcnew AgentPosition();
	}

	void Run()
	{
		AgentPosition^ pos;
		int x = 0;
		int y = 0;

		for (int i = 0; i < 50; i++)
		{
			pos = gcnew AgentPosition();
			pos->Object = '1';
			pos->XPos = x;
			pos->YPos = y;

			Dispatch(pos);

			x++;

			if (x > 7)
			{
				x = 0;
				y++;

				if (y > 7)
				{
					y = 0;
				}
			}
		}
	}
private:
	void Work(Object^ sender, DoWorkEventArgs^ e)
	{
		this->callback->Add(OperationContext::Current->GetCallbackChannel<IAgentServiceCallback^>());
	}

	void Dispatch(AgentPosition^ pos)
	{
		for (int i = 0; i < AgentService::callback->Count/*this->callback->Count*/; i++)
		{
			AgentService::callback[i]->Inform(pos);
			//this->callback[i]->Inform(pos);
		}
	}
};

int main(array<System::String ^> ^args)
{
	AgentService^ service = gcnew AgentService();
	ServiceHost^ host = gcnew ServiceHost(service->GetType()/*AgentService::typeid*/);
    //host->AddServiceEndpoint(IAgentService::typeid, gcnew NetTcpBinding(), L"net.tcp://localhost:5000/AgentService");
    host->Open();
    Console::WriteLine("service is up...");
    Console::ReadLine();
	service->Run();
	Console::ReadLine();
    host->Close();
    return 0;
}
