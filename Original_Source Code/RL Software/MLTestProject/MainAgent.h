/////////////////////////////////////////////////////////
//MainAgent.h
//Defines the Main Agent class.
//
//Manuel Madera
//8/8/09
/////////////////////////////////////////////////////////

#ifndef MainAgent_h
#define MainAgent_h

#include "Framework/Agent.h"
#include "Framework/IMessageDistributor.h"
#include "Framework/IEventDistributor.h"
#include "Framework/Maze.h"
#include "IEventDistributor.h"
#include "MLTestProject.h"
#include "ml.h"
#include "Framework/Position.h"
#include <map>
#include <semaphore.h>
#include "StatsFileIO.h"
#include "Framework\LogManager.h"

namespace MLTest
{
	class ML_API MainAgent : public Framework::Agents::Agent, public Framework::Eventing::ISubscriber
	{
	private:
		StatsFileIO *statsFile;

		list<string> elements;

		double agentStartTime;
		double agentEndTime;

		Framework::Messaging::IMessageDistributor *msgDist;
		Framework::Eventing::IEventDistributor *eventDist;

		Framework::Eventing::SubscriptionTypes::SubscriptionType type;
		Framework::Eventing::SubscriptionTypes::SubscriptionLife life;

		unsigned nodes;
		unsigned agentsPerNode;
		unsigned totalNumOfAgents;
		unsigned totalNumReadyNodes;
		unsigned numAgentsDone;

		string statsFileName;
		string agentFileName;

		Framework::Maze *maze;
		Framework::Logging::ILog *log;

		sem_t nodesReady;

	public:
		MainAgent(unsigned packageNode, 
				  unsigned packageDest, 
				  unsigned numNodes,
				  unsigned numAgentsPerNode,
				  unsigned xMazeSize, 
				  unsigned yMazeSize, 
				  char *mazeFile, 
				  const string &statsFile,
				  const string &agentFile,
				  Framework::Messaging::IMessageDistributor *messageDistributor,
				  Framework::Eventing::IEventDistributor *eventDistributor);

		virtual ~MainAgent();

		virtual void Run();
		virtual void Notify(const Framework::Eventing::ISubscription *subscription);
	private:
		void SetupNodes();
		void SetupAgents();

		static void MainAgentCallback(void *ownerObject, Framework::Packages::IPackage *package);
		static void GetObjectOnCallback(void *ownerObject, Framework::Packages::IPackage *package);
		static void NodeReadyCallback(void *ownerObject, Framework::Packages::IPackage *package);
	};
}

#endif	//MainAgent_h