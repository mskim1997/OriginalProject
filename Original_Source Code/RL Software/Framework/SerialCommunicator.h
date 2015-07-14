/////////////////////////////////////////////////////////
//SerialCommunicator.h
//ICommunicator implementation that hides the communication layer
//and performs sends and receives serially.
//
//Manuel Madera
//4/3/10
/////////////////////////////////////////////////////////

#ifndef SerialCommunicator_h
#define SerialCommunicator_h

#include "ICommunicator.h"
#include <semaphore.h>
#include "Framework.h"
#include "LogManager.h"

namespace Framework
{
	namespace Communications
	{
		class FRAMEWORK_API SerialCommunicator : public ICommunicator
		{
		private:
			int sendPosition;
			int recvPosition;
			int *messageRequest;
			bool cancel;
			sem_t cancelSem;
			sem_t binSem;
			bool isWaitingForPackage;
			Logging::ILog *log;
		public:
			SerialCommunicator();
			virtual ~SerialCommunicator();
	                            
			void SendPacked(void *buf, int destination, unsigned messageTag, CommType::Type commType);
			bool GetPacked(int bufSize, CommType::Type commType, /*out*/void *buf);
			bool GetPacked(int bufSize, CommType::Type commType, unsigned source, unsigned messageTag, /*out*/void *buf);

			virtual void CancelASynch();

			void Pack(void *inData, int inCount, DataType::Type type, void *buf, int bufSize);
			void Unpack(void *buf, int bufSize, DataType::Type type, int outCount, /*out*/void *data);
		};
	}
}

#endif  //SerialCommunicator_h