/////////////////////////////////////////////////////////
//MLAgentEvents.h
//Event raised when an agent has reached its goal.
//
//Manuel Madera
//10/24/09
/////////////////////////////////////////////////////////

#ifndef MLAgentEvents_h
#define MLAgentEvents_h

#include "IEventDistributor.h"
#include "Packages.h"
#include "ISubscription.h"
#include "RemoteSubscription.h"
#include "ICommunicator.h"

#define MAX_VAL (float)10000

#define AgentDoneEventPackageBufSize		13	//52 bytes
#define PhysicalPathDoneEventPackageBufSize	11

using namespace std;

namespace Framework
{
	namespace Eventing
	{
		//AgentDone Subscription
		class FRAMEWORK_API AgentDoneEventPackage : public Framework::Eventing::RemoteSubscription
		{
		private:
			unsigned agentID;
			bool isTransfered;
			bool isGoalFound;
			int totalNumberOfSteps;
			unsigned transferId;
			double minRadius;
			Framework::Position<int> pos;
			Framework::Eventing::SubscriptionTypes::SubscriptionType type;
			Framework::Eventing::SubscriptionTypes::SubscriptionLife life;
			double myBuffer[AgentDoneEventPackageBufSize];

		public:
			AgentDoneEventPackage();
			AgentDoneEventPackage(unsigned packageSource, unsigned packageDest, unsigned id = 0, int xPos = 0, int yPos = 0, int numSteps = 0, bool transfered = false, bool hasFoundGoal = false, float minRadius = MAX_VAL, unsigned transferID = 0);

			virtual ~AgentDoneEventPackage();

			Framework::Eventing::SubscriptionTypes::SubscriptionType GetSubscriptionType();

			Framework::Eventing::SubscriptionTypes::SubscriptionLife GetSubscriptionLife();

			void SetSubscriptionLife(Framework::Eventing::SubscriptionTypes::SubscriptionLife life);

			float GetMinRadius() const;

			unsigned GetAgentId() const;

			int GetTotalNumberOfSteps() const;

			bool IsTransfered() const;

			bool HasFoundGoal() const;

			int GetXPosition() const;

			unsigned GetTransferID() const;

			int GetYPosition() const;

			RemoteSubscription *Clone() const;

			static IPackage *CreatePackage(unsigned source, unsigned dest);

		protected:
			void Send(unsigned destRank, unsigned messageTag, Framework::Communications::ICommunicator *comm);
			void Get(unsigned sourceRank, unsigned messageTag, Framework::Communications::ICommunicator *comm);

			void Serialize(Framework::Communications::ICommunicator *comm);
			void Deserialize(Framework::Communications::ICommunicator *comm);
		};

		typedef struct 
		{
			unsigned agentId;
			char start;
			char goal;
			unsigned pathNum;
			double pathTime;	
			unsigned numSteps;
		} PhysicalPathEventInfo;

		//AgentDone Subscription
		class FRAMEWORK_API PhysicalPathDoneEventPackage : public Framework::Eventing::RemoteSubscription
		{
		private:
			PhysicalPathEventInfo info;
			Framework::Eventing::SubscriptionTypes::SubscriptionType type;
			Framework::Eventing::SubscriptionTypes::SubscriptionLife life;
			double myBuffer[PhysicalPathDoneEventPackageBufSize];

		public:
			PhysicalPathDoneEventPackage();
			PhysicalPathDoneEventPackage(unsigned packageSource, unsigned packageDest);
			PhysicalPathDoneEventPackage(unsigned packageSource, unsigned packageDest, PhysicalPathEventInfo agentInfo);

			virtual ~PhysicalPathDoneEventPackage();

			Framework::Eventing::SubscriptionTypes::SubscriptionType GetSubscriptionType();

			Framework::Eventing::SubscriptionTypes::SubscriptionLife GetSubscriptionLife();

			void SetSubscriptionLife(Framework::Eventing::SubscriptionTypes::SubscriptionLife life);

			PhysicalPathEventInfo GetAgentInfo() const;

			RemoteSubscription *Clone() const;

			static IPackage *CreatePackage(unsigned source, unsigned dest);

		protected:
			void Send(unsigned destRank, unsigned messageTag, Framework::Communications::ICommunicator *comm);
			void Get(unsigned sourceRank, unsigned messageTag, Framework::Communications::ICommunicator *comm);

			void Serialize(Framework::Communications::ICommunicator *comm);
			void Deserialize(Framework::Communications::ICommunicator *comm);
		};
	}
}

#endif	//MLAgentEvents_h