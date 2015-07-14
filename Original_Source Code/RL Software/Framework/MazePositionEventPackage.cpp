#include "MazePositionEventPackage.h"
#include "PackagerFactory.h"
#include "Communicator.h"
#include "Debug.h"

#ifdef PrintMemoryLeak
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif
#endif

using namespace Framework::Communications;
using namespace Framework::Factories;
using namespace Framework::Eventing;

namespace Framework
{ 
	namespace Packages
	{
		/////////////////////////////////////////////////////////
		//MazePositionEventPackage
		/////////////////////////////////////////////////////////
		MazePositionEventPackage::MazePositionEventPackage() 
			: RemoteSubscription(0, 0, PackageType::MAZE_POSITION_EVENT), isResponsible(true), package(0), type(Eventing::SubscriptionTypes::MAZE_POSITION_EVENT), life(Eventing::SubscriptionTypes::NOTIFY_ALL)
		{
		}

		MazePositionEventPackage::MazePositionEventPackage(unsigned packageSource, unsigned packageDest, Eventing::SubscriptionTypes::SubscriptionLife subscriptionLife)
			: RemoteSubscription(packageSource, packageDest, PackageType::MAZE_POSITION_EVENT), isResponsible(true), package(0),
			type(Eventing::SubscriptionTypes::MAZE_POSITION_EVENT), life(subscriptionLife)
		{
		}

		MazePositionEventPackage::MazePositionEventPackage(unsigned packageSource, unsigned packageDest, MazePositionPackage *posPackage, Eventing::SubscriptionTypes::SubscriptionLife subscriptionLife)
			: RemoteSubscription(packageSource, packageDest, PackageType::MAZE_POSITION_EVENT), package(posPackage), isResponsible(true), type(Eventing::SubscriptionTypes::MAZE_POSITION_EVENT), life(subscriptionLife)
		{
		}

		MazePositionEventPackage::~MazePositionEventPackage()
		{
			if (this->isResponsible)
			{
				delete package; 
				package = NULL;
			}
		}                        
		    
		RemoteSubscription *MazePositionEventPackage::Clone() const
		{
			MazePositionEventPackage *clone = NULL;
			MazePositionPackage *tempPackage = dynamic_cast<MazePositionPackage *>(const_cast<IPackage *>(package));

			if (!Validator::IsNull(tempPackage, NAME("tempPackage")))
			{
				MazePositionPackage *newPackage = new MazePositionPackage(tempPackage->GetSource(), tempPackage->GetDestination(), tempPackage->GetObject(), tempPackage->GetPosition());

				if (!Validator::IsNull(newPackage, NAME("newPackage")))
				{
					clone = new MazePositionEventPackage(packageSource, packageDest, newPackage, life); 
				}
			}

			return clone;
		}

		/*static*/ IPackage *MazePositionEventPackage::CreatePackage(unsigned source, unsigned dest)
		{
			return new MazePositionEventPackage(source, dest);
		}

		void MazePositionEventPackage::SetPositionPackage(MazePositionPackage *posPackage)
		{
			this->package = posPackage;
		}

		IPackage *MazePositionEventPackage::GetPackage()
		{
			this->isResponsible = false;
			return this->package;
		}

		MazePositionPackage *MazePositionEventPackage::GetPositionPackage() const
		{
			MazePositionPackage *tempPackage = dynamic_cast<MazePositionPackage *>(const_cast<IPackage *>(package));
			MazePositionPackage *newPackage = NULL;

			if (!Validator::IsNull(tempPackage, NAME("tempPackage")))
			{
				newPackage = new MazePositionPackage(tempPackage->GetSource(), tempPackage->GetDestination(), tempPackage->GetObject(), tempPackage->GetPosition());
			}

			return newPackage;
		}
		
		void MazePositionEventPackage::Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			Serialize(comm);

			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->SendPacked(&myBuffer, destRank, messageTag, CommType::Synch);
			}

			if (!Validator::IsNull(this->package, NAME("this->package")))
			{
				this->package->Send(destRank, messageTag, comm);
			}			
		}

		void MazePositionEventPackage::Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			//comm = new Communicator();
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->GetPacked(MazePositionEventPackageBufSize*IntSize, CommType::ASynch, sourceRank, messageTag, &myBuffer);
			}
	        
			Deserialize(comm);

			if (!Validator::IsNull(this->package, NAME("this->package")))
			{
				this->package->Get(sourceRank, messageTag, comm);
			}
		}

		void MazePositionEventPackage::Serialize(Communications::ICommunicator *comm)
		{
			BasePackage::Serialize(&myBuffer, MazePositionEventPackageBufSize*IntSize, comm);

			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Pack(&type, 1, DataType::Int, &myBuffer, MazePositionEventPackageBufSize*IntSize);
				comm->Pack(&life, 1, DataType::Int, &myBuffer, MazePositionEventPackageBufSize*IntSize);
			}
		}

		void MazePositionEventPackage::Deserialize(Communications::ICommunicator *comm)
		{
			BasePackage::Deserialize(&myBuffer, MazePositionEventPackageBufSize*IntSize, comm);

			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Unpack(&myBuffer, MazePositionEventPackageBufSize*IntSize, DataType::Int, 1, &type);
				comm->Unpack(&myBuffer, MazePositionEventPackageBufSize*IntSize, DataType::Int, 1, &life);
			}
	    
			PackagerFactory *packageFactory = PackagerFactory::Instance();

			if (!Validator::IsNull(packageFactory, NAME("packageFactory")))
			{
				package = packageFactory->CreatePackage(PackageType::MAZE_POSITION);
			}
		}

		Eventing::SubscriptionTypes::SubscriptionType MazePositionEventPackage::GetSubscriptionType()
		{
			//this->isResponsible = false;
			return this->type;
		}

		Eventing::SubscriptionTypes::SubscriptionLife MazePositionEventPackage::GetSubscriptionLife()
		{
			//this->isResponsible = false;
			return this->life;
		}

		void MazePositionEventPackage::SetSubscriptionLife(Eventing::SubscriptionTypes::SubscriptionLife life)
		{
			//this->isResponsible = false;
			this->life = life;
		}
	}
}