/////////////////////////////////////////////////////////
//MainEventDistributorRemoteAgent.h
//Defines the EventDistributorRemoteAgent, this object is
//responsible for handling remote subscriptions.
//
//Manuel Madera
//9/07/09
/////////////////////////////////////////////////////////

#ifndef MainEventDistributorRemoteAgent_h
#define MainEventDistributorRemoteAgent_h

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

using namespace std;

namespace Framework
{      
	namespace Eventing
	{
		class FRAMEWORK_API MainEventDistributorRemoteAgent : public IEventDistributorRemoteAgent, public ISubscriber
		{
		private:
			sem_t binSem;
			Messaging::IMessageDistributor *msgDist;
			IEventDistributor *eventDist;
			unsigned rank;
			unsigned maxNumNode;
			map<Packages::PackageType::Package, Eventing::IPublisher *> packageTypeToPublisherMap;

		public:
			MainEventDistributorRemoteAgent(unsigned sourceRank, unsigned maxNumNodes, Messaging::IMessageDistributor *messageDist);
			virtual ~MainEventDistributorRemoteAgent();

			void RegisterRemoteSubscription(Packages::PackageType::Package remoteSubscriptionType, SubscriptionTypes::SubscriptionType type);

			void Subscribe(SubscriptionTypes::SubscriptionType type, NotificationType::Life life);
			void Unsubscribe(SubscriptionTypes::SubscriptionType type);

			void Notify(const ISubscription *subscription);
		private:
			void SendSubscriptionToAllNodes(Packages::EventSubscriptionPackage *package, unsigned subscribingNode);
			static void RegisterRemoteSubscriber(void *ownerObject, Packages::IPackage *package);
			static void OnEvent(void *ownerObject, Packages::IPackage *package);
		};
	}
}

#endif  //MainEventDistributorRemoteAgent_h