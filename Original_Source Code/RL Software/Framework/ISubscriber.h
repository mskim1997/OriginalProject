/////////////////////////////////////////////////////////
//ISubscriber.h
//Defines the ISubscription interface.
//
//Manuel Madera
//7/12/09
/////////////////////////////////////////////////////////

#ifndef ISubscriber_h
#define ISubscriber_h

#include "ISubscription.h"
#include "Framework.h"

namespace Framework
{      
	namespace Eventing
	{
		namespace NotificationType
		{
			enum Life
			{
				ONETIME,
				LONGLIVED
			};
		}

		class FRAMEWORK_API ISubscriber
		{
		public:
			virtual ~ISubscriber() {}

			virtual void Notify(const ISubscription *subscription) = 0;
		};
	}
}

#endif  //ISubscriber_h