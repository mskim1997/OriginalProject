/////////////////////////////////////////////////////////
//Publisher.h
//Defines the IPublisher interface.
//
//Manuel Madera
//7/12/09
/////////////////////////////////////////////////////////

#ifndef Publisher_h
#define Publisher_h

#include "Framework.h"
#include "IEventDistributor.h"
#include "ISubscription.h"
#include "IPublisher.h"
#include "Debug.h"

namespace Framework
{     
	namespace Eventing
	{
		class Publisher : public IPublisher
		{
		private:
			IEventDistributor *eventDist;
		public:
			Publisher(IEventDistributor *dist, SubscriptionTypes::SubscriptionType publisherType): eventDist(dist)
			{
				if (!Validator::IsNull(eventDist, NAME("eventDist")))
				{
					eventDist->Register(this, publisherType);
				}
			}

			virtual ~Publisher()
			{
				if (!Validator::IsNull(eventDist, NAME("eventDist")))
				{
					eventDist->Unregiter(this);
				}
			}

			void Publish(ISubscription *subscription)
			{
				if (!Validator::IsNull(eventDist, NAME("eventDist")))
				{
					eventDist->Publish(this, subscription);
				}
			}
		};
	}
}

#endif  //Publisher_h