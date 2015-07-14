/////////////////////////////////////////////////////////
//RLAgent.h
//Defines the RLAgent class.  This agent encapsulates all
//the reenforcement learning code.
//
//Manuel Madera
//7/22/09
/////////////////////////////////////////////////////////

#ifndef RLAgent_h
#define RLAgent_h

#include "Agent.h"
#include <pthread.h>
#include "Framework.h"

namespace Framework
{
	namespace Agents
	{
		class FRAMEWORK_API RLAgent : public Agent
			{
			private:
				pthread_t mainAgentThread;

			public:
				RLAgent();
				RLAgent(unsigned packageNode, unsigned packageDest);

				virtual ~RLAgent();

				virtual void Run();

			protected:
				//virtual void Init();

				//virtual void Send(unsigned destRank, unsigned messageTag);
				//virtual void Get(unsigned sourceRank, unsigned messageTag);

				//virtual void Serialize();

				//virtual void Deserialize();
			private:
				static void *MainThread(void *arg);
			};
	}
}

#endif	//RLAgent_h