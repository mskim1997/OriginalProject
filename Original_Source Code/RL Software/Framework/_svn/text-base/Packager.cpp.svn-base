#include "Packager.h"
#ifdef WIN32
#include <mpi.h>
#endif
#include <iostream>
#include "Communicator.h"
#include "Debug.h"

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
		Packager::Packager(int tag) : messageTag(tag), packageTag(0), comm(0)
		{
			comm = new Communicator();
			this->log = LogManager::Instance();
			this->packageFactory = PackagerFactory::Instance();
		}

		Packager::~Packager()
		{
			//comm->CancelASynch();
			delete comm;
			comm = NULL;
			//delete this->pPackage; this->pPackage = NULL;
		}

		void Packager::Terminate()
		{
		}

		void Packager::PackAndSend(IPackage *package)
		{
			if (!Validator::IsNull(package, NAME("package")))
			{
				PackageType::Package packageType = package->GetType();
				unsigned sourceRank = package->GetSource();
				int buffer[BufferSize];
				int bufferSizeInBytes = BufferSize*IntSize;

				//Send what could be called the header of the message.  The source, package tag to be used
				//and the package type so that the correct package instantiated on the destination side.
				if (!Validator::IsNull(comm, NAME("comm")))
				{
					comm->Pack(&sourceRank, 1, DataType::Int, &buffer, bufferSizeInBytes);
					comm->Pack(&packageTag, 1, DataType::Int, &buffer, bufferSizeInBytes);
					comm->Pack(&packageType, 1, DataType::Int, &buffer, bufferSizeInBytes);

					comm->SendPacked(&buffer, package->GetDestination(), messageTag, CommType::Synch);
				}

#ifdef Debug
				if (!Validator::IsNull(log, NAME("log")))
				{
					log->Log(&typeid(this), "Package sent, source: %d, dest: %d, packageTag: %d, packageType: %d", sourceRank, package->GetDestination(), packageTag, packageType);
				}
#endif

				package->Send(packageTag++, comm);
			}
			//NO need to delete the package, since it is being deleted by the commMgr.
		}
	    
		/*static*/ IPackager *Packager::CreatePackager(unsigned tag)
		{
			return new Packager(tag);
		}

		IPackage *Packager::GetAndUnpack()
		{
			IPackage *pPackage = NULL;
			PackageType::Package packageType;
			int buffer[BufferSize];
			unsigned sourceRank = 0;
			int position = 0;
			int bufferSizeInBytes = BufferSize*IntSize;

			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->GetPacked(bufferSizeInBytes, CommType::Synch, &buffer);

				//Strip off the header.
				comm->Unpack(&buffer, bufferSizeInBytes, DataType::Int, 1, &sourceRank);
				comm->Unpack(&buffer, bufferSizeInBytes, DataType::Int, 1, &packageTag);
				comm->Unpack(&buffer, bufferSizeInBytes, DataType::Int, 1, &packageType);

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
			}

			if(!Validator::IsNull(pPackage, NAME("pPackage")))
			{
				#ifdef Debug
				if (!Validator::IsNull(log, NAME("log")))
				{
					log->Log(&typeid(this), "Getting Package ...");
				}
				#endif 

				pPackage->Get(sourceRank, packageTag, comm);
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