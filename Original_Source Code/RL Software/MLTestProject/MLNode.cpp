/////////////////////////////////////////////////////////
//MLNode.cpp
//Defines the specific MLNode implementations.
//
//Manuel Madera
//8/18/09
/////////////////////////////////////////////////////////

#include "MLNode.h"
#include "Framework\MazeReceivedSubscription.h"
#include "Framework\MazePositionEventPackage.h"
#include <iostream>
#include "Framework\Debug.h"
#include "MLMaze.h"
#include "Framework\PackagerFactory.h"
#include "MLAgent.h"
#include <sstream>
#include "MLNodeBrain.h"
#include "Framework\Communicator.h"

#define FILE_NAME_SIZE	30

#ifdef PrintMemoryLeak
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif
#endif

using namespace std;
using namespace Framework::Messaging;
using namespace Framework::Eventing;
using namespace Framework::Agents;
using namespace Framework::Packages;
using namespace Framework::Factories;
using namespace Framework::Logging;
using namespace Framework::Communications;
using namespace MLTest;

namespace Framework
{
	MLNode::MLNode(unsigned rank, bool allowSharing) : Node(rank, allowSharing), nodeBrain(NULL)
	{
		Init();
	}

    MLNode::MLNode(unsigned rank, bool allowSharing, IAgent *newAgent) : Node(rank, allowSharing, newAgent), nodeBrain(NULL)
    {
		Init();
    }
    
	void MLNode::Init()
	{
		sem_init(&mazeSem, 0, 0);
		sem_init(&doneSem, 0, 0);
		sem_init(&binSem, 0, 1);

		if (this->eventDist != NULL)
		{
			eventDist->Subscribe(this, SubscriptionTypes::MAZE_RECEIVED, NotificationType::ONETIME);
			eventDist->Subscribe(this, SubscriptionTypes::MAZE_POSITION_EVENT, NotificationType::LONGLIVED);
		}

		PackagerFactory *packageFactory = PackagerFactory::Instance();

		if (packageFactory != NULL)
		{
			packageFactory->RegisterPackage(PackageType::MAZE_AGENT, MazeAgentPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::NEW_MAZE_AGENT, NewMazeAgentPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::NODE_READY, NodeReadyPackage::CreatePackage);
		}

		if (this->msgDist != NULL)
		{
			this->msgDist->RegisterCallback(PackageType::NEW_MAZE_AGENT, this, MLNode::NewMazeAgentCallback);
			this->msgDist->RegisterCallback(PackageType::MAZE_AGENT, this, MLNode::MazeAgentCallback);
			this->msgDist->RegisterCallback(PackageType::TERMINATE, this, MLNode::TerminateCallback);
		}

		if (this->eventDist != NULL)
		{
			this->eventDist->RegisterRemoteSubscription(PackageType::AGENT_DONE_EVENT, SubscriptionTypes::AGENT_DONE);
			this->eventDist->RegisterRemoteSubscription(PackageType::PHYSICAL_PATH_DONE_EVENT, SubscriptionTypes::PHYSICAL_PATH_DONE_EVENT);
		}
	}

    MLNode::~MLNode() 
    {
		delete nodeBrain;
		nodeBrain = NULL;
		
		list<pthread_t *>::iterator iter;
		for (iter = this->agentList.begin(); iter != this->agentList.end(); iter++)
		{
			delete *iter; 
			*iter = NULL;
		}

		this->agentList.clear();

		sem_destroy(&this->mazeSem);
		sem_destroy(&this->doneSem);
		sem_destroy(&this->binSem);

		#ifdef PrintMemoryLeak
			CHECK_HEAP();
		#endif
    }

	void MLNode::Initialize()
	{
		//Notify main node that i am ready.
		//Communicator::Synch();

		//this->msgDist->SendPackage(new NodeReadyPackage(this->myRank, 0));

		sem_wait(&this->mazeSem);
	}

	void MLNode::Wait()
	{
		sem_wait(&this->doneSem);

		/*list<pthread_t *>::iterator iter;
		for (iter = this->agentList.begin(); iter != this->agentList.end(); iter++)
		{
			pthread_join(**iter, NULL);
		}*/
	}

	void MLNode::Terminate()
	{
		static_cast<MLMaze *>(MLMaze::Instance())->Shutdown();

		Node::Terminate();		
	}

	void MLNode::Run()
	{
	}

	/*static*/ void MLNode::NewMazeAgentCallback(void *ownerObject, Framework::Packages::IPackage *package)
	{	
		MLNode *pThis = static_cast<MLNode *>(ownerObject);
		
		if (pThis != NULL)
		{
#ifdef DebugMLNode
			if (!Validator::IsNull(pThis->log, NAME("pThis->log")))
			{
				pThis->log->Log(&typeid(pThis), "Node: %d is creating a new agent", pThis->myRank);
			}
#endif

			NewMazeAgentPackage *mazeAgentPackage = dynamic_cast<NewMazeAgentPackage *>(package);

			if ((mazeAgentPackage != NULL)&&(pThis->maze != NULL)&&(pThis->nodeBrain != NULL))
			{
				pthread_t *workerThread = new pthread_t();				

				unsigned ySize = pThis->maze->GetYSize();
				unsigned xSize = pThis->maze->GetXSize();

				//TODO: for the time being, the agents delete their own agentBrain.
				MLAgent *agent = new MLAgent(pThis->myRank, 0, "", ySize, xSize, pThis->maze, pThis->nodeBrain->GetBrain(), false, true);

				if (!Validator::IsNull(agent, NAME("agent")))
				{
					MazeAgentInfo agentInfo;
					agentInfo.start = mazeAgentPackage->GetStart();
					agentInfo.goal = mazeAgentPackage->GetGoal();

					agent->SetAgentInfo(agentInfo);

					int rc = pthread_create(workerThread, NULL, MLNode::NewAgentThread, agent);

					pThis->agentList.push_back(workerThread);
				}
				else
				{
					delete workerThread; 
					workerThread = NULL;
				}				
			}
		}
		
		delete package; 
		package = NULL;
	}

	/*static*/ void MLNode::MazeAgentCallback(void *ownerObject, Framework::Packages::IPackage *package)
	{		
		MLNode *pThis = static_cast<MLNode *>(ownerObject);
		
		if (pThis != NULL)
		{
			MazeAgentPackage *mazeAgentPackage = dynamic_cast<MazeAgentPackage *>(package);

			if ((mazeAgentPackage != NULL)&&(pThis->maze != NULL)&&(pThis->nodeBrain != NULL))
			{
				MazeAgentInfo agentInfo = mazeAgentPackage->GetAgentInfo();

				if (pThis->transferedAgents.find(agentInfo.agentId) == pThis->transferedAgents.end())
				{
					pthread_t *workerThread = new pthread_t();	
					
					string agentID;
					string nodeID;
					stringstream out;

					out << agentInfo.agentId;
					agentID = out.str();

					out.str("");
					out.flush();

					out << pThis->myRank;
					nodeID = out.str();

					out.str("");
					out.flush();

					string agentFile = "Out\\transfer" + nodeID + "_" + agentID + ".txt";
					char *fileName = new char[FILE_NAME_SIZE];

					strcpy(fileName, agentFile.c_str());

					unsigned ySize = pThis->maze->GetYSize();
					unsigned xSize = pThis->maze->GetXSize();

					//TODO: for the time being, the agents delete their own agentBrain.
					MLAgent *agent = new MLAgent(pThis->myRank, 0, fileName, ySize, xSize, pThis->maze, pThis->nodeBrain->GetBrain(), true, true);
					pThis->transferedAgents[agentInfo.agentId] = agent;

					if (!Validator::IsNull(agent, NAME("agent")))
					{
						agent->RegisterTransfer(agentInfo.agentId);
						agent->SetAgentInfo(agentInfo);

						int rc = pthread_create(workerThread, NULL, MLNode::TransferAgentThread, agent);

						pThis->agentList.push_back(workerThread);
					}
					else
					{
						delete workerThread;
						workerThread = NULL;
					}					
				}
				else
				{
					pThis->transferedAgents[agentInfo.agentId]->SetAgentInfo(agentInfo);
					pThis->transferedAgents[agentInfo.agentId]->SignalAgentTransfered();
				}
			}
			//sem_post(&pThis->binSem);
		}

		delete package;
		package = NULL;
	}

	/*static*/ void MLNode::TerminateCallback(void *ownerObject, Framework::Packages::IPackage *package)
	{
		MLNode *pThis = static_cast<MLNode *>(ownerObject);

		if (pThis != NULL)
		{
			sem_post(&pThis->doneSem);
		}

		delete package; 
		package = NULL;
	}

	void MLNode::Notify(const Eventing::ISubscription *subscription)
	{		
		Eventing::ISubscription *tempSubscription = const_cast<Eventing::ISubscription *>(subscription);

		if (tempSubscription != NULL)
		{
			if (tempSubscription->GetSubscriptionType() == SubscriptionTypes::MAZE_RECEIVED)
			{
				const MazeReceivedSubscription *mazeSubscription = static_cast<const MazeReceivedSubscription *>(subscription);

				if (mazeSubscription != NULL)
				{
					//sem_wait(&this->binSem);
					//this->maze = mazeSubscription->GetMaze();
					static_cast<MLMaze *>(MLMaze::Instance())->Init(mazeSubscription->GetMaze(), this->msgDist, this->eventDist, this->myRank);
					this->maze = MLMaze::Instance();

					if (this->maze != NULL)
					{
						this->nodeBrain = new MLNodeBrain(this->maze->GetYSize(), this->maze->GetXSize());
					}
					//sem_post(&this->binSem);

#ifdef DebugMLNode
					if (!Validator::IsNull(log, NAME("log")))
					{
						log->Log(&typeid(this), "Maze in Node: %d", this->myRank);
					}
#endif

					//Notify main node that i got the maze.
					if (this->msgDist != NULL)
					{
						this->msgDist->SendPackage(new NodeReadyPackage(this->myRank, 0));
					}

					sem_post(&this->mazeSem);
				}
			}
			else if (tempSubscription->GetSubscriptionType() == SubscriptionTypes::MAZE_POSITION_EVENT)
			{
				const MazePositionEventPackage *mazePositionSubscription = static_cast<const MazePositionEventPackage *>(subscription);

				if (mazePositionSubscription != NULL)
				{
					MazePositionPackage *package = mazePositionSubscription->GetPositionPackage();

					if (package != NULL)
					{
#ifdef DebugMLNode
						if (!Validator::IsNull(log, NAME("log")))
						{
							log->Log(&typeid(this), "Node: %d, received maze position event, object received was %c", this->myRank, package->GetObject());
						}
#endif

						if (package->GetObject() == 'B')
						{
							if (this->shareInfo)
							{
								//sem_wait(&this->binSem);
								state_t newState;
								newState.x = package->GetPosition().GetXCoor();
								newState.y = package->GetPosition().GetYCoor();
								newState.locSt = BARRIER;
								newState.isVsted = NEVER;

								if (this->nodeBrain != NULL)
								{
									this->nodeBrain->SetLocationState(newState);
								}
								//sem_post(&this->binSem);
							}
						}

						delete package; 
						package = NULL;
					}
				}
			}	
		}
	}

	/*static*/ void *MLNode::NewAgentThread(void *arg)
	{
		IAgent *agent = static_cast<IAgent *>(arg);		

		if (!Validator::IsNull(agent, NAME("agent")))
		{
			agent->Register();

			string agentID;
			string nodeID;
			stringstream out;

			out << agent->GetAgentUI();
			agentID = out.str();

			out.str("");
			out.flush();

			IPackage *agentPackage = dynamic_cast<IPackage *>(agent);

			if (agentPackage != NULL)
			{
				out << agentPackage->GetSource();
				nodeID = out.str();
			}	

			out.str("");
			out.flush();

			string agentFile = "Out\\out" + nodeID + "_" + agentID + ".txt";
			char *fileName = new char[FILE_NAME_SIZE];

			strcpy(fileName, agentFile.c_str());		

			MLAgent *mlAgent = dynamic_cast<MLAgent *>(agent);

			if (mlAgent != NULL)
			{
				mlAgent->SetAgentFile(fileName);
			}

#ifdef DebugMLNode
			ILog *log = LogManager::Instance();

			if (!Validator::IsNull(log, NAME("log")))
			{
				log->Log(&typeid(MLNode), "Agent: %d running ...", agent->GetAgentUI());
			}
#endif
			agent->Run();

			//delete[] fileName;
			delete agent; 
			agent = NULL;
		}

		#ifdef LINUX
				pthread_exit(NULL);
			#endif

#ifdef WIN32
        return NULL;
#endif
	}

	/*static*/ void *MLNode::TransferAgentThread(void *arg)
	{
		IAgent *agent = static_cast<IAgent *>(arg);

		if (!Validator::IsNull(agent, NAME("agent")))
		{
			agent->Register();

#ifdef DebugMLNode
			ILog *log = LogManager::Instance();

			if (!Validator::IsNull(log, NAME("log")))
			{
				log->Log(&typeid(MLNode), "Agent: %d running ...", agent->GetAgentUI());
			}
#endif
			agent->Run();

			delete agent; 
			agent = NULL;
		}

		#ifdef LINUX
				pthread_exit(NULL);
			#endif

#ifdef WIN32
        return NULL;
#endif
	}
}