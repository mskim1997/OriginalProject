/////////////////////////////////////////////////////////
//OneWayCommMgr.h
//Responsible for handling one way communication between nodes.
//
//Manuel Madera
//6/21/09
/////////////////////////////////////////////////////////

#ifndef OneWayCommMgr_h
#define OneWayCommMgr_h

#include "ICommMgr.h"
#include "Packager.h"
#include <pthread.h>
#include <semaphore.h>
#include <list>
#include "Framework.h"

using namespace std;

namespace Framework
{
	namespace Communications
	{
		namespace ConnectionType
		{
			enum Type
			{
				Send,
				Recv
			};
		}

		class FRAMEWORK_API OneWayCommMgr : public ICommMgr
		{
		private:
			ConnectionType::Type commType;
			Packagers::IPackager *pPackager;

			list<Packages::IPackage *> packageQueue;

			bool terminate;

			sem_t ready;

			unsigned myRank;

			pthread_t commThread;
		public:    
			OneWayCommMgr(Packagers::PackagerType::Packagers packagerType);
			OneWayCommMgr(Packagers::PackagerType::Packagers packagerType, unsigned myRank, ConnectionType::Type commType);
			virtual ~OneWayCommMgr();
	                                        
			void SendPackage(Packages::IPackage *package);
			Packages::IPackage *GetPackage();
			void Terminate();

		private:
			void Init(Packagers::PackagerType::Packagers packagerType);

			static void *SendThread(void *arg);
			static void *ReceiveThread(void *arg);
		};
	}
}

#endif  //OneWayCommMgr_h