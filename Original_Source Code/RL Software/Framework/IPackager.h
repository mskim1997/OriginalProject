/////////////////////////////////////////////////////////
//IPackager.h
//Defines the IPackager interface.
//
//Manuel Madera
//5/8/09
/////////////////////////////////////////////////////////

#ifndef IPackager_h
#define IPackager_h

#include "Packages.h"
#include <iostream>
#include "Framework.h"

namespace Framework
{       
	namespace Packagers
	{
		namespace PackagerType
		{
			enum Packagers
			{
				Synch_t = 0,
				ASynch_t,
				Dummy_t
			};
		}

		class FRAMEWORK_API IPackager
		{
		public:
			virtual ~IPackager() {}
	                        
			virtual void PackAndSend(Packages::IPackage *package) = 0;
			virtual void Terminate() = 0;
			virtual Packages::IPackage *GetAndUnpack() = 0;
		};
	}
}

#endif  //IPackager_h