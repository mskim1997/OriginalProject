/////////////////////////////////////////////////////////
//EventDistributorRemoteAgent.cpp
//Defines the EventDistributorRemoteAgent, this object is
//responsible for handling remote subscriptions.
//
//Manuel Madera
//9/07/09
/////////////////////////////////////////////////////////

#include "EventDistributorRemoteAgent.h"
#include "EventDistributor.h"
#include "PackagerFactory.h"
#include "Debug.h"

#ifdef PrintMemoryLeak
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif
#endif

using namespace Framework::Packages;
using namespace Framework::Messaging;
using namespace Framework::Factories;

namespace Framework
{      
	namespace Eventing
	{
		EventDistributorRemoteAgent::EventDistributorRemoteAgent(unsigned sourceRank,IMessageDistributor *messageDist) : rank(sourceRank)
		{
			sem_init(&binSem, 0, 1);
			sem_init(&subscriptionSem, 0, 1);

			this->msgDist = messageDist;
			this->eventDist = EventDistributor::Instance();

			PackagerFactory *packageFactory = PackagerFactory::Instance();

			if (!Validator::IsNull(packageFactory, NAME("packageFactory")))
			{
				packageFactory->RegisterPackage(PackageType::EVENT_SUBSCRIPTION, EventSubscriptionPackage::CreatePackage);
				packageFactory->RegisterPackage(PackageType::MAZE_POSITION_EVENT, MazePositionEventPackage::CreatePackage);
			}

			//Register callbacks
			if (!Validator::IsNull(this->msgDist, NAME("this->msgDist")))
			{
				this->msgDist->RegisterCallback(PackageType::EVENT_SUBSCRIPTION, this, &EventDistributorRemoteAgent::RegisterRemoteSubscriber);
			}

			RegisterRemoteSubscription(PackageType::MAZE_POSITION_EVENT, SubscriptionTypes::MAZE_POSITION_EVENT);
		}
		
		EventDistributorRemoteAgent::~EventDistributorRemoteAgent()
		{
			//sem_wait(&this->binSem);

			sem_destroy(&this->binSem);
			sem_destroy(&this->subscriptionSem);

			map<PackageType::Package, IPublisher *>::iterator iter;

			for (iter = this->packageTypeToPublisherMap.begin(); iter != this->packageTypeToPublisherMap.end(); iter++)
			{
				delete iter->second; 
				iter->second = NULL;
			}
		}
		
		void EventDistributorRemoteAgent::RegisterRemoteSubscription(Packages::PackageType::Package remoteSubscriptionType, SubscriptionTypes::SubscriptionType type)
		{
			sem_wait(&this->binSem);

			if (!Validator::IsNull(this->msgDist, NAME("this->msgDist")))
			{
				this->msgDist->RegisterCallback(remoteSubscriptionType, this, &EventDistributorRemoteAgent::OnEvent);
			}

			if (this->packageTypeToPublisherMap.find(remoteSubscriptionType) == this->packageTypeToPublisherMap.end())
			{
				this->packageTypeToPublisherMap[remoteSubscriptionType] = new Publisher(this->eventDist, type);
			}
			sem_post(&this->binSem);
		}

		void EventDistributorRemoteAgent::Subscribe(SubscriptionTypes::SubscriptionType type, NotificationType::Life life)
		{
			sem_wait(&this->binSem);
			if (!Validator::IsNull(this->msgDist, NAME("this->msgDist")))
			{
				this->msgDist->SendPackage(new EventSubscriptionPackage(rank, 0, type, life));
			}
			sem_post(&this->binSem);
		}

		void EventDistributorRemoteAgent::Unsubscribe(SubscriptionTypes::SubscriptionType type)
		{
			//This method might not be needed since unsubscribing with the EventDistributor, removes the subscriber form the list.
		}
		
		void EventDistributorRemoteAgent::Notify(const ISubscription *subscription)
		{
			//sem_wait(&this->binSem);
			const RemoteSubscription *eventSubscription = dynamic_cast<const RemoteSubscription *>(subscription);
			IPackage *package = NULL;
			
			if (!Validator::IsNull(eventSubscription, NAME("eventSubscription")))
			{
				package = eventSubscription->Clone();
			}

			ISubscription *subscriptionClone = dynamic_cast<ISubscription *>(package);
			unsigned nextDestination;

			if ((!Validator::IsNull(package, NAME("package"))) && (!Validator::IsNull(subscriptionClone, NAME("subscriptionClone"))))
			{
				if (package->GetDestination() != this->rank)
				{
					package->SetSource(this->rank);

					sem_wait(&this->subscriptionSem);
					nextDestination = this->subscriptionQueueMap[subscriptionClone->GetSubscriptionType()].front();
					this->subscriptionQueueMap[subscriptionClone->GetSubscriptionType()].pop();

					package->SetDestination(nextDestination);
					this->subscriptionQueueMap[subscriptionClone->GetSubscriptionType()].push(nextDestination);
					sem_post(&this->subscriptionSem);

					if (!Validator::IsNull(this->msgDist, NAME("this->msgDist")))
					{
						this->msgDist->SendPackage(package);
					}
					else
					{
						delete package; 
						package = NULL;
					}
				}
			}
		}

		/*static*/ void EventDistributorRemoteAgent::RegisterRemoteSubscriber(void *ownerObject, Packages::IPackage *package)
		{					
			EventDistributorRemoteAgent *pThis = static_cast<EventDistributorRemoteAgent *>(ownerObject);

			if (!Validator::IsNull(pThis, NAME("pThis")))
			{
				EventSubscriptionPackage *subscription = dynamic_cast<EventSubscriptionPackage *>(package);			

				if (!Validator::IsNull(subscription, NAME("subscription")))
				{
					SubscriptionTypes::SubscriptionType type = subscription->GetSubscription();

					if (!Validator::IsNull(pThis->eventDist, NAME("pThis->eventDist")))
					{
						pThis->eventDist->Subscribe(pThis, type, subscription->GetSubscriptionLife());
					}

					sem_wait(&pThis->subscriptionSem);
					if (pThis->subscriptionQueueMap.find(type) == pThis->subscriptionQueueMap.end())
					{
						queue<unsigned> newQueue;
						pThis->subscriptionQueueMap[type] = newQueue;
					}

					if (!Validator::IsNull(package, NAME("package")))
					{
						pThis->subscriptionQueueMap[type].push(package->GetSource());
					}

					sem_post(&pThis->subscriptionSem);
				}
			}

			delete package; 
			package = NULL;
		}

		/*static*/ void EventDistributorRemoteAgent::OnEvent(void *ownerObject, Packages::IPackage *package)
		{
			EventDistributorRemoteAgent *pThis = static_cast<EventDistributorRemoteAgent *>(ownerObject);

			if ((!Validator::IsNull(pThis, NAME("pThis"))) && (!Validator::IsNull(package, NAME("package"))))
			{
				sem_wait(&pThis->binSem);
				if (!Validator::IsNull(pThis->packageTypeToPublisherMap[package->GetType()], NAME("pThis->packageTypeToPublisherMap[package->GetType()]")))
				{
					pThis->packageTypeToPublisherMap[package->GetType()]->Publish(dynamic_cast<ISubscription *>(package));
				}
				sem_post(&pThis->binSem);
			}
		}
	}
}