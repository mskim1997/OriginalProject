/////////////////////////////////////////////////////////
//PackagerFactory.h
//Creates and returns packager related object.
//
//Manuel Madera
//6/26/09
/////////////////////////////////////////////////////////

#ifndef PackagerFactory_h
#define PackagerFactory_h

#include "IPackager.h"
#include "Packages.h"
#include "Framework.h"
#include <semaphore.h>
#include <map>

using namespace std;

namespace Framework
{
	namespace Factories
	{
		typedef Packagers::IPackager *(*PackagerFactoryMethod)(unsigned tag);
		typedef Packages::IPackage *(*PackageFactoryMethod)(unsigned packageSource, unsigned packageDest);

		class FRAMEWORK_API PackagerFactory
		{
		private:
			sem_t binSem;
			map<Packages::PackageType::Package, PackageFactoryMethod> packageFactories;
			map<Packagers::PackagerType::Packagers, PackagerFactoryMethod> packagerFactories;
			static PackagerFactory *instance;
			PackagerFactory();

		public:
			virtual ~PackagerFactory();
	        
			//No need to be thread-safe since it is called once
			//at the beginning of each node.
			static PackagerFactory *Instance();

			//This method can only be called once
			void Shutdown();

			void RegisterPackage(Packages::PackageType::Package newPackageType, PackageFactoryMethod newFactoryMethod);
			void RegisterPackager(Packagers::PackagerType::Packagers newPackagerType, PackagerFactoryMethod newFactoryMethod);

			Packagers::IPackager *CreatePackager(Packagers::PackagerType::Packagers packagerType);
			Packagers::IPackager *CreatePackager(Packagers::PackagerType::Packagers packagerType, unsigned tag);

			Packages::IPackage *CreatePackage(Packages::PackageType::Package packageType);
			Packages::IPackage *CreatePackage(Packages::PackageType::Package packageType, unsigned packageSource, unsigned packageDest);
		};
	}
}

#endif  //PackagerFactory_h