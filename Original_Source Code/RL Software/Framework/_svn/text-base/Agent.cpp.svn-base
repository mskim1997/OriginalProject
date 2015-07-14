/////////////////////////////////////////////////////////
//Agent.cpp
//Defines the Agent class.
//
//Manuel Madera
//7/5/09
/////////////////////////////////////////////////////////

#include "Agent.h"
#include "Communicator.h"
#include "MessageDistributor.h"
#include "Debug.h"

#ifdef PrintMemoryLeak
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif
#endif

using namespace Framework::Agents;
using namespace Framework::Communications;
using namespace Framework::Packages;
using namespace Framework::Messaging;

namespace Framework
{
	namespace Agents
	{
		Agent::Agent() : isRegistered(false), isTransfered(false)
		{
			Init();
		}

		Agent::Agent(unsigned packageNode, unsigned packageDest)
			: isRegistered(false), isTransfered(false)
		{ 
			Init();
		}

		void Agent::Init()
		{
			msgDist = MessageDistributor::Instance();
		}

		Agent::~Agent() 
		{
			if (this->isRegistered)
			{
				if (!Validator::IsNull(msgDist, NAME("msgDist")))
				{
					msgDist->Unregister(this);
				}
			}
		}

		void Agent::Run(bool isTransfer) 
		{		
		}

		void Agent::SendPackage(IPackage *package)
		{
			if (this->isRegistered)
			{
				if (!Validator::IsNull(msgDist, NAME("msgDist")))
				{
					msgDist->SendPackage(package);
				}
			}
		}

		//Base methods
		bool Agent::IsRegistered() 
		{
			return isRegistered; 
		}

		bool Agent::IsTransfered()
		{
			return isTransfered;
		}

		void Agent::Register()
		{
			if (!Validator::IsNull(msgDist, NAME("msgDist")))
			{
				msgDist->Register(this, &this->myMessageQueue);
			}
		}

		void Agent::RegisterTransfer(unsigned id)
		{
			if (!this->isRegistered)
			{
				this->id = id;
				this->isRegistered = true;
			}
		}

		void Agent::SetAgentUI(unsigned id) 
		{ 
			if (!this->isRegistered)
			{
				this->id = id;
				this->isRegistered = true;
			}
		}

		unsigned Agent::GetAgentUI() 
		{ 
			return this->id; 
		}
	}
}