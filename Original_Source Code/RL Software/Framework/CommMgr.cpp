#include "CommMgr.h"
#include "PackagerFactory.h"
#include "Debug.h"

#ifdef PrintMemoryLeak
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif
#endif

using namespace Framework::Packagers;
using namespace Framework::Packages;
using namespace Framework::Factories;

namespace Framework
{       
	namespace Communications
	{
		CommMgr::CommMgr(PackagerType::Packagers packagerType) : myRank(0), terminate(false), pPackager(0)
		{
			Init(packagerType);
		}

		CommMgr::CommMgr(PackagerType::Packagers packagerType, unsigned myRank) : terminate(false), myRank(myRank), pPackager(0)
		{
			Init(packagerType);
		}

		void CommMgr::Init(PackagerType::Packagers packagerType)
		{
			PackagerFactory *packageFactory = PackagerFactory::Instance();

			if (!Validator::IsNull(packageFactory, NAME("packageFactory")))
			{
				pPackager = packageFactory->CreatePackager(packagerType, 100 * this->myRank);
			}

			sem_init(&sendQReady, 0, 0);
			sem_init(&recvQReady, 0, 0);
			sem_init(&recBinSem, 0, 1);
			sem_init(&sendBinSem, 0, 1);

			int rc1, rc2;	

			rc2 = pthread_create(&this->recvThread, NULL, CommMgr::ReceiveThread, (void *)this);
			rc1 = pthread_create(&this->sendThread, NULL, CommMgr::SendThread, (void *)this);         
		}

		CommMgr::~CommMgr()
		{
			Terminate();

			pthread_join(this->sendThread, NULL);
			sem_destroy(&this->sendQReady);
			sem_destroy(&this->sendBinSem);			

			pthread_cancel(this->recvThread);

			//pthread_join(this->recvThread, NULL);			
			sem_destroy(&this->recvQReady);
			sem_destroy(&this->recBinSem);		

			//pthread_join(this->recvThread, NULL);
			delete this->pPackager; 
			this->pPackager = NULL;
		}

		void CommMgr::SendPackage(IPackage *package)
		{
			sem_wait(&this->sendBinSem);
			this->sendQueue.push_back(package);
			sem_post(&this->sendBinSem);

			sem_post(&this->sendQReady);
		}
	    
		IPackage *CommMgr::GetPackage()
		{
			sem_wait(&this->recvQReady);
			IPackage *package = NULL;

			while ((!this->terminate)&&(package == NULL))
			{
				sem_wait(&this->recBinSem);
				if(this->recvQueue.size() > 0)
				{			
					package = this->recvQueue.front();
					this->recvQueue.pop_front();			
				}
				sem_post(&this->recBinSem);
			}

			return package;
		}

		void CommMgr::Terminate()
		{
			this->terminate = true;

			if (!Validator::IsNull(this->pPackager, NAME("pPackager")))
			{
				this->pPackager->Terminate();
			}

			sem_post(&this->sendQReady);
		}

		/*static*/ void *CommMgr::SendThread(void *arg)
		{
			CommMgr *pThis = static_cast<CommMgr *>(arg);

			if (!Validator::IsNull(pThis, NAME("pThis")))
			{
				IPackage *package = NULL;

				while(!pThis->terminate)
				{
					sem_wait(&pThis->sendQReady);

					sem_wait(&pThis->sendBinSem);
					if(pThis->sendQueue.size() > 0 )
					{
						package = pThis->sendQueue.front();
						pThis->sendQueue.pop_front();

						if (!Validator::IsNull(pThis->pPackager, NAME("pThis->pPackager")))
						{
							pThis->pPackager->PackAndSend(package);
						}

						delete package; 
						package = NULL;
					}
					sem_post(&pThis->sendBinSem);
				}

				sem_post(&pThis->sendQReady);
			}

			#ifdef LINUX
				pthread_exit(NULL);
			#endif

			#ifdef WIN32
				return NULL;
			#endif
		}

		/*static*/ void *CommMgr::ReceiveThread(void *arg)
		{
			CommMgr *pThis = static_cast<CommMgr *>(arg);

			if (!Validator::IsNull(pThis, NAME("pThis")))
			{
				IPackage *tempPackage = 0;

				while(!pThis->terminate)
				{             
					if (!Validator::IsNull(pThis->pPackager, NAME("pThis->pPackager")))
					{
						tempPackage = pThis->pPackager->GetAndUnpack();
					}

					sem_wait(&pThis->recBinSem);
					if (!Validator::IsNull(tempPackage, NAME("tempPackage")))
					{
						pThis->recvQueue.push_back(tempPackage);
					}
					sem_post(&pThis->recBinSem);

					if (!Validator::IsNull(tempPackage, NAME("tempPackage")))
					{
						sem_post(&pThis->recvQReady);
					}
				}

				sem_wait(&pThis->recBinSem);
				while (pThis->recvQueue.size() > 0)
				{
					IPackage *package = pThis->recvQueue.front();
					pThis->recvQueue.pop_front();

					delete package; 
					package = NULL;
				}
				sem_post(&pThis->recBinSem);

				sem_post(&pThis->recvQReady);
			}

			#ifdef LINUX
				pthread_exit(NULL);
			#endif

			#ifdef WIN32
					return NULL;
			#endif
		}
	}
}