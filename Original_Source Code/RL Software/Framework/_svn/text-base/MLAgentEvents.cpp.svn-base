/////////////////////////////////////////////////////////
//MLAgentEvents.h
//Event raised when an agent has reached its goal.
//
//Manuel Madera
//10/24/09
/////////////////////////////////////////////////////////
#include "MLAgentEvents.h"
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
using namespace Framework::Packages;
using namespace Framework::Eventing;

namespace Framework
{
	namespace Eventing
	{
		AgentDoneEventPackage::AgentDoneEventPackage()
			: RemoteSubscription(0, 0, PackageType::AGENT_DONE_EVENT), type(Framework::Eventing::SubscriptionTypes::AGENT_DONE), life(Framework::Eventing::SubscriptionTypes::NOTIFY_ALL), agentID(0), isTransfered(false), totalNumberOfSteps(0), transferId(0)
		{
		}

		AgentDoneEventPackage::AgentDoneEventPackage(unsigned packageSource, unsigned packageDest, unsigned id, int xPos, int yPos, int numSteps, bool transfered, bool hasFoundGoal, float minRadius, unsigned transferID) 
			: RemoteSubscription(packageSource, packageDest, PackageType::AGENT_DONE_EVENT), type(Framework::Eventing::SubscriptionTypes::AGENT_DONE), life(Framework::Eventing::SubscriptionTypes::NOTIFY_ALL), transferId(transferID), agentID(id), isTransfered(transfered), isGoalFound(hasFoundGoal), totalNumberOfSteps(numSteps), pos(Position<int>(xPos, yPos)), minRadius(minRadius)
		{
		}

		AgentDoneEventPackage::~AgentDoneEventPackage()
		{
		}

		Framework::Eventing::SubscriptionTypes::SubscriptionType AgentDoneEventPackage::GetSubscriptionType()
		{
			return this->type;
		}

		Framework::Eventing::SubscriptionTypes::SubscriptionLife AgentDoneEventPackage::GetSubscriptionLife()
		{
			return this->life;
		}

		void AgentDoneEventPackage::SetSubscriptionLife(Framework::Eventing::SubscriptionTypes::SubscriptionLife life)
		{
			this->life = life;
		}

		int AgentDoneEventPackage::GetTotalNumberOfSteps() const
		{
			return this->totalNumberOfSteps;
		}

		float AgentDoneEventPackage::GetMinRadius() const
		{ 
			return this->minRadius; 
		}

		unsigned AgentDoneEventPackage::GetAgentId() const
		{ 
			return this->agentID; 
		}

		bool AgentDoneEventPackage::IsTransfered() const
		{
			return this->isTransfered;
		}

		bool AgentDoneEventPackage::HasFoundGoal() const
		{
			return this->isGoalFound;
		}

		int AgentDoneEventPackage::GetXPosition() const
		{
			return this->pos.GetXCoor();
		}

		unsigned AgentDoneEventPackage::GetTransferID() const
		{
			return this->transferId;
		}

		int AgentDoneEventPackage::GetYPosition() const
		{
			return this->pos.GetYCoor();
		}

		RemoteSubscription *AgentDoneEventPackage::Clone() const
		{
			return new AgentDoneEventPackage(this->packageSource, this->packageDest, this->agentID, this->pos.GetXCoor(), this->pos.GetYCoor(), this->totalNumberOfSteps, this->isTransfered, this->isGoalFound, this->minRadius, this->transferId);
		}

		/*static*/ IPackage *AgentDoneEventPackage::CreatePackage(unsigned source, unsigned dest)
		{
			return new AgentDoneEventPackage(source, dest);
		}

		void AgentDoneEventPackage::Send(unsigned destRank, unsigned messageTag, ICommunicator *comm)
		{
			Serialize(comm);

			if (comm != NULL)
			{
				comm->SendPacked(&myBuffer, destRank, messageTag, CommType::Synch);			
			}
		}

		void AgentDoneEventPackage::Get(unsigned sourceRank, unsigned messageTag, ICommunicator *comm)
		{
			//comm = new Communicator();
			if (comm != NULL)
			{
				comm->GetPacked(AgentDoneEventPackageBufSize*DoubleSize, CommType::ASynch, sourceRank, messageTag, &myBuffer);
			}

			Deserialize(comm);
		}

		void AgentDoneEventPackage::Serialize(ICommunicator *comm)
		{
			BasePackage::Serialize(&myBuffer, AgentDoneEventPackageBufSize*DoubleSize, comm);

			int xPos = this->pos.GetXCoor();
			int yPos = this->pos.GetYCoor();

			if (comm != NULL)
			{
				comm->Pack(&agentID, 1, DataType::Int, &myBuffer, AgentDoneEventPackageBufSize*DoubleSize);
				comm->Pack(&type, 1, DataType::Int, &myBuffer, AgentDoneEventPackageBufSize*DoubleSize);
				comm->Pack(&life, 1, DataType::Int, &myBuffer, AgentDoneEventPackageBufSize*DoubleSize);
				comm->Pack(&isTransfered, 1, DataType::Int, &myBuffer, AgentDoneEventPackageBufSize*DoubleSize);
				comm->Pack(&isGoalFound, 1, DataType::Int, &myBuffer, AgentDoneEventPackageBufSize*DoubleSize);
				comm->Pack(&totalNumberOfSteps, 1, DataType::Int, &myBuffer, AgentDoneEventPackageBufSize*DoubleSize);
				comm->Pack(&xPos, 1, DataType::Int, &myBuffer, AgentDoneEventPackageBufSize*DoubleSize);
				comm->Pack(&yPos, 1, DataType::Int, &myBuffer, AgentDoneEventPackageBufSize*DoubleSize);
				comm->Pack(&minRadius, 1, DataType::DOUBLE, &myBuffer, AgentDoneEventPackageBufSize*DoubleSize);
				comm->Pack(&transferId, 1, DataType::Int, &myBuffer, AgentDoneEventPackageBufSize*DoubleSize);
			}
		}

		void AgentDoneEventPackage::Deserialize(ICommunicator *comm)
		{
			BasePackage::Deserialize(&myBuffer, AgentDoneEventPackageBufSize*DoubleSize, comm);

			int xPos = 0;
			int yPos = 0;

			if (comm != NULL)
			{
				comm->Unpack(&myBuffer, AgentDoneEventPackageBufSize*DoubleSize, DataType::Int, 1, &agentID);
				comm->Unpack(&myBuffer, AgentDoneEventPackageBufSize*DoubleSize, DataType::Int, 1, &type);
				comm->Unpack(&myBuffer, AgentDoneEventPackageBufSize*DoubleSize, DataType::Int, 1, &life);
				comm->Unpack(&myBuffer, AgentDoneEventPackageBufSize*DoubleSize, DataType::Int, 1, &isTransfered);
				comm->Unpack(&myBuffer, AgentDoneEventPackageBufSize*DoubleSize, DataType::Int, 1, &isGoalFound);
				comm->Unpack(&myBuffer, AgentDoneEventPackageBufSize*DoubleSize, DataType::Int, 1, &totalNumberOfSteps);
				comm->Unpack(&myBuffer, AgentDoneEventPackageBufSize*DoubleSize, DataType::Int, 1, &xPos);
				comm->Unpack(&myBuffer, AgentDoneEventPackageBufSize*DoubleSize, DataType::Int, 1, &yPos);
				comm->Unpack(&myBuffer, AgentDoneEventPackageBufSize*DoubleSize, DataType::DOUBLE, 1, &minRadius);
				comm->Unpack(&myBuffer, AgentDoneEventPackageBufSize*DoubleSize, DataType::Int, 1, &transferId);

				this->pos = Position<int>(xPos, yPos);
			}
		}

		PhysicalPathDoneEventPackage::PhysicalPathDoneEventPackage()
			: RemoteSubscription(0, 0, PackageType::PHYSICAL_PATH_DONE_EVENT), type(Framework::Eventing::SubscriptionTypes::PHYSICAL_PATH_DONE_EVENT), life(Framework::Eventing::SubscriptionTypes::NOTIFY_ALL)
		{
		}

		PhysicalPathDoneEventPackage::PhysicalPathDoneEventPackage(unsigned packageSource, unsigned packageDest)
			: RemoteSubscription(packageSource, packageDest, PackageType::PHYSICAL_PATH_DONE_EVENT), type(Framework::Eventing::SubscriptionTypes::PHYSICAL_PATH_DONE_EVENT), life(Framework::Eventing::SubscriptionTypes::NOTIFY_ALL)
		{
		}

		PhysicalPathDoneEventPackage::PhysicalPathDoneEventPackage(unsigned packageSource, unsigned packageDest, PhysicalPathEventInfo agentInfo)
			: RemoteSubscription(packageSource, packageDest, PackageType::PHYSICAL_PATH_DONE_EVENT), type(Framework::Eventing::SubscriptionTypes::PHYSICAL_PATH_DONE_EVENT), life(Framework::Eventing::SubscriptionTypes::NOTIFY_ALL), info(agentInfo)
		{
		}

		PhysicalPathDoneEventPackage::~PhysicalPathDoneEventPackage()
		{
		}

		Framework::Eventing::SubscriptionTypes::SubscriptionType PhysicalPathDoneEventPackage::GetSubscriptionType()
		{
			return this->type;
		}

		Framework::Eventing::SubscriptionTypes::SubscriptionLife PhysicalPathDoneEventPackage::GetSubscriptionLife()
		{
			return this->life;
		}

		void PhysicalPathDoneEventPackage::SetSubscriptionLife(Framework::Eventing::SubscriptionTypes::SubscriptionLife life)
		{
			this->life = life;
		}

		PhysicalPathEventInfo PhysicalPathDoneEventPackage::GetAgentInfo() const
		{
			return this->info;
		}

		RemoteSubscription *PhysicalPathDoneEventPackage::Clone() const
		{
			return new PhysicalPathDoneEventPackage(this->packageSource, this->packageDest, this->info);
		}

		/*static*/ IPackage *PhysicalPathDoneEventPackage::CreatePackage(unsigned source, unsigned dest)
		{
			return new PhysicalPathDoneEventPackage(source, dest);
		}

		void PhysicalPathDoneEventPackage::Send(unsigned destRank, unsigned messageTag, ICommunicator *comm)
		{
			Serialize(comm);

			if (comm != NULL)
			{
				comm->SendPacked(&myBuffer, destRank, messageTag, CommType::Synch);
			}
		}

		void PhysicalPathDoneEventPackage::Get(unsigned sourceRank, unsigned messageTag, ICommunicator *comm)
		{
			//comm = new Communicator();
			if (comm != NULL)
			{
				comm->GetPacked(PhysicalPathDoneEventPackageBufSize*DoubleSize, CommType::ASynch, sourceRank, messageTag, &myBuffer);
			}

			Deserialize(comm);
		}

		void PhysicalPathDoneEventPackage::Serialize(ICommunicator *comm)
		{
			BasePackage::Serialize(&myBuffer, PhysicalPathDoneEventPackageBufSize*DoubleSize, comm);

			if (comm != NULL)
			{
				comm->Pack(&type, 1, DataType::Int, &myBuffer, PhysicalPathDoneEventPackageBufSize*DoubleSize);
				comm->Pack(&life, 1, DataType::Int, &myBuffer, PhysicalPathDoneEventPackageBufSize*DoubleSize);
				comm->Pack(&info.agentId, 1, DataType::Int, &myBuffer, PhysicalPathDoneEventPackageBufSize*DoubleSize);
				comm->Pack(&info.start, 1, DataType::Int, &myBuffer, PhysicalPathDoneEventPackageBufSize*DoubleSize);
				comm->Pack(&info.goal, 1, DataType::Int, &myBuffer, PhysicalPathDoneEventPackageBufSize*DoubleSize);
				comm->Pack(&info.numSteps, 1, DataType::Int, &myBuffer, PhysicalPathDoneEventPackageBufSize*DoubleSize);
				comm->Pack(&info.pathNum, 1, DataType::Int, &myBuffer, PhysicalPathDoneEventPackageBufSize*DoubleSize);
				comm->Pack(&info.pathTime, 1, DataType::DOUBLE, &myBuffer, PhysicalPathDoneEventPackageBufSize*DoubleSize);
			}
		}

		void PhysicalPathDoneEventPackage::Deserialize(ICommunicator *comm)
		{
			BasePackage::Deserialize(&myBuffer, PhysicalPathDoneEventPackageBufSize*DoubleSize, comm);		

			if (comm != NULL)
			{
				comm->Unpack(&myBuffer, PhysicalPathDoneEventPackageBufSize*DoubleSize, DataType::Int, 1, &type);
				comm->Unpack(&myBuffer, PhysicalPathDoneEventPackageBufSize*DoubleSize, DataType::Int, 1, &life);
				comm->Unpack(&myBuffer, PhysicalPathDoneEventPackageBufSize*DoubleSize, DataType::Int, 1, &info.agentId);
				comm->Unpack(&myBuffer, PhysicalPathDoneEventPackageBufSize*DoubleSize, DataType::Int, 1, &info.start);
				comm->Unpack(&myBuffer, PhysicalPathDoneEventPackageBufSize*DoubleSize, DataType::Int, 1, &info.goal);
				comm->Unpack(&myBuffer, PhysicalPathDoneEventPackageBufSize*DoubleSize, DataType::Int, 1, &info.numSteps);
				comm->Unpack(&myBuffer, PhysicalPathDoneEventPackageBufSize*DoubleSize, DataType::Int, 1, &info.pathNum);
				comm->Unpack(&myBuffer, PhysicalPathDoneEventPackageBufSize*DoubleSize, DataType::DOUBLE, 1, &info.pathTime);
			}
		}
	}
}