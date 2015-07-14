/////////////////////////////////////////////////////////
//Packages.cpp
//Defines the different packages.
//
//Manuel Madera
//5/8/09
/////////////////////////////////////////////////////////

#include "Packages.h"
#include "Communicator.h"
#include "PackagerFactory.h"
#include "Debug.h"
#include "LogManager.h"

#ifdef PrintMemoryLeak
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif
#endif

using namespace Framework::Communications;
using namespace Framework::Factories;
using namespace Framework::Eventing;
using namespace Framework::Logging;

namespace Framework
{ 
	namespace Packages
	{
		/////////////////////////////////////////////////////////
		//BasePackage
		/////////////////////////////////////////////////////////
		BasePackage::BasePackage()
		{
			//comm = new Communicator();
		}

		BasePackage::BasePackage(unsigned packageSrc, unsigned packageDestination, PackageType::Package type) 
			: packageSource(packageSrc), packageDest(packageDestination), myType(type)
		{
			//comm = new Communicator();
		}

		BasePackage::~BasePackage() 
		{ 
			//if (!Validator::IsNull(comm, NAME("comm")))
			//{
			//	delete comm; comm = NULL; 
			//}
		}
	                    
		void BasePackage::Serialize(Communications::ICommunicator *comm) 
		{
			Serialize(&buffer, BasePackageBufSize*IntSize, comm);
		}

		void BasePackage::Deserialize(Communications::ICommunicator *comm) 
		{
			Deserialize(&buffer, BasePackageBufSize*IntSize, comm);
		}

		void BasePackage::Send(unsigned messageTag, Communications::ICommunicator *comm) 
		{ 
			Send(this->packageDest, messageTag, comm);
		}

		void BasePackage::Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			Serialize(comm);

			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->SendPacked(&buffer, destRank, messageTag, CommType::Synch);
			}
		}

		void BasePackage::Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm) 
		{
			//comm = new Communicator();
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->GetPacked(BasePackageBufSize*IntSize, CommType::ASynch, sourceRank, messageTag, &buffer);
			}
	        
			Deserialize(comm);
		}

		void BasePackage::SetDestination(unsigned destRank) 
		{ 
			this->packageDest = destRank; 
		}

		void BasePackage::SetSource(unsigned sourceRank) 
		{ 
			this->packageSource = sourceRank; 
		}

		unsigned BasePackage::GetDestination()
		{
			return this->packageDest;
		}

		unsigned BasePackage::GetSource() 
		{ 
			return this->packageSource; 
		}

		PackageType::Package BasePackage::GetType() 
		{ 
			return myType; 
		}

		void BasePackage::Serialize(void *buf, unsigned bufSize, Communications::ICommunicator *comm)
		{
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Pack(&packageSource, 1, DataType::Int, buf, bufSize);
				comm->Pack(&packageDest, 1, DataType::Int, buf, bufSize);
				comm->Pack(&myType, 1, DataType::Int, buf, bufSize);
			}
		}

		//Use when creating a child of this class
		void BasePackage::Deserialize(void *buf, unsigned bufSize, Communications::ICommunicator *comm)
		{
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Unpack(buf, bufSize, DataType::Int, 1, &packageSource);
				comm->Unpack(buf, bufSize, DataType::Int, 1, &packageDest);
				comm->Unpack(buf, bufSize, DataType::Int, 1, &myType);
			}
		}

		/*static*/ IPackage *BasePackage::CreatePackage(unsigned source, unsigned dest)
		{
			return new BasePackage(source, dest, PackageType::NONE);
		}

		/////////////////////////////////////////////////////////
		//EmptyPackage
		/////////////////////////////////////////////////////////
		EmptyPackage::EmptyPackage() : BasePackage()
		{
		}

		EmptyPackage::EmptyPackage(unsigned packageSource, unsigned packageDest)
			: BasePackage(packageSource, packageDest, PackageType::NONE)
		{
		}

		EmptyPackage::~EmptyPackage() 
		{
		}

		/*static*/ IPackage *EmptyPackage::CreatePackage(unsigned source, unsigned dest)
		{
			return new EmptyPackage(source, dest);
		}

		/////////////////////////////////////////////////////////
		//NodeReadyPackage
		/////////////////////////////////////////////////////////
		NodeReadyPackage::NodeReadyPackage() : BasePackage()
		{
		}

		NodeReadyPackage::NodeReadyPackage(unsigned packageSource, unsigned packageDest)
			: BasePackage(packageSource, packageDest, PackageType::NODE_READY)
		{
		}

		NodeReadyPackage::~NodeReadyPackage() 
		{
		}

		/*static*/ IPackage *NodeReadyPackage::CreatePackage(unsigned source, unsigned dest)
		{
			return new NodeReadyPackage(source, dest);
		}

		/////////////////////////////////////////////////////////
		//TerminatePackage
		/////////////////////////////////////////////////////////
		TerminatePackage::TerminatePackage() : BasePackage()
		{
		}

		TerminatePackage::TerminatePackage(unsigned packageSource, unsigned packageDest)
			: BasePackage(packageSource, packageDest, PackageType::TERMINATE)
		{
		}

		TerminatePackage::~TerminatePackage() 
		{
		}

		/*static*/ IPackage *TerminatePackage::CreatePackage(unsigned source, unsigned dest)
		{
			return new TerminatePackage(source, dest);
		}
		/////////////////////////////////////////////////////////
		//MessagePackage
		/////////////////////////////////////////////////////////
		MessagePackage::MessagePackage() :  BasePackage(0, 0, PackageType::MESSAGE), deleteMessage(true), message(0) 
		{
		}

		MessagePackage::MessagePackage(unsigned packageSource, unsigned packageDest)
			: BasePackage(packageSource, packageDest, PackageType::MESSAGE), deleteMessage(true), message(0)
		{
		}

		MessagePackage::MessagePackage(unsigned packageSource, unsigned packageDest, IPackage *msg) 
			: BasePackage(packageSource, packageDest, PackageType::MESSAGE), deleteMessage(true), message(msg)
		{
		}

		MessagePackage::~MessagePackage() 
		{
			if (deleteMessage)
			{
				delete message; 
				message = NULL; 
			}
		}
	                    
		void MessagePackage::Serialize(Communications::ICommunicator *comm) 
		{ 
			PackageType::Package packageType;
	        
			if (!Validator::IsNull(message, NAME("message")))
			{
				packageType = message->GetType();
			}
			else
			{
				packageType = PackageType::NONE;
			}

#ifdef Debug
			ILog *log = LogManager::Instance();

			if (!Validator::IsNull(log, NAME("log")))
			{
				log->Log(&typeid(this), "Serializing message of type: %d", packageType);
			}
#endif

			BasePackage::Serialize(&myBuffer, MessagePackageBufSize*IntSize, comm);
			
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Pack(&deleteMessage, 1, DataType::Int, &myBuffer, MessagePackageBufSize*IntSize);
				comm->Pack(&packageType, 1, DataType::Int, &myBuffer, MessagePackageBufSize*IntSize);
			}
		}

		void MessagePackage::Deserialize(Communications::ICommunicator *comm) 
		{ 
			PackageType::Package messageType;        

			BasePackage::Deserialize(&myBuffer, MessagePackageBufSize*IntSize, comm);

			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Unpack(&myBuffer, MessagePackageBufSize*IntSize, DataType::Int, 1, &deleteMessage);
				comm->Unpack(&myBuffer, MessagePackageBufSize*IntSize, DataType::Int, 1, &messageType);
			}

#ifdef Debug
			ILog *log = LogManager::Instance();

			if (!Validator::IsNull(log, NAME("log")))
			{
				log->Log(&typeid(this), "Deserializing message of type: %d", messageType);
			}
#endif

			PackagerFactory *packageFactory = PackagerFactory::Instance();

			if (!Validator::IsNull(packageFactory, NAME("packageFactory")))
			{
				message = packageFactory->CreatePackage(messageType);
			}
		}

		void MessagePackage::Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			Serialize(comm);		

			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->SendPacked(&myBuffer, destRank, messageTag, CommType::Synch);   
			}

			if (!Validator::IsNull(message, NAME("message")))
			{
				message->Send(destRank, messageTag, comm);
			}
		}

		void MessagePackage::Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm) 
		{
			//comm = new Communicator();
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->GetPacked(MessagePackageBufSize*IntSize, CommType::ASynch, sourceRank, messageTag, &myBuffer);
			}

			Deserialize(comm);

			if (!Validator::IsNull(message, NAME("message")))
			{
				message->Get(sourceRank, messageTag, comm);
			}
		}

		PackageType::Package MessagePackage::GetInnerPackageType()
		{
			PackageType::Package packageType = PackageType::NONE;

			if (!Validator::IsNull(message, NAME("message")))
			{
				packageType = message->GetType();
			}

			return packageType;
		}

		IPackage *MessagePackage::GetPackage() 
		{
			deleteMessage = false;
			return message; 
		}

		/*static*/ IPackage *MessagePackage::CreatePackage(unsigned source, unsigned dest)
		{
			return new MessagePackage(source, dest);
		}
		/////////////////////////////////////////////////////////
		//RegistrationPackage
		/////////////////////////////////////////////////////////
		RegistrationPackage::RegistrationPackage() : BasePackage(0, 0, PackageType::REGISTRATION)
		{
		}

		RegistrationPackage::RegistrationPackage(unsigned packageSource, unsigned packageDest) 
			: BasePackage(packageSource, packageDest, PackageType::REGISTRATION)
		{
			this->id = 0;
		}

		RegistrationPackage::RegistrationPackage(unsigned packageSource, unsigned packageDest, unsigned id) 
			: BasePackage(packageSource, packageDest, PackageType::REGISTRATION)
		{
			this->id = id;
		}

		RegistrationPackage::~RegistrationPackage()
		{
		}
	                    
		void RegistrationPackage::Serialize(Communications::ICommunicator *comm)
		{
			BasePackage::Serialize(&myBuffer, RegistrationPackageBufSize*IntSize, comm);

			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Pack(&id, 1, DataType::Int, &myBuffer, RegistrationPackageBufSize*IntSize);
			}
		}

		void RegistrationPackage::Deserialize(Communications::ICommunicator *comm)
		{
			BasePackage::Deserialize(&myBuffer, RegistrationPackageBufSize*IntSize, comm);

			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Unpack(&myBuffer, RegistrationPackageBufSize*IntSize, DataType::Int, 1, &id);
			}
		}

		void RegistrationPackage::Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			Serialize(comm);

			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->SendPacked(&myBuffer, destRank, messageTag, CommType::Synch);
			}
		}

		void RegistrationPackage::Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			//comm = new Communicator();
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->GetPacked(RegistrationPackageBufSize*IntSize, CommType::ASynch, sourceRank, messageTag, &myBuffer);
			}

			Deserialize(comm);
		}

		unsigned RegistrationPackage::GetID() 
		{ 
			return this->id; 
		}

		void RegistrationPackage::SetID(unsigned id) 
		{ 
			this->id = id; 
		}

		/*static*/ IPackage *RegistrationPackage::CreatePackage(unsigned source, unsigned dest)
		{
			return new RegistrationPackage(source, dest);
		}
		/////////////////////////////////////////////////////////
		//UnregisterPackage
		/////////////////////////////////////////////////////////
		UnregisterPackage::UnregisterPackage() : BasePackage(0, 0, PackageType::UNREGISTER)
		{
		}

		UnregisterPackage::UnregisterPackage(unsigned packageSource, unsigned packageDest) 
			: BasePackage(packageSource, packageDest, PackageType::UNREGISTER)
		{
			this->id = 0;
		}

		UnregisterPackage::UnregisterPackage(unsigned packageSource, unsigned packageDest, unsigned id) 
			: BasePackage(packageSource, packageDest, PackageType::UNREGISTER)
		{
			this->id = id;
		}

		UnregisterPackage::~UnregisterPackage()
		{
		}
	                    
		void UnregisterPackage::Serialize(Communications::ICommunicator *comm)
		{
			BasePackage::Serialize(&myBuffer, RegistrationPackageBufSize*IntSize, comm);

			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Pack(&id, 1, DataType::Int, &myBuffer, RegistrationPackageBufSize*IntSize);
			}
		}

		void UnregisterPackage::Deserialize(Communications::ICommunicator *comm)
		{
			BasePackage::Deserialize(&myBuffer, RegistrationPackageBufSize*IntSize, comm);
			
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Unpack(&myBuffer, RegistrationPackageBufSize*IntSize, DataType::Int, 1, &id);
			}
		}

		void UnregisterPackage::Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			Serialize(comm);
			
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->SendPacked(&myBuffer, destRank, messageTag, CommType::Synch);
			}
		}

		void UnregisterPackage::Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			//comm = new Communicator();
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->GetPacked(RegistrationPackageBufSize*IntSize, CommType::ASynch, sourceRank, messageTag, &myBuffer);
			}

			Deserialize(comm);
		}

		unsigned UnregisterPackage::GetID() 
		{ 
			return this->id; 
		}

		/*static*/ IPackage *UnregisterPackage::CreatePackage(unsigned source, unsigned dest)
		{
			return new UnregisterPackage(source, dest);
		}
		/////////////////////////////////////////////////////////
		//ConfirmationPackage
		/////////////////////////////////////////////////////////
		ConfirmationPackage::ConfirmationPackage() : BasePackage(0, 0, PackageType::CONFIRMATION)
		{
		}

		ConfirmationPackage::ConfirmationPackage(unsigned packageSource, unsigned packageDest) 
			: BasePackage(packageSource, packageDest, PackageType::CONFIRMATION)
		{
		}

		ConfirmationPackage::~ConfirmationPackage()
		{
		}

		void ConfirmationPackage::Serialize(Communications::ICommunicator *comm)
		{
			BasePackage::Serialize(&myBuffer, RegistrationPackageBufSize*IntSize, comm);
			
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Pack(&id, 1, DataType::Int, &myBuffer, RegistrationPackageBufSize*IntSize);
			}
		}

		void ConfirmationPackage::Deserialize(Communications::ICommunicator *comm)
		{
			BasePackage::Deserialize(&myBuffer, RegistrationPackageBufSize*IntSize, comm);
			
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Unpack(&myBuffer, RegistrationPackageBufSize*IntSize, DataType::Int, 1, &id);
			}
		}

		void ConfirmationPackage::Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			Serialize(comm);

			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->SendPacked(&myBuffer, destRank, messageTag, CommType::Synch);
			}
		}

		void ConfirmationPackage::Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			//comm = new Communicator();
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->GetPacked(RegistrationPackageBufSize*IntSize, CommType::ASynch, sourceRank, messageTag, &myBuffer);
			}

			Deserialize(comm);
		}

		/*static*/ IPackage *ConfirmationPackage::CreatePackage(unsigned source, unsigned dest)
		{
			return new ConfirmationPackage(source, dest);
		}
		/////////////////////////////////////////////////////////
		//PositionPackage
		/////////////////////////////////////////////////////////
		PositionPackage::PositionPackage() : BasePackage(0, 0, PackageType::POSITION)
		{
		}

		PositionPackage::PositionPackage(unsigned packageSource, unsigned packageDest)
			: BasePackage(packageSource, packageDest, PackageType::POSITION)
		{
		}

		PositionPackage::PositionPackage(unsigned packageSource, unsigned packageDest, Position<double> newPos)
			: BasePackage(packageSource, packageDest, PackageType::POSITION)
		{
			SetPosition(newPos);
		}

		PositionPackage::~PositionPackage()
		{
		}
		
		Position<double> PositionPackage::GetPosition()
		{
			return this->pos;
		}

		void PositionPackage::SetPosition(Position<double> newPos)
		{
			this->pos = newPos;
		}

		void PositionPackage::Serialize(Communications::ICommunicator *comm)
		{
			BasePackage::Serialize(&myBuffer, PositionPackageBufSize*DoubleSize, comm);

			double tempX = this->pos.GetXCoor();
			double tempY = this->pos.GetYCoor();

			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Pack(&tempX, 1, DataType::DOUBLE, &myBuffer, PositionPackageBufSize*DoubleSize);
				comm->Pack(&tempY, 1, DataType::DOUBLE, &myBuffer, PositionPackageBufSize*DoubleSize);
			}
		}

		void PositionPackage::Deserialize(Communications::ICommunicator *comm)
		{
			double tempX;
			double tempY;

			BasePackage::Deserialize(&myBuffer, PositionPackageBufSize*DoubleSize, comm);
			
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Unpack(&myBuffer, PositionPackageBufSize*DoubleSize, DataType::DOUBLE, 1, &tempX);
				comm->Unpack(&myBuffer, PositionPackageBufSize*DoubleSize, DataType::DOUBLE, 1, &tempY);
			}

			this->pos.SetXCoor(tempX);
			this->pos.SetYCoor(tempY);
		}

		void PositionPackage::Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			Serialize(comm);
			
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->SendPacked(&myBuffer, destRank, messageTag, CommType::Synch);
			}
		}

		void PositionPackage::Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			//comm = new Communicator();
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->GetPacked(PositionPackageBufSize*DoubleSize, CommType::ASynch, sourceRank, messageTag, &myBuffer);
			}

			Deserialize(comm);
		}

		/*static*/ IPackage *PositionPackage::CreatePackage(unsigned source, unsigned dest)
		{
			return new PositionPackage(source, dest);
		}

		/////////////////////////////////////////////////////////
		//MazePositionPackage
		/////////////////////////////////////////////////////////
		MazePositionPackage::MazePositionPackage() : BasePackage(0, 0, PackageType::MAZE_POSITION)
		{
		}

		MazePositionPackage::MazePositionPackage(unsigned packageSource, unsigned packageDest)
			: BasePackage(packageSource, packageDest, PackageType::MAZE_POSITION)
		{
		}

		MazePositionPackage::MazePositionPackage(unsigned packageSource, unsigned packageDest, char object, Position<int> newPos)
			: BasePackage(packageSource, packageDest, PackageType::MAZE_POSITION)
		{
			SetPosition(newPos);
			this->objectOnPos = object;
		}

		MazePositionPackage::~MazePositionPackage()
		{
		}
		
		Position<int> MazePositionPackage::GetPosition()
		{
			return this->pos;
		}

		void MazePositionPackage::SetPosition(Position<int> newPos)
		{
			this->pos = newPos;
		}

		char MazePositionPackage::GetObject()
		{
			return this->objectOnPos;
		}

		void MazePositionPackage::SetObject(char newObject)
		{
			this->objectOnPos = newObject;
		}

		MazePositionPackage *MazePositionPackage::Clone()
		{
			return new MazePositionPackage(this->packageSource, this->packageDest, this->objectOnPos, this->pos);
		}

		void MazePositionPackage::Serialize(Communications::ICommunicator *comm)
		{
			BasePackage::Serialize(&myBuffer, MazePositionPackageBufSize*IntSize, comm);

			int tempX = this->pos.GetXCoor();
			int tempY = this->pos.GetYCoor();

			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Pack(&this->objectOnPos, 1, DataType::Int, &myBuffer, MazePositionPackageBufSize*IntSize);
				comm->Pack(&tempX, 1, DataType::Int, &myBuffer, MazePositionPackageBufSize*IntSize);
				comm->Pack(&tempY, 1, DataType::Int, &myBuffer, MazePositionPackageBufSize*IntSize);
			}
		}

		void MazePositionPackage::Deserialize(Communications::ICommunicator *comm)
		{
			int tempX;
			int tempY;

			BasePackage::Deserialize(&myBuffer, MazePositionPackageBufSize*IntSize, comm);
			
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Unpack(&myBuffer, MazePositionPackageBufSize*IntSize, DataType::Int, 1, &this->objectOnPos);
				comm->Unpack(&myBuffer, MazePositionPackageBufSize*IntSize, DataType::Int, 1, &tempX);
				comm->Unpack(&myBuffer, MazePositionPackageBufSize*IntSize, DataType::Int, 1, &tempY);
			}

			this->pos.SetXCoor(tempX);
			this->pos.SetYCoor(tempY);
		}

		void MazePositionPackage::Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			Serialize(comm);
			
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->SendPacked(&myBuffer, destRank, messageTag, CommType::Synch);
			}
		}

		void MazePositionPackage::Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			//comm = new Communicator();
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->GetPacked(MazePositionPackageBufSize*IntSize, CommType::ASynch, sourceRank, messageTag, &myBuffer);
			}

			Deserialize(comm);
		}

		/*static*/ IPackage *MazePositionPackage::CreatePackage(unsigned source, unsigned dest)
		{
			return new MazePositionPackage(source, dest);
		}

		/////////////////////////////////////////////////////////
		//MazeLimitPackage
		/////////////////////////////////////////////////////////
		MazeLimitPackage::MazeLimitPackage(): BasePackage(0, 0, PackageType::MAZE_LIMIT)
		{
		}

		MazeLimitPackage::MazeLimitPackage(unsigned packageSource, unsigned packageDest)
			: BasePackage(packageSource, packageDest, PackageType::MAZE_LIMIT)
		{
		}

		MazeLimitPackage::MazeLimitPackage(unsigned packageSource, unsigned packageDest, Position<int> newLimit)
			: BasePackage(packageSource, packageDest, PackageType::MAZE_LIMIT), limit(newLimit)
		{
		}

		MazeLimitPackage::~MazeLimitPackage()
		{
		}                        
		
		Position<int> MazeLimitPackage::GetLimit()
		{
			return this->limit;
		}

		void MazeLimitPackage::SetLimit(Position<int> limit)
		{
			this->limit = limit;
		}

		/*static*/ IPackage *MazeLimitPackage::CreatePackage(unsigned source, unsigned dest)
		{
			return new MazeLimitPackage(source, dest);
		}

		void MazeLimitPackage::Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			Serialize(comm);
			
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->SendPacked(&myBuffer, destRank, messageTag, CommType::Synch);
			}
		}

		void MazeLimitPackage::Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			//comm = new Communicator();
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->GetPacked(MazeLimitPackageBufSize*IntSize, CommType::ASynch, sourceRank, messageTag, &myBuffer);
			}

			Deserialize(comm);
		}

		void MazeLimitPackage::Serialize(Communications::ICommunicator *comm)
		{
			BasePackage::Serialize(&myBuffer, MazeLimitPackageBufSize*IntSize, comm);

			int tempX = this->limit.GetXCoor();
			int tempY = this->limit.GetYCoor();

			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Pack(&tempX, 1, DataType::Int, &myBuffer, MazeLimitPackageBufSize*IntSize);
				comm->Pack(&tempY, 1, DataType::Int, &myBuffer, MazeLimitPackageBufSize*IntSize);
			}
		}

		void MazeLimitPackage::Deserialize(Communications::ICommunicator *comm)
		{
			int tempX;
			int tempY;

			BasePackage::Deserialize(&myBuffer, MazeLimitPackageBufSize*IntSize, comm);
			
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Unpack(&myBuffer, MazeLimitPackageBufSize*IntSize, DataType::Int, 1, &tempX);
				comm->Unpack(&myBuffer, MazeLimitPackageBufSize*IntSize, DataType::Int, 1, &tempY);
			}

			this->limit.SetXCoor(tempX);
			this->limit.SetYCoor(tempY);
		}

		/////////////////////////////////////////////////////////
		//MazeCanMovePackage
		/////////////////////////////////////////////////////////
		MazeCanMovePackage::MazeCanMovePackage() : BasePackage(0, 0, PackageType::MAZE_CAN_MOVE)
		{
		}

		MazeCanMovePackage::MazeCanMovePackage(unsigned packageSource, unsigned packageDest)
			: BasePackage(packageSource, packageDest, PackageType::MAZE_CAN_MOVE)
		{
		}

		MazeCanMovePackage::MazeCanMovePackage(unsigned packageSource, unsigned packageDest, Position<int> pos, unsigned id)
			: BasePackage(packageSource, packageDest, PackageType::MAZE_CAN_MOVE), pos(pos), agentId(id)
		{
		}

		MazeCanMovePackage::~MazeCanMovePackage()
		{
		}                        
		
		Position<int> MazeCanMovePackage::GetPos()
		{
			return this->pos;
		}

		void MazeCanMovePackage::SetPos(Position<int> pos)
		{
			this->pos = pos;
		}

		unsigned MazeCanMovePackage::GetAgentId()
		{
			return this->agentId;
		}

		void MazeCanMovePackage::SetAgentId(unsigned id)
		{
			this->agentId = id;
		}

		/*static*/ IPackage *MazeCanMovePackage::CreatePackage(unsigned source, unsigned dest)
		{
			return new MazeCanMovePackage(source, dest);
		}

		void MazeCanMovePackage::Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			Serialize(comm);
			
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->SendPacked(&myBuffer, destRank, messageTag, CommType::Synch);
			}
		}

		void MazeCanMovePackage::Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			//comm = new Communicator();
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->GetPacked(MazeCanMovePackageBufSize*IntSize, CommType::ASynch, sourceRank, messageTag, &myBuffer);
			}

			Deserialize(comm);
		}

		void MazeCanMovePackage::Serialize(Communications::ICommunicator *comm)
		{
			BasePackage::Serialize(&myBuffer, MazeCanMovePackageBufSize*IntSize, comm);

			int tempX = this->pos.GetXCoor();
			int tempY = this->pos.GetYCoor();

			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Pack(&agentId, 1, DataType::Int, &myBuffer, MazeCanMovePackageBufSize*IntSize);
				comm->Pack(&tempX, 1, DataType::Int, &myBuffer, MazeCanMovePackageBufSize*IntSize);
				comm->Pack(&tempY, 1, DataType::Int, &myBuffer, MazeCanMovePackageBufSize*IntSize);
			}
		}

		void MazeCanMovePackage::Deserialize(Communications::ICommunicator *comm)
		{
			int tempX;
			int tempY;

			BasePackage::Deserialize(&myBuffer, MazeCanMovePackageBufSize*IntSize, comm);
			
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Unpack(&myBuffer, MazeCanMovePackageBufSize*IntSize, DataType::Int, 1, &agentId);
				comm->Unpack(&myBuffer, MazeCanMovePackageBufSize*IntSize, DataType::Int, 1, &tempX);
				comm->Unpack(&myBuffer, MazeCanMovePackageBufSize*IntSize, DataType::Int, 1, &tempY);
			}

			this->pos.SetXCoor(tempX);
			this->pos.SetYCoor(tempY);
		}

		/////////////////////////////////////////////////////////
		//MazeCanMoveReplyPackage
		/////////////////////////////////////////////////////////
		MazeCanMoveReplyPackage::MazeCanMoveReplyPackage() : BasePackage(0, 0, PackageType::MAZE_CAN_MOVE_REPLY)
		{
		}

		MazeCanMoveReplyPackage::MazeCanMoveReplyPackage(unsigned packageSource, unsigned packageDest)
			: BasePackage(packageSource, packageDest, PackageType::MAZE_CAN_MOVE_REPLY)
		{
		}

		MazeCanMoveReplyPackage::MazeCanMoveReplyPackage(unsigned packageSource, unsigned packageDest, bool move, unsigned id, char newStart)
			: BasePackage(packageSource, packageDest, PackageType::MAZE_CAN_MOVE_REPLY), canMove(move), agentId(id), newStartPos(newStart)
		{
		}

		MazeCanMoveReplyPackage::~MazeCanMoveReplyPackage()
		{
		}                        
		
		bool MazeCanMoveReplyPackage::GetCanMove()
		{
			return this->canMove;
		}

		void MazeCanMoveReplyPackage::SetCanMove(bool move)
		{
			this->canMove = move;
		}

		char MazeCanMoveReplyPackage::GetStartLoc()
		{
			return this->newStartPos;
		}

		void MazeCanMoveReplyPackage::SetStartLoc(char startLoc)
		{
			this->newStartPos = startLoc;
		}

		unsigned MazeCanMoveReplyPackage::GetAgentId()
		{
			return this->agentId;
		}

		void MazeCanMoveReplyPackage::SetAgentId(unsigned id)
		{
			this->agentId = id;
		}

		/*static*/ IPackage *MazeCanMoveReplyPackage::CreatePackage(unsigned source, unsigned dest)
		{
			return new MazeCanMoveReplyPackage(source, dest);
		}

		void MazeCanMoveReplyPackage::Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			Serialize(comm);
			
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->SendPacked(&myBuffer, destRank, messageTag, CommType::Synch);
			}
		}

		void MazeCanMoveReplyPackage::Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			//comm = new Communicator();
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->GetPacked(MazeCanMoveReplyPackageBufSize*IntSize, CommType::ASynch, sourceRank, messageTag, &myBuffer);
			}

			Deserialize(comm);
		}

		void MazeCanMoveReplyPackage::Serialize(Communications::ICommunicator *comm)
		{
			BasePackage::Serialize(&myBuffer, MazeCanMoveReplyPackageBufSize*IntSize, comm);

			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Pack(&agentId, 1, DataType::Int, &myBuffer, MazeCanMoveReplyPackageBufSize*IntSize);
				comm->Pack(&canMove, 1, DataType::Int, &myBuffer, MazeCanMoveReplyPackageBufSize*IntSize);
				comm->Pack(&newStartPos, 1, DataType::Int, &myBuffer, MazeCanMoveReplyPackageBufSize*IntSize);
			}
		}

		void MazeCanMoveReplyPackage::Deserialize(Communications::ICommunicator *comm)
		{
			BasePackage::Deserialize(&myBuffer, MazeCanMoveReplyPackageBufSize*IntSize, comm);
			
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Unpack(&myBuffer, MazeCanMoveReplyPackageBufSize*IntSize, DataType::Int, 1, &agentId);
				comm->Unpack(&myBuffer, MazeCanMoveReplyPackageBufSize*IntSize, DataType::Int, 1, &canMove);
				comm->Unpack(&myBuffer, MazeCanMoveReplyPackageBufSize*IntSize, DataType::Int, 1, &newStartPos);
			}
		}

		/////////////////////////////////////////////////////////
		//MazePositionPackage
		/////////////////////////////////////////////////////////
		GetObjectOnPackage::GetObjectOnPackage() : BasePackage(0, 0, PackageType::GET_OBJ_ON)
		{
		}

		GetObjectOnPackage::GetObjectOnPackage(unsigned packageSource, unsigned packageDest)
			: BasePackage(packageSource, packageDest, PackageType::GET_OBJ_ON)
		{
		}

		GetObjectOnPackage::GetObjectOnPackage(unsigned packageSource, unsigned packageDest, QueryType newType, char object)
			: BasePackage(packageSource, packageDest, PackageType::GET_OBJ_ON), pos(0,0), objectOnPos(object), queryType(newType)
		{
		}

		GetObjectOnPackage::GetObjectOnPackage(unsigned packageSource, unsigned packageDest, QueryType newType, Position<int> newPos)
			: BasePackage(packageSource, packageDest, PackageType::GET_OBJ_ON), pos(newPos), objectOnPos(' '), queryType(newType)
		{
		}

		GetObjectOnPackage::GetObjectOnPackage(unsigned packageSource, unsigned packageDest, QueryType newType, char object, Position<int> newPos)
			: BasePackage(packageSource, packageDest, PackageType::GET_OBJ_ON), pos(newPos), objectOnPos(object), queryType(newType)
		{
		}

		GetObjectOnPackage::~GetObjectOnPackage()
		{
		}
		
		Position<int> GetObjectOnPackage::GetPosition()
		{
			return this->pos;
		}

		void GetObjectOnPackage::SetPosition(Position<int> newPos)
		{
			this->pos = newPos;
		}

		char GetObjectOnPackage::GetObj()
		{
			return this->objectOnPos;
		}

		void GetObjectOnPackage::SetObj(char newObject)
		{
			this->objectOnPos = newObject;
		}

		GetObjectOnPackage::QueryType GetObjectOnPackage::GetQueryType()
		{
			return this->queryType;
		}

		void GetObjectOnPackage::SetQueryType(GetObjectOnPackage::QueryType newType)
		{
			this->queryType = newType;
		}

		void GetObjectOnPackage::Serialize(Communications::ICommunicator *comm)
		{
			BasePackage::Serialize(&myBuffer, GetObjectOnPackageBufSize*IntSize, comm);

			int tempX = this->pos.GetXCoor();
			int tempY = this->pos.GetYCoor();

			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Pack(&this->queryType, 1, DataType::Int, &myBuffer, GetObjectOnPackageBufSize*IntSize);
				comm->Pack(&this->objectOnPos, 1, DataType::Int, &myBuffer, GetObjectOnPackageBufSize*IntSize);
				comm->Pack(&tempX, 1, DataType::Int, &myBuffer, GetObjectOnPackageBufSize*IntSize);
				comm->Pack(&tempY, 1, DataType::Int, &myBuffer, GetObjectOnPackageBufSize*IntSize);
			}
		}

		void GetObjectOnPackage::Deserialize(Communications::ICommunicator *comm)
		{
			int tempX;
			int tempY;

			BasePackage::Deserialize(&myBuffer, GetObjectOnPackageBufSize*IntSize, comm);
			
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Unpack(&myBuffer, GetObjectOnPackageBufSize*IntSize, DataType::Int, 1, &this->queryType);
				comm->Unpack(&myBuffer, GetObjectOnPackageBufSize*IntSize, DataType::Int, 1, &this->objectOnPos);
				comm->Unpack(&myBuffer, GetObjectOnPackageBufSize*IntSize, DataType::Int, 1, &tempX);
				comm->Unpack(&myBuffer, GetObjectOnPackageBufSize*IntSize, DataType::Int, 1, &tempY);
			}

			this->pos.SetXCoor(tempX);
			this->pos.SetYCoor(tempY);
		}

		void GetObjectOnPackage::Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			Serialize(comm);
			
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->SendPacked(&myBuffer, destRank, messageTag, CommType::Synch);
			}
		}

		void GetObjectOnPackage::Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			//comm = new Communicator();
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->GetPacked(GetObjectOnPackageBufSize*IntSize, CommType::ASynch, sourceRank, messageTag, &myBuffer);
			}

			Deserialize(comm);
		}

		/*static*/ IPackage *GetObjectOnPackage::CreatePackage(unsigned source, unsigned dest)
		{
			return new GetObjectOnPackage(source, dest);
		}

		/////////////////////////////////////////////////////////
		//NewMazeAgentPackage
		/////////////////////////////////////////////////////////
		NewMazeAgentPackage::NewMazeAgentPackage() : BasePackage(0, 0, PackageType::NEW_MAZE_AGENT)
		{
		}

		NewMazeAgentPackage::NewMazeAgentPackage(unsigned packageSource, unsigned packageDest)
			: BasePackage(packageSource, packageDest, PackageType::NEW_MAZE_AGENT)
		{
		}

		NewMazeAgentPackage::NewMazeAgentPackage(unsigned packageSource, unsigned packageDest, char startLoc, char goalLoc)
			: BasePackage(packageSource, packageDest, PackageType::NEW_MAZE_AGENT), start(startLoc), goal(goalLoc)
		{
		}

		NewMazeAgentPackage::~NewMazeAgentPackage()
		{
		}                        
		
		char NewMazeAgentPackage::GetStart()
		{
			return this->start;
		} 

		void NewMazeAgentPackage::SetStart(char start)
		{
			this->start = start;
		} 

		char NewMazeAgentPackage::GetGoal()
		{
			return this->goal;
		} 

		void NewMazeAgentPackage::SetGoal(char goal)
		{
			this->goal = goal;
		}		

		/*static*/ IPackage *NewMazeAgentPackage::CreatePackage(unsigned source, unsigned dest)
		{
			return new NewMazeAgentPackage(source, dest);
		}

		void NewMazeAgentPackage::Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			Serialize(comm);
			
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->SendPacked(&myBuffer, destRank, messageTag, CommType::Synch);
			}
		}

		void NewMazeAgentPackage::Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			//comm = new Communicator();
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->GetPacked(NewMazeAgentPackageBufSize*IntSize, CommType::ASynch, sourceRank, messageTag, &myBuffer);
			}

			Deserialize(comm);
		}

		void NewMazeAgentPackage::Serialize(Communications::ICommunicator *comm)
		{
			BasePackage::Serialize(&myBuffer, NewMazeAgentPackageBufSize*IntSize, comm);

			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Pack(&start, 1, DataType::Int, &myBuffer, NewMazeAgentPackageBufSize*IntSize);
				comm->Pack(&goal, 1, DataType::Int, &myBuffer, NewMazeAgentPackageBufSize*IntSize);
			}
		}

		void NewMazeAgentPackage::Deserialize(Communications::ICommunicator *comm)
		{
			BasePackage::Deserialize(&myBuffer, NewMazeAgentPackageBufSize*IntSize, comm);
			
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Unpack(&myBuffer, NewMazeAgentPackageBufSize*IntSize, DataType::Int, 1, &start);
				comm->Unpack(&myBuffer, NewMazeAgentPackageBufSize*IntSize, DataType::Int, 1, &goal);
			}
		}

		/////////////////////////////////////////////////////////
		//MazeAgentPackage
		/////////////////////////////////////////////////////////
		MazeAgentPackage::MazeAgentPackage() : BasePackage(0, 0, PackageType::MAZE_AGENT)
		{
		}

		MazeAgentPackage::MazeAgentPackage(unsigned packageSource, unsigned packageDest)
			: BasePackage(packageSource, packageDest, PackageType::MAZE_AGENT)
		{
		}

		MazeAgentPackage::MazeAgentPackage(unsigned packageSource, unsigned packageDest, MazeAgentInfo agentInformation)
			: BasePackage(packageSource, packageDest, PackageType::MAZE_AGENT), agentInfo(agentInformation)
		{
		}

		MazeAgentPackage::~MazeAgentPackage()
		{
		}                        
		
		MazeAgentInfo MazeAgentPackage::GetAgentInfo()
		{
			return this->agentInfo;
		}

		void MazeAgentPackage::SetAgentInfo(MazeAgentInfo agentInfo)
		{
			this->agentInfo = agentInfo;
		}

		/*static*/ IPackage *MazeAgentPackage::CreatePackage(unsigned source, unsigned dest)
		{
			return new MazeAgentPackage(source, dest);
		}

		void MazeAgentPackage::Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			Serialize(comm);
			
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->SendPacked(&myBuffer, destRank, messageTag, CommType::Synch);
			}
		}

		void MazeAgentPackage::Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			//comm = new Communicator();
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->GetPacked(MazeAgentPackageBufSize*IntSize, CommType::ASynch, sourceRank, messageTag, &myBuffer);
			}

			Deserialize(comm);
		}

		void MazeAgentPackage::Serialize(Communications::ICommunicator *comm)
		{
			BasePackage::Serialize(&myBuffer, MazeAgentPackageBufSize*IntSize, comm);

			if (!Validator::IsNull(comm, NAME("comm")))
			{
				int x = this->agentInfo.prevPos.GetXCoor();
				int y = this->agentInfo.prevPos.GetYCoor();

				comm->Pack(&this->agentInfo.agentId, 1, DataType::Int, &myBuffer, MazeAgentPackageBufSize*IntSize);
				comm->Pack(&this->agentInfo.start, 1, DataType::Int, &myBuffer, MazeAgentPackageBufSize*IntSize);
				comm->Pack(&this->agentInfo.goal, 1, DataType::Int, &myBuffer, MazeAgentPackageBufSize*IntSize);
				comm->Pack(&this->agentInfo.lastDir, 1, DataType::Int, &myBuffer, MazeAgentPackageBufSize*IntSize);
				comm->Pack(&this->agentInfo.phsPathNum, 1, DataType::Int, &myBuffer, MazeAgentPackageBufSize*IntSize);
				comm->Pack(&this->agentInfo.totalNumSteps, 1, DataType::Int, &myBuffer, MazeAgentPackageBufSize*IntSize);
				comm->Pack(&this->agentInfo.transferId, 1, DataType::Int, &myBuffer, MazeAgentPackageBufSize*IntSize);
				comm->Pack(&x, 1, DataType::Int, &myBuffer, MazeAgentPackageBufSize*IntSize);
				comm->Pack(&y, 1, DataType::Int, &myBuffer, MazeAgentPackageBufSize*IntSize);
			}
		}

		void MazeAgentPackage::Deserialize(Communications::ICommunicator *comm)
		{
			BasePackage::Deserialize(&myBuffer, MazeAgentPackageBufSize*IntSize, comm);
			
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				int x = 0;
				int y = 0;

				comm->Unpack(&myBuffer, MazeAgentPackageBufSize*IntSize, DataType::Int, 1, &this->agentInfo.agentId);
				comm->Unpack(&myBuffer, MazeAgentPackageBufSize*IntSize, DataType::Int, 1, &this->agentInfo.start);
				comm->Unpack(&myBuffer, MazeAgentPackageBufSize*IntSize, DataType::Int, 1, &this->agentInfo.goal);
				comm->Unpack(&myBuffer, MazeAgentPackageBufSize*IntSize, DataType::Int, 1, &this->agentInfo.lastDir);
				comm->Unpack(&myBuffer, MazeAgentPackageBufSize*IntSize, DataType::Int, 1, &this->agentInfo.phsPathNum);
				comm->Unpack(&myBuffer, MazeAgentPackageBufSize*IntSize, DataType::Int, 1, &this->agentInfo.totalNumSteps);
				comm->Unpack(&myBuffer, MazeAgentPackageBufSize*IntSize, DataType::Int, 1, &this->agentInfo.transferId);
				comm->Unpack(&myBuffer, MazeAgentPackageBufSize*IntSize, DataType::Int, 1, &x);
				comm->Unpack(&myBuffer, MazeAgentPackageBufSize*IntSize, DataType::Int, 1, &y);

				this->agentInfo.prevPos.SetXCoor(x);
				this->agentInfo.prevPos.SetYCoor(y);
			}
		}

		/////////////////////////////////////////////////////////
		//GetMLAgentInfoPackage
		/////////////////////////////////////////////////////////
		GetMLAgentInfoPackage::GetMLAgentInfoPackage() : BasePackage(0, 0, PackageType::GET_ML_AGENT_INFO)
		{
		}

		GetMLAgentInfoPackage::GetMLAgentInfoPackage(unsigned packageSource, unsigned packageDest) 
			: BasePackage(packageSource, packageDest, PackageType::GET_ML_AGENT_INFO)
		{
			this->id = 0;
		}

		GetMLAgentInfoPackage::GetMLAgentInfoPackage(unsigned packageSource, unsigned packageDest, unsigned id) 
			: BasePackage(packageSource, packageDest, PackageType::GET_ML_AGENT_INFO)
		{
			this->id = id;
		}

		GetMLAgentInfoPackage::~GetMLAgentInfoPackage()
		{
		}
	                    
		void GetMLAgentInfoPackage::Serialize(Communications::ICommunicator *comm)
		{
			BasePackage::Serialize(&myBuffer, RegistrationPackageBufSize*IntSize, comm);
			
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Pack(&id, 1, DataType::Int, &myBuffer, RegistrationPackageBufSize*IntSize);
			}
		}

		void GetMLAgentInfoPackage::Deserialize(Communications::ICommunicator *comm)
		{
			BasePackage::Deserialize(&myBuffer, RegistrationPackageBufSize*IntSize, comm);
			
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Unpack(&myBuffer, RegistrationPackageBufSize*IntSize, DataType::Int, 1, &id);
			}
		}

		void GetMLAgentInfoPackage::Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			Serialize(comm);

			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->SendPacked(&myBuffer, destRank, messageTag, CommType::Synch);
			}
		}

		void GetMLAgentInfoPackage::Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			//comm = new Communicator();
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->GetPacked(RegistrationPackageBufSize*IntSize, CommType::ASynch, sourceRank, messageTag, &myBuffer);
			}
			Deserialize(comm);
		}

		unsigned GetMLAgentInfoPackage::GetID() 
		{ 
			return this->id; 
		}

		void GetMLAgentInfoPackage::SetID(unsigned id) 
		{ 
			this->id = id; 
		}

		/*static*/ IPackage *GetMLAgentInfoPackage::CreatePackage(unsigned source, unsigned dest)
		{
			return new GetMLAgentInfoPackage(source, dest);
		}
		/////////////////////////////////////////////////////////
		//MLAgentInfoPackage
		/////////////////////////////////////////////////////////
		MLAgentInfoPackage::MLAgentInfoPackage() : BasePackage(0, 0, PackageType::ML_AGENT_INFO)
		{
		}

		MLAgentInfoPackage::MLAgentInfoPackage(unsigned packageSource, unsigned packageDest)
			: BasePackage(packageSource, packageDest, PackageType::ML_AGENT_INFO)
		{
		}

		MLAgentInfoPackage::MLAgentInfoPackage(unsigned packageSource, unsigned packageDest, MLAgentInfo newInfo)
			: BasePackage(packageSource, packageDest, PackageType::ML_AGENT_INFO)
		{
			SetInfo(newInfo);
		}

		MLAgentInfoPackage::~MLAgentInfoPackage()
		{
		}
		
		MLAgentInfo MLAgentInfoPackage::GetInfo()
		{
			return this->info;
		}

		void MLAgentInfoPackage::SetInfo(MLAgentInfo newInfo)
		{
			this->info.SetGoal(newInfo.GetGoal());
			this->info.SetStart(newInfo.GetStart());
		}

		void MLAgentInfoPackage::Serialize(Communications::ICommunicator *comm)
		{
			BasePackage::Serialize(&myBuffer, MLAgentInfoPackageBufSize*IntSize, comm);

			char start = this->info.GetStart();
			char goal = this->info.GetGoal();

			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Pack(&start, 1, DataType::Int, &myBuffer, MLAgentInfoPackageBufSize*IntSize);
				comm->Pack(&goal, 1, DataType::Int, &myBuffer, MLAgentInfoPackageBufSize*IntSize);
			}
		}

		void MLAgentInfoPackage::Deserialize(Communications::ICommunicator *comm)
		{
			char start;
			char goal;

			BasePackage::Deserialize(&myBuffer, MLAgentInfoPackageBufSize*IntSize, comm);
			
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Unpack(&myBuffer, MLAgentInfoPackageBufSize*IntSize, DataType::Int, 1, &start);
				comm->Unpack(&myBuffer, MLAgentInfoPackageBufSize*IntSize, DataType::Int, 1, &goal);
			}

			this->info.SetStart(start);
			this->info.SetGoal(goal);
		}

		void MLAgentInfoPackage::Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			Serialize(comm);
			
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->SendPacked(&myBuffer, destRank, messageTag, CommType::Synch);
			}
		}

		void MLAgentInfoPackage::Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			//comm = new Communicator();
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->GetPacked(MLAgentInfoPackageBufSize*IntSize, CommType::ASynch, sourceRank, messageTag, &myBuffer);
			}

			Deserialize(comm);
		}

		/*static*/ IPackage *MLAgentInfoPackage::CreatePackage(unsigned source, unsigned dest)
		{
			return new MLAgentInfoPackage(source, dest);
		}
		/////////////////////////////////////////////////////////
		//MazePackage
		/////////////////////////////////////////////////////////
		MazePackage::MazePackage() : BasePackage(0, 0, PackageType::MAZE), maze(NULL), secondBuffer(NULL), isResponsible(true)
		{
		}

		MazePackage::MazePackage(unsigned packageSource, unsigned packageDest) 
			: BasePackage(packageSource, packageDest, PackageType::MAZE), maze(NULL), secondBuffer(NULL), isResponsible(true)
		{
		}

		MazePackage::MazePackage(unsigned packageSource, unsigned packageDest, Maze *newMaze, Position<int> newOffset)
			: BasePackage(packageSource, packageDest, PackageType::MAZE), maze(newMaze), secondBuffer(NULL), isResponsible(true), offset(newOffset)
		{
			if (!Validator::IsNull(this->maze, NAME("this->maze")))
			{
				unsigned tempYSize = this->maze->GetYSize();
				unsigned tempXSize = this->maze->GetXSize();

				this->secondBuffer = new unsigned[tempYSize*tempXSize];
				this->totalPackageSize = (tempYSize*tempXSize)*IntSize;
			}
		}

		MazePackage::~MazePackage()
		{
			if (this->isResponsible)
			{
				delete maze; 
				maze = NULL;
			}

			delete [] this->secondBuffer; 
			this->secondBuffer = NULL;
		}

		void MazePackage::SetMaze(Maze *newMaze)
		{
			this->maze = newMaze;

			if (!Validator::IsNull(this->maze, NAME("this->maze")))
			{
				unsigned tempYSize = this->maze->GetYSize();
				unsigned tempXSize = this->maze->GetXSize();				

				this->secondBuffer = new unsigned[tempYSize*tempXSize];
				this->totalPackageSize = (tempYSize*tempXSize)*IntSize;
			}
			
			delete [] this->secondBuffer;
			this->secondBuffer = NULL;
		}

		Maze *MazePackage::GetMaze()
		{
			this->isResponsible = false;
			return this->maze;
		}

		void MazePackage::SetOffset(Position<int> newOffset)
		{
			this->offset = newOffset;
		}

		Position<int> MazePackage::GetOffset()
		{
			return this->offset;
		}

		void MazePackage::Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			Serialize(comm);
			
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->SendPacked(&firstBuffer, destRank, messageTag, CommType::Synch);
			}

			SerializeMaze(comm);

			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->SendPacked(secondBuffer, destRank, messageTag, CommType::Synch);
			}
		}

		void MazePackage::Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			//comm = new Communicator();

			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->GetPacked(MazePackageBufSize*IntSize, CommType::ASynch, sourceRank, messageTag, &firstBuffer);			
			}

			Deserialize(comm);

			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->GetPacked(totalPackageSize, CommType::ASynch, sourceRank, messageTag, secondBuffer);
			}

			DeserializeMaze(comm);
		}

		void MazePackage::Serialize(Communications::ICommunicator *comm)
		{
			BasePackage::Serialize(&firstBuffer, MazePackageBufSize*IntSize, comm);

			if (!Validator::IsNull(this->maze, NAME("this->maze")))
			{
				unsigned ySize = this->maze->GetYSize();
				unsigned xSize = this->maze->GetXSize();
				unsigned xOffset = this->offset.GetXCoor();
				unsigned yOffset = this->offset.GetYCoor();

				if (!Validator::IsNull(comm, NAME("comm")))
				{
					comm->Pack(&xSize, 1, DataType::Int, &firstBuffer, MazePackageBufSize*IntSize);
					comm->Pack(&ySize, 1, DataType::Int, &firstBuffer, MazePackageBufSize*IntSize);

					comm->Pack(&xOffset, 1, DataType::Int, &firstBuffer, MazePackageBufSize*IntSize);
					comm->Pack(&yOffset, 1, DataType::Int, &firstBuffer, MazePackageBufSize*IntSize);
				}
			}
			else
			{
				ILog *log = LogManager::Instance();

				if (!Validator::IsNull(log, NAME("log")))
				{
					log->Log(&typeid(this), "Maze is NULL, package source: %d, package dest: %d", this->packageSource, this->packageDest);
				}
			}
		}

		void MazePackage::SerializeMaze(Communications::ICommunicator *comm)
		{
			if (!Validator::IsNull(this->maze, NAME("this->maze")))
			{
				unsigned tempYSize = this->maze->GetYSize();
				unsigned tempXSize = this->maze->GetXSize();
				char **tempMaze = this->maze->GetMaze();

				if (!Validator::IsNull(comm, NAME("comm")))
				{
					for (unsigned i = 0; i < tempYSize; i++)
					{
						for (unsigned j = 0; j < tempXSize; j++)
						{
							comm->Pack(&tempMaze[i][j], 1, DataType::Int, secondBuffer, totalPackageSize);
						}
					}
				}
			}
		}

		void MazePackage::Deserialize(Communications::ICommunicator *comm)
		{
			unsigned xOffset;
			unsigned yOffset;

			BasePackage::Deserialize(&firstBuffer, MazePackageBufSize*IntSize, comm);

			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Unpack(&firstBuffer, MazePackageBufSize*IntSize, DataType::Int, 1, &this->xSize);
				comm->Unpack(&firstBuffer, MazePackageBufSize*IntSize, DataType::Int, 1, &this->ySize);

				comm->Unpack(&firstBuffer, MazePackageBufSize*IntSize, DataType::Int, 1, &xOffset);
				comm->Unpack(&firstBuffer, MazePackageBufSize*IntSize, DataType::Int, 1, &yOffset);
			}

			this->offset.SetXCoor(xOffset);
			this->offset.SetYCoor(yOffset);

			this->secondBuffer = new unsigned[this->ySize*this->xSize];
			this->totalPackageSize = this->ySize*this->xSize*IntSize;
		}

		void MazePackage::DeserializeMaze(Communications::ICommunicator *comm)
		{
			char **tempMaze = new char *[this->ySize];

			for (unsigned i = 0; i < this->ySize; i++)
			{
				tempMaze[i] = new char[this->xSize];
			}

			if (!Validator::IsNull(comm, NAME("comm")))
			{
				for (unsigned i = 0; i < this->ySize; i++)
				{
					for (unsigned j = 0; j < this->xSize; j++)
					{
						comm->Unpack(secondBuffer, totalPackageSize, DataType::Int, 1, &tempMaze[i][j]);
					}
				}
			}

			this->maze = new Maze(ySize, xSize, tempMaze);
		}

		/*static*/ IPackage *MazePackage::CreatePackage(unsigned source, unsigned dest)
		{
			return new MazePackage(source, dest);
		}
		/////////////////////////////////////////////////////////
		//EventSubscriptionPackage
		/////////////////////////////////////////////////////////
		EventSubscriptionPackage::EventSubscriptionPackage() : BasePackage(0, 0, PackageType::EVENT_SUBSCRIPTION)
		{
		}

		EventSubscriptionPackage::EventSubscriptionPackage(unsigned packageSource, unsigned packageDest) 
			: BasePackage(packageSource, packageDest, PackageType::EVENT_SUBSCRIPTION)
		{
		}

		EventSubscriptionPackage::EventSubscriptionPackage(unsigned packageSource, unsigned packageDest, 
									 Eventing::SubscriptionTypes::SubscriptionType subscription,
									 Eventing::NotificationType::Life life)
			: BasePackage(packageSource, packageDest, PackageType::EVENT_SUBSCRIPTION), subscriptionType(subscription), subscriptionLife(life)
		{
		}

		EventSubscriptionPackage::~EventSubscriptionPackage()
		{
		}

		void EventSubscriptionPackage::Serialize(Communications::ICommunicator *comm)
		{
			BasePackage::Serialize(&myBuffer, EventSubscriptionPackageBufSize*IntSize, comm);
			
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Pack(&subscriptionType, 1, DataType::Int, &myBuffer, EventSubscriptionPackageBufSize*IntSize);
				comm->Pack(&subscriptionLife, 1, DataType::Int, &myBuffer, EventSubscriptionPackageBufSize*IntSize);
			}
		}

		void EventSubscriptionPackage::Deserialize(Communications::ICommunicator *comm)
		{
			BasePackage::Deserialize(&myBuffer, EventSubscriptionPackageBufSize*IntSize, comm);
			
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->Unpack(&myBuffer, EventSubscriptionPackageBufSize*IntSize, DataType::Int, 1, &subscriptionType);
				comm->Unpack(&myBuffer, EventSubscriptionPackageBufSize*IntSize, DataType::Int, 1, &subscriptionLife);
			}
		}

		void EventSubscriptionPackage::Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			Serialize(comm);
			//_asm {int 3};
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->SendPacked(&myBuffer, destRank, messageTag, CommType::Synch);
			}
		}

		void EventSubscriptionPackage::Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm)
		{
			//comm = new Communicator();
			if (!Validator::IsNull(comm, NAME("comm")))
			{
				comm->GetPacked(EventSubscriptionPackageBufSize*IntSize, CommType::ASynch, sourceRank, messageTag, &myBuffer);
			}

			Deserialize(comm);
		}

		void EventSubscriptionPackage::SetSubscription(SubscriptionTypes::SubscriptionType subscription)
		{
			this->subscriptionType = subscription;
		}

		SubscriptionTypes::SubscriptionType EventSubscriptionPackage::GetSubscription()
		{
			return this->subscriptionType;
		}

		void EventSubscriptionPackage::SetSubscriptionLife(Eventing::NotificationType::Life life)
		{
			this->subscriptionLife = life;
		}

		Eventing::NotificationType::Life EventSubscriptionPackage::GetSubscriptionLife()
		{
			return this->subscriptionLife;
		}

		/*static*/ IPackage *EventSubscriptionPackage::CreatePackage(unsigned source, unsigned dest)
		{
			return new EventSubscriptionPackage(source, dest);
		}		
	}
}