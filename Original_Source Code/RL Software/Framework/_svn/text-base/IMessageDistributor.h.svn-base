/////////////////////////////////////////////////////////
//IMessageDistributor.h
//Defines the IMessageDistributor interface.
//
//Manuel Madera
//6/26/09
/////////////////////////////////////////////////////////

#ifndef IMessageDistributor_h
#define IMessageDistributor_h

#include "Packages.h"
#include "IAgent.h"
#include "Framework.h"
#include "Position.h"

namespace Framework
{
	namespace Messaging
	{
		typedef void(*MessageCallback)(void *ownerObject, Packages::IPackage *package);

		class FRAMEWORK_API IMessageDistributor
		{
		public:
			virtual ~IMessageDistributor() {}
	                
			virtual void Register(Agents::IAgent *agent, Agents::AgentQueue *agentQ) = 0;
			virtual void Unregister(Agents::IAgent *agent) = 0;

			virtual void RegisterCallback(Packages::PackageType::Package type, void *ownerObject, MessageCallback callback) = 0;
			virtual void UnregisterCallback(Packages::PackageType::Package type) = 0;

			virtual void SendPackage(Packages::IPackage *package) = 0;
			virtual void Broadcast(unsigned senderID, Framework::Packages::MazePositionPackage *mazePosPackage) = 0;

			virtual Position<int> GetOffset() = 0;
		};
	}
}

#endif  //IMessageDistributor_h