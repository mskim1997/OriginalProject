/////////////////////////////////////////////////////////
//PackagerFactory.cpp
//Creates and returns package related objects.
//
//Manuel Madera
//6/26/09
/////////////////////////////////////////////////////////

#include "PackagerFactory.h"
#include "Agent.h"
#include "Packager.h"
#include "ASynchPackager.h"
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
using namespace Framework::Agents;

namespace Framework
{
	namespace Factories
	{
		PackagerFactory *PackagerFactory::instance = 0;

		/*static*/ PackagerFactory *PackagerFactory::Instance()
		{
			if (instance == 0)
			{
				instance = new PackagerFactory();
			}

			return instance;
		}

		PackagerFactory::PackagerFactory()
		{
			sem_init(&binSem, 0, 1);
		}

		PackagerFactory::~PackagerFactory()
		{
			//sem_wait(&this->binSem);

			sem_destroy(&this->binSem);
			this->binSem = NULL;
		}

		void PackagerFactory::Shutdown()
		{
			delete instance; 
			instance = NULL;
		}

		void PackagerFactory::RegisterPackage(Packages::PackageType::Package newPackageType, PackageFactoryMethod newFactoryMethod)
		{
			sem_wait(&this->binSem);
			if (this->packageFactories.find(newPackageType) == this->packageFactories.end())
			{
				this->packageFactories[newPackageType] = newFactoryMethod;
			}
			sem_post(&this->binSem);
		}

		void PackagerFactory::RegisterPackager(Packagers::PackagerType::Packagers newPackagerType, PackagerFactoryMethod newFactoryMethod)
		{
			sem_wait(&this->binSem);
			if (this->packagerFactories.find(newPackagerType) == this->packagerFactories.end())
			{
				this->packagerFactories[newPackagerType] = newFactoryMethod;
			}
			sem_post(&this->binSem);
		}

		/*static*/ IPackager *PackagerFactory::CreatePackager(PackagerType::Packagers packagerType)
		{
			IPackager *packager = 0;

			if (this->binSem != NULL)
			{
				sem_wait(&this->binSem);
				if (this->packagerFactories.find(packagerType) != this->packagerFactories.end())
				{
					packager = this->packagerFactories[packagerType](0);
				}
				sem_post(&this->binSem);
			}

			return packager;
		}

		/*static*/ IPackager *PackagerFactory::CreatePackager(PackagerType::Packagers packagerType, unsigned tag)
		{
			IPackager *packager = 0;

			if (this->binSem != NULL)
			{
				sem_wait(&this->binSem);
				if (this->packagerFactories.find(packagerType) != this->packagerFactories.end())
				{
					packager = this->packagerFactories[packagerType](tag);
				}
				sem_post(&this->binSem);
			}

			return packager;
		}

		/*static*/ IPackage *PackagerFactory::CreatePackage(PackageType::Package packageType)
		{
			IPackage *package = 0;

			if (this->binSem != NULL)
			{
				sem_wait(&this->binSem);
				if (this->packageFactories.find(packageType) != this->packageFactories.end())
				{
					package = this->packageFactories[packageType](0, 0);
				}
				sem_post(&this->binSem);
			}

			return package;
		}

		/*static*/ IPackage *PackagerFactory::CreatePackage(PackageType::Package packageType, unsigned packageSource, unsigned packageDest)
		{
			IPackage *package = 0;

			if (this->binSem != NULL)
			{
				sem_wait(&this->binSem);
				if (this->packageFactories.find(packageType) != this->packageFactories.end())
				{
					package = this->packageFactories[packageType](packageSource, packageDest);
				}
				sem_post(&this->binSem);
			}

			return package;
		}
	}
}