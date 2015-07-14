/////////////////////////////////////////////////////////
//MessageDistributor.h
//Defines the MessageDistributor, responsible for 
//distributing messages between the agents.
//
//Manuel Madera
//6/26/09
/////////////////////////////////////////////////////////

#ifndef MessageDistributor_h
#define MessageDistributor_h

#include "IMessageDistributor.h"
#include <semaphore.h>
#include <pthread.h>
#include <map>
#include "ICommMgr.h"
#include "IEventDistributor.h"
#include "Framework.h"
#include "Publisher.h"
#include "MazeReceivedSubscription.h"
#include "Packages.h"
#include "MazePositionEventPackage.h"
#include "LogManager.h"

using namespace std;

namespace Framework
{
	namespace Messaging
	{
		//RegistrationChange Subscription
		class FRAMEWORK_API RegistrationChangeSubscription : public Eventing::ISubscription
		{
		private:
			Packages::PackageType::Package value;
			Packages::IPackage *data;
			Eventing::SubscriptionTypes::SubscriptionType type;
			Eventing::SubscriptionTypes::SubscriptionLife life;

		public:
			RegistrationChangeSubscription(Packages::PackageType::Package newVal, Packages::IPackage *newData) 
				: value(newVal), type(Eventing::SubscriptionTypes::REGISTRATION_CHANGE), life(Eventing::SubscriptionTypes::NOTIFY_ONCE), data(newData)
			{
			}

			virtual ~RegistrationChangeSubscription() {}

			Eventing::SubscriptionTypes::SubscriptionType GetSubscriptionType()
			{
				return this->type;
			}

			Eventing::SubscriptionTypes::SubscriptionLife GetSubscriptionLife()
			{
				return this->life;
			}

			void SetSubscriptionLife(Eventing::SubscriptionTypes::SubscriptionLife life)
			{
				this->life = life;
			}

			Packages::IPackage *GetData() const
			{ 
				return this->data; 
			}

			Packages::PackageType::Package GetValue() const
			{
				return this->value;
			}

		};

		//Message distributor singleton.
		class FRAMEWORK_API MessageDistributor : public IMessageDistributor, public Eventing::ISubscriber
		{
		private:
			typedef struct
			{
				void *ownerObject;
				MessageCallback callback;
			} Callback;

			map<unsigned, Agents::AgentQueue *> agentsManaged;
			map<Packages::PackageType::Package, Callback> callbackMap;
			static IMessageDistributor *instance;
			Communications::ICommMgr *pMyCommMgr;
			sem_t binSem;
			sem_t callbackSem;
			unsigned rank;
			bool isInitialized;
			bool terminate;

			sem_t registrationSem;
			sem_t confirmationSem;

			Agents::AgentQueue *registrationQueue;
			Agents::AgentQueue *confirmationQueue;

			Position<int> offset;

			Eventing::IPublisher *regChangePublisher;
			Eventing::IPublisher *mazeRecvPublisher;
			Eventing::IPublisher *mazePositionPublisher;

			pthread_t recvThread;

			Eventing::IEventDistributor *eventDist;

			bool shareAgentInfo;

			Logging::ILog *log;
			MessageDistributor();

		public:
			//This method can only be called once
			void Init(unsigned rank, bool allowSharing);

			//No need to be thread-safe since it is called once
			//at the beginning of each node.
			static IMessageDistributor *Instance();

			virtual ~MessageDistributor();
	                
			//This method can only be called once
			void Shutdown();

			void Register(Agents::IAgent *agent, Agents::AgentQueue *agentQ);
			void Unregister(Agents::IAgent *agent);

			void RegisterCallback(Packages::PackageType::Package type, void *ownerObject, MessageCallback callback);
			void UnregisterCallback(Packages::PackageType::Package type);

			void SendPackage(Packages::IPackage *package);

			void Notify(const Eventing::ISubscription *subscription);

			void Broadcast(unsigned senderID, Framework::Packages::MazePositionPackage *mazePosPackage);

			Framework::Position<int> GetOffset();
		private:
			void Send(Packages::IPackage *package);

			static void DistributorCallback(void *ownerObject, Packages::IPackage *package);
			static void MazePackageCallback(void *ownerObject, Packages::IPackage *package);
			static void AgentPackageCallback(void *ownerObject, Packages::IPackage *package);

			static void *ReceiveThread(void *arg);
		};
	}
}

#endif  //IAgentMgr_h