#include "OneWayCommMgr.h"
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
		OneWayCommMgr::OneWayCommMgr(PackagerType::Packagers packagerType) : myRank(0), terminate(false), commType(ConnectionType::Send), pPackager(0)
		{
			Init(packagerType);
		}

		OneWayCommMgr::OneWayCommMgr(PackagerType::Packagers packagerType, unsigned myRank, ConnectionType::Type commType) : terminate(false), myRank(myRank), commType(commType), pPackager(0)
		{
			Init(packagerType);
		}

		void OneWayCommMgr::Init(PackagerType::Packagers packagerType)
		{
			PackagerFactory *packageFactory = PackagerFactory::Instance();

			if (!Validator::IsNull(packageFactory, NAME("packageFactory")))
			{
				pPackager = packageFactory->CreatePackager(packagerType, 400);
			}

			sem_init(&ready, 0, 0);

			int rc;
	        
			if (commType == ConnectionType::Send)
			{
				rc = pthread_create(&this->commThread, NULL, OneWayCommMgr::SendThread, (void *)this);
			}
			else
			{
				rc = pthread_create(&this->commThread, NULL, OneWayCommMgr::ReceiveThread, (void *)this);
			}                
		}

		OneWayCommMgr::~OneWayCommMgr()
		{
			Terminate();

			pthread_join(this->commThread, NULL);

			//sem_wait(&this->ready);

			sem_destroy(&this->ready);

			delete this->pPackager; 
			this->pPackager = NULL;
		}

		void OneWayCommMgr::SendPackage(IPackage *package)
		{
			if (commType == ConnectionType::Send)
			{
				this->packageQueue.push_back(package);
				sem_post(&this->ready);
			}
		}
	    
		IPackage *OneWayCommMgr::GetPackage()
		{
			IPackage *package = NULL;

			if (commType == ConnectionType::Recv)
			{
				sem_wait(&this->ready);        

				if(this->packageQueue.size() > 0)
				{
					package = this->packageQueue.front();
					this->packageQueue.pop_front();
				}
			}

			return package;
		}

		void OneWayCommMgr::Terminate()
		{
			this->terminate = true;

			if (!Validator::IsNull(this->pPackager, NAME("this->pPackager")))
			{
				this->pPackager->Terminate();
			}

			if (commType == ConnectionType::Send)
			{            
				sem_post(&this->ready);
			}
		}

		/*static*/ void *OneWayCommMgr::SendThread(void *arg)
		{
			OneWayCommMgr *pThis = static_cast<OneWayCommMgr *>(arg);
			
			if (!Validator::IsNull(pThis, NAME("pThis")))
			{
				IPackage *package = NULL;

				while(!pThis->terminate)
				{
					sem_wait(&pThis->ready);

					if(pThis->packageQueue.size() > 0 )
					{
						package = pThis->packageQueue.front();
						pThis->packageQueue.pop_front();

						if (!Validator::IsNull(pThis->pPackager, NAME("pThis->pPackager")))
						{
							pThis->pPackager->PackAndSend(package);
						}
					}
				}

				sem_post(&pThis->ready);
			}

			#ifdef LINUX
				pthread_exit(NULL);
			#endif

	#ifdef WIN32
			return NULL;
	#endif
		}

		/*static*/ void *OneWayCommMgr::ReceiveThread(void *arg)
		{
			OneWayCommMgr *pThis = static_cast<OneWayCommMgr *>(arg);

			if (!Validator::IsNull(pThis, NAME("pThis")))
			{
				while(!pThis->terminate)
				{            
					if (!Validator::IsNull(pThis->pPackager, NAME("pThis->pPackager")))
					{
						pThis->packageQueue.push_back(pThis->pPackager->GetAndUnpack());
					}

					sem_post(&pThis->ready);
				}

				while (pThis->packageQueue.size() > 0)
				{
					IPackage *package = pThis->packageQueue.front();
					pThis->packageQueue.pop_front();

					delete package;
					package = NULL;
				}

				sem_post(&pThis->ready);
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