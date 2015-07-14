/////////////////////////////////////////////////////////
//Communicator.h
//Defines the ICommunicator interface.  It simply hides
//the communication layer.
//
//Manuel Madera
//7/4/09
/////////////////////////////////////////////////////////

#ifndef Communicator_h
#define Communicator_h

#include "ICommunicator.h"
#include <semaphore.h>
#include "Framework.h"
#include "LogManager.h"

namespace Framework
{
	namespace Communications
	{
		class FRAMEWORK_API Communicator : public ICommunicator
		{
		private:
			int sendPosition;
			int recvPosition;
			int *messageRequest;
			bool cancel;
			sem_t cancelSem;
			Logging::ILog *log;
		public:
			Communicator();
			virtual ~Communicator();
	                                
			static void Synch();
			void SendPacked(void *buf, int destination, unsigned messageTag, CommType::Type commType);
			bool GetPacked(int bufSize, CommType::Type commType, /*out*/void *buf);
			bool GetPacked(int bufSize, CommType::Type commType, unsigned source, unsigned messageTag, /*out*/void *buf);

			virtual void CancelASynch();

			void Pack(void *inData, int inCount, DataType::Type type, void *buf, int bufSize);
			void Unpack(void *buf, int bufSize, DataType::Type type, int outCount, /*out*/void *data);
		};
	}
}

#endif  //Communicator_h