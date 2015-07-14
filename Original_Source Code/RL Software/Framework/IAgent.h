/////////////////////////////////////////////////////////
//IAgent.h
//Defines the IAgent interface.
//
//Manuel Madera
//5/8/09
/////////////////////////////////////////////////////////

#ifndef IAgent_h
#define IAgent_h

#include "Packages.h"
#include <list>
#include <semaphore.h>
#include "Framework.h"
#include "Debug.h"

using namespace std;

namespace Framework
{
	namespace Agents
	{
		class FRAMEWORK_API AgentQueue
		{
		protected:
			list<Packages::IPackage *> queue;
			sem_t readySem;			//Puts thread to sleep if the queue is empty
			sem_t binSem;			//Protects the queue.

		public:
			AgentQueue() 
			{ 
				sem_init(&readySem, 0, 0); 
				sem_init(&binSem, 0, 1);
			}

			virtual ~AgentQueue() 
			{ 
				Packages::IPackage *item = 0; 

				sem_wait(&this->binSem);
				while (queue.size() > 0)
				{
					sem_wait(&this->readySem);
					item = queue.front();
					queue.pop_front();

					delete item;
					item = NULL;
				}
				sem_post(&this->binSem);

				queue.clear();

				sem_destroy(&this->readySem);
				sem_destroy(&this->binSem);
			}

			virtual unsigned Size()
			{
				return queue.size();
			}

			virtual Packages::IPackage *Dequeue()
			{       
				sem_wait(&this->binSem);

				Packages::IPackage *item = 0;

				if (queue.size() > 0)
				{					
					sem_wait(&this->readySem);
					item = queue.front();
					queue.pop_front();					
				}

				sem_post(&this->binSem);
				return item;
			}

			virtual void Enqueue(Packages::IPackage *item)
			{
				sem_wait(&this->binSem);
				queue.push_back(item);
				sem_post(&this->binSem);
				sem_post(&this->readySem);
			}
		};

		class FRAMEWORK_API IAgent
		{
		public:
			virtual ~IAgent() {}
	                
			virtual void Run() = 0;
			virtual bool IsRegistered() = 0;
			virtual bool IsTransfered() = 0;

			//Register with the local MessageDistributor, which in turn will register globally
			virtual void Register() = 0;		
			virtual void RegisterTransfer(unsigned id) = 0;
			virtual void SetAgentUI(unsigned id) = 0;
			virtual unsigned GetAgentUI() = 0;
			virtual void SendPackage(Packages::IPackage *package) = 0;
		};   
	}
}

#endif  //IAgent_h