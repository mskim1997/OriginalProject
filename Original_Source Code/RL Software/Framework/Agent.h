/////////////////////////////////////////////////////////
//Agent.h
//Defines the Agent class.
//
//Manuel Madera
//7/5/09
/////////////////////////////////////////////////////////

#ifndef Agent_h
#define Agent_h

#include "IAgent.h"
#include "IMessageDistributor.h"
#include "Framework.h"

namespace Framework
{
	namespace Agents
	{
		class FRAMEWORK_API Agent : public IAgent
		{
		protected:
			unsigned id;
			bool isRegistered;
			bool isTransfered;
			AgentQueue myMessageQueue;
			Messaging::IMessageDistributor *msgDist;

		public:
			Agent();
			Agent(unsigned packageNode, unsigned packageDest);

			virtual ~Agent();

			virtual void Run(bool isTransfer);

			//Base methods
			bool IsRegistered();
			bool IsTransfered();

			//Register with the local MessageDistributor, which in turn will register globally
			void Register();
			void RegisterTransfer(unsigned id);
			void SetAgentUI(unsigned id);
			unsigned GetAgentUI();

		protected:
			virtual void Init();

			virtual void SendPackage(Packages::IPackage *package);
		};
	}
}

#endif	//Agent_h