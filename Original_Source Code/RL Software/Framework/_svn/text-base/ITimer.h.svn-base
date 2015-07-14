/////////////////////////////////////////////////////////
//INode.h
//Defines the ITimer interface.
//
//Manuel Madera
//7/25/09
/////////////////////////////////////////////////////////

#ifndef ITimer_h
#define ITimer_h

#include "Framework.h"
#include "Debug.h"
#include <iostream>

using namespace std;

namespace Framework
{
	namespace Timers
	{
		class FRAMEWORK_API ITimer
		{
		public:
			virtual ~ITimer() {}
	        
			virtual void Start() = 0;

			virtual void Stop() = 0;

			virtual double GetElapsed() = 0;
		};

		//class FRAMEWORK_API DummyTimer : public ITimer
		//{
		//public:
		//	DummyTimer() {}
		//	virtual ~DummyTimer() {}
	 //       
		//	void Start()
		//	{
		//		#ifdef Debug
		//			cout << "Dummy Timer started" << endl;
		//		#endif
		//	}

		//	void Stop()
		//	{
		//		#ifdef Debug
		//			cout << "Dummy Timer stopped" << endl;
		//		#endif
		//	}

		//	double GetElapsed()
		//	{
		//		#ifdef Debug
		//			cout << "Dummy Timer Get elapsed" << endl;
		//		#endif

		//		return 1.0;
		//	}
		//};
	}
}

#endif  //ITimer_h