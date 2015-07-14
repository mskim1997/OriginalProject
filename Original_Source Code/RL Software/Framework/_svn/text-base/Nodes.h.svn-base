/////////////////////////////////////////////////////////
//Nodes.h
//Defines the specific Node implementations.
//
//Manuel Madera
//5/5/09
/////////////////////////////////////////////////////////

#ifndef Node_h
#define Node_h

#include "INode.h"
#include "ICommMgr.h"
#include <pthread.h>
#include "IMessageDistributor.h"
#include "IEventDistributor.h"
#include <semaphore.h>
#include "Framework.h"
#include <list>
#include "LogManager.h"

using namespace std;

namespace Framework
{
    class FRAMEWORK_API Node : public INode
    {
    protected:
        unsigned myRank;
        //ICommMgr *pMyCommMgr;
		Messaging::IMessageDistributor *msgDist;
		Eventing::IEventDistributor *eventDist;
        pthread_t mainThread;
        bool terminate;
        sem_t binarySem;
		list<Agents::IAgent *> agentList;
		bool shareInfo;
		Logging::ILog *log;
		//Agents::IAgent *agent;

    public:
		Node(unsigned rank, bool allowSharing);
		Node(unsigned rank, bool allowSharing, Agents::IAgent *newAgent);
        virtual ~Node();
            
        virtual void Run();
        virtual void Wait();
        virtual void Terminate();
        virtual void AddAgent(Agents::IAgent *newAgent);

		Messaging::IMessageDistributor *GetMessageDistributor();
		Eventing::IEventDistributor *GetEventDistributor();
    private:
        static void *MainThread(void *arg);		
        bool IsTerminated();
		void Init(unsigned rank, bool allowSharing);
	protected:

		static void *WorkerThread(void *arg);
    };

}

#endif  //Node_h