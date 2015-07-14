/////////////////////////////////////////////////////////
//IEventDistributorRemoteAgent.h
//Defines the IEventDistributorRemoteAgent interface.
//
//Manuel Madera
//9/07/09
/////////////////////////////////////////////////////////

#ifndef IEventDistributorRemoteAgent_h
#define IEventDistributorRemoteAgent_h

#include "ISubscription.h"
#include "Framework.h"
#include "ISubscriber.h"
#include "Packages.h"

namespace Framework
{      
	namespace Eventing
	{
		class FRAMEWORK_API IEventDistributorRemoteAgent
		{
		public:
			virtual ~IEventDistributorRemoteAgent() {}

			virtual void RegisterRemoteSubscription(Packages::PackageType::Package remoteSubscriptionType, SubscriptionTypes::SubscriptionType type) = 0;
			virtual void Subscribe(SubscriptionTypes::SubscriptionType type, NotificationType::Life life) = 0;
			virtual void Unsubscribe(SubscriptionTypes::SubscriptionType type) = 0;
		};
	}
}

#endif  //IEventDistributorRemoteAgent_h