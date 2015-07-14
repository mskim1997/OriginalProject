/////////////////////////////////////////////////////////
//ASynchPackager.h
//Responsible for packing/unpacking packages asynchronously.
//
//Manuel Madera
//6/27/09
/////////////////////////////////////////////////////////

#ifndef ASynchPackager_h
#define ASynchPackager_h

#include "IPackager.h"
#include "IAgent.h"
#include <semaphore.h>
#include "ICommunicator.h"
#include "Framework.h"
#include "LogManager.h"
#include "PackagerFactory.h"

namespace Framework
{        
	namespace Packagers
	{
		class FRAMEWORK_API ASynchPackager : public IPackager
		{
		private:        
			unsigned packageTag;
			unsigned messageTag;
			int *messageRequest;
			Communications::ICommunicator *comm;
			Logging::ILog *log;
			Factories::PackagerFactory *packageFactory;
			sem_t binSem;
	        
		public:
			ASynchPackager(int tag);
			virtual ~ASynchPackager();
	                        
			//Send a package synchronously
			void PackAndSend(Packages::IPackage *pPackage);

			void Terminate();

			//Receive a package asynchronously
			Packages::IPackage *GetAndUnpack();

			static IPackager *CreatePackager(unsigned tag);
		};
	}
}

#endif  //ASynchPackager_h