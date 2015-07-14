/////////////////////////////////////////////////////////
//Communicator.cpp
//Defines the ICommunicator interface.  It simply hides
//the communication layer.
//
//Manuel Madera
//7/4/09
/////////////////////////////////////////////////////////

#include "Communicator.h"
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
		Communicator::Communicator() : sendPosition(0), recvPosition(0), messageRequest(0), cancel(false)
		{
			sem_init(&cancelSem, 0, 1);
			this->log = LogManager::Instance();
		}

		Communicator::~Communicator()
		{
			sem_destroy(&this->cancelSem);

			delete this->messageRequest; 
			this->messageRequest = NULL;
		}
	                            
		void Communicator::SendPacked(void *buf, int destination, unsigned messageTag, CommType::Type commType)
		{
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
		}

		/*static*/ void Communicator::Synch()
		{
			//sleep(7);
			MPI_Barrier(MPI_COMM_WORLD);
		}

		bool Communicator::GetPacked(int bufSize, CommType::Type commType, unsigned source, unsigned messageTag, /*out*/void *buf)
		{
			MPI_Status status;
			MPI_Request *request = 0;
			int flag;

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

				//This is necessary to cancel
				sem_wait(&this->cancelSem);
				this->messageRequest = request;

				if (this->cancel)
				{
					if (!Validator::IsNull(request, NAME("request")))
					{
						MPI_Cancel(request);
					}
				}
				sem_post(&this->cancelSem);
				
				if (!Validator::IsNull(request, NAME("request")))
				{
					MPI_Wait(request, &status);

					MPI_Test_cancelled( &status, &flag );            
				}

				sem_wait(&this->cancelSem);
				delete request; 
				request = NULL;

				this->messageRequest = 0;
				sem_post(&this->cancelSem);
			}

			#ifdef Debug
			if (!Validator::IsNull(log, NAME("log")))
			{
				log->Log(&typeid(this), "Comunicator::GetPacked: source: %u, Tag: %d", source, messageTag);
			}
			#endif

			return (flag != 0);
		}

		bool Communicator::GetPacked(int bufSize, CommType::Type commType, /*out*/void *buf)
		{
			return GetPacked(bufSize, commType, MPI_ANY_SOURCE, MPI_ANY_TAG, buf);
		}

		void Communicator::CancelASynch()
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

		void Communicator::Pack(void *inData, int inCount, DataType::Type type, void *buf, int bufSize)
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

		void Communicator::Unpack(void *buf, int bufSize, DataType::Type type, int outCount, /*out*/void *data)
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