#include "Nodes.h"
#include <iostream>
#include "CommMgr.h"
#include "MessageDistributor.h"
#include "EventDistributor.h"
#include "RLAgent.h"
#include "Debug.h"
#include "PackagerFactory.h"

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

using namespace std;
using namespace Framework::Messaging;
using namespace Framework::Eventing;
using namespace Framework::Agents;
using namespace Framework::Packages;
using namespace Framework::Factories;
using namespace Framework::Logging;
using namespace Framework::Communications;

namespace Framework
{

	Node::Node(unsigned rank, bool allowSharing) : myRank(rank), terminate(false), msgDist(0), shareInfo(allowSharing), eventDist(0)
	{
		Init(rank, allowSharing);
	}

    Node::Node(unsigned rank, bool allowSharing, IAgent *newAgent) : myRank(rank), /*agent(newAgent),*/ terminate(false), msgDist(0), shareInfo(allowSharing), eventDist(0)
    {
        Init(rank, allowSharing);

		AddAgent(newAgent);
    }
    
	void Node::Init(unsigned rank, bool allowSharing)
	{
		sem_init(&binarySem, 0, 1);		

		this->log = LogManager::Instance();

		//Create an instance of the packager before Threads are being created.
		PackagerFactory::Instance();
		this->msgDist = MessageDistributor::Instance();
		this->eventDist = EventDistributor::Instance();

		MessageDistributor *msgDist = static_cast<MessageDistributor *>(MessageDistributor::Instance());

		if (!Validator::IsNull(msgDist, NAME("msgDist")))
		{
			msgDist->Init(rank, allowSharing);    
		}
		
		EventDistributor *eventDist = static_cast<EventDistributor *>(EventDistributor::Instance());

		if (!Validator::IsNull(eventDist, NAME("eventDist")))
		{
			eventDist->Init(rank, msgDist);
		}
	}

    Node::~Node() 
    {
        sem_destroy(&this->binarySem);
    }
                
    void Node::Run()
    {
        int rc;
        
        rc = pthread_create(&mainThread, NULL, Node::MainThread, (void *)this); 
    }

    void Node::Terminate()
    {
		#ifdef Debug
			log->Log(&typeid(this), "Terminating Node: %d", this->myRank);
		#endif

        sem_wait(&this->binarySem);
        this->terminate = true;

		//Send registration request
		IPackage *terminate = new TerminatePackage(this->myRank, 0);

		if (!Validator::IsNull(this->msgDist, NAME("this->msgDist")))
		{
			this->msgDist->SendPackage(terminate);
		}

		Factories::PackagerFactory *packageFactory = Factories::PackagerFactory::Instance();

		if (!Validator::IsNull(packageFactory, NAME("packageFactory")))
		{
			packageFactory->Shutdown();
		}

		MessageDistributor *msgDist = static_cast<MessageDistributor *>(MessageDistributor::Instance());

		if (!Validator::IsNull(msgDist, NAME("msgDist")))
		{
			msgDist->Shutdown();
		}

		EventDistributor *eventDist = static_cast<EventDistributor *>(EventDistributor::Instance());

		if (!Validator::IsNull(eventDist, NAME("eventDist")))
		{
			eventDist->Shutdown();
		}
        sem_post(&this->binarySem);
    }

    void Node::Wait()
    {
        pthread_join(mainThread, NULL);
    }

	void Node::AddAgent(IAgent *newAgent)
	{
		sem_wait(&this->binarySem);
		agentList.push_back(newAgent);
		sem_post(&this->binarySem);
	}

    bool Node::IsTerminated()
    {
        sem_wait(&this->binarySem);
        bool isTerminated = this->terminate;
        sem_post(&this->binarySem);
        return isTerminated;
    }

	IMessageDistributor *Node::GetMessageDistributor()
	{
		return this->msgDist;
	}

	Eventing::IEventDistributor *Node::GetEventDistributor()
	{
		return this->eventDist;
	}

    /*static*/ void *Node::MainThread(void *arg)
    {
        Node *pThis = static_cast<Node *>(arg);
		list<IAgent *>::iterator iter;
		unsigned i = 0;
    
		if (!Validator::IsNull(pThis, NAME("pThis")))
		{
#ifdef Debug
			pThis->log->Log(&typeid(pThis), "Hello World! I am Node = %d", pThis->myRank);
#endif

			//IAgent *agent = new RLAgent(pThis->myRank, 0);

			sem_wait(&pThis->binarySem);
			for (iter = pThis->agentList.begin(); iter != pThis->agentList.end(); iter++)
			{
#ifdef Debug
				pThis->log->Log(&typeid(pThis), "Registering agent...");
#endif
				(*iter)->Register();
				//pThis->agent->Register();
			}		

			int *rc = new int[pThis->agentList.size()];
			pthread_t *threadArray = new pthread_t[pThis->agentList.size()];

			for (i = 0, iter = pThis->agentList.begin(); i < pThis->agentList.size(); i++, iter++)
			{
				rc[i] = pthread_create(&threadArray[i], NULL, Node::WorkerThread, *iter);
			}

			sem_post(&pThis->binarySem);

			for (i = 0; i < pThis->agentList.size(); i++)
			{
				pthread_join(threadArray[i], NULL);
			}

			sem_wait(&pThis->binarySem);
			for (iter = pThis->agentList.begin(); iter != pThis->agentList.end(); iter++)
			{
#ifdef Debug
				pThis->log->Log(&typeid(pThis), "Deleting agent...");
#endif

				delete *iter;
				*iter = NULL;
			}
			sem_post(&pThis->binarySem);

			delete[] rc;
			rc = NULL;

			delete[] threadArray;
			threadArray = NULL;
		}

        #ifdef LINUX
				pthread_exit(NULL);
		#endif

#ifdef WIN32
        return NULL;
#endif
    }

	/*static*/ void *Node::WorkerThread(void *arg)
	{
		IAgent *agent = static_cast<IAgent *>(arg);

#ifdef Debug
		ILog *log = LogManager::Instance();

		if (!Validator::IsNull(log, NAME("log")))
		{
			log->Log(&typeid(Node), "Running...");
		}
#endif		

		if (!Validator::IsNull(agent, NAME("agent")))
		{
			agent->Run();
		}

#ifdef LINUX
		pthread_exit(NULL);
#endif

#ifdef WIN32
		return NULL;
#endif
	}
}