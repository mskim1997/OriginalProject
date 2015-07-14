#include "OneWayCommMgr.h"
#include "RootNode.h"
#include "IPackager.h"
#include "PackagerFactory.h"
#include <iostream>
#include "Debug.h"
#include "EventDistributor.h"
#include "MainEventDistributorRemoteAgent.h"
#include "MessageDistributor.h"
#include "Communicator.h"

#ifdef WIN32
#include <windows.h>
#endif

#ifdef PrintMemoryLeak
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif
#endif

using namespace Framework::Logging;
using namespace Framework::Messaging;
using namespace Framework::Agents;
using namespace Framework::Eventing;
using namespace Framework::Communications;

namespace Framework
{
    RootNode::RootNode(unsigned rank, unsigned totalNumNode) 
		: myRank(rank), totalNumNodes(totalNumNode), terminate(false), agent(NULL), isAgentAdded(false), mainDistributor(NULL), eventDist(NULL)
    {
        sem_init(&binarySem, 0, 1);
		
		this->log = LogManager::Instance();
        mainDistributor = new MainMessageDistributor(rank, totalNumNode);
		MainEventDistributorRemoteAgent *remoteAgent = new MainEventDistributorRemoteAgent(rank, totalNumNode, mainDistributor);

		eventDist = EventDistributor::Instance();
		EventDistributor *eventDistributor = static_cast<EventDistributor *>(EventDistributor::Instance());

		if (!Validator::IsNull(eventDistributor, NAME("eventDistributor")))
		{
			eventDistributor->Init(rank, mainDistributor, remoteAgent);
		}        
    }
    
    RootNode::~RootNode()
    { 
		delete this->agent; 
		this->agent = NULL;

		delete this->mainDistributor;
		this->mainDistributor = NULL;

        sem_destroy(&this->binarySem);

		#ifdef PrintMemoryLeak
			CHECK_HEAP();
		#endif
    }

    void RootNode::Run()
    {
        int rc;

        rc = pthread_create(&mainThread, NULL, RootNode::MainThread, (void *)this); 
    }
        
    void RootNode::Wait()
    {
		if (!Validator::IsNull(this->mainDistributor, NAME("this->mainDistributor")))
		{
			this->mainDistributor->Wait();
		}

        pthread_join(mainThread, NULL);
    }

    void RootNode::Terminate()
    {
		#ifdef Debug
		if (!Validator::IsNull(log, NAME("log")))
		{
			log->Log(&typeid(this), "Terminating Node: %d", this->myRank);
		}
		#endif

        sem_wait(&this->binarySem);
        this->terminate = true;
        
		if (!Validator::IsNull(this->mainDistributor, NAME("this->mainDistributor")))
		{
			this->mainDistributor->Terminate();
		}

		EventDistributor *eventDist = static_cast<EventDistributor *>(EventDistributor::Instance());

		if (!Validator::IsNull(eventDist, NAME("eventDist")))
		{
			eventDist->Shutdown();
		}

		Factories::PackagerFactory *packageFactory = Factories::PackagerFactory::Instance();

		if (!Validator::IsNull(packageFactory, NAME("packageFactory")))
		{
			packageFactory->Shutdown();
		}

		//Since the MessageDistributor is a singleton, it will exist on the root node.
		static_cast<MessageDistributor *>(MessageDistributor::Instance())->Shutdown();
        sem_post(&this->binarySem);
    }

	void RootNode::AddAgent(IAgent *newAgent)
	{
		sem_wait(&this->binarySem);
		if (!this->isAgentAdded)
		{
			agent = newAgent;
			this->isAgentAdded = true;
		}
		sem_post(&this->binarySem);
	}

    bool RootNode::IsTerminated()
    {
        sem_wait(&this->binarySem);
        bool isTerminated = this->terminate;
        sem_post(&this->binarySem);
        return isTerminated;
    }

	IMessageDistributor *RootNode::GetMessageDistributor()
	{
		return this->mainDistributor;
	}

	Eventing::IEventDistributor *RootNode::GetEventDistributor()
	{
		return this->eventDist;
	}

    /*static*/ void *RootNode::MainThread(void *arg)
    {
        RootNode *pThis = static_cast<RootNode *>(arg);

		if (!Validator::IsNull(pThis, NAME("pThis")))
		{
#ifdef Debug
			if (!Validator::IsNull(pThis->log, NAME("pThis->log")))
			{
				pThis->log->Log(&typeid(pThis), "Hello World! I am Node = %d", pThis->myRank);
			}
#endif

			sem_wait(&pThis->binarySem);
			if (!Validator::IsNull(pThis->agent, NAME("pThis->agent")))
			{
				pThis->agent->Run();			
			}
			sem_post(&pThis->binarySem);

			//pThis->mainDistributor->Wait();
		}

        #ifdef LINUX
				pthread_exit(NULL);
			#endif

#ifdef WIN32
        return NULL;
#endif
    }

}