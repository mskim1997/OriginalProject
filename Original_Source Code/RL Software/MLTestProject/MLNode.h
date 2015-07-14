/////////////////////////////////////////////////////////
//MLNode.h
//Defines the specific Node implementations.
//
//Manuel Madera
//8/18/09
/////////////////////////////////////////////////////////

#ifndef MLNode_h
#define MLNode_h

#include "Framework\Nodes.h"
#include "IEventDistributor.h"
#include "Framework\Maze.h"
#include "MLTestProject.h"
#include <list>
#include <pthread.h>
#include "INodeBrain.h"
#include "IMaze.h"
#include <map>
#include "MLAgent.h"

using namespace std;

namespace Framework
{
	class ML_API MLNode : public Framework::Node, public Eventing::ISubscriber
    {
    private:
		sem_t mazeSem;
		sem_t doneSem;
		sem_t binSem;

		Eventing::SubscriptionTypes::SubscriptionType type;
		Eventing::SubscriptionTypes::SubscriptionLife life;

		list<pthread_t *> agentList;
		map<unsigned, MLTest::MLAgent *> transferedAgents;

		MLTest::INodeBrain *nodeBrain;
		MLTest::IMaze *maze;

		bool shareInfo;

    public:
		MLNode(unsigned rank, bool allowSharing);
		MLNode(unsigned rank, bool allowSharing, Agents::IAgent *newAgent);
        virtual ~MLNode();

		virtual void Wait();
		virtual void Initialize();
		virtual void Run();
		virtual void Terminate();
		virtual void Notify(const Eventing::ISubscription *subscription);

	private:
		void Init();

		static void MazeAgentCallback(void *ownerObject, Framework::Packages::IPackage *package);
		static void NewMazeAgentCallback(void *ownerObject, Framework::Packages::IPackage *package);
		static void TerminateCallback(void *ownerObject, Framework::Packages::IPackage *package);

		static void *TransferAgentThread(void *arg);
		static void *NewAgentThread(void *arg);
    };

}

#endif  //MLNode_h