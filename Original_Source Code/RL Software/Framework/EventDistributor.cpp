/////////////////////////////////////////////////////////
//EventDistributor.cpp
//Defines the EventDistributor who is responsible for managing
//local events and notifications
//
//Manuel Madera
//7/12/09
/////////////////////////////////////////////////////////

#include "EventDistributor.h"
#include "Packages.h"
#include "EventDistributorRemoteAgent.h"
#include "Debug.h"

#ifdef PrintMemoryLeak
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif
#endif

using namespace std;
using namespace Framework::Packages;
using namespace Framework::Messaging;

namespace Framework
{   
	namespace Eventing
	{
		IEventDistributor *EventDistributor::instance = 0;

		EventDistributor::EventDistributor() : terminate(false), isInitialized(false), remoteAgent(0)
		{
			sem_init(&subscriptionSem, 0, 1);
			sem_init(&publishersTableSem, 0, 1);
			sem_init(&subscribersTableSem, 0, 1);

			sem_init(&publicationSem, 0, 0);
			sem_init(&initSem, 0, 0);
		}

		void EventDistributor::Init(unsigned rank, IMessageDistributor *msgDist, IEventDistributorRemoteAgent *remAgent)
		{
			if (!this->isInitialized)
			{
				int rc;
				this->rank = rank;

				if (remAgent == NULL)
				{
					this->remoteAgent = new EventDistributorRemoteAgent(this->rank, msgDist);
				}
				else
				{
					this->remoteAgent = remAgent;
				}

				rc = pthread_create(&this->mainThread, NULL, EventDistributor::EventDistributorThread, (void *)this);
	            
				this->isInitialized = true;

				sem_post(&this->initSem);
			}
		}

		/*static*/ IEventDistributor *EventDistributor::Instance()
		{
			if (instance == 0)
			{
				instance = new EventDistributor();
			}

			return instance;
		}

		EventDistributor::~EventDistributor() 
		{			
			if (this->isInitialized)
			{
				delete remoteAgent;
				remoteAgent = NULL;

				terminate = true;
				sem_post(&this->publicationSem);

				pthread_join(this->mainThread, NULL);

				sem_destroy(&this->subscriptionSem);
				sem_destroy(&this->publishersTableSem);
				sem_destroy(&this->subscribersTableSem);

				sem_destroy(&this->publicationSem);			
			}
		}

		void EventDistributor::WaitForInit()
		{
			if (!this->isInitialized)
			{
				sem_wait(&this->initSem);
			}
		}

		void EventDistributor::Shutdown()
		{
			delete instance; 
			instance = NULL;
		}

		void EventDistributor::RegisterRemoteSubscription(Packages::PackageType::Package remoteSubscriptionType, SubscriptionTypes::SubscriptionType type)
		{
			//It is assumed that the RegisterRemoteSubscription method is thread-safe.
			if (!Validator::IsNull(this->remoteAgent, NAME("this->remoteAgent")))
			{
				this->remoteAgent->RegisterRemoteSubscription(remoteSubscriptionType, type);
			}
		}

		bool EventDistributor::Register(IPublisher *publisher, SubscriptionTypes::SubscriptionType subscriptionType) 
		{
			bool success = false;

			sem_wait(&this->publishersTableSem);
			success = true;
			publisherTable[publisher] = subscriptionType;
			sem_post(&this->publishersTableSem);

			return success;
		}

		void EventDistributor::Unregiter(IPublisher *publisher) 
		{
			sem_wait(&this->publishersTableSem);
			publisherTable.erase(publisher);
			sem_post(&this->publishersTableSem);
		}

		bool EventDistributor::Subscribe(ISubscriber *subscriber, SubscriptionTypes::SubscriptionType subscriptionType, NotificationType::Life notificationLife, bool notifyOnRemoteAlso) 
		{
			bool success = false;

			sem_wait(&this->subscribersTableSem);
			if (this->isInitialized)
			{
				success = true;

				SubscriptionItem item;

				item.subscriber = subscriber;
				item.subscriptionType = subscriptionType;
				item.notificationLife = notificationLife;

				subscribersList.push_back(item);

				if (notifyOnRemoteAlso)
				{
					if (!Validator::IsNull(this->remoteAgent, NAME("this->remoteAgent")))
					{
						this->remoteAgent->Subscribe(subscriptionType, notificationLife);
					}
				}
			}
			sem_post(&this->subscribersTableSem);

			return success;
		}

		void EventDistributor::Unsubscribe(ISubscriber *subscriber, SubscriptionTypes::SubscriptionType subscriptionType) 
		{
			sem_wait(&this->subscribersTableSem);
			list<SubscriptionItem>::iterator iter;

			for (iter = subscribersList.begin(); iter != subscribersList.end(); iter++)
			{
				if ((iter->subscriber == subscriber)&&(iter->subscriptionType == subscriptionType))
				{
					subscribersList.erase(iter);
					break;
				}
			}
			sem_post(&this->subscribersTableSem);
		}

		void EventDistributor::Publish(IPublisher *publisher, ISubscription *subscription) 
		{
			sem_wait(&this->publishersTableSem);
			map<IPublisher *, SubscriptionTypes::SubscriptionType>::iterator iter;

			iter = publisherTable.find(publisher);

			if (iter != publisherTable.end())
			{
				if (!Validator::IsNull(subscription, NAME("subscription")))
				{
					if (iter->second == subscription->GetSubscriptionType())
					{
						sem_wait(&this->subscriptionSem);
						subscriptionQueue.push_back(subscription);
						sem_post(&this->subscriptionSem);
					}
				}
			}
			else
			{
				delete subscription;
				subscription = NULL;
			}

			sem_post(&this->publicationSem);
			sem_post(&this->publishersTableSem);
		}

		/*static*/ void *EventDistributor::EventDistributorThread(void *arg)
		{
			EventDistributor* pThis = static_cast<EventDistributor *>(arg);

			if (!Validator::IsNull(pThis, NAME("pThis")))
			{
				list<SubscriptionItem>::iterator iter;
				ISubscription *subscription = NULL;
				bool subscriptionHasBeenDeleted = false;
				list<list<SubscriptionItem>::iterator> subscribersToRemove;
				list<list<SubscriptionItem>::iterator>::iterator subscribersToRemoveIter;

				while (!pThis->terminate)
				{
					subscriptionHasBeenDeleted = false;

					sem_wait(&pThis->publicationSem);

					sem_wait(&pThis->subscriptionSem);
					if (pThis->subscriptionQueue.size() > 0)
					{
						subscription = pThis->subscriptionQueue.front();
						pThis->subscriptionQueue.pop_front();
					}
					else
					{
						subscription = NULL;
					}
					sem_post(&pThis->subscriptionSem);

					if (!Validator::IsNull(subscription, NAME("subscription")))
					{
						sem_wait(&pThis->subscribersTableSem);
						for (iter = pThis->subscribersList.begin(); iter != pThis->subscribersList.end(); iter++)
						{
							if (!Validator::IsNull(subscription, NAME("subscription")))
							{
								if (iter->subscriptionType == subscription->GetSubscriptionType())
								{
									if (!Validator::IsNull(iter->subscriber, NAME("iter->subscriber")))
									{
										iter->subscriber->Notify(subscription);
									}

									if (iter->notificationLife == NotificationType::ONETIME)
									{
										subscribersToRemove.push_back(iter);
									}

									if (subscription->GetSubscriptionLife() == SubscriptionTypes::NOTIFY_ONCE)
									{
										delete subscription;
										subscription = NULL;
										subscriptionHasBeenDeleted = true;
										break;
									}
								}
							}
						}

						//Delete all subscribers marked to be deleted
						for (subscribersToRemoveIter = subscribersToRemove.begin(); subscribersToRemoveIter != subscribersToRemove.end(); subscribersToRemoveIter++)
						{
							pThis->subscribersList.erase(*subscribersToRemoveIter);
						}

						subscribersToRemove.clear();

						sem_post(&pThis->subscribersTableSem);

						if (!subscriptionHasBeenDeleted)
						{
							delete subscription;
							subscription = NULL;
						}
					}
				}
			}

			#ifdef LINUX
				pthread_exit(NULL);
			#endif

	#ifdef WIN32
			return NULL;
	#endif
		}
	}
}