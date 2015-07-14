/////////////////////////////////////////////////////////
//RemoteSubscription.h
//Abstract base class for all remote subscription.
//
//Manuel Madera
//10/24/09
/////////////////////////////////////////////////////////

#ifndef RemoteSubscription_h
#define RemoteSubscription_h

#include "Framework.h"
#include "Packages.h"

namespace Framework
{      
	namespace Eventing
	{
		class FRAMEWORK_API RemoteSubscription : public Packages::BasePackage, public ISubscription
		{
		public:
			RemoteSubscription() : Packages::BasePackage()
			{
			}

			RemoteSubscription(unsigned source, unsigned dest, Packages::PackageType::Package packageType) 
				: Packages::BasePackage(source, dest, packageType)
			{
			}

			virtual ~RemoteSubscription() {}

			virtual RemoteSubscription *Clone() const = 0;
		};
	}
}

#endif  //RemoteSubscription_h