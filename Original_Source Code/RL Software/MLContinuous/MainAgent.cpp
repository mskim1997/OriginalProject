/////////////////////////////////////////////////////////
//MainAgent.cpp
//Defines the Main Agent class.
//
//Manuel Madera
//8/8/09
/////////////////////////////////////////////////////////

#include "MainAgent.h"
#include <iostream>
#include <fstream>
#include "Framework\Packages.h"
#include "Framework\EventDistributor.h"
#include "Framework\PackagerFactory.h"
#include "MLAgentEvents.h"
#include "AgentFileIO.h"
#include "DateTime.h"
#include "Framework\Debug.h"
#include "Framework\Communicator.h"

#ifdef PrintMemoryLeak
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif
#endif

#ifdef WIN32

#define sleep(x)	Sleep(x*1000)
#endif

using namespace std;
using namespace Framework;
using namespace Framework::Eventing;
using namespace Framework::Agents;
using namespace Framework::Packages;
using namespace Framework::Eventing;
using namespace Framework::Factories;
using namespace Framework::Logging;
using namespace Framework::Communications;

namespace MLContinuous
{
	MainAgent::MainAgent(unsigned packageNode, 
					     unsigned packageDest, 
						 unsigned numNodes,
						 unsigned numAgentsPerNode,
						 unsigned xMazeSize, 
						 unsigned yMazeSize, 
						 char *mazeFile, 
						 const string &statsFile,
						 const string &agentFile,
						 Framework::Messaging::IMessageDistributor *messageDistributor,
						 Framework::Eventing::IEventDistributor *eventDistributor)
		: Agent(packageNode, packageDest), msgDist(messageDistributor), eventDist(eventDistributor), nodes(numNodes), agentsPerNode(numAgentsPerNode), statsFileName(statsFile), agentFileName(agentFile), totalNumOfAgents(0), numAgentsDone(0), totalNumReadyNodes(0), statsFile(0), maze(0)
	{
		if (this->msgDist != NULL)
		{
			this->msgDist->RegisterCallback(PackageType::MAZE_LIMIT, this, MainAgent::MainAgentCallback);
			this->msgDist->RegisterCallback(PackageType::GET_OBJ_ON, this, MainAgent::GetObjectOnCallback);
			this->msgDist->RegisterCallback(PackageType::NODE_READY, this, MainAgent::NodeReadyCallback);
		}

		PackagerFactory *packageFactory = PackagerFactory::Instance();

		if (packageFactory != NULL)
		{
			packageFactory->RegisterPackage(PackageType::MAZE_LIMIT, MazeLimitPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::AGENT_DONE_EVENT, AgentDoneEventPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::PHYSICAL_PATH_DONE_EVENT, PhysicalPathDoneEventPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::NODE_READY, NodeReadyPackage::CreatePackage);
		}

		if (this->eventDist != NULL)
		{
			this->eventDist->RegisterRemoteSubscription(PackageType::AGENT_DONE_EVENT, SubscriptionTypes::AGENT_DONE);
			this->eventDist->RegisterRemoteSubscription(PackageType::PHYSICAL_PATH_DONE_EVENT, SubscriptionTypes::PHYSICAL_PATH_DONE_EVENT);

			eventDist->Subscribe(this, SubscriptionTypes::AGENT_DONE, NotificationType::LONGLIVED, true);
			eventDist->Subscribe(this, SubscriptionTypes::PHYSICAL_PATH_DONE_EVENT, NotificationType::LONGLIVED, true);
		}

		char **tempMaze = new char *[yMazeSize];

		for (unsigned i = 0; i < yMazeSize; i++)
		{
			tempMaze[i] = new char[xMazeSize];
		}
		
		ifstream file;
		
		file.open(mazeFile);	//open a file
		
		for (unsigned i = 0; i < yMazeSize; i++)
		{
			for (unsigned j = 0; j < xMazeSize; j++)
			{
				file >> tempMaze[i][j];
			}
		}		

		file.close();

		maze = new Maze(yMazeSize, xMazeSize, tempMaze);

		sem_init(&nodesReady, 0, 0);

		this->statsFile = new StatsFileIO(this->statsFileName);

		list<string> headers;
		headers.push_back("Id");
		headers.push_back("Start");
		headers.push_back("Goal");
		headers.push_back("Path Num");
		headers.push_back("Steps");
		headers.push_back("Time");

		if (this->statsFile != NULL)
		{
			this->statsFile->WriteHeader(headers);
		}

		this->log = LogManager::Instance();
	}

	MainAgent::~MainAgent()
	{
		delete this->statsFile;
		this->statsFile = NULL;

		delete this->maze; 
		this->maze = NULL;

		sem_destroy(&this->nodesReady);
	}

	void MainAgent::Run()
	{
		if (nodes > 0)
		{
			//Communicator::Synch();

			//sem_wait(&this->nodesReady);

			SetupNodes();

			sem_wait(&this->nodesReady);

			SetupAgents();
		}
	}

	void MainAgent::Notify(const Eventing::ISubscription *subscription)
	{
		ISubscription *tempSubscription = const_cast<ISubscription *>(subscription);

		if (tempSubscription != NULL)
		{
			if (tempSubscription->GetSubscriptionType() == SubscriptionTypes::AGENT_DONE)
			{
				const AgentDoneEventPackage *agentDoneSubscription = static_cast<const AgentDoneEventPackage *>(subscription);

				if (agentDoneSubscription != NULL)
				{
					if (!agentDoneSubscription->IsTransfered())
					{
						double agentDoneTime = 0.0;
						struct timeval time2;

						this->numAgentsDone++;

						// Get end time in micro-seconds.
						gettimeofday(&time2, NULL);
						agentDoneTime = time2.tv_sec + (time2.tv_usec/1000000.0);

						if (!Validator::IsNull(log, NAME("log")))
						{
							log->Log(&typeid(this), "Agent: %d took: %f s.", agentDoneSubscription->GetAgentId(), agentDoneTime - this->agentStartTime);
						}

						printf("Agent: %d took: %f s.\n", agentDoneSubscription->GetAgentId(), agentDoneTime - this->agentStartTime);

						if (this->numAgentsDone == this->totalNumOfAgents)
						{
							// Get end time in micro-seconds.
							gettimeofday(&time2, NULL);
							agentEndTime = time2.tv_sec + (time2.tv_usec/1000000.0);

							if (!Validator::IsNull(log, NAME("log")))
							{
								log->Log(&typeid(this), "Total time taken: %f s.", this->agentEndTime - this->agentStartTime);
							}

							printf("Total time taken: %f s.", this->agentEndTime - this->agentStartTime);

							for (unsigned i = 1; i <= nodes; i++)
							{
								if (this->msgDist != NULL)
								{
									this->msgDist->SendPackage(new TerminatePackage(0, i));
								}
							}

							this->elements.push_back("ALL");
							this->elements.push_back("NA");
							this->elements.push_back("NA");
							this->elements.push_back("NA");
							this->elements.push_back("NA");
							this->elements.push_back(StatsFileIO::ConvertToString(this->agentEndTime - this->agentStartTime));

							if (this->statsFile != NULL)
							{
								this->statsFile->WriteRow(elements);
							}

							this->elements.clear();
						}
					}
				}
			}
			else if (tempSubscription->GetSubscriptionType() == SubscriptionTypes::PHYSICAL_PATH_DONE_EVENT)
			{
				const PhysicalPathDoneEventPackage *physicalPathDoneSubscription = static_cast<const PhysicalPathDoneEventPackage *>(subscription);
				
				if (physicalPathDoneSubscription != NULL)
				{
					PhysicalPathEventInfo agentInfo = physicalPathDoneSubscription->GetAgentInfo();

					this->elements.push_back(StatsFileIO::ConvertToString(agentInfo.agentId));
					this->elements.push_back(StatsFileIO::ConvertToString(agentInfo.start));
					this->elements.push_back(StatsFileIO::ConvertToString(agentInfo.goal));
					this->elements.push_back(StatsFileIO::ConvertToString(agentInfo.pathNum));

					this->elements.push_back(StatsFileIO::ConvertToString(agentInfo.numSteps));
					this->elements.push_back(StatsFileIO::ConvertToString(agentInfo.pathTime));

					if (this->statsFile != NULL)
					{
						this->statsFile->WriteRow(elements);
					}

					this->elements.clear();

					if (!Validator::IsNull(log, NAME("log")))
					{
						log->Log(&typeid(this), "\nAgent: %d, physical path: %d, took: %f s, and %d steps to goal = %c from start = %c.\n", agentInfo.agentId, agentInfo.pathNum, agentInfo.pathTime, agentInfo.numSteps, agentInfo.goal, agentInfo.start);
					}

					printf("\nAgent: %d, physical path: %d, took: %f s, and %d steps to goal = %c from start = %c.\n", agentInfo.agentId, agentInfo.pathNum, agentInfo.pathTime, agentInfo.numSteps, agentInfo.goal, agentInfo.start);
				}
			}
		}
	}

	void MainAgent::SetupNodes()
	{
		if ((this->msgDist != NULL)&&(this->maze != NULL))
		{
			char ***nodeMaze = new char **[nodes];
			IPackage *mazePackage;

			unsigned ySize = this->maze->GetYSize();
			unsigned xSize = this->maze->GetXSize();
			char **tempMaze = this->maze->GetMaze();

			unsigned mazeYSize = 0;
			unsigned j = 0;
			unsigned m = 0;
			unsigned n = 0;

			//_asm { int 3 };

			if (ySize%2 == 0)
			{
				mazeYSize = ySize/nodes;

				for (unsigned i = 0; i < nodes; i++)
				{
					nodeMaze[i] = new char *[mazeYSize];

					for (j = 0; j < mazeYSize; j++)
					{
						nodeMaze[i][j] = new char[xSize];

						for (n = 0; n < xSize; n++)
						{
							nodeMaze[i][j][n] = tempMaze[m][n];
						}

						m++;
					}
				}

				mazePackage = new MazePackage(0, 1, new Maze(mazeYSize, xSize, nodeMaze[0]), Position<int>(0, 0));
				this->msgDist->SendPackage(mazePackage);
			}
			else
			{
				mazeYSize = (ySize - 1)/nodes;

				nodeMaze[0] = new char *[mazeYSize + 1];

				for (unsigned i = 1; i < nodes; i++)
				{
					nodeMaze[i] = new char *[mazeYSize];

					for (j = 0; j < mazeYSize; j++)
					{
						nodeMaze[i][j] = new char[xSize];

						for (n = 0; n < xSize; n++)
						{
							nodeMaze[i][j][n] = tempMaze[m][n];
						}

						m++;
					}
				}

				mazePackage = new MazePackage(0, 1, new Maze(mazeYSize + 1, xSize, nodeMaze[0]), Position<int>(0, 0));
				this->msgDist->SendPackage(mazePackage);
			}

			for (unsigned i = 1; i < nodes; i++)
			{
				mazePackage = new MazePackage(0, (i + 1), new Maze(mazeYSize, xSize, nodeMaze[i]), Position<int>(0, mazeYSize*i));
				this->msgDist->SendPackage(mazePackage);
			}

			delete nodeMaze;
			nodeMaze = NULL;
		}
	}

	void MainAgent::SetupAgents()
	{
		AgentFileIO agentInfoFile(this->agentFileName);
		MLAgentInfo agentInfo;
		struct timeval time1;
		map<unsigned, unsigned> nodeToAgentsMap;
		unsigned nodeId = 0;

		// Get starting time in micro-seconds.
		gettimeofday(&time1, NULL);
		agentStartTime = time1.tv_sec + (time1.tv_usec/1000000.0);

		if (this->msgDist != NULL)
		{
			while (!agentInfoFile.IsEmpty())
			{
				agentInfo = agentInfoFile.GetAgentInfo();

				nodeId = agentInfo.GetNodeId();

				//If the node exists
				if (nodeId <= this->nodes)
				{
					//If the maximum number of allowed agents have already been created, don't allow anymore
					if (nodeToAgentsMap.find(nodeId) == nodeToAgentsMap.end())
					{
						nodeToAgentsMap[nodeId] = 0;
					}

					//Tell the node to create one more agent, only if the max num of agents hasn't been reached.
					if (nodeToAgentsMap[nodeId] < this->agentsPerNode)
					{
						this->msgDist->SendPackage(new NewMazeAgentPackage(0, nodeId, agentInfo.GetStart(), agentInfo.GetGoal()));

						nodeToAgentsMap[nodeId]++;

						totalNumOfAgents++;
					}
				}
			}
		}
	}

	/*static*/ void MainAgent::MainAgentCallback(void *ownerObject, Packages::IPackage *package)
	{
		MainAgent *pThis = static_cast<MainAgent *>(ownerObject);

		if ((pThis != NULL)&&(package != NULL))
		{
			MessagePackage *message = dynamic_cast<MessagePackage *>(package);

			if (message != NULL)
			{
				MazeLimitPackage *limitPackage = static_cast<MazeLimitPackage *>(message->GetPackage());

				if ((limitPackage != NULL)&&(pThis->maze != NULL)&&(pThis->msgDist != NULL))
				{
					Position<int> limit(pThis->maze->GetXSize(), pThis->maze->GetYSize());
					limitPackage->SetLimit(limit);

					unsigned dest = limitPackage->GetSource();
					limitPackage->SetDestination(dest);
					limitPackage->SetSource(0);

					pThis->msgDist->SendPackage(limitPackage);
				}
			}
		}

		delete package; 
		package = NULL;
	}

	/*static*/ void MainAgent::NodeReadyCallback(void *ownerObject, Packages::IPackage *package)
	{
		MainAgent *pThis = static_cast<MainAgent *>(ownerObject);
		
		if (pThis != NULL)
		{
			pThis->totalNumReadyNodes++;

			if (pThis->totalNumReadyNodes == pThis->nodes)
			{
				pThis->totalNumReadyNodes = 0;
				sem_post(&pThis->nodesReady);			
			}
		}

		delete package;
		package = NULL;
	}

	/*static*/ void MainAgent::GetObjectOnCallback(void *ownerObject, Framework::Packages::IPackage *package)
	{
		MainAgent *pThis = static_cast<MainAgent *>(ownerObject);
		MessagePackage *message = dynamic_cast<MessagePackage *>(package);

		if ((pThis != NULL)&&(message != NULL)&&(package != NULL))
		{
			GetObjectOnPackage *objectOnPackage = dynamic_cast<GetObjectOnPackage *>(message->GetPackage());

			if ((pThis->maze != NULL)&&(objectOnPackage != NULL))
			{
				unsigned ySize = pThis->maze->GetYSize();
				unsigned xSize = pThis->maze->GetXSize();
				char **maze = pThis->maze->GetMaze();
				bool found = false;

				if (objectOnPackage->GetQueryType() == GetObjectOnPackage::GET_OBJECT)
				{			
					char object = objectOnPackage->GetObj();

					for (unsigned i = 0; i < ySize; i++)
					{
						for (unsigned j = 0; j < xSize; j++)
						{
							if (maze[i][j] == object)
							{
								Position<int> objPos;
								objPos.SetXCoor(j);
								objPos.SetYCoor(i);

								objectOnPackage->SetPosition(objPos);
								unsigned dest = objectOnPackage->GetSource();

								objectOnPackage->SetSource(0);
								objectOnPackage->SetDestination(dest);

								found = true;
								break;
							}
						}
					}
				}
				else
				{
					unsigned packageX = objectOnPackage->GetPosition().GetXCoor();
					unsigned packageY = objectOnPackage->GetPosition().GetYCoor();

					if ((packageX > 0)||
						(packageX <= xSize)||
						(packageY > 0)||
						(packageY <= ySize))
					{
						char object = maze[packageY][packageX];
						found = true;

						objectOnPackage->SetObj(object);
						unsigned dest = objectOnPackage->GetSource();

						objectOnPackage->SetSource(0);
						objectOnPackage->SetDestination(dest);
					}			 
				}

				if (found)
				{
					if (pThis->msgDist != NULL)
					{
						pThis->msgDist->SendPackage(objectOnPackage);
					}
				}
				else
				{
					delete objectOnPackage;
					objectOnPackage = NULL;
				}
			}
		}

		delete package;
		package = NULL;
	}
}