/////////////////////////////////////////////////////////
//ASynchPackager.cpp
//Responsible for packing/unpacking packages asynchronously.
//
//Manuel Madera
//6/27/09
/////////////////////////////////////////////////////////

#include "ASynchPackager.h"
#include "PackagerFactory.h"
#include "Communicator.h"
#include <iostream>
#include "Debug.h"
#include "SerialCommunicator.h"

#ifdef PrintMemoryLeak
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif
#endif

using namespace std;
using namespace Framework::Communications;
using namespace Framework::Packages;
using namespace Framework::Factories;
using namespace Framework::Logging;

#define BufferSize  3
//#define IntSize     4

namespace Framework
{       
	namespace Packagers
	{
		ASynchPackager::ASynchPackager(int tag) : messageTag(0), packageTag(tag), comm(NULL), packageFactory(NULL)
		{
			sem_init(&binSem, 0, 1);
			//comm = new SerialCommunicator();
			comm = new Communicator();
			this->packageFactory = PackagerFactory::Instance();
			this->log = LogManager::Instance();
		}

		ASynchPackager::~ASynchPackager()
		{
			sem_destroy(&this->binSem);
			//comm->CancelASynch();

			delete comm; 
			comm = NULL;
			//delete this->pPackage; this->pPackage = NULL;
		}

		void ASynchPackager::PackAndSend(IPackage *package)
		{
			if (!Validator::IsNull(package, NAME("package")))
			{
				PackageType::Package packageType = package->GetType();
				unsigned tempTag;
				unsigned sourceRank = package->GetSource();
				unsigned buffer[BufferSize];
				int bufferSizeInBytes = BufferSize*IntSize;

				//tempTag = packageTag++;
				if (packageType == PackageType::MESSAGE)
				{
					tempTag = packageTag + static_cast<unsigned>(static_cast<MessagePackage *>(package)->GetInnerPackageType());
				}
				else
				{
					tempTag = packageTag + static_cast<unsigned>(packageType);
				}

#ifdef Debug
				if (!Validator::IsNull(log, NAME("log")))
				{
					log->Log(&typeid(this), "Packing ...");
				}
#endif

				//Send what could be called the header of the message.  The source, package tag to be used
				//and the package type so that the correct package is instantiated on the destination side.
				if (!Validator::IsNull(comm, NAME("comm")))
				{
					comm->Pack(&sourceRank, 1, DataType::Int, &buffer, bufferSizeInBytes);
					comm->Pack(&tempTag, 1, DataType::Int, &buffer, bufferSizeInBytes);
					comm->Pack(&packageType, 1, DataType::Int, &buffer, bufferSizeInBytes);

					comm->SendPacked(&buffer, package->GetDestination(), tempTag, CommType::Synch);
				}

#ifdef Debug
				if (!Validator::IsNull(log, NAME("log")))
				{
					log->Log(&typeid(this), "Package sent, source: %d, dest: %d, packageTag: %d, packageType: %d", sourceRank, package->GetDestination(), packageTag, packageType);
				}
#endif

				package->Send(tempTag, comm);
			}

			//NO need to delete the package, since it is being deleted by the commMgr.
		}
	    
		void ASynchPackager::Terminate()
		{
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->CancelASynch();
			}
		}

		/*static*/ IPackager *ASynchPackager::CreatePackager(unsigned tag)
		{
			return new ASynchPackager(tag);
		}

		IPackage *ASynchPackager::GetAndUnpack()
		{
			IPackage *pPackage = NULL;
			PackageType::Package packageType;
			bool flag;
			unsigned buffer[BufferSize];
			unsigned sourceRank = 0;
			int bufferSizeInBytes = BufferSize*IntSize;

			if (!Validator::IsNull(comm, NAME("comm")))
			{
				flag = comm->GetPacked(bufferSizeInBytes, CommType::ASynch, &buffer);
			}

			if (flag)
			{
				return pPackage;
			}

#ifdef Debug
			if (!Validator::IsNull(log, NAME("log")))
			{
				log->Log(&typeid(this), "Unpacking ...");
			}
#endif

			//Strip off the header.
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Unpack(&buffer, bufferSizeInBytes, DataType::Int, 1, &sourceRank);
				comm->Unpack(&buffer, bufferSizeInBytes, DataType::Int, 1, &messageTag);
				comm->Unpack(&buffer, bufferSizeInBytes, DataType::Int, 1, &packageType);
			}

#ifdef Debug
			if (!Validator::IsNull(log, NAME("log")))
			{
				log->Log(&typeid(this), "Package received, source: %d, packageTag: %d, packageType: %d", sourceRank, messageTag, packageType);
				log->Log(&typeid(this), "Creating package ...");
			}
#endif

			if (!Validator::IsNull(packageFactory, NAME("packageFactory")))
			{
				pPackage = packageFactory->CreatePackage(packageType);
			}

			if(!Validator::IsNull(pPackage, NAME("pPackage")))
			{
#ifdef Debug
				if (!Validator::IsNull(log, NAME("log")))
				{
					log->Log(&typeid(this), "Getting Package ...");
				}
#endif            

				pPackage->Get(sourceRank, messageTag, comm);
			}

#ifdef Debug
			if (!Validator::IsNull(log, NAME("log")))
			{
				log->Log(&typeid(this), "Returning package...");
			}
#endif		

			return pPackage;
		}
	}
}