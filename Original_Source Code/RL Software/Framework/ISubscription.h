/////////////////////////////////////////////////////////
//ISubscription.h
//Defines the ISubscription interface.
//
//Manuel Madera
//7/12/09
/////////////////////////////////////////////////////////

#ifndef ISubscription_h
#define ISubscription_h

#include "Framework.h"

namespace Framework
{      
	namespace Eventing
	{
		namespace SubscriptionTypes
		{
			enum SubscriptionType
			{
				REGISTRATION_CHANGE,
				MAZE_RECEIVED,
				AGENT_DONE,
				PHYSICAL_PATH_DONE_EVENT,
				MAZE_POSITION_EVENT
			};

			enum SubscriptionLife
			{
				NOTIFY_ONCE,
				NOTIFY_ALL
			};
		}

		class FRAMEWORK_API ISubscription
		{
		public:
			virtual ~ISubscription() {}

			virtual SubscriptionTypes::SubscriptionType GetSubscriptionType() = 0;
			virtual SubscriptionTypes::SubscriptionLife GetSubscriptionLife() = 0;

			virtual void SetSubscriptionLife(SubscriptionTypes::SubscriptionLife life) = 0;
		};
	}
}

#endif  //ISubscription_h