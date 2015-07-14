/////////////////////////////////////////////////////////
//MainAgent.cpp
//Defines the Main Agent class.
//
//Manuel Madera
//7/10/10
/////////////////////////////////////////////////////////

#include "MainAgent.h"
#include <iostream>
#include <fstream>
#include "Framework\Packages.h"
#include "Framework\EventDistributor.h"
#include "Framework\PackagerFactory.h"
#include "Framework\MLAgentEvents.h"
#include "Framework\Debug.h"
#include "Framework\Communicator.h"

#ifdef PrintMemoryLeak
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif
#endif

#ifdef WIN32

#define sleep(x)	Sleep(x*1000)
#endif

using namespace std;
using namespace Framework;
using namespace Framework::Eventing;
using namespace Framework::Agents;
using namespace Framework::Packages;
using namespace Framework::Factories;
using namespace Framework::Logging;
using namespace Framework::Communications;

namespace WorldTest
{
	MainAgent::MainAgent(unsigned packageNode, 
		unsigned packageDest, 
		unsigned numNodes,
		unsigned numAgentsPerNode,
		Framework::Messaging::IMessageDistributor *messageDistributor,
		Framework::Eventing::IEventDistributor *eventDistributor)
		: Agent(packageNode, packageDest), msgDist(messageDistributor), eventDist(eventDistributor), nodes(numNodes), agentsPerNode(numAgentsPerNode), totalNumOfAgents(0), numAgentsDone(0)
	{
		PackagerFactory *packageFactory = PackagerFactory::Instance();

		if (packageFactory != NULL)
		{
			packageFactory->RegisterPackage(PackageType::AGENT_DONE_EVENT, AgentDoneEventPackage::CreatePackage);
		}

		if (this->eventDist != NULL)
		{
			this->eventDist->RegisterRemoteSubscription(PackageType::AGENT_DONE_EVENT, SubscriptionTypes::AGENT_DONE);

			eventDist->Subscribe(this, SubscriptionTypes::AGENT_DONE, NotificationType::LONGLIVED, true);
		}

		this->totalNumOfAgents = nodes * agentsPerNode;

		this->log = LogManager::Instance();
	}

	MainAgent::~MainAgent()
	{
	}

	void MainAgent::Run()
	{
	}

	void MainAgent::Notify(const Eventing::ISubscription *subscription)
	{
		ISubscription *tempSubscription = const_cast<ISubscription *>(subscription);

		if (tempSubscription != NULL)
		{
			if (tempSubscription->GetSubscriptionType() == SubscriptionTypes::AGENT_DONE)
			{
				const AgentDoneEventPackage *agentDoneSubscription = static_cast<const AgentDoneEventPackage *>(subscription);

				if (agentDoneSubscription != NULL)
				{
					this->numAgentsDone++;

					if (!Validator::IsNull(log, NAME("log")))
					{
						log->Log(&typeid(this), "Agent: %d is done.", agentDoneSubscription->GetAgentId());
					}

					printf("Agent: %d is done.\n", agentDoneSubscription->GetAgentId());

					if (this->numAgentsDone == this->totalNumOfAgents)
					{
						for (unsigned i = 1; i <= nodes; i++)
						{
							if (this->msgDist != NULL)
							{
								this->msgDist->SendPackage(new TerminatePackage(0, i));
							}
						}
					}
				}
			}
		}
	}
}