/////////////////////////////////////////////////////////
//LogManager.cpp
//Responsible for handling log messages.
//
//Manuel Madera
//1/25/10
/////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include "LogManager.h"
#include "ConsoleLogger.h"
#include "FileLogger.h"
#include <sstream>

#define MAX_MESSAGE_SIZE 10

namespace Framework
{
	namespace Logging
	{
		ILog *LogManager::instance = 0;

		LogManager::LogManager() : isInitialized(false), rank(0), stop(false), logger(0)
		{
			sem_init(&binSem, 0, 1);
			sem_init(&canDeleteSem, 0, 1);

			for (unsigned i = 0; i < MAX_MESSAGE_SIZE; i++)
			{
				this->messagePool.Enqueue(new Message());
			}
		}

		LogManager::~LogManager()
		{
			sem_wait(&this->canDeleteSem);

			delete this->logger; 
			this->logger = NULL;

			unsigned poolSize = this->messagePool.Size();

			Message *message = NULL;

			for (unsigned i = 0; i < poolSize; i++)
			{
				message = this->messagePool.Dequeue();

				delete message; 
				message = NULL;
			}

			pthread_cancel(this->loggingThread);

			sem_destroy(&this->binSem);
			sem_destroy(&this->canDeleteSem);
		}

		void LogManager::Init(unsigned rank, LoggerType loggerType)
		{
			if (!this->isInitialized)
			{
				int rc;			

				string number;
				stringstream out;
				string outputFile;

				this->isInitialized = true;

				this->rank = rank;

				switch (loggerType)
				{
				case FILE:
					out << rank;
					number = out.str();

					out.str("");
					out.flush();

					outputFile = "Logging." + number + ".txt";
					this->logger = new FileLogger(outputFile);
					break;
				case CONSOLE:
				default:
					this->logger = new ConsoleLogger();
					break;
				}

				rc = pthread_create(&this->loggingThread, NULL, LogManager::LoggingThread, (void *)this);
			}
		}

		/*static*/ ILog *LogManager::Instance()
		{
			if (instance == 0)
			{
				instance = new LogManager();
			}

			return instance;
		}

		void LogManager::Shutdown()
		{
			stop = true;
			this->messageQueue.Enqueue(NULL);

			delete LogManager::instance;
			LogManager::instance = NULL;
		}

		void LogManager::Log(const type_info *component, string formattedMessage, ...)
		{						
			Message *message = this->messagePool.Dequeue();

			if (message != NULL)
			{
				if (component != NULL)
				{
					message->SetComponentName(component->name());
				}

				sem_wait(&this->binSem);
				va_list args;
				va_start (args, formattedMessage);
				vsprintf (message->GetMessage(), formattedMessage.c_str(), args);
				this->messageQueue.Enqueue(message);//FormatMessage(component->name(), buffer));
				va_end (args);
				sem_post(&this->binSem);
			}
		}

		void LogManager::Log(const type_info *component, char *format, ...)
		{
			Message *message = this->messagePool.Dequeue();

			if (message != NULL)
			{
				if (component != NULL)
				{
					message->SetComponentName(component->name());
				}

				sem_wait(&this->binSem);
				va_list args;
				va_start (args, format);
				vsprintf (message->GetMessage(), format, args);
				this->messageQueue.Enqueue(message);//FormatMessage(component->name(), buffer));
				va_end (args);
				sem_post(&this->binSem);
			}
		}

		string LogManager::FormatMessage(string componentName, char *message)
		{			
			time_t ltime;		/* calendar time */  
			ltime = time(NULL);	/* get current cal time */
			char *timeStamp = asctime( localtime(&ltime) );
			string logMessage = "[" + componentName + "] " + message + ", " + timeStamp;
			
			return logMessage;
		}

		/*static*/ void *LogManager::LoggingThread(void *arg)
		{
			LogManager* pThis = static_cast<LogManager *>(arg);
			Message *message = NULL;

			if (pThis != NULL)
			{
				while (!pThis->stop)
				{
					sem_wait(&pThis->canDeleteSem);
					message = pThis->messageQueue.Dequeue();

					if (message != NULL)
					{
						if (pThis->logger != NULL)
						{
							pThis->logger->Log(pThis->FormatMessage(message->GetComponentName(), message->GetMessage()));
						}

						pThis->messagePool.Enqueue(message);
						message = NULL;
					}
					sem_post(&pThis->canDeleteSem);
				}
			}

			delete message; 
			message = NULL;

	#ifdef LINUX
			pthread_exit(NULL);
	#endif

	#ifdef WIN32
			return NULL;
	#endif
		}
	}
}