/////////////////////////////////////////////////////////
//ICommunicator.h
//Defines the ICommunicator interface.  It simply hides
//the communication layer.
//
//Manuel Madera
//7/4/09
/////////////////////////////////////////////////////////

#ifndef ICommunicator_h
#define ICommunicator_h

#include "Framework.h"

namespace Framework
{
	namespace Communications
	{
		namespace DataType
		{
			enum Type
			{
				Int = 0,
				DOUBLE
			};
		}
	        
		namespace CommType
		{
			enum Type
			{
				Synch = 0,
				ASynch
			};
		}

		class FRAMEWORK_API ICommunicator
		{
		public:
			virtual ~ICommunicator() {}
	                                
			virtual void SendPacked(void *buf, int destination, unsigned messageTag, CommType::Type commType) = 0;
			virtual bool GetPacked(int bufSize, CommType::Type commType, /*out*/void *buf) = 0;
			virtual bool GetPacked(int bufSize, CommType::Type commType, unsigned source, unsigned messageTag, /*out*/void *buf) = 0;

			virtual void CancelASynch() = 0;

			virtual void Pack(void *inData, int inCount, DataType::Type type, void *buf, int bufSize) = 0;
			virtual void Unpack(void *buf, int bufSize, DataType::Type type, int outCount, /*out*/void *data) = 0;
		};
	}
}

#endif  //ICommunicator_h