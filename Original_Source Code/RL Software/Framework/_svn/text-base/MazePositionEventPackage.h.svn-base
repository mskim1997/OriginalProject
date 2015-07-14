/////////////////////////////////////////////////////////
//MazePositionEventPackage.h
//Defines the MazePositionEventPackage interface.
//
//Manuel Madera
//7/12/09
/////////////////////////////////////////////////////////

#ifndef MazePositionEventPackage_h
#define MazePositionEventPackage_h

#include "RemoteSubscription.h"
#include "Packages.h"

#define MazePositionEventPackageBufSize	5	//20 bytes

#define IntSize                     sizeof(int)//4

namespace Framework
{ 
	namespace Packages
	{
		/////////////////////////////////////////////////////////
		//MazePositionEventPackage
		/////////////////////////////////////////////////////////
		class FRAMEWORK_API MazePositionEventPackage : public Framework::Eventing::RemoteSubscription
		{
		private:
			IPackage *package;
			bool isResponsible;
			Eventing::SubscriptionTypes::SubscriptionType type;
			Eventing::SubscriptionTypes::SubscriptionLife life;
			unsigned myBuffer[MazePositionEventPackageBufSize];

		public:
			MazePositionEventPackage();
			MazePositionEventPackage(unsigned packageSource, unsigned packageDest, Eventing::SubscriptionTypes::SubscriptionLife subscriptionLife = Eventing::SubscriptionTypes::NOTIFY_ALL);
			MazePositionEventPackage(unsigned packageSource, unsigned packageDest, MazePositionPackage *posPackage, Eventing::SubscriptionTypes::SubscriptionLife subscriptionLife = Eventing::SubscriptionTypes::NOTIFY_ALL);

			virtual ~MazePositionEventPackage();                        
		    
			RemoteSubscription *Clone() const;

			static IPackage *CreatePackage(unsigned source, unsigned dest);

			void SetPositionPackage(MazePositionPackage *posPackage);
			MazePositionPackage *GetPositionPackage() const;
			IPackage *GetPackage();

			Eventing::SubscriptionTypes::SubscriptionType Eventing::ISubscription::GetSubscriptionType();
			Eventing::SubscriptionTypes::SubscriptionLife GetSubscriptionLife();

			void SetSubscriptionLife(Eventing::SubscriptionTypes::SubscriptionLife life);
		protected:
			void Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm);
			void Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm);

			void Serialize(Communications::ICommunicator *comm);
			void Deserialize(Communications::ICommunicator *comm);
		};
	}
}
#endif  //MazePositionEventPackage_h