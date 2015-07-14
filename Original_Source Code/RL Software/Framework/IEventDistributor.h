/////////////////////////////////////////////////////////
//IEventDistributor.h
//Defines the IEventDistributor interface.
//
//Manuel Madera
//7/12/09
/////////////////////////////////////////////////////////

#ifndef IEventDistributor_h
#define IEventDistributor_h

#include "ISubscription.h"
#include "ISubscriber.h"
#include "IPublisher.h"
#include "Framework.h"
#include "Packages.h"

namespace Framework
{   
	namespace Eventing
	{
		class FRAMEWORK_API IEventDistributor
		{
		public:
			virtual ~IEventDistributor() {}

			virtual bool Register(IPublisher *publisher, SubscriptionTypes::SubscriptionType subscriptionType) = 0;
			virtual void Unregiter(IPublisher *publisher) = 0;

			virtual void RegisterRemoteSubscription(Packages::PackageType::Package remoteSubscriptionType, SubscriptionTypes::SubscriptionType type) = 0;

			virtual bool Subscribe(ISubscriber *subscriber, SubscriptionTypes::SubscriptionType subscriptionType, NotificationType::Life notificationLife, bool notifyOnRemoteAlso = false) = 0;
			virtual void Unsubscribe(ISubscriber *subscriber, SubscriptionTypes::SubscriptionType subscriptionType) = 0;

			virtual void Publish(IPublisher *publisher, ISubscription *subscription) = 0;

			virtual void WaitForInit() = 0;
		};
	}
}

#endif  //IEventDistributor_h