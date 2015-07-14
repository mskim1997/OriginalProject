/////////////////////////////////////////////////////////
//MainAgent.h
//Defines the Main Agent class.
//
//Manuel Madera
//7/7/10
/////////////////////////////////////////////////////////

#ifndef MainAgent_h
#define MainAgent_h

#include "Framework/Agent.h"
#include "Framework/IMessageDistributor.h"
#include "Framework/IEventDistributor.h"
#include "IEventDistributor.h"
#include "Framework/Position.h"
#include <map>
#include <semaphore.h>
#include "Framework\LogManager.h"

namespace WorldTest
{
	class MainAgent : public Framework::Agents::Agent, public Framework::Eventing::ISubscriber
	{
	private:
		Framework::Messaging::IMessageDistributor *msgDist;
		Framework::Eventing::IEventDistributor *eventDist;

		Framework::Eventing::SubscriptionTypes::SubscriptionType type;
		Framework::Eventing::SubscriptionTypes::SubscriptionLife life;

		unsigned nodes;
		unsigned agentsPerNode;
		unsigned totalNumOfAgents;
		unsigned totalNumReadyNodes;
		unsigned numAgentsDone;

		Framework::Logging::ILog *log;

	public:
		MainAgent(unsigned packageNode, 
				  unsigned packageDest, 
				  unsigned numNodes,
				  unsigned numAgentsPerNode,
				  Framework::Messaging::IMessageDistributor *messageDistributor,
				  Framework::Eventing::IEventDistributor *eventDistributor);

		virtual ~MainAgent();

		virtual void Run();
		virtual void Notify(const Framework::Eventing::ISubscription *subscription);
	};
}

#endif	//MainAgent_h