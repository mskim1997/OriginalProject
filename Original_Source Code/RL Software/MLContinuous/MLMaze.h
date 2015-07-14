/////////////////////////////////////////////////////////
//MLMaze.h
//Defines the MLMaze class.
//
//Manuel Madera
//10/10/09
/////////////////////////////////////////////////////////

#ifndef MLMaze_h
#define MLMaze_h

#include "IMaze.h"
#include "MLTestProject.h"
#include "Framework\Maze.h"
#include <map>
#include <list>
#include <semaphore.h>
#include "Framework\IMessageDistributor.h"
#include "Framework\IEventDistributor.h"
#include "Framework\Publisher.h"

namespace MLContinuous
{
	typedef struct 
	{
		bool canMove;
		unsigned node;
		char startLoc;
	} MoveReply;

	typedef struct
	{
		Framework::Packages::GetObjectOnPackage::QueryType queryType;
		Framework::Position<int> pos;
		char object;
	} GetObjectOnReply;

	class ML_API MLMaze : public IMaze
	{
	private:
		Framework::Eventing::IPublisher *agentDonePublisher;
		Framework::Eventing::IPublisher *agentPhysicalPathDonePublisher;
		Framework::Maze *maze;
		unsigned nodeId;
		static IMaze *instance;
		bool isInitialized;
		sem_t binSem;
		std::map<unsigned, Framework::Position<int> *> agentPos;
		std::map<unsigned, Framework::Position<int> *> transferedAgentsPos;
		Framework::Messaging::IMessageDistributor *msgDist;
		Framework::Eventing::IEventDistributor *eventDist;
		Framework::Position<int> limit;
		std::map<char, Framework::Position<int> *> transferAgentStart;
		std::map<unsigned, MoveReply> canMoveReplyMap;
		std::list<GetObjectOnReply *> getObjectOnReplyList;
		sem_t canMoveReplySem;

		MLMaze();
	public:
		//No need to be thread-safe since it is called once
		//at the beginning of each node.
		static IMaze *Instance();
		virtual ~MLMaze();

		void Init(Framework::Maze *maze, Framework::Messaging::IMessageDistributor *msgDist, Framework::Eventing::IEventDistributor *eventDistributor, unsigned nodeID);
		void Shutdown();	

		char GetObjectOn(Framework::Position<int> pos);

		//Assumes that if the return is true, the agent will move to the specified position
		bool CanMove(unsigned agentID, Framework::Position<int> pos);
		bool CanMoveToGlobal(unsigned agentID, Framework::Position<int> movingFomPos, Framework::Position<int> movingToPos, unsigned &dest, char &newStartLoc);

		Framework::Position<int> GetPosition(char object);

		void NotifyAgentIsDone(unsigned agentId, int totalNumSteps, float minRadius, int xPos, int yPos, bool isTransferedAgent = false, bool hasFoundGoal = true, unsigned transferId = 0);

		void NotifyPhysicalPathDone(Framework::Eventing::PhysicalPathEventInfo agentInfo);

		unsigned GetXSize()
		{
			unsigned size = 0;

			if (this->maze != NULL)
			{
				size = this->maze->GetXSize();
			}

			return size;
		}

		unsigned GetYSize()
		{
			unsigned size = 0;

			if (this->maze != NULL)
			{
				size = this->maze->GetYSize();
			}

			return size;
		}

		Framework::Position<int> GetLimits();

		bool IsPositionWithinLocalLimits(Framework::Position<int> pos);

		bool IsPositionWithinGlobalLimits(Framework::Position<int> pos);
	private:
		bool IsAvailable(Framework::Position<int> pos);

		static void MazeLimitCallback(void *ownerObject, Framework::Packages::IPackage *package);

		static void CanMoveCallback(void *ownerObject, Framework::Packages::IPackage *package);

		static void CanMoveReplyCallback(void *ownerObject, Framework::Packages::IPackage *package);

		static void GetObjectOnCallback(void *ownerObject, Framework::Packages::IPackage *package);
	};
}

#endif	//MLMaze_h