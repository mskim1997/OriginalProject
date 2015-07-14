/////////////////////////////////////////////////////////
//EventDistributorRemoteAgent.h
//Defines the EventDistributorRemoteAgent, this object is
//responsible for handling remote subscriptions.
//
//Manuel Madera
//9/07/09
/////////////////////////////////////////////////////////

#ifndef EventDistributorRemoteAgent_h
#define EventDistributorRemoteAgent_h

#include "IEventDistributorRemoteAgent.h"
#include "Framework.h"
#include "Packages.h"
#include "IMessageDistributor.h"
#include <map>
#include "ISubscriber.h"
#include "EventDistributor.h"
#include "Publisher.h"
#include "MazePositionEventPackage.h"
#include <semaphore.h>
#include <queue>
#include "ISubscription.h"

using namespace std;

namespace Framework
{      
	namespace Eventing
	{
		class FRAMEWORK_API EventDistributorRemoteAgent : public IEventDistributorRemoteAgent, public ISubscriber
		{
		private:
			sem_t binSem;
			sem_t subscriptionSem;
			Messaging::IMessageDistributor *msgDist;
			IEventDistributor *eventDist;
			unsigned rank;
			map<Packages::PackageType::Package, Eventing::IPublisher *> packageTypeToPublisherMap;
			map<SubscriptionTypes::SubscriptionType, queue<unsigned>> subscriptionQueueMap;

		public:
			EventDistributorRemoteAgent(unsigned sourceRank, Messaging::IMessageDistributor *messageDist);
			virtual ~EventDistributorRemoteAgent();

			void RegisterRemoteSubscription(Packages::PackageType::Package remoteSubscriptionType, SubscriptionTypes::SubscriptionType type);

			void Subscribe(SubscriptionTypes::SubscriptionType type, NotificationType::Life life);
			void Unsubscribe(SubscriptionTypes::SubscriptionType type);

			void Notify(const ISubscription *subscription);
		private:
			static void RegisterRemoteSubscriber(void *ownerObject, Packages::IPackage *package);
			static void OnEvent(void *ownerObject, Packages::IPackage *package);
		};
	}
}

#endif  //IEventDistributorRemoteAgent_h