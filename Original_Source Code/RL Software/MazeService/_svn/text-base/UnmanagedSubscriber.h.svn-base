#pragma unmanaged

#ifndef UnmanagedSubscriber_h
#define UnmanagedSubscriber_h

#include "Framework\ISubscriber.h"
#include "Framework\MazePositionEventPackage.h"
#include "Framework\EventDistributor.h"
#include "Framework\BlockingQueue.h"

class UnmanagedSubscriber : public Framework::Eventing::ISubscriber
{
private:
	Framework::Agents::BlockingQueue<Framework::Packages::IPackage> queue;

public:
	UnmanagedSubscriber();

	~UnmanagedSubscriber();

	void Run();

	void Notify(const Framework::Eventing::ISubscription *subscription);

	Framework::Packages::MazePositionEventPackage *GetPackage();

	void Terminate();
};

#endif	//UnmanagedSubscriber_h