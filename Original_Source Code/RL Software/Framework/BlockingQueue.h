/////////////////////////////////////////////////////////
//BlockingQueue.h
//Defines the BlockingQueue class.
//
//Manuel Madera
//5/8/09
/////////////////////////////////////////////////////////

#ifndef BlockingQueue_h
#define BlockingQueue_h

#include "Packages.h"
#include <list>
#include <semaphore.h>
#include "Framework.h"

using namespace std;

namespace Framework
{
	namespace Agents
	{
		template<class T>
		class BlockingQueue
		{
		protected:
			list<T *> queue;
			sem_t readySem;			//Puts thread to sleep if the queue is empty
			sem_t binSem;			//Protects the queue.

		public:
			BlockingQueue() 
			{ 
				sem_init(&readySem, 0, 0); 
				sem_init(&binSem, 0, 1);
			}

			virtual ~BlockingQueue() 
			{ 
				T *item = 0; 

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

			virtual T *Dequeue()
			{       
				sem_wait(&this->readySem);				

				T *item = 0;

				sem_wait(&this->binSem);
				if (queue.size() > 0)
				{					
					item = queue.front();
					queue.pop_front();					
				}
				sem_post(&this->binSem);

				return item;
			}

			virtual void Enqueue(T *item)
			{
				sem_wait(&this->binSem);
				queue.push_back(item);
				sem_post(&this->binSem);
				sem_post(&this->readySem);
			}
		};
	}
}
#endif	//BlockingQueue_h