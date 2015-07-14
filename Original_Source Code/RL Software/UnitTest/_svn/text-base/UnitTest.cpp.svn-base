/////////////////////////////////////////////////////////
//UnitTest.h
//Defines the UnitTest class, which contains the framework unit test.
//
//Manuel Madera
//2/20/10
/////////////////////////////////////////////////////////

#include "UnitTest.h"
#include "Framework/Nodes.h"
#include "Framework/RootNode.h"
#include "Framework/Communicator.h"
#include "Framework/Packages.h"
#include "TestFailedException.h"
#include "Framework/ASynchPackager.h"
#include "Framework/PackagerFactory.h"
#include "Framework/Validator.h"
#include "Framework/Position.h"

#ifdef WIN32
#include <windows.h>

#define sleep(x) Sleep(x*1000)
#endif

using namespace Framework;
using namespace Framework::Agents;
using namespace Framework::Logging;
using namespace Framework::Communications;
using namespace Framework::Packages;
using namespace Framework::Packagers;
using namespace Framework::Factories;

#define ROOT_NODE 0
#define COMM_BUF_SIZE 1000
#define MAX_PACKET_NUM 1000
#define DOUBLE_SIZE sizeof(double)

namespace Testing
{
    UnitTest::UnitTest()
    {		
    }
    
    UnitTest::~UnitTest()
    {	
		static_cast<LogManager *>(LogManager::Instance())->Shutdown();
    }
    
    void UnitTest::Run(unsigned rank, int size)
    {
		INode *node = NULL;	
		static_cast<LogManager *>(LogManager::Instance())->Init(rank, Logging::CONSOLE);
		this->log = LogManager::Instance();

		try
		{			
			this->log->Log(&typeid(this), "Testing Position ...");
			TestPosition(rank);

			/*this->log->Log(&typeid(this), "Testing the TestFailedException ...");
			TestTestException(rank);

			this->log->Log(&typeid(this), "Testing Validator ...");
			TestValidator(rank);

			this->log->Log(&typeid(this), "Testing Communicator ...");
			TestCommunicator(rank);

			this->log->Log(&typeid(this), "Testing Packages ...");
			TestPackages(rank);

			this->log->Log(&typeid(this), "Testing Packagers ...");
			TestPackagers(rank);

			this->log->Log(&typeid(this), "Testing Event Distributor ...");
			TestEventDistributor(rank);

			this->log->Log(&typeid(this), "Testing Message Distributor ...");
			TestMessageDistributor(rank);

			this->log->Log(&typeid(this), "Testing Remote Subscription ...");
			TestRemoteSubscription(rank);

			this->log->Log(&typeid(this), "Testing Nodes ...");
			TestNodes(rank);*/
		}
		catch (TestFailedException *e)
		{
			//printf("%s", e->what());
			delete e;
			e = NULL;
		}

		//static_cast<LogManager *>(LogManager::Instance())->Shutdown();
    }

	void UnitTest::TestPosition(unsigned rank)
	{
		Framework::Position<int> testIntPosition(0, 0);

		Framework::Position<int> testPosition = testIntPosition - Framework::Position<int>(10, 10);

		if (testPosition.GetXCoor() != -10)
		{
			throw new TestFailedException(MESSAGE("Substraction failed, x value was incorrect"));
		}

		if (testPosition.GetYCoor() != -10)
		{
			throw new TestFailedException(MESSAGE("Substraction failed, x value was incorrect"));
		}

		testIntPosition = testPosition * Framework::Position<int>(2, 2);

		if (testIntPosition.GetXCoor() != -20)
		{
			throw new TestFailedException(MESSAGE("Multiplication failed, x value was incorrect"));
		}

		if (testIntPosition.GetYCoor() != -20)
		{
			throw new TestFailedException(MESSAGE("Multiplication failed, x value was incorrect"));
		}

		testIntPosition = testIntPosition + 20;

		if (testIntPosition.GetXCoor() != 0)
		{
			throw new TestFailedException(MESSAGE("Addition failed, x value was incorrect"));
		}

		if (testIntPosition.GetYCoor() != 0)
		{
			throw new TestFailedException(MESSAGE("Addition failed, x value was incorrect"));
		}

		testPosition = Framework::Position<int>(5, -5) + testIntPosition;

		if (testPosition.GetXCoor() != 5)
		{
			throw new TestFailedException(MESSAGE("Addition failed, x value was incorrect"));
		}

		if (testPosition.GetYCoor() != -5)
		{
			throw new TestFailedException(MESSAGE("Addition failed, x value was incorrect"));
		}
	}

	void UnitTest::TestValidator(unsigned rank)
	{
		//Framework::Validator::IsNull(NULL, NAME("NULL"));

		int *testPtr = NULL;
		Framework::Validator::IsNull(testPtr, NAME("testPtr"));

		BasePackage *testObjPtr = 0;		
		Framework::Validator::IsNull(testObjPtr, NAME("testObjPtr"));
	}

	void UnitTest::TestTestException(unsigned rank)
	{
		try
		{
			throw new TestFailedException(MESSAGE("Testing the TestFailedException"));

			//If this line is reached the above exception failed.
			throw new std::exception("TestException failed");
		}
		catch (TestFailedException *e)
		{
			//If the exception was caught the test passed.
		}
	}

	void UnitTest::TestCommunicator(unsigned rank)
	{
		double buf[COMM_BUF_SIZE];
		int messageSize = COMM_BUF_SIZE*DOUBLE_SIZE;
		ICommunicator *comm = new Communicator();
		double testValue = 50.094302;
		unsigned tag = 1000;

		if (rank == ROOT_NODE)
		{
			for (int j = 0; j < MAX_PACKET_NUM; j++)
			{
				for (int i = 0; i < COMM_BUF_SIZE; i++)
				{
					comm->Pack(&testValue, 1, Framework::Communications::DataType::DOUBLE, buf, messageSize); 
				}

				comm->SendPacked(&buf, 1, tag, Framework::Communications::CommType::Synch);

				for (int i = 0; i < COMM_BUF_SIZE; i++)
				{
					comm->Pack(&testValue, 1, Framework::Communications::DataType::DOUBLE, buf, messageSize); 
				}

				comm->SendPacked(&buf, 2, tag, Framework::Communications::CommType::Synch);

				testValue++;
			}
		}
		else if (rank == 1)
		{
			for (int j = 0; j < MAX_PACKET_NUM; j++)
			{
				comm->GetPacked(messageSize, Framework::Communications::CommType::ASynch, &buf);

				for (int i = 0; i < COMM_BUF_SIZE; i++)
				{
					if (buf[i] != testValue)
					{
						throw new TestFailedException(MESSAGE("Incorrect value received"));
					}
				}

				testValue++;
			}
		}
		else
		{
			for (int j = 0; j < MAX_PACKET_NUM; j++)
			{
				comm->GetPacked(messageSize, Framework::Communications::CommType::ASynch, 0, tag, &buf);

				for (int i = 0; i < COMM_BUF_SIZE; i++)
				{
					if (buf[i] != testValue)
					{
						throw new TestFailedException(MESSAGE("Incorrect value received"));
					}
				}

				testValue++;
			}
		}
	}

	void UnitTest::TestPackages(unsigned rank)
	{
		IPackage *package = NULL;
		int testIntValue = 1010;
		double testDoubleValue = 123.456;
		bool testBoolValue = true;
		char testCharValue = 'X';
		unsigned testUnsignedValue = 0101;
		unsigned tag = 101;
		ICommunicator *comm = new Communicator();
		Position<int> testIntPosition(testIntValue, testIntValue);
		Position<double> testDoublePosition(testDoubleValue, testDoubleValue);
		
		MazeAgentInfo testMazeAgentInfo;
		testMazeAgentInfo.start = testCharValue;
		testMazeAgentInfo.goal = testCharValue;
		testMazeAgentInfo.agentId = testUnsignedValue;
		testMazeAgentInfo.lastDir = testUnsignedValue;
		testMazeAgentInfo.phsPathNum = testUnsignedValue;
		testMazeAgentInfo.canNotify = testBoolValue;

		MLAgentInfo testMLAgentInfo(testCharValue, testCharValue, testUnsignedValue);
		
		PackagerFactory *packageFactory = PackagerFactory::Instance();

		if (!Validator::IsNull(packageFactory, NAME("packageFactory")))
		{
			packageFactory->RegisterPackage(PackageType::NONE, EmptyPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::MESSAGE, MessagePackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::REGISTRATION, RegistrationPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::UNREGISTER, UnregisterPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::CONFIRMATION, ConfirmationPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::POSITION, PositionPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::MAZE_POSITION, MazePositionPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::MAZE_LIMIT, MazeLimitPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::MAZE_CAN_MOVE, MazeCanMovePackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::MAZE_CAN_MOVE_REPLY, MazeCanMoveReplyPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::MAZE_AGENT, MazeAgentPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::NEW_MAZE_AGENT, NewMazeAgentPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::GET_OBJ_ON, GetObjectOnPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::TERMINATE, TerminatePackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::GET_ML_AGENT_INFO, GetMLAgentInfoPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::ML_AGENT_INFO, MLAgentInfoPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::MAZE, MazePackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::EVENT_SUBSCRIPTION, EventSubscriptionPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::NODE_READY, NodeReadyPackage::CreatePackage);
		}

		if (rank == ROOT_NODE)
		{
			for (unsigned i = 1; i < 3; i++)
			{
				tag = 101;

				BasePackage *basePackage = new BasePackage(0, i, Framework::Packages::PackageType::NONE);
				IPackage *message = new MessagePackage(0, i, basePackage);
				message->Send(tag++, comm);
				delete message; 
				message = NULL;
				message = NULL;

				EmptyPackage *emptyPackage = new EmptyPackage(0, i);
				message = new MessagePackage(0, i, emptyPackage);
				message->Send(tag++, comm);
				delete message; 
				message = NULL; message = NULL;

				NodeReadyPackage *nodeReadyPackage = new NodeReadyPackage(0, i);
				message = new MessagePackage(0, i, nodeReadyPackage);
				message->Send(tag++, comm);
				delete message; 
				message = NULL;

				TerminatePackage *terminatePackage = new TerminatePackage(0, i);
				message = new MessagePackage(0, i, terminatePackage);
				message->Send(tag++, comm);
				delete message; 
				message = NULL;

				RegistrationPackage *regPackage = new RegistrationPackage(0, i, testUnsignedValue);
				message = new MessagePackage(0, i, regPackage);
				message->Send(tag++, comm);
				delete message; 
				message = NULL;

				UnregisterPackage *unregPackage = new UnregisterPackage(0, i, testUnsignedValue);
				message = new MessagePackage(0, i, unregPackage);
				message->Send(tag++, comm);
				delete message; 
				message = NULL;

				ConfirmationPackage *confirmPackage = new ConfirmationPackage(0, i);
				message = new MessagePackage(0, i, confirmPackage);
				message->Send(tag++, comm);
				delete message; 
				message = NULL;

				PositionPackage *posPackage = new PositionPackage(0, i, testDoublePosition);
				message = new MessagePackage(0, i, posPackage);
				message->Send(tag++, comm);
				delete message;
				message = NULL;

				MazePositionPackage *mazePosPackage = new MazePositionPackage(0, i, testCharValue, testIntPosition);
				message = new MessagePackage(0, i, mazePosPackage);
				message->Send(tag++, comm);
				delete message; 
				message = NULL;

				MazeLimitPackage *limitPackage = new MazeLimitPackage(0, i, testIntPosition);
				message = new MessagePackage(0, i, limitPackage);
				message->Send(tag++, comm);
				delete message; 
				message = NULL;

				MazeCanMovePackage *canMovePackage = new MazeCanMovePackage(0, i, testIntPosition, testUnsignedValue);
				message = new MessagePackage(0, i, canMovePackage);
				message->Send(tag++, comm);
				delete message; 
				message = NULL;

				MazeCanMoveReplyPackage *canMoveReplyPackage = new MazeCanMoveReplyPackage(0, i, testBoolValue, testIntValue, testCharValue);
				message = new MessagePackage(0, i, canMoveReplyPackage);
				message->Send(tag++, comm);
				delete message; 
				message = NULL;

				GetObjectOnPackage *getObjPackage = new GetObjectOnPackage(0, i, GetObjectOnPackage::QueryType::GET_OBJECT, testCharValue, testIntPosition);
				message = new MessagePackage(0, i, getObjPackage);
				message->Send(tag++, comm);
				delete message; 
				message = NULL;

				NewMazeAgentPackage *newMazeAgentPackage = new NewMazeAgentPackage(0, i, testCharValue, testCharValue);
				message = new MessagePackage(0, i, newMazeAgentPackage);
				message->Send(tag++, comm);
				delete message;
				message = NULL;

				MazeAgentPackage *mazeAgentPackage = new MazeAgentPackage(0, i, testMazeAgentInfo);
				message = new MessagePackage(0, i, mazeAgentPackage);
				message->Send(tag++, comm);
				delete message; 
				message = NULL;

				GetMLAgentInfoPackage *getMlInfoPackage = new GetMLAgentInfoPackage(0, i, testIntValue);
				message = new MessagePackage(0, i, getMlInfoPackage);
				message->Send(tag++, comm);
				delete message; 
				message = NULL;

				MLAgentInfoPackage *mlAgentInfoPackage = new MLAgentInfoPackage(0, i, testMLAgentInfo);
				message = new MessagePackage(0, i, mlAgentInfoPackage);
				message->Send(tag++, comm);
				delete message; 
				message = NULL;

				EventSubscriptionPackage *eventPackage = new EventSubscriptionPackage(0, i, Framework::Eventing::SubscriptionTypes::PHYSICAL_PATH_DONE_EVENT, Framework::Eventing::NotificationType::LONGLIVED);
				message = new MessagePackage(0, i, eventPackage);
				message->Send(tag++, comm);
				delete message; 
				message = NULL;

				char **maze = new char *[20];

				for (unsigned k = 0; k < 20; k++)
				{
					maze[k] = new char[20];
				}

				Maze *newMaze = new Maze(20, 20, maze);

				MazePackage *mazePackage = new MazePackage(0, i, newMaze, testIntPosition);
				message = new MessagePackage(0, i, mazePackage);
				message->Send(tag++, comm);
				delete message; 
				message = NULL;
			}
		}
		else
		{			
			MessagePackage *messagePackage = new MessagePackage();
			IPackage *message = messagePackage;

			message->Get(0, tag++, comm);
			package = messagePackage->GetPackage();
			BasePackage *basePackage = static_cast<BasePackage *>(package);
			delete package; 
			package = NULL;

			message->Get(0, tag++, comm);
			package = messagePackage->GetPackage();
			EmptyPackage *emptyPackage = static_cast<EmptyPackage *>(package);
			delete package; 
			package = NULL;

			message->Get(0, tag++, comm);
			package = messagePackage->GetPackage();
			NodeReadyPackage *nodeReadyPackage = static_cast<NodeReadyPackage *>(package);
			delete package; 
			package = NULL;

			message->Get(0, tag++, comm);
			package = messagePackage->GetPackage();
			TerminatePackage *terminatePackage = static_cast<TerminatePackage *>(package);
			delete package; 
			package = NULL;

			message->Get(0, tag++, comm);
			package = messagePackage->GetPackage();
			RegistrationPackage *registerPackage = static_cast<RegistrationPackage *>(package);
			delete package; 
			package = NULL;

			message->Get(0, tag++, comm);
			package = messagePackage->GetPackage();
			UnregisterPackage *unregisterPackage = static_cast<UnregisterPackage *>(package);
			delete package; 
			package = NULL;

			message->Get(0, tag++, comm);
			package = messagePackage->GetPackage();
			ConfirmationPackage *confirmationPackage = static_cast<ConfirmationPackage *>(package);
			delete package; 
			package = NULL;

			message->Get(0, tag++, comm);
			package = messagePackage->GetPackage();
			PositionPackage *positionPackage = static_cast<PositionPackage *>(package);
			delete package; 
			package = NULL;

			message->Get(0, tag++, comm);
			package = messagePackage->GetPackage();
			MazePositionPackage *mazePositionPackage = static_cast<MazePositionPackage *>(package);
			delete package; 
			package = NULL;

			message->Get(0, tag++, comm);
			package = messagePackage->GetPackage();
			MazeLimitPackage *mazeLimitPackage = static_cast<MazeLimitPackage *>(package);
			delete package;
			package = NULL;

			message->Get(0, tag++, comm);
			package = messagePackage->GetPackage();
			MazeCanMovePackage *mazeCanMovePackage = static_cast<MazeCanMovePackage *>(package);
			delete package;
			package = NULL;

			message->Get(0, tag++, comm);
			package = messagePackage->GetPackage();
			MazeCanMoveReplyPackage *mazeCanMoveReplyPackage = static_cast<MazeCanMoveReplyPackage *>(package);
			delete package; 
			package = NULL;

			message->Get(0, tag++, comm);
			package = messagePackage->GetPackage();
			GetObjectOnPackage *getObjectPackage = static_cast<GetObjectOnPackage *>(package);
			delete package;
			package = NULL;

			message->Get(0, tag++, comm);
			package = messagePackage->GetPackage();
			NewMazeAgentPackage *newMazeAgentPackage = static_cast<NewMazeAgentPackage *>(package);
			delete package; 
			package = NULL;

			message->Get(0, tag++, comm);
			package = messagePackage->GetPackage();
			MazeAgentPackage *mazeAgentPackage = static_cast<MazeAgentPackage *>(package);
			delete package; 
			package = NULL;

			message->Get(0, tag++, comm);
			package = messagePackage->GetPackage();
			GetMLAgentInfoPackage *getMlAgentInfoPackage = static_cast<GetMLAgentInfoPackage *>(package);
			delete package; 
			package = NULL;

			message->Get(0, tag++, comm);
			package = messagePackage->GetPackage();
			MLAgentInfoPackage *mlAgentInfoPackage = static_cast<MLAgentInfoPackage *>(package);
			delete package;
			package = NULL;

			message->Get(0, tag++, comm);
			package = messagePackage->GetPackage();
			EventSubscriptionPackage *eventPackage = static_cast<EventSubscriptionPackage *>(package);
			delete package;
			package = NULL;

			message->Get(0, tag++, comm);
			package = messagePackage->GetPackage();
			MazePackage *mazePackage = static_cast<MazePackage *>(package);
			delete package; 
			package = NULL;

			delete message; 
			message = NULL;
		}

		delete comm; 
		comm = NULL;
	}

	void UnitTest::TestPackagers(unsigned rank)
	{
		IPackage *package = NULL;
		int testIntValue = 1010;
		double testDoubleValue = 123.456;
		bool testBoolValue = true;
		char testCharValue = 'X';
		unsigned testUnsignedValue = 0101;
		IPackager *packager = new ASynchPackager(rank*100);
		Position<int> testIntPosition(testIntValue, testIntValue);
		Position<double> testDoublePosition(testDoubleValue, testDoubleValue);
		
		MazeAgentInfo testMazeAgentInfo;
		testMazeAgentInfo.start = testCharValue;
		testMazeAgentInfo.goal = testCharValue;
		testMazeAgentInfo.agentId = testUnsignedValue;
		testMazeAgentInfo.lastDir = testUnsignedValue;
		testMazeAgentInfo.phsPathNum = testUnsignedValue;
		testMazeAgentInfo.canNotify = testBoolValue;

		MLAgentInfo testMLAgentInfo(testCharValue, testCharValue, testUnsignedValue);

		PackagerFactory *packageFactory = PackagerFactory::Instance();

		if (!Validator::IsNull(packageFactory, NAME("packageFactory")))
		{
			packageFactory->RegisterPackage(PackageType::NONE, EmptyPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::MESSAGE, MessagePackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::REGISTRATION, RegistrationPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::UNREGISTER, UnregisterPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::CONFIRMATION, ConfirmationPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::POSITION, PositionPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::MAZE_POSITION, MazePositionPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::MAZE_LIMIT, MazeLimitPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::MAZE_CAN_MOVE, MazeCanMovePackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::MAZE_CAN_MOVE_REPLY, MazeCanMoveReplyPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::MAZE_AGENT, MazeAgentPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::NEW_MAZE_AGENT, NewMazeAgentPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::GET_OBJ_ON, GetObjectOnPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::TERMINATE, TerminatePackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::GET_ML_AGENT_INFO, GetMLAgentInfoPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::ML_AGENT_INFO, MLAgentInfoPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::MAZE, MazePackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::EVENT_SUBSCRIPTION, EventSubscriptionPackage::CreatePackage);
			packageFactory->RegisterPackage(PackageType::NODE_READY, NodeReadyPackage::CreatePackage);
		}

		if (rank == ROOT_NODE)
		{
			for (unsigned i = 1; i < 3; i++)
			{
				EmptyPackage *emptyPackage = new EmptyPackage(0, i);
				IPackage *message = new MessagePackage(0, i, emptyPackage);
				packager->PackAndSend(message);
				delete message;
				message = NULL;

				NodeReadyPackage *nodeReadyPackage = new NodeReadyPackage(0, i);
				message = new MessagePackage(0, i, nodeReadyPackage);
				packager->PackAndSend(message);
				delete message;
				message = NULL;

				TerminatePackage *terminatePackage = new TerminatePackage(0, i);
				message = new MessagePackage(0, i, terminatePackage);
				packager->PackAndSend(message);
				delete message;
				message = NULL;

				RegistrationPackage *regPackage = new RegistrationPackage(0, i, testUnsignedValue);
				message = new MessagePackage(0, i, regPackage);
				packager->PackAndSend(message);
				delete message;
				message = NULL;

				UnregisterPackage *unregPackage = new UnregisterPackage(0, i, testUnsignedValue);
				message = new MessagePackage(0, i, unregPackage);
				packager->PackAndSend(message);
				delete message;
				message = NULL;

				ConfirmationPackage *confirmPackage = new ConfirmationPackage(0, i);
				message = new MessagePackage(0, i, confirmPackage);
				packager->PackAndSend(message);
				delete message;
				message = NULL;

				PositionPackage *posPackage = new PositionPackage(0, i, testDoublePosition);
				message = new MessagePackage(0, i, posPackage);
				packager->PackAndSend(message);
				delete message;
				message = NULL;

				MazePositionPackage *mazePosPackage = new MazePositionPackage(0, i, testCharValue, testIntPosition);
				message = new MessagePackage(0, i, mazePosPackage);
				packager->PackAndSend(message);
				delete message; 
				message = NULL;

				MazeLimitPackage *limitPackage = new MazeLimitPackage(0, i, testIntPosition);
				message = new MessagePackage(0, i, limitPackage);
				packager->PackAndSend(message);
				delete message;
				message = NULL;

				MazeCanMovePackage *canMovePackage = new MazeCanMovePackage(0, i, testIntPosition, testUnsignedValue);
				message = new MessagePackage(0, i, canMovePackage);
				packager->PackAndSend(message);
				delete message;
				message = NULL;

				MazeCanMoveReplyPackage *canMoveReplyPackage = new MazeCanMoveReplyPackage(0, i, testBoolValue, testIntValue, testCharValue);
				message = new MessagePackage(0, i, canMoveReplyPackage);
				packager->PackAndSend(message);
				delete message;
				message = NULL;

				GetObjectOnPackage *getObjPackage = new GetObjectOnPackage(0, i, GetObjectOnPackage::QueryType::GET_OBJECT, testCharValue, testIntPosition);
				message = new MessagePackage(0, i, getObjPackage);
				packager->PackAndSend(message);
				delete message;
				message = NULL;

				NewMazeAgentPackage *newMazeAgentPackage = new NewMazeAgentPackage(0, i, testCharValue, testCharValue);
				message = new MessagePackage(0, i, newMazeAgentPackage);
				packager->PackAndSend(message);
				delete message;
				message = NULL;

				MazeAgentPackage *mazeAgentPackage = new MazeAgentPackage(0, i, testMazeAgentInfo);
				message = new MessagePackage(0, i, mazeAgentPackage);
				packager->PackAndSend(message);
				delete message; 
				message = NULL;

				GetMLAgentInfoPackage *getMlInfoPackage = new GetMLAgentInfoPackage(0, i, testIntValue);
				message = new MessagePackage(0, i, getMlInfoPackage);
				packager->PackAndSend(message);
				delete message;
				message = NULL;

				MLAgentInfoPackage *mlAgentInfoPackage = new MLAgentInfoPackage(0, i, testMLAgentInfo);
				message = new MessagePackage(0, i, mlAgentInfoPackage);
				packager->PackAndSend(message);
				delete message;
				message = NULL;

				EventSubscriptionPackage *eventPackage = new EventSubscriptionPackage(0, i, Framework::Eventing::SubscriptionTypes::PHYSICAL_PATH_DONE_EVENT, Framework::Eventing::NotificationType::LONGLIVED);
				message = new MessagePackage(0, i, eventPackage);
				packager->PackAndSend(message);
				delete message; 
				message = NULL;


				char **maze = new char *[20];

				for (unsigned k = 0; k < 20; k++)
				{
					maze[k] = new char[20];
				}

				Maze *newMaze = new Maze(20, 20, maze);

				MazePackage *mazePackage = new MazePackage(0, i, newMaze, testIntPosition);
				message = new MessagePackage(0, i, mazePackage);
				packager->PackAndSend(message);
				delete message; 
				message = NULL;
			}
		}
		else
		{			
			MessagePackage *messagePackage = static_cast<MessagePackage *>(packager->GetAndUnpack());
			package = messagePackage->GetPackage();
			EmptyPackage *emptyPackage = static_cast<EmptyPackage *>(package);
			delete package; 
			package = NULL;
			delete messagePackage;

			messagePackage = static_cast<MessagePackage *>(packager->GetAndUnpack());
			package = messagePackage->GetPackage();
			NodeReadyPackage *nodeReadyPackage = static_cast<NodeReadyPackage *>(package);
			delete package;
			package = NULL;
			delete messagePackage;

			messagePackage = static_cast<MessagePackage *>(packager->GetAndUnpack());
			package = messagePackage->GetPackage();
			TerminatePackage *terminatePackage = static_cast<TerminatePackage *>(package);
			delete package;
			package = NULL;
			delete messagePackage;

			messagePackage = static_cast<MessagePackage *>(packager->GetAndUnpack());
			package = messagePackage->GetPackage();
			RegistrationPackage *registerPackage = static_cast<RegistrationPackage *>(package);
			delete package; 
			package = NULL;
			delete messagePackage;

			messagePackage = static_cast<MessagePackage *>(packager->GetAndUnpack());
			package = messagePackage->GetPackage();
			UnregisterPackage *unregisterPackage = static_cast<UnregisterPackage *>(package);
			delete package;
			package = NULL;
			delete messagePackage;

			messagePackage = static_cast<MessagePackage *>(packager->GetAndUnpack());
			package = messagePackage->GetPackage();
			ConfirmationPackage *confirmationPackage = static_cast<ConfirmationPackage *>(package);
			delete package;
			package = NULL;
			delete messagePackage;

			messagePackage = static_cast<MessagePackage *>(packager->GetAndUnpack());
			package = messagePackage->GetPackage();
			PositionPackage *positionPackage = static_cast<PositionPackage *>(package);
			delete package; 
			package = NULL;
			delete messagePackage;

			messagePackage = static_cast<MessagePackage *>(packager->GetAndUnpack());
			package = messagePackage->GetPackage();
			MazePositionPackage *mazePositionPackage = static_cast<MazePositionPackage *>(package);
			delete package;
			package = NULL;
			delete messagePackage;

			messagePackage = static_cast<MessagePackage *>(packager->GetAndUnpack());
			package = messagePackage->GetPackage();
			MazeLimitPackage *mazeLimitPackage = static_cast<MazeLimitPackage *>(package);
			delete package;
			package = NULL;
			delete messagePackage;

			messagePackage = static_cast<MessagePackage *>(packager->GetAndUnpack());
			package = messagePackage->GetPackage();
			MazeCanMovePackage *mazeCanMovePackage = static_cast<MazeCanMovePackage *>(package);
			delete package;
			package = NULL;
			delete messagePackage;

			messagePackage = static_cast<MessagePackage *>(packager->GetAndUnpack());
			package = messagePackage->GetPackage();
			MazeCanMoveReplyPackage *mazeCanMoveReplyPackage = static_cast<MazeCanMoveReplyPackage *>(package);
			delete package;
			package = NULL;
			delete messagePackage;

			messagePackage = static_cast<MessagePackage *>(packager->GetAndUnpack());
			package = messagePackage->GetPackage();
			GetObjectOnPackage *getObjectPackage = static_cast<GetObjectOnPackage *>(package);
			delete package;
			package = NULL;
			delete messagePackage;

			messagePackage = static_cast<MessagePackage *>(packager->GetAndUnpack());
			package = messagePackage->GetPackage();
			NewMazeAgentPackage *newMazeAgentPackage = static_cast<NewMazeAgentPackage *>(package);
			delete package; 
			package = NULL;
			delete messagePackage;

			messagePackage = static_cast<MessagePackage *>(packager->GetAndUnpack());
			package = messagePackage->GetPackage();
			MazeAgentPackage *mazeAgentPackage = static_cast<MazeAgentPackage *>(package);
			delete package;
			package = NULL;
			delete messagePackage;

			messagePackage = static_cast<MessagePackage *>(packager->GetAndUnpack());
			package = messagePackage->GetPackage();
			GetMLAgentInfoPackage *getMlAgentInfoPackage = static_cast<GetMLAgentInfoPackage *>(package);
			delete package;
			package = NULL;
			delete messagePackage;

			messagePackage = static_cast<MessagePackage *>(packager->GetAndUnpack());
			package = messagePackage->GetPackage();
			MLAgentInfoPackage *mlAgentInfoPackage = static_cast<MLAgentInfoPackage *>(package);
			delete package;
			package = NULL;
			delete messagePackage;

			messagePackage = static_cast<MessagePackage *>(packager->GetAndUnpack());
			package = messagePackage->GetPackage();
			EventSubscriptionPackage *eventPackage = static_cast<EventSubscriptionPackage *>(package);
			delete package; 
			package = NULL;
			delete messagePackage;

			messagePackage = static_cast<MessagePackage *>(packager->GetAndUnpack());
			package = messagePackage->GetPackage();
			MazePackage *mazePackage = static_cast<MazePackage *>(package);
			delete package; 
			package = NULL;
			delete messagePackage;
		}

		delete packager; 
		packager = NULL;
		packager = NULL;
	}

	void UnitTest::TestEventDistributor(unsigned rank)
	{
	}

	void UnitTest::TestMessageDistributor(unsigned rank)
	{
	}

	void UnitTest::TestRemoteSubscription(unsigned rank)
	{
	}

	void UnitTest::TestNodes(unsigned rank)
	{
		
	}
}