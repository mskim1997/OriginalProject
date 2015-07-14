/////////////////////////////////////////////////////////
//MainEventDistributorRemoteAgent.cpp
//Defines the EventDistributorRemoteAgent, this object is
//responsible for handling remote subscriptions.
//
//Manuel Madera
//9/07/09
/////////////////////////////////////////////////////////

#include "MainEventDistributorRemoteAgent.h"
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
		MainEventDistributorRemoteAgent::MainEventDistributorRemoteAgent(unsigned sourceRank, unsigned maxNumNodes, IMessageDistributor *messageDist) : rank(sourceRank), maxNumNode(maxNumNodes)
		{
			sem_init(&binSem, 0, 1);

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
				this->msgDist->RegisterCallback(PackageType::EVENT_SUBSCRIPTION, this, &MainEventDistributorRemoteAgent::RegisterRemoteSubscriber);
			}

			RegisterRemoteSubscription(PackageType::MAZE_POSITION_EVENT, SubscriptionTypes::MAZE_POSITION_EVENT);
		}
		
		MainEventDistributorRemoteAgent::~MainEventDistributorRemoteAgent()
		{
			//sem_wait(&this->binSem);

			sem_destroy(&this->binSem);

			map<PackageType::Package, IPublisher *>::iterator iter;

			for (iter = this->packageTypeToPublisherMap.begin(); iter != this->packageTypeToPublisherMap.end(); iter++)
			{
				delete iter->second;
				iter->second = NULL;
			}

			if (!Validator::IsNull(this->msgDist, NAME("this->msgDist")))
			{
				this->msgDist->UnregisterCallback(PackageType::EVENT_SUBSCRIPTION);
			}
		}
		
		void MainEventDistributorRemoteAgent::RegisterRemoteSubscription(Packages::PackageType::Package remoteSubscriptionType, SubscriptionTypes::SubscriptionType type)
		{
			sem_wait(&this->binSem);

			if (!Validator::IsNull(this->msgDist, NAME("this->msgDist")))
			{
				this->msgDist->RegisterCallback(remoteSubscriptionType, this, &MainEventDistributorRemoteAgent::OnEvent);
			}

			if (this->packageTypeToPublisherMap.find(remoteSubscriptionType) == this->packageTypeToPublisherMap.end())
			{
				this->packageTypeToPublisherMap[remoteSubscriptionType] = new Publisher(this->eventDist, type);
			}
			sem_post(&this->binSem);
		}

		void MainEventDistributorRemoteAgent::Subscribe(SubscriptionTypes::SubscriptionType type, NotificationType::Life life)
		{
			//Send registration to every available node
			sem_wait(&this->binSem);
			SendSubscriptionToAllNodes(new EventSubscriptionPackage(rank, 0, type, life), 0);
			sem_post(&this->binSem);
		}

		void MainEventDistributorRemoteAgent::Unsubscribe(SubscriptionTypes::SubscriptionType type)
		{
		}
		
		void MainEventDistributorRemoteAgent::SendSubscriptionToAllNodes(Packages::EventSubscriptionPackage *package, unsigned subscribingNode)
		{
			EventSubscriptionPackage *subscriptionPackage = NULL;

			for (unsigned i = 1; i < this->maxNumNode; i++)
			{
				if (i != subscribingNode)
				{
					if (!Validator::IsNull(package, NAME("package")))
					{
						subscriptionPackage = new EventSubscriptionPackage(package->GetSource(), 
																		   package->GetDestination(), 
																		   package->GetSubscription(), 
																		   package->GetSubscriptionLife());
					}

					if (!Validator::IsNull(subscriptionPackage, NAME("subscriptionPackage")))
					{
						subscriptionPackage->SetDestination(i);
					}

					if (!Validator::IsNull(this->msgDist, NAME("this->msgDist")))
					{
						this->msgDist->SendPackage(subscriptionPackage);
					}
				}
			}

			delete package;
			package = NULL;
		}

		void MainEventDistributorRemoteAgent::Notify(const ISubscription *subscription)
		{
			//const MazePositionEventPackage *eventSubcription = static_cast<const MazePositionEventPackage *>(subscription);
			//MazePositionEventPackage *package = new MazePositionEventPackage(this->rank, 0, eventSubcription->GetPositionPackage());

			//this->msgDist->SendPackage(package);
		}

		/*static*/ void MainEventDistributorRemoteAgent::RegisterRemoteSubscriber(void *ownerObject, Packages::IPackage *package)
		{			
			MainEventDistributorRemoteAgent *pThis = static_cast<MainEventDistributorRemoteAgent *>(ownerObject);
			MessagePackage *message = dynamic_cast<MessagePackage *>(package);

			//sem_wait(&pThis->binSem);
			if ((!Validator::IsNull(pThis, NAME("pThis")))&&(!Validator::IsNull(message, NAME("message"))))
			{
				pThis->SendSubscriptionToAllNodes(dynamic_cast<EventSubscriptionPackage *>(message->GetPackage()), message->GetSource());
			}
			//sem_post(&pThis->binSem);
		}

		/*static*/ void MainEventDistributorRemoteAgent::OnEvent(void *ownerObject, Packages::IPackage *package)
		{
			MainEventDistributorRemoteAgent *pThis = static_cast<MainEventDistributorRemoteAgent *>(ownerObject);
			//EventDistributorRemoteAgent *pThis = static_cast<EventDistributorRemoteAgent *>(ownerObject);

			if (!Validator::IsNull(package, NAME("package")))
			{
				if (!Validator::IsNull(pThis, NAME("pThis")))
				{
					unsigned source = package->GetSource();
					IPackage *subscriptionPackage = NULL;

					MessagePackage *message = dynamic_cast<MessagePackage *>(package);
					RemoteSubscription *remoteSubscription = NULL;

					if (!Validator::IsNull(message, NAME("message")))
					{
						remoteSubscription = dynamic_cast<RemoteSubscription *>(message->GetPackage());
					}
					//IPackage *remoteSubscription = message->GetPackage();

					//ISubscription *subscription = dynamic_cast<ISubscription *>(remoteSubscription);

					sem_wait(&pThis->binSem);
					if (!Validator::IsNull(remoteSubscription, NAME("remoteSubscription")))
					{
						if (remoteSubscription->GetDestination() == 0)
						{
							pThis->packageTypeToPublisherMap[remoteSubscription->GetType()]->Publish(remoteSubscription);
						}
						else
						{
							subscriptionPackage = remoteSubscription->Clone();

							if (!Validator::IsNull(subscriptionPackage, NAME("subscriptionPackage")))
							{
								subscriptionPackage->SetDestination(remoteSubscription->GetDestination());
								subscriptionPackage->SetSource(remoteSubscription->GetSource());

								if (!Validator::IsNull(pThis->msgDist, NAME("pThis->msgDist")))
								{
									pThis->msgDist->SendPackage(subscriptionPackage);
								}
							}
						}
					}
					sem_post(&pThis->binSem);
				}

				delete package; 
				package = NULL;
			}
		}
	}
}