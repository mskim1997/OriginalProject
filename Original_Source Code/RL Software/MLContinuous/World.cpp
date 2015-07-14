//#ifndef	_CRTDBG_MAP_ALLOC
//#endif
//#include <stdlib.h>
//#include <crtdbg.h>

#include "World.h"
#include "MLNode.h"
#include "Framework/RootNode.h"
#include "MLAgent.h"
#include "Framework/Debug.h"
#include "MainAgent.h"
#include "Framework/LogManager.h"
#include "Framework/Communicator.h"
#include <pthread.h>

#ifdef WIN32
#include <windows.h>
#endif

#ifdef PrintMemoryLeak
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif
#endif

#include <iostream>

using namespace std;
using namespace Framework;
using namespace Framework::Agents;
using namespace Framework::Logging;
using namespace Framework::Communications;

#define ROOT_NODE 0

namespace MLContinuous
{
    World::World()
    {
		//Subscribe to MazePackageReceivedEvent
    }
    
    World::~World()
    {
    }
    
    void World::Run(UserInput userInput)
    {
		INode *node = NULL;
		static_cast<LogManager *>(LogManager::Instance())->Init(userInput.rank, Logging::FILE);
		ILog *log = LogManager::Instance();		

		try
		{
			if (userInput.rank == ROOT_NODE)
			{
				node = new RootNode(userInput.rank, userInput.size);
				char *maze = const_cast<char *>(userInput.mazeFile.c_str());

				if (node != NULL)
				{
					#ifdef DebugMLAgent
						log->Log(&typeid(this), "Node: %d, Creating main agent ...", userInput.rank);
					#endif
					IAgent *agent = new MainAgent(userInput.rank, 0, (userInput.size - 1), userInput.numAgents, userInput.mazeXSize, userInput.mazeYSize, maze, userInput.statsFile, userInput.agentFile, node->GetMessageDistributor(), node->GetEventDistributor());

					#ifdef DebugMLAgent
						log->Log(&typeid(this), "Node: %d, Adding agent ...", userInput.rank);
					#endif
					node->AddAgent(agent);				

					#ifdef DebugMLAgent
						log->Log(&typeid(this), "Node: %d, Running ...", userInput.rank);
					#endif
					node->Run();  

					#ifdef DebugMLAgent
						log->Log(&typeid(this), "Node: %d, Waiting ...", userInput.rank);
					#endif
					node->Wait();

					#ifdef DebugMLAgent
						log->Log(&typeid(this), "Node: %d, Terminating ...", userInput.rank);
					#endif
					node->Terminate();
		            
					#ifdef DebugMLAgent
						log->Log(&typeid(this), "Node: %d, Deleting ...", userInput.rank);
					#endif
					delete node; 
					node = NULL;

					#ifdef DebugMLAgent
						log->Log(&typeid(this), "Node: %d, Done ...", userInput.rank);
					#endif
				}
			}
			else
			{
				//Wait on a semaphore that's released when the MazePackageReceivedEvent 
				//is fired.
				#ifdef DebugMLAgent
					log->Log(&typeid(this), "Node: %d, Creating Node ...", userInput.rank);
				#endif
				node = new MLNode(userInput.rank, userInput.allowSharing, userInput.slidingWindowLookAhead, userInput.minRadius, userInput.minTurningAngle, userInput.immediateMoves, userInput.runNumber);

				if (node != NULL)
				{
					#ifdef DebugMLAgent
						log->Log(&typeid(this), "Node: %d, Initializing ...", userInput.rank);
					#endif
					MLNode *tempNodePtr = dynamic_cast<MLNode *>(node);

					if (tempNodePtr != NULL)
					{
						tempNodePtr->Initialize();
					}

					#ifdef DebugMLAgent
						log->Log(&typeid(this), "Node: %d, Running ...", userInput.rank);
					#endif
					node->Run();			
		            
					#ifdef DebugMLAgent
						log->Log(&typeid(this), "Node: %d, Waiting ...", userInput.rank);
					#endif
					node->Wait();

					#ifdef DebugMLAgent
						log->Log(&typeid(this), "Node: %d, Terminating ...", userInput.rank);
					#endif
					node->Terminate();
		            
					#ifdef DebugMLAgent
						log->Log(&typeid(this), "Node: %d, Deleting ...", userInput.rank);
					#endif
					delete node; 
					node = NULL; 

					#ifdef DebugMLAgent
						log->Log(&typeid(this), "Node: %d, Done ...", userInput.rank);
					#endif
				}
			}	
		}
		catch(std::exception &e)
		{
			log->Log(&typeid(this), "Node: %d, threw the following exception %s", userInput.rank, e.what());

			if (node != NULL)
			{
				#ifdef DebugMLAgent
				if (log != NULL)
				{
					log->Log(&typeid(this), "Node: %d, Deleting ...", userInput.rank);
				}
				#endif
				delete node; 
				node = NULL; 
			}

			#ifdef DebugMLAgent
			if (log != NULL)
			{
				log->Log(&typeid(this), "Node: %d, Done ...", userInput.rank);
			}
			#endif
		}
		catch(...)
		{
			if (log != NULL)
			{
				log->Log(&typeid(this), "Node: %d, threw an unhandled exception", userInput.rank);
			}

			if (node != NULL)
			{
				#ifdef DebugMLAgent
				if (log != NULL)
				{
					log->Log(&typeid(this), "Node: %d, Deleting ...", userInput.rank);
				}
				#endif
				delete node; 
				node = NULL; 
			}

			#ifdef DebugMLAgent
			if (log != NULL)
			{
				log->Log(&typeid(this), "Node: %d, Done ...", userInput.rank);
			}
			#endif
		}

		static_cast<LogManager *>(LogManager::Instance())->Shutdown();
    }
}