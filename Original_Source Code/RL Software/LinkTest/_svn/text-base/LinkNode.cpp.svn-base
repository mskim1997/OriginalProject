/////////////////////////////////////////////////////////
//LinkNode.cpp
//Defines the specific MLNode implementations.
//
//Manuel Madera
//7/10/10
/////////////////////////////////////////////////////////

#include "LinkNode.h"
#include "Framework\MazeReceivedSubscription.h"
#include "Framework\MazePositionEventPackage.h"
#include "Framework\Debug.h"
#include "Framework\Communicator.h"

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
using namespace Framework::Logging;
using namespace Framework::Communications;

namespace Framework
{
	LinkNode::LinkNode(unsigned rank, bool allowSharing) : Node(rank, allowSharing)
	{
		Init();
	}

    LinkNode::LinkNode(unsigned rank, bool allowSharing, IAgent *newAgent) : Node(rank, allowSharing, newAgent)
    {
		Init();
    }
    
	void LinkNode::Init()
	{
		sem_init(&doneSem, 0, 0);

		if (this->msgDist != NULL)
		{
			this->msgDist->RegisterCallback(PackageType::TERMINATE, this, LinkNode::TerminateCallback);
		}

		if (this->eventDist != NULL)
		{
			this->eventDist->RegisterRemoteSubscription(PackageType::AGENT_DONE_EVENT, SubscriptionTypes::AGENT_DONE);
		}
	}

    LinkNode::~LinkNode() 
    {
		sem_destroy(&this->doneSem);

		#ifdef PrintMemoryLeak
			CHECK_HEAP();
		#endif
    }

	void LinkNode::Wait()
	{
		sem_wait(&this->doneSem);
	}

	/*static*/ void LinkNode::TerminateCallback(void *ownerObject, Framework::Packages::IPackage *package)
	{
		LinkNode *pThis = static_cast<LinkNode *>(ownerObject);

		if (pThis != NULL)
		{
			sem_post(&pThis->doneSem);
		}

		delete package; 
		package = NULL;
	}
}