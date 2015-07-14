/////////////////////////////////////////////////////////
//LogManager.h
//Responsible for handling log messages.
//
//Manuel Madera
//1/25/10
/////////////////////////////////////////////////////////

#ifndef LogManager_h
#define LogManager_h

#include "Framework.h"
#include "ILog.h"
#include <semaphore.h>
#include "ILogger.h"
#include <pthread.h>
#include "BlockingQueue.h"

#define MAX_BUFFER_SIZE 10000

namespace Framework
{
	namespace Logging
	{
		enum LoggerType
		{
			CONSOLE,
			FILE
		};

		class FRAMEWORK_API Message
		{
		private:
			string componentName;
			char *message;

		public:
			Message() : message(NULL)
			{
				message = new char[MAX_BUFFER_SIZE];
			}

			virtual ~Message()
			{
				delete [] this->message;
				this->message = NULL;
			}

			char *GetMessage()
			{
				return this->message;
			}

			string GetComponentName()
			{
				return this->componentName;
			}

			void SetComponentName(string newName)
			{
				this->componentName = newName;
			}
		};

		class FRAMEWORK_API LogManager : public ILog
		{
		private:
			unsigned rank;
			bool isInitialized;
			sem_t binSem;
			sem_t canDeleteSem;
			static ILog *instance;
			ILogger *logger;
			Agents::BlockingQueue<Message> messagePool;
			Agents::BlockingQueue<Message> messageQueue;
			bool stop;

			pthread_t loggingThread;

			LogManager();

		public:
			virtual ~LogManager();

			//This method can only be called once
			void Init(unsigned rank, LoggerType loggerType);

			//No need to be thread-safe since it is called once
			//at the beginning of each node.
			static ILog *Instance();

			//This method can only be called once
			void Shutdown();        

			virtual void Log(const type_info *component, string formattedMessage, ...);

			virtual void Log(const type_info *component, char *format, ...);

		private:
			virtual string FormatMessage(string componentName, char *message);

			static void *LoggingThread(void *arg);
		};
	}
}

#endif  //LogManager_h