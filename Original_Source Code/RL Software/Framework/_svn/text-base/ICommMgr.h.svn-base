/////////////////////////////////////////////////////////
//ICommMgr.h
//Defines the ICommMgr interface.
//
//Manuel Madera
//5/8/09
/////////////////////////////////////////////////////////

#ifndef ICommMgr_h
#define ICommMgr_h

#include "Packages.h"
#include "Framework.h"

namespace Framework
{
	namespace Communications
	{
		class FRAMEWORK_API ICommMgr
		{
		public:
			virtual ~ICommMgr() {}
	                                
			virtual void SendPackage(Packages::IPackage *package) = 0;
			virtual Packages::IPackage *GetPackage() = 0;
		};
	}
}

#endif  //ICommMgr_h