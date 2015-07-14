/////////////////////////////////////////////////////////
//UnitTest.h
//Defines the UnitTest class, which contains the framework unit test.
//
//Manuel Madera
//2/20/10
/////////////////////////////////////////////////////////

#ifndef UnitTest_h
#define UnitTest_h

#include "Framework/LogManager.h"

namespace Testing
{
    class UnitTest
    {     
	private:
		Framework::Logging::ILog *log;

    public:
        UnitTest();
        virtual ~UnitTest();
        
        void Run(unsigned rank, int size);

	private:
		void TestPosition(unsigned rank);
		void TestCommunicator(unsigned rank);
		void TestPackages(unsigned rank);
		void TestPackagers(unsigned rank);
		void TestEventDistributor(unsigned rank);
		void TestMessageDistributor(unsigned rank);
		void TestRemoteSubscription(unsigned rank);
		void TestNodes(unsigned rank);
		void TestTestException(unsigned rank);
		void TestValidator(unsigned rank);
    };
}
    
#endif  //UnitTest_h