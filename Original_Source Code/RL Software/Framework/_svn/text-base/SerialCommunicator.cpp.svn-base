/////////////////////////////////////////////////////////
//SerialCommunicator.h
//ICommunicator implementation that hides the communication layer
//and performs sends and receives serially.
//
//Manuel Madera
//4/3/10
/////////////////////////////////////////////////////////

#include "SerialCommunicator.h"
#ifdef WIN32
#include <mpi.h>
#include <windows.h>

#define sleep(x) Sleep(x*1000)
#endif

#include "Debug.h"

#ifdef PrintMemoryLeak
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif
#endif

using namespace std;
using namespace Framework::Logging;

namespace Framework
{
	namespace Communications
	{
		SerialCommunicator::SerialCommunicator() : sendPosition(0), recvPosition(0), messageRequest(0), cancel(false)
		{
			sem_init(&cancelSem, 0, 1);
			sem_init(&binSem, 0, 1);

			this->log = LogManager::Instance();
			this->isWaitingForPackage = false;
		}

		SerialCommunicator::~SerialCommunicator()
		{
			sem_destroy(&this->cancelSem);
			sem_destroy(&this->binSem);

			delete this->messageRequest; 
			this->messageRequest = NULL;
		}
	                            
		void SerialCommunicator::SendPacked(void *buf, int destination, unsigned messageTag, CommType::Type commType)
		{
			sem_wait(&this->binSem);
			if (commType == CommType::Synch)
			{
				#ifdef Debug
				if (!Validator::IsNull(log, NAME("log")))
				{
					log->Log(&typeid(this), "Comunicator::SendPacked: dest: %d, Tag: %d", destination, messageTag);
				}
				#endif

				MPI_Send(buf, this->sendPosition, MPI_PACKED, destination, messageTag, MPI_COMM_WORLD);

				this->sendPosition = 0;
			}
			else
			{
				//ASynch send must be implemented if needed.
			}
			sem_post(&this->binSem);
		}

		bool SerialCommunicator::GetPacked(int bufSize, CommType::Type commType, unsigned source, unsigned messageTag, /*out*/void *buf)
		{
			MPI_Status status;
			MPI_Request *request = 0;
			int flag;

			do
			{
				sem_wait(&this->binSem);
				if (!this->isWaitingForPackage)
				{
					if (commType == CommType::Synch)
					{            
						MPI_Recv(buf, bufSize, MPI_PACKED, source, messageTag, MPI_COMM_WORLD, &status); 

						this->recvPosition = 0;

						flag = 0;
					}
					else
					{
						request = new MPI_Request();

						MPI_Irecv(buf, bufSize, MPI_PACKED, source, /*messageTag*/messageTag, MPI_COMM_WORLD, request);

						this->recvPosition = 0;					

						sem_wait(&this->cancelSem);
						this->messageRequest = request;
						sem_post(&this->cancelSem);

						this->isWaitingForPackage = true;           
					}
				}
				else
				{
					sem_wait(&this->cancelSem);
					if (this->cancel)
					{
						if (!Validator::IsNull(request, NAME("request")))
						{
							MPI_Cancel(request);
						}
					}
					sem_post(&this->cancelSem);					

					//Check if the request completed.
					if (!Validator::IsNull(request, NAME("request")))
					{
						MPI_Test(request, &flag, &status);
					}

					if (flag != 0)
					{
						sem_wait(&this->cancelSem);
						delete request; 
						request = NULL;

						this->messageRequest = 0;
						sem_post(&this->cancelSem);

						this->isWaitingForPackage = false;
					}

					MPI_Test_cancelled( &status, &flag );
				}
				sem_post(&this->binSem);

				if (this->isWaitingForPackage)
				{
					#ifdef Debug
					if (!Validator::IsNull(log, NAME("log")))
					{
						log->Log(&typeid(this), "Waiting for package ...");
					}
					#endif

					sleep(3);
				}

			}while(this->isWaitingForPackage);

			#ifdef Debug
			if (!Validator::IsNull(log, NAME("log")))
			{
				log->Log(&typeid(this), "Comunicator::GetPacked: source: %u, Tag: %d", source, messageTag);
			}
			#endif

			return (flag != 0);
		}

		bool SerialCommunicator::GetPacked(int bufSize, CommType::Type commType, /*out*/void *buf)
		{
			return GetPacked(bufSize, commType, MPI_ANY_SOURCE, MPI_ANY_TAG, buf);
		}

		void SerialCommunicator::CancelASynch()
		{
			sem_wait(&this->cancelSem);			
			if (!Validator::IsNull(this->messageRequest, NAME("this->messageRequest")))
			{	
				MPI_Cancel(this->messageRequest);
			}
			else
			{
				cancel = true;
			}
			sem_post(&this->cancelSem);
		}

		void SerialCommunicator::Pack(void *inData, int inCount, DataType::Type type, void *buf, int bufSize)
		{
			MPI_Datatype mpiType;

			switch (type)
			{
			case DataType::DOUBLE:
				mpiType = MPI_DOUBLE;
				break;
			case DataType::Int:     //Intentional
			default:
				mpiType = MPI_INT;
				break;
			}

			MPI_Pack(inData, inCount, mpiType, buf, bufSize, &this->sendPosition, MPI_COMM_WORLD);
		}

		void SerialCommunicator::Unpack(void *buf, int bufSize, DataType::Type type, int outCount, /*out*/void *data)
		{
			MPI_Datatype mpiType;

			switch (type)
			{
			case DataType::DOUBLE:
				mpiType = MPI_DOUBLE;
				break;
			case DataType::Int:     //Intentional
			default:
				mpiType = MPI_INT;
				break;
			}        

			MPI_Unpack(buf, bufSize, &this->recvPosition, data, outCount, mpiType, MPI_COMM_WORLD);
		}
	}
}