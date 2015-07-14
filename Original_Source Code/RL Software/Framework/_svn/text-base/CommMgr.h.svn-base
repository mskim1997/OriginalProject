/////////////////////////////////////////////////////////
//CommMgr.h
//Responsible for handling the communication between nodes.
//
//Manuel Madera
//5/8/09
/////////////////////////////////////////////////////////

#ifndef CommMgr_h
#define CommMgr_h

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
		class FRAMEWORK_API CommMgr : public ICommMgr
		{
		private:
			Packagers::IPackager *pPackager;
			list<Packages::IPackage *> sendQueue;
			list<Packages::IPackage *> recvQueue;
			bool terminate;
			sem_t sendQReady;
			sem_t recvQReady;
			sem_t recBinSem;
			sem_t sendBinSem;
			unsigned myRank;

			pthread_t sendThread;
			pthread_t recvThread;
		public:    
			CommMgr(Packagers::PackagerType::Packagers packagerType);
			CommMgr(Packagers::PackagerType::Packagers packagerType, unsigned myRank);
			virtual ~CommMgr();
	                                        
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

#endif  //CommMgr_h