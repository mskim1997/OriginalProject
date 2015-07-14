/////////////////////////////////////////////////////////
//RootNode.h
//Defines the specific Root Node implementation.
//
//Manuel Madera
//6/15/09
/////////////////////////////////////////////////////////

#ifndef RootNode_h
#define RootNode_h

#include "INode.h"
#include "ICommMgr.h"
#include <pthread.h>
#include "MainMessageDistributor.h"
#include "IEventDistributor.h"
#include <semaphore.h>
#include "Framework.h"
#include "LogManager.h"

namespace Framework
{
    class FRAMEWORK_API RootNode : public INode
    {
    private:
        unsigned myRank;
        int totalNumNodes;
        //ICommMgr *pMyCommMgr;
		Messaging::MainMessageDistributor *mainDistributor;
		Eventing::IEventDistributor *eventDist;
        pthread_t mainThread;
        bool terminate;
		bool isAgentAdded;
        sem_t binarySem;
		Framework::Agents::IAgent *agent;
		Logging::ILog *log;
        
    public:
        RootNode(unsigned rank, unsigned totalNumNodes);
        virtual ~RootNode();
            
        void Run();
        void Wait();
        void Terminate();
		void AddAgent(Framework::Agents::IAgent *newAgent);
        Messaging::IMessageDistributor *GetMessageDistributor();
		Eventing::IEventDistributor *GetEventDistributor();

    private:
        static void *MainThread(void *arg);
        bool IsTerminated();
    };

}

#endif  //RootNode_h