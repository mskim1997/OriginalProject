/////////////////////////////////////////////////////////
//Packager.h
//Responsible for packing/unpacking packages.
//
//Manuel Madera
//5/8/09
/////////////////////////////////////////////////////////

#ifndef Packager_h
#define Packager_h

#include "IPackager.h"
#include "IAgent.h"
#include "ICommunicator.h"
#include "Framework.h"
#include "LogManager.h"
#include "PackagerFactory.h"

namespace Framework
{     
	namespace Packagers
	{
		class FRAMEWORK_API Packager : public IPackager
		{
		private:        
			int packageTag;
			int messageTag;
			Communications::ICommunicator *comm;
			Logging::ILog *log;
			Factories::PackagerFactory *packageFactory;

		public:
			Packager(int tag);
			virtual ~Packager();
	                        
			void PackAndSend(Packages::IPackage *pPackage);
			void Terminate();
			Packages::IPackage *GetAndUnpack();

			static IPackager *CreatePackager(unsigned tag);
		};
	}
}

#endif  //Packager_h