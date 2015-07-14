/////////////////////////////////////////////////////////
//Maze.cpp
//Defines the Maze class.
//
//Manuel Madera
//10/10/09
/////////////////////////////////////////////////////////

#include "MLMaze.h"
#include "Framework\Packages.h"
#include "Framework\PackagerFactory.h"
#include <sstream>
#include "Framework\EventDistributor.h"
#include "Framework\Debug.h"

#ifdef PrintMemoryLeak
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif
#endif

#define MAX_LOOP	3
#define TIME_OUT	0.5 //seconds

#ifdef WIN32
#include <windows.h>

#define sleep(x)	Sleep(x*1000)
#endif

using namespace Framework::Factories;
using namespace Framework::Packages;
using namespace Framework::Eventing;
using namespace std;

namespace MLContinuous
{
	IMaze *MLMaze::instance = 0;

	MLMaze::MLMaze() : isInitialized(false), maze(NULL), msgDist(NULL), eventDist(NULL), agentDonePublisher(NULL), agentPhysicalPathDonePublisher(NULL)
	{
		sem_init(&binSem, 0, 1);
		sem_init(&canMoveReplySem, 0, 0);		

		PackagerFactory *packageFactory = PackagerFactory::Instance();

		if (packageFactory != NULL)
		{
			packageFactory->RegisterPackage(PackageType::MAZE_LIMIT, MazeLimitPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::MAZE_CAN_MOVE, MazeCanMovePackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::MAZE_CAN_MOVE_REPLY, MazeCanMoveReplyPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::GET_OBJ_ON, GetObjectOnPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::AGENT_DONE_EVENT, AgentDoneEventPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::PHYSICAL_PATH_DONE_EVENT, PhysicalPathDoneEventPackage::CreatePackage);
		}
	}

	void MLMaze::Init(Framework::Maze *maze, Framework::Messaging::IMessageDistributor *msgDist, Framework::Eventing::IEventDistributor *eventDistributor, unsigned nodeID)
	{
		if (!this->isInitialized)
		{
			this->eventDist = eventDistributor;
			this->msgDist = msgDist;
			this->maze = maze;
			this->isInitialized = true;
			
			if (this->msgDist != NULL)
			{
				this->msgDist->RegisterCallback(PackageType::MAZE_LIMIT, this, MLMaze::MazeLimitCallback);
				this->msgDist->RegisterCallback(PackageType::MAZE_CAN_MOVE, this, MLMaze::CanMoveCallback);
				this->msgDist->RegisterCallback(PackageType::MAZE_CAN_MOVE_REPLY, this, MLMaze::CanMoveReplyCallback);
				this->msgDist->RegisterCallback(PackageType::GET_OBJ_ON, this, MLMaze::GetObjectOnCallback);

				this->msgDist->SendPackage(new MazeLimitPackage(nodeID, 0));
			}

			this->nodeId = nodeID;
		}
	}

	void MLMaze::Shutdown()
	{
		delete instance; 
		instance = NULL;
	}

	/*static*/ IMaze *MLMaze::Instance()
	{
		if (instance == 0)
		{
			instance = new MLMaze();
		}

		return instance;
	}

	MLMaze::~MLMaze()
	{
		//sem_wait(&this->binSem);

		sem_destroy(&this->binSem);
		sem_destroy(&this->canMoveReplySem);

		map<unsigned, Framework::Position<int> *>::iterator iter;

		for (iter = this->agentPos.begin(); iter != this->agentPos.end(); iter++)
		{
			delete iter->second; 
			iter->second = NULL;
		}

		map<char, Framework::Position<int> *>::iterator charIter;

		for (charIter = this->transferAgentStart.begin(); charIter != this->transferAgentStart.end(); charIter++)
		{
			delete charIter->second;
			charIter->second = NULL;
		}

		delete this->agentDonePublisher;
		this->agentDonePublisher = NULL;

		delete this->agentPhysicalPathDonePublisher;
		this->agentPhysicalPathDonePublisher = NULL;

		delete this->maze; 
		this->maze = NULL;

		if (this->msgDist != NULL)
		{
			this->msgDist->UnregisterCallback(PackageType::MAZE_LIMIT);
			this->msgDist->UnregisterCallback(PackageType::MAZE_CAN_MOVE);
			this->msgDist->UnregisterCallback(PackageType::MAZE_CAN_MOVE_REPLY);
			this->msgDist->UnregisterCallback(PackageType::GET_OBJ_ON);
		}

	}

	bool MLMaze::IsAvailable(Framework::Position<int> pos)
	{		
		bool isAvailable = false;

		isAvailable = IsPositionWithinLocalLimits(pos);

		if (isAvailable)
		{
			if (this->maze != NULL)
			{
				isAvailable = (this->maze->GetMaze()[pos.GetYCoor()][pos.GetXCoor()] != 'B');
			}
		}

		if (isAvailable)
		{
			map<unsigned, Framework::Position<int> *>::iterator iter;

			for (iter = this->agentPos.begin(); iter != this->agentPos.end(); iter++)
			{
				if (iter->second != NULL)
				{
					if ((iter->second->GetXCoor() == pos.GetXCoor())&&
						(iter->second->GetYCoor() == pos.GetYCoor()))
					{
						isAvailable = false;
						break;
					}
				}
			}
		}

		return isAvailable;
	}

	char MLMaze::GetObjectOn(Framework::Position<int> pos)
	{
		char object = 'O';		

		if (IsPositionWithinLocalLimits(pos))
		{
			if (this->maze != NULL)
			{
				object = this->maze->GetMaze()[pos.GetYCoor()][pos.GetXCoor()];
			}
		}
		else if (IsPositionWithinGlobalLimits(pos))
		{
			bool isFound = false;
			GetObjectOnReply *storedReply = NULL;

			sem_wait(&this->binSem);
			list<GetObjectOnReply *>::iterator iter;
			for (iter = getObjectOnReplyList.begin(); iter != getObjectOnReplyList.end(); iter++)
			{
				if (*iter != NULL)
				{
					storedReply = *iter;

					if (storedReply->queryType == GetObjectOnPackage::GET_OBJECT)
					{
						if ((storedReply->pos.GetXCoor() == pos.GetXCoor())&&
							(storedReply->pos.GetYCoor() == pos.GetYCoor()))
						{
							object = storedReply->object;
							isFound = true;
							break;
						}
					}
				}
			}
			sem_post(&this->binSem);

			if (!isFound)
			{
				if (this->msgDist != NULL)
				{
					this->msgDist->SendPackage(new GetObjectOnPackage(this->nodeId, 0, GetObjectOnPackage::GET_OBJECT, pos + this->msgDist->GetOffset()));
				}

				sleep(TIME_OUT);

				sem_wait(&this->binSem);
				for (iter = getObjectOnReplyList.begin(); iter != getObjectOnReplyList.end(); iter++)
				{
					if (*iter != NULL)
					{
						storedReply = *iter;

						if (storedReply->queryType == GetObjectOnPackage::GET_OBJECT)
						{
							if ((storedReply->pos.GetXCoor() == pos.GetXCoor())&&
								(storedReply->pos.GetYCoor() == pos.GetYCoor()))
							{
								object = storedReply->object;
								isFound = true;
								break;
							}
						}
					}
				}

				if ((isFound)&&(storedReply->object != 'B'))
				{
					delete *iter; 
					*iter = NULL;
					
					getObjectOnReplyList.erase(iter);
				}				
				sem_post(&this->binSem);
			}
		}

		return object;
	}

	//Assumes that if the return is true, the agent will move to the specified position
	bool MLMaze::CanMove(unsigned agentID, Framework::Position<int> pos)
	{
		bool canMove = false;

		sem_wait(&this->binSem);
		if (IsAvailable(pos))
		{
			canMove = true;

			if (this->transferedAgentsPos.find(agentID) != this->transferedAgentsPos.end())
			{
				canMove = false;
				/*Framework::Position<int> *tempPos = this->transferedAgentsPos[agentID];

				if ((tempPos->GetXCoor() == pos.GetXCoor())&&
					(tempPos->GetYCoor() == pos.GetYCoor()))
				{
					canMove = false;
				}*/
			}

			if (canMove)
			{
				if (this->agentPos.find(agentID) == this->agentPos.end())
				{
					this->agentPos[agentID] = new Framework::Position<int>(pos);
				}
				else
				{
					if (this->agentPos[agentID] != NULL)
					{
						this->agentPos[agentID]->SetXCoor(pos.GetXCoor());
						this->agentPos[agentID]->SetYCoor(pos.GetYCoor());
					}
				}
			}
		}
		sem_post(&this->binSem);

		return canMove;
	}

	bool MLMaze::CanMoveToGlobal(unsigned agentID, Framework::Position<int> movingFomPos, Framework::Position<int> movingToPos, unsigned &dest, char &newStartLoc)
	{
		bool canMove = false;

		if (this->msgDist != NULL)
		{
			this->msgDist->SendPackage(new MazeCanMovePackage(this->nodeId, 0, movingToPos + this->msgDist->GetOffset(), agentID));
		}
		//sem_wait(&this->canMoveReplySem);

		//for (int i = 0; i < MAX_LOOP; i++)
		//{
			sleep(TIME_OUT);

			sem_wait(&this->binSem);
			if (this->canMoveReplyMap.find(agentID) != this->canMoveReplyMap.end())
			{
				canMove = this->canMoveReplyMap[agentID].canMove;
				dest = this->canMoveReplyMap[agentID].node;
				newStartLoc = this->canMoveReplyMap[agentID].startLoc;

				if (canMove)
				{
					Framework::Position<int> *temp = this->agentPos[agentID];

					this->agentPos.erase(agentID);

					delete temp; 
					temp = NULL;

					//Add to the transfered agents pos list, in case the transfered agent tries to
					//come back to this position.
					if (this->transferedAgentsPos.find(agentID) == this->transferedAgentsPos.end())
					{
						this->transferedAgentsPos[agentID] = new Framework::Position<int>(movingFomPos);
					}
					else
					{
						if (this->transferedAgentsPos[agentID] != NULL)
						{
							this->transferedAgentsPos[agentID]->SetXCoor(movingFomPos.GetXCoor());
							this->transferedAgentsPos[agentID]->SetYCoor(movingFomPos.GetYCoor());
						}
					}
				}

				//break;
			}
			sem_post(&this->binSem);
		//}

		return canMove;
	}

	Framework::Position<int> MLMaze::GetLimits()
	{
		Framework::Position<int> limits;

		if (this->maze != NULL)
		{
			limits = Framework::Position<int>(this->maze->GetXSize() - 1, this->maze->GetYSize() - 1);
		}

		return limits;
	}

	void MLMaze::NotifyAgentIsDone(unsigned agentId, int totalNumSteps, float minRadius, int xPos, int yPos, bool isTransferedAgent, bool hasFoundGoal, unsigned transferId)
	{
		sem_wait(&this->binSem);
		if (this->agentDonePublisher == NULL)
		{
			//this->eventDist->RegisterRemoteSubscription(PackageType::AGENT_DONE_EVENT, SubscriptionTypes::AGENT_DONE);

			this->agentDonePublisher = new Publisher(this->eventDist, SubscriptionTypes::AGENT_DONE);
		}
		
		if (this->agentDonePublisher != NULL)
		{
			this->agentDonePublisher->Publish(new AgentDoneEventPackage(this->nodeId, 0, agentId, xPos, yPos, totalNumSteps, isTransferedAgent, hasFoundGoal, minRadius, transferId));
		}

		//Remove agent from the map once it has found its goal, and it is done searching
		if (this->agentPos.find(agentId) != this->agentPos.end())
		{
			Framework::Position<int> *temp = this->agentPos[agentId];
			this->agentPos.erase(agentId);

			delete temp; 
			temp = NULL;
		}

		if (this->transferedAgentsPos.find(agentId) != this->transferedAgentsPos.end())
		{
			Framework::Position<int> *temp = this->transferedAgentsPos[agentId];
			this->transferedAgentsPos.erase(agentId);
			
			delete temp; 
			temp = NULL;
		}
		sem_post(&this->binSem);
	}

	void MLMaze::NotifyPhysicalPathDone(PhysicalPathEventInfo agentInfo)
	{
		sem_wait(&this->binSem);
		if (this->agentPhysicalPathDonePublisher == NULL)
		{
			this->agentPhysicalPathDonePublisher = new Publisher(this->eventDist, SubscriptionTypes::PHYSICAL_PATH_DONE_EVENT);
		}
		
		if (this->agentPhysicalPathDonePublisher != NULL)
		{
			this->agentPhysicalPathDonePublisher->Publish(new PhysicalPathDoneEventPackage(this->nodeId, 0, agentInfo));
		}

		//Remove agent from the map once it has found its goal, and it is done searching
		if (this->agentPos.find(agentInfo.agentId) != this->agentPos.end())
		{
			Framework::Position<int> *temp = this->agentPos[agentInfo.agentId];
			this->agentPos.erase(agentInfo.agentId);
			
			delete temp; 
			temp = NULL;
		}

		if (this->transferedAgentsPos.find(agentInfo.agentId) != this->transferedAgentsPos.end())
		{
			Framework::Position<int> *temp = this->transferedAgentsPos[agentInfo.agentId];
			this->transferedAgentsPos.erase(agentInfo.agentId);
			
			delete temp; 
			temp = NULL;
		}
		sem_post(&this->binSem);
	}

	bool MLMaze::IsPositionWithinLocalLimits(Framework::Position<int> pos)
	{
		bool isWithinLimits = true;

		if (this->maze != NULL)
		{
			if ((pos.GetXCoor() < 0)||
				(pos.GetXCoor() >= this->maze->GetXSize())||
				(pos.GetYCoor() < 0)||
				(pos.GetYCoor() >= this->maze->GetYSize()))
			{
				isWithinLimits = false;
			}
		}

		return isWithinLimits;
	}

	bool MLMaze::IsPositionWithinGlobalLimits(Framework::Position<int> pos)
	{
		bool isWithinLimits = true;

		if (this->msgDist != NULL)
		{
			Framework::Position<int> tempPos = pos + this->msgDist->GetOffset();

			if ((tempPos.GetXCoor() < 0)||
				(tempPos.GetXCoor() >= this->limit.GetXCoor())||
				(tempPos.GetYCoor() < 0)||
				(tempPos.GetYCoor() >= this->limit.GetYCoor()))
			{
				isWithinLimits = false;
			}
		}

		return isWithinLimits;
	}

	Framework::Position<int> MLMaze::GetPosition(char object)
	{
		bool found = false;
		Framework::Position<int> objPos;

		if (this->maze != NULL)
		{
			char **maze = this->maze->GetMaze();
			unsigned ySize = this->maze->GetYSize();
			unsigned xSize = this->maze->GetXSize();
			

			//Check the maze
			for (unsigned i = 0; i < ySize; i++)
			{
				for (unsigned j = 0; j < xSize; j++)
				{
					if (maze[i][j] == object)
					{
						objPos.SetXCoor(j);
						objPos.SetYCoor(i);

						found = true;
						break;
					}
				}
			}
		}

		//Check to see if it was a transfered agent
		if (!found)
		{
			sem_wait(&this->binSem);
			if (this->transferAgentStart.find(object) != this->transferAgentStart.end())
			{
				objPos = *this->transferAgentStart[object];
				found = true;
			}
			sem_post(&this->binSem);
		}

		//As a last resort, ask the main node.
		if (!found)
		{
			bool isFound = false;
			GetObjectOnReply *storedReply = NULL;

			sem_wait(&this->binSem);
			list<GetObjectOnReply *>::iterator iter;
			for (iter = getObjectOnReplyList.begin(); iter != getObjectOnReplyList.end(); iter++)
			{
				if (*iter != NULL)
				{
					storedReply = *iter;

					if (storedReply->queryType == GetObjectOnPackage::GET_POSITION)
					{
						if (storedReply->object == object)
						{
							objPos = storedReply->pos;
							isFound = true;
							break;
						}
					}
				}
			}
			sem_post(&this->binSem);

			if (!isFound)
			{
				if (this->msgDist != NULL)
				{
					this->msgDist->SendPackage(new GetObjectOnPackage(this->nodeId, 0, GetObjectOnPackage::GET_POSITION, object));
				}

				sleep(TIME_OUT);

				sem_wait(&this->binSem);
				for (iter = getObjectOnReplyList.begin(); iter != getObjectOnReplyList.end(); iter++)
				{
					if (*iter != NULL)
					{
						storedReply = *iter;

						if (storedReply->queryType == GetObjectOnPackage::GET_POSITION)
						{
							if (storedReply->object == object)
							{
								objPos = storedReply->pos;
								isFound = true;
								break;
							}
						}
					}
				}

				if (isFound)
				{
					delete *iter; 
					*iter = NULL;
					
					getObjectOnReplyList.erase(iter);
				}				
				sem_post(&this->binSem);
			}
		}

		return objPos;
	}

	/*static*/ void MLMaze::MazeLimitCallback(void *ownerObject, Framework::Packages::IPackage *package)
	{
		MLMaze *pThis = static_cast<MLMaze *>(ownerObject);

		if (pThis != NULL)
		{
			MazeLimitPackage *limitPackage = dynamic_cast<MazeLimitPackage *>(package);

			sem_wait(&pThis->binSem);

			if (limitPackage != NULL)
			{
				pThis->limit = limitPackage->GetLimit();
			}

			sem_post(&pThis->binSem);
		}

		delete package; 
		package = NULL;
	}

	/*static*/ void MLMaze::CanMoveCallback(void *ownerObject, Framework::Packages::IPackage *package)
	{
		MLMaze *pThis = static_cast<MLMaze *>(ownerObject);

		if (pThis != NULL)
		{
			MazeCanMovePackage *canMovePackage = dynamic_cast<MazeCanMovePackage *>(package);

			if (canMovePackage != NULL)
			{
				Framework::Position<int> tempPos = canMovePackage->GetPos() - pThis->msgDist->GetOffset();

				if (pThis->IsPositionWithinLocalLimits(tempPos))
				{
					bool canMove = pThis->CanMove(canMovePackage->GetAgentId(), tempPos);
					unsigned id = canMovePackage->GetAgentId();
					string agentID;
					stringstream out;

					out << id;
					agentID = out.str();

					out.str("");
					out.flush();

					char start[5];
					strcpy(start, agentID.c_str());

					sem_wait(&pThis->binSem);
					if (canMove)
					{
						pThis->transferAgentStart[start[0]] = new Framework::Position<int>(tempPos);
					}
					sem_post(&pThis->binSem);

					if (pThis->msgDist != NULL)
					{
						pThis->msgDist->SendPackage(new MazeCanMoveReplyPackage(pThis->nodeId, canMovePackage->GetSource(), canMove, id, start[0]));
					}
				}
			}
		}

		delete package; 
		package = NULL;
	}

	/*static*/ void MLMaze::GetObjectOnCallback(void *ownerObject, Framework::Packages::IPackage *package)
	{
		MLMaze *pThis = static_cast<MLMaze *>(ownerObject);

		if (pThis != NULL)
		{
			GetObjectOnPackage *objectOnPackage = dynamic_cast<GetObjectOnPackage *>(package);

			GetObjectOnReply *reply = new GetObjectOnReply();

			if ((objectOnPackage != NULL)&&(reply != NULL))
			{
				reply->object = objectOnPackage->GetObj();
				if (objectOnPackage->GetQueryType() == GetObjectOnPackage::GET_POSITION)
				{
					if (pThis->msgDist != NULL)
					{
						reply->pos = objectOnPackage->GetPosition() - pThis->msgDist->GetOffset();
					}
				}
				else
				{
					reply->pos = objectOnPackage->GetPosition();
				}

				reply->queryType = objectOnPackage->GetQueryType();

				sem_wait(&pThis->binSem);
				pThis->getObjectOnReplyList.push_back(reply);
				sem_post(&pThis->binSem);
			}
			
			delete reply; 
			reply = NULL;
		}

		delete package; 
		package = NULL;
	}

	/*static*/ void MLMaze::CanMoveReplyCallback(void *ownerObject, Framework::Packages::IPackage *package)
	{
		MLMaze *pThis = static_cast<MLMaze *>(ownerObject);

		if (pThis != NULL)
		{
			MazeCanMoveReplyPackage *canMoveReplyPackage = dynamic_cast<MazeCanMoveReplyPackage *>(package);

			if (canMoveReplyPackage != NULL)
			{
				MoveReply reply;
				reply.canMove = canMoveReplyPackage->GetCanMove();
				reply.node = canMoveReplyPackage->GetSource();
				reply.startLoc = canMoveReplyPackage->GetStartLoc();

				pThis->canMoveReplyMap[canMoveReplyPackage->GetAgentId()] = reply;
			}
		}

		delete package; 
		package = NULL;
		//sem_post(&pThis->canMoveReplySem);
	}
}