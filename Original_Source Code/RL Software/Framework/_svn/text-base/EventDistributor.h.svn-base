/////////////////////////////////////////////////////////
//EventDistributor.h
//Defines the EventDistributor who is responsible for managing
//local events and notifications
//
//Manuel Madera
//7/12/09
/////////////////////////////////////////////////////////

#ifndef EventDistributor_h
#define EventDistributor_h

#include "IEventDistributor.h"
#include <semaphore.h>
#include <pthread.h>
#include <map>
#include <list>
#include "Framework.h"
#include "IEventDistributorRemoteAgent.h"
#include "IMessageDistributor.h"

namespace Framework
{   
	namespace Eventing
	{
		class FRAMEWORK_API EventDistributor : public IEventDistributor
		{
		private:
			typedef struct
			{
				ISubscriber *subscriber;
				SubscriptionTypes::SubscriptionType subscriptionType;
				NotificationType::Life notificationLife;
			}SubscriptionItem;

			bool isInitialized;
			bool terminate;
			sem_t publishersTableSem;
			sem_t subscribersTableSem;
			sem_t subscriptionSem;
			sem_t publicationSem;
			sem_t initSem;
			static IEventDistributor *instance;
			std::map<IPublisher *, SubscriptionTypes::SubscriptionType> publisherTable;
			std::list<SubscriptionItem> subscribersList;
			std::list<ISubscription *> subscriptionQueue;
			pthread_t mainThread;
			unsigned rank;
			IEventDistributorRemoteAgent *remoteAgent;

			EventDistributor();

		public:		
			//This method can only be called once
			void Init(unsigned rank, Messaging::IMessageDistributor *msgDist, IEventDistributorRemoteAgent *remAgent = NULL);

			//No need to be thread-safe since it is called once
			//at the beginning of each node.
			static IEventDistributor *Instance();

			virtual ~EventDistributor();

			//This method can only be called once
			void Shutdown();

			//A remote subscription must be registered before it could be used.
			void RegisterRemoteSubscription(Packages::PackageType::Package remoteSubscriptionType, SubscriptionTypes::SubscriptionType type);

			//A publisher can only publish one time of subscription. If the same publisher calls this
			//method more than once, the previous subscription type will be overwritten.
			bool Register(IPublisher *publisher, SubscriptionTypes::SubscriptionType subscriptionType);
			void Unregiter(IPublisher *publisher);

			//A subscriber can subscribe to many different subscriptions.
			bool Subscribe(ISubscriber *subscriber, SubscriptionTypes::SubscriptionType subscriptionType, NotificationType::Life notificationLife, bool notifyOnRemoteAlso = false);
			void Unsubscribe(ISubscriber *subscriber, SubscriptionTypes::SubscriptionType subscriptionType);

			void Publish(IPublisher *publisher, ISubscription *subscription);

			void WaitForInit();
		private:
			static void *EventDistributorThread(void *arg);
		};
	}
}

#endif  //EventDistributor_h