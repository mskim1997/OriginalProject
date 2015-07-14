#pragma unmanaged
#include "stdafx.h"
#include "UnmanagedSubscriber.h"
#include "Framework\Debug.h"

#ifdef PrintMemoryLeak
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif
#endif

#ifdef WIN32
#include <windows.h>
#endif

using namespace Framework::Packages;
using namespace Framework::Eventing;

UnmanagedSubscriber::UnmanagedSubscriber()
{	
}

UnmanagedSubscriber::~UnmanagedSubscriber()
{
}

void UnmanagedSubscriber::Notify(const ISubscription *subscription)
{
	//AgentService* agentService = AgentService::Instance;
	const MazePositionEventPackage *pos = dynamic_cast<const MazePositionEventPackage *>(subscription);

	if (pos != NULL)
	{
		//_asm {int 3};

		queue.Enqueue(new MazePositionEventPackage(0, 0, pos->GetPositionPackage()));
		/*onEvent(new MazePositionEventPackage(0, 0, pos->GetPositionPackage()));*/
	}
}

void UnmanagedSubscriber::Run()
{
	IEventDistributor *eventDist = EventDistributor::Instance();

	if (eventDist != NULL)
	{
		eventDist->WaitForInit();
		eventDist->Subscribe(this, SubscriptionTypes::MAZE_POSITION_EVENT, NotificationType::LONGLIVED, true);
	}
}

MazePositionEventPackage *UnmanagedSubscriber::GetPackage()
{
	return dynamic_cast<MazePositionEventPackage *>(queue.Dequeue());
}

void UnmanagedSubscriber::Terminate()
{
	queue.Enqueue(NULL);
}