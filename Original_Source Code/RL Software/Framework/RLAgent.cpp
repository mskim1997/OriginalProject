/////////////////////////////////////////////////////////
//RLAgent.cpp
//Defines the RLAgent class.  This agent encapsulates all
//the reenforcement learning code.
//
//Manuel Madera
//7/22/09
/////////////////////////////////////////////////////////

#include "RLAgent.h"
#include "Communicator.h"
#include "MessageDistributor.h"
#include "Debug.h"

#ifdef PrintMemoryLeak
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif
#endif

namespace Framework
{
	namespace Agents
	{
		RLAgent::RLAgent() : Agent()
		{
		}

		RLAgent::RLAgent(unsigned packageNode, unsigned packageDest)
			: Agent(packageNode, packageDest)
		{ 		
		}

		RLAgent::~RLAgent() 
		{
		}

		void RLAgent::Run() 
		{
			int rc;
	        
			rc = pthread_create(&this->mainAgentThread, NULL, RLAgent::MainThread, (void *)this);

			pthread_join(this->mainAgentThread, NULL);
		}

		/*static*/ void *RLAgent::MainThread(void *arg)
		{
			RLAgent *pThis = static_cast<RLAgent *>(arg);

			#ifdef LINUX
				pthread_exit(NULL);
			#endif

	#ifdef WIN32
			return NULL;
	#endif
		}
	}
}