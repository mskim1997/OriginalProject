/////////////////////////////////////////////////////////
//MLAgent.cpp
//Defines the Machine Learning Agent class.
//
//Manuel Madera
//8/8/09
/////////////////////////////////////////////////////////

#include "MLAgent.h"
#include "Framework\Packages.h"
#include "Framework\EventDistributor.h"

#ifdef WIN32
#include <windows.h>
#endif
#include "MLMaze.h"
#include "Framework\Debug.h"
#include "Framework\Validator.h"

#ifdef PrintMemoryLeak
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif
#endif

using namespace Framework;
using namespace Framework::Logging;
using namespace Framework::Packages;
using namespace Framework::Agents;
using namespace Framework::Eventing;

//Time in milliseconds to sleep if the message queue is empty
#define WAIT_TIME	500

namespace MLTest
{
	MLAgent::MLAgent(unsigned packageNode, unsigned packageDest, char *agentFile, unsigned ySize, unsigned xSize, IMaze *nodeMaze, state_t **agentBrain, bool transfered, bool localFirst) 
		: Agent(packageNode, packageDest), exploreLocalFirst(localFirst), isGoalOnAnotherNode(false), maze(0), nodeTransferedTo(0)
	{		 
		this->maze = nodeMaze;
		this->canNotifyWhenDone = true;

		this->info.xSize = xSize;             
		this->info.ySize = ySize;
		this->info.agentBrain = agentBrain;
		this->info.agentFile = agentFile;  
		this->info.maxPhsPaths = 5;       
		this->info.maxPhsPathsOn = TRUE_t; 
		this->info.maxLrnPaths = 1;       
		this->info.maxLrnPathsOn = TRUE_t; 
		this->info.lrnMethod = COMBINE_ALL; 
		this->info.cntrlFirstOn = FALSE_t;  
		this->info.initFullExploreOn = FALSE_t; 
		this->info.lrnGreedyOn = TRUE_t;    
		this->info.phsGreedyOn = FALSE_t;    
		this->info.constTawOn = FALSE_t;     
		this->info.w1On = FALSE_t;           
		this->info.w2On = FALSE_t;           
		this->info.w3On = FALSE_t;           
		this->info.nAhead = 1;               
		this->info.pathFileMode = BOTH_PATH_FILE;
		this->info.seedOn = FALSE_t;        
		this->info.alpha = D_ALPHA;        
		this->info.gamma = D_GAMMA;        
		this->info.lambda = D_LAMBDA;      
		this->info.taw = INIT_TAW;         
		this->info.w1 = 0.8;                 
		this->info.w2 = 0.5;                 
		this->info.w3 = 0.5;                 
		this->info.hm = "\0"; 
		this->info.maze = 0;
		this->info.cntrlFirstFile = 0;

		this->log = LogManager::Instance();
		this->isTransfered = transfered;		

		sem_init(&transferedAgentDoneSem, 0, 0);
		sem_init(&agentTransferedSem, 0, 1);
	}

	MLAgent::~MLAgent()
	{
		if (!this->isTransfered)
		{
			IEventDistributor *eventDist = EventDistributor::Instance();

			if (eventDist != NULL)
			{
				eventDist->Unsubscribe(this, SubscriptionTypes::AGENT_DONE);
			}
		}

		sem_destroy(&this->transferedAgentDoneSem);
		sem_destroy(&this->agentTransferedSem);

		//This is a memory leak that needs to be fixed.
		//for (unsigned i = 0; i < this->info.ySize; i++)
		//{
			//delete [] this->info.agentBrain[i];
				//free (this->info.agentBrain[i]);
			//this->info.agentBrain[i] = NULL;
		//}

		delete [] this->info.agentBrain;
			//free (this->info.agentBrain);
		this->info.agentBrain = NULL;

		delete this->info.agentFile;
		this->info.agentFile = NULL;

		#ifdef PrintMemoryLeak
			CHECK_HEAP();
		#endif
	}

	void MLAgent::SignalAgentTransfered()
	{
		sem_post(&this->agentTransferedSem);
	}

	void MLAgent::Notify(const ISubscription *subscription)
	{
		ISubscription *tempSubscription = const_cast<ISubscription *>(subscription);

		if (tempSubscription != NULL)
		{
			if (tempSubscription->GetSubscriptionType() == SubscriptionTypes::AGENT_DONE)
			{
				const AgentDoneEventPackage *agentDoneSubscription = static_cast<const AgentDoneEventPackage *>(subscription);
				
				if (agentDoneSubscription != NULL)
				{
					AgentDoneEventPackage *package = const_cast<AgentDoneEventPackage *>(agentDoneSubscription);

					if ((agentDoneSubscription->IsTransfered())&&
						(agentDoneSubscription->GetAgentId() == this->id)&&
						(package->GetSource() == this->nodeTransferedTo))
					{
						this->isGoalOnAnotherNode = agentDoneSubscription->HasFoundGoal();
						sem_post(&this->transferedAgentDoneSem);						
					}
				}
			}
		}
	}

	void MLAgent::SetMaze(int xMazeSize, int yMazeSize, char **maze)
	{
		this->info.xSize = xMazeSize;             
		this->info.ySize = yMazeSize;    
		this->info.maze = maze;
	}

	void MLAgent::SetAgentInfo(MazeAgentInfo agentInfo)
	{
		this->info.start = agentInfo.start;
		this->info.goal = agentInfo.goal;
		this->lastDir = static_cast<dirVal_t>(agentInfo.lastDir);
		this->currentPhsPathNum = agentInfo.phsPathNum;
		this->canNotifyWhenDone = agentInfo.canNotify;

		//this->exploreLocalFirst = this->isTransfered;
	}

	void MLAgent::SetAgentFile(char *newAgentFile)
	{
		this->info.agentFile = newAgentFile;
	}

	void MLAgent::Run()
	{
		this->info.agentID = this->id;	

		IEventDistributor *eventDist = EventDistributor::Instance();

		if (eventDist != NULL)
		{
			eventDist->Subscribe(this, SubscriptionTypes::AGENT_DONE, NotificationType::LONGLIVED, true);
		}
		/*if (!this->isTransfered)
		{
			IEventDistributor *eventDist = EventDistributor::Instance();

			if (eventDist != NULL)
			{
				eventDist->Subscribe(this, SubscriptionTypes::AGENT_DONE, NotificationType::LONGLIVED, true);
			}
		}*/

		ml(this->info);
	}

	void MLAgent::MoveTo(unsigned dest, char newStartLoc)
	{
		//TODO: Remove this once the original agent blocks 'til all threads have finished.
		//this->isTransfered = true;
		//this->isRegistered = false;		//This is needed so that the agent won't unregister itself upon destruction
		MazeAgentInfo agentInfo;
		agentInfo.agentId = this->id;
		agentInfo.start = newStartLoc;
		agentInfo.goal = this->info.goal;
		agentInfo.lastDir = static_cast<unsigned>(this->lastDir);
		agentInfo.phsPathNum = this->currentPhsPathNum;
		agentInfo.canNotify = true;

		this->nodeTransferedTo = dest;

		#ifdef DebugMLAgent
		if (!Validator::IsNull(log, NAME("log")))
		{
			log->Log(&typeid(this), "Agent: %d, transfered on physical path %d", this->id, this->currentPhsPathNum);
		}
		#endif

		if (this->msgDist != NULL)
		{
			this->msgDist->SendPackage(new MazeAgentPackage(this->id, dest, agentInfo));
		}

		sem_wait(&this->transferedAgentDoneSem);
		/*if (!this->isTransfered)
		{
			sem_wait(&this->transferedAgentDoneSem);
		}
		else
		{
			this->canNotifyWhenDone = false;
			this->isGoalOnAnotherNode = true;
		}*/
	}
}