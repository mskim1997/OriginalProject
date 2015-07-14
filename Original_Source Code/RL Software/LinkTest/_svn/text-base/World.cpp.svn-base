#include "World.h"
#include "LinkNode.h"
#include "Framework/RootNode.h"
#include "LinkedAgent.h"
#include "Framework/LogManager.h"
#include "Framework/Communicator.h"
#include "MainAgent.h"

#ifdef WIN32
#include <windows.h>
#endif

using namespace Framework;
using namespace Framework::Agents;
using namespace Framework::Logging;
using namespace Framework::Communications;

#define ROOT_NODE 0

#define KP				(double)0.5
#define LINK			(double)0.2
#define STEP			(double)0.3//0.05//0.005//0.003
#define ABSALON			(double)3.5

#define AGENT_1_X_POS	(double)1
#define AGENT_1_Y_POS	(double)6

#define AGENT_2_X_POS	(double)2
#define AGENT_2_Y_POS	(double)7

#define AGENT_3_X_POS	(double)3
#define AGENT_3_Y_POS	(double)8

#define AGENT_4_X_POS	(double)4
#define AGENT_4_Y_POS	(double)9

#define AGENT_5_X_POS	(double)5
#define AGENT_5_Y_POS	(double)10

#define AGENT_1_X_FINAL_POS	(double)10
#define AGENT_1_Y_FINAL_POS	(double)60

#define AGENT_2_X_FINAL_POS	(double)20
#define AGENT_2_Y_FINAL_POS	(double)70

#define AGENT_3_X_FINAL_POS	(double)30
#define AGENT_3_Y_FINAL_POS	(double)80

#define AGENT_4_X_FINAL_POS	(double)40
#define AGENT_4_Y_FINAL_POS	(double)90

#define AGENT_5_X_FINAL_POS	(double)50
#define AGENT_5_Y_FINAL_POS	(double)100

namespace WorldTest
{
    World::World()
    {
    }
    
    World::~World()
    {
    }
    
    void World::Run(unsigned rank, int size)
    {
		INode *node = NULL;
		static_cast<LogManager *>(LogManager::Instance())->Init(rank, Logging::FILE);
		ILog *log = LogManager::Instance();	
		Position<double> initPos(0, 0);
		Position<double> finalPos(0, 0);

		if (rank == 1)
		{
			initPos.SetXCoor(AGENT_1_X_POS);
			initPos.SetYCoor(AGENT_1_Y_POS);

			finalPos.SetXCoor(AGENT_1_X_FINAL_POS);
			finalPos.SetYCoor(AGENT_1_Y_FINAL_POS);
		}
		else if (rank == 2)
		{
			initPos.SetXCoor(AGENT_2_X_POS);
			initPos.SetYCoor(AGENT_2_Y_POS);

			finalPos.SetXCoor(AGENT_2_X_FINAL_POS);
			finalPos.SetYCoor(AGENT_2_Y_FINAL_POS);
		}
		else if (rank == 3)
		{
			initPos.SetXCoor(AGENT_3_X_POS);
			initPos.SetYCoor(AGENT_3_Y_POS);

			finalPos.SetXCoor(AGENT_3_X_FINAL_POS);
			finalPos.SetYCoor(AGENT_3_Y_FINAL_POS);
		}
		else if (rank == 4)
		{
			initPos.SetXCoor(AGENT_4_X_POS);
			initPos.SetYCoor(AGENT_4_Y_POS);

			finalPos.SetXCoor(AGENT_4_X_FINAL_POS);
			finalPos.SetYCoor(AGENT_4_Y_FINAL_POS);
		}
		else
		{
			initPos.SetXCoor(AGENT_5_X_POS);
			initPos.SetYCoor(AGENT_5_Y_POS);

			finalPos.SetXCoor(AGENT_5_X_FINAL_POS);
			finalPos.SetYCoor(AGENT_5_Y_FINAL_POS);
		}

		if (rank == ROOT_NODE)
		{
#ifdef DebugLinkedAgent
			log->Log(&typeid(this), "Node: %d, Creating main agent ...", rank);
#endif
			node = new RootNode(rank, size);

			if (node != NULL)
			{
#ifdef DebugLinkedAgent
				log->Log(&typeid(this), "Node: %d, Creating main agent ...", userInput.rank);
#endif
				IAgent *agent = new MainAgent(rank, 0, (size - 1), 1, node->GetMessageDistributor(), node->GetEventDistributor());

#ifdef DebugLinkedAgent
				log->Log(&typeid(this), "Node: %d, Adding agent ...", userInput.rank);
#endif
				node->AddAgent(agent);

#ifdef DebugLinkedAgent
				log->Log(&typeid(this), "Node: %d, Waiting for other nodes ...", rank);
#endif

#ifdef DebugLinkedAgent
				log->Log(&typeid(this), "Node: %d, Running ...", rank);
#endif
				node->Run();

#ifdef DebugLinkedAgent
				log->Log(&typeid(this), "Node: %d, Waiting ...", rank);
#endif
				node->Wait();

#ifdef DebugLinkedAgent
				log->Log(&typeid(this), "Node: %d, Terminating ...", rank);
#endif
				node->Terminate();

#ifdef DebugLinkedAgent
				log->Log(&typeid(this), "Node: %d, Deleting ...", rank);
#endif
				delete node; 
				node = NULL;

#ifdef DebugLinkedAgent
				log->Log(&typeid(this), "Node: %d, Done ...", rank);
#endif
			}
        }
		else
		{
#ifdef DebugLinkedAgent
			log->Log(&typeid(this), "Node: %d, Creating Node ...", rank);
#endif
			node = new LinkNode(rank, true);

			IAgent *agent = new LinkedAgent(rank, 0, size, initPos, finalPos, KP, LINK, STEP, ABSALON);

			if (node != NULL)
			{
#ifdef DebugLinkedAgent
				log->Log(&typeid(this), "Node: %d, Waiting for other nodes ...", rank);
#endif

#ifdef DebugLinkedAgent
				log->Log(&typeid(this), "Node: %d, Adding Agents ...", rank);
#endif
				node->AddAgent(agent);			

#ifdef DebugLinkedAgent
				log->Log(&typeid(this), "Node: %d, Running ...", rank);
#endif
				node->Run();

#ifdef DebugLinkedAgent
				log->Log(&typeid(this), "Node: %d, Waiting ...", rank);
#endif
				node->Wait();

#ifdef DebugLinkedAgent
				log->Log(&typeid(this), "Node: %d, Terminating ...", rank);
#endif
				node->Terminate();

#ifdef DebugLinkedAgent
				log->Log(&typeid(this), "Node: %d, Deleting ...", rank);
#endif
				delete node; 
				node = NULL; 

#ifdef DebugLinkedAgent
				log->Log(&typeid(this), "Node: %d, Done ...", rank);
#endif
			}
		}

		static_cast<LogManager *>(LogManager::Instance())->Shutdown();
    }
}
    
    