/////////////////////////////////////////////////////////
//IPublisher.h
//Defines the IPublisher interface.
//
//Manuel Madera
//7/12/09
/////////////////////////////////////////////////////////

#ifndef IPublisher_h
#define IPublisher_h

#include "Framework.h"
#include "ISubscription.h"

namespace Framework
{     
	namespace Eventing
	{
		class FRAMEWORK_API IPublisher
		{
		public:
			virtual ~IPublisher() {}

			virtual void Publish(ISubscription *subscription) = 0;
		};
	}
}

#endif  //IPublisher_h