/////////////////////////////////////////////////////////
//Packages.h
//Defines the different packages.
//
//Manuel Madera
//5/8/09
/////////////////////////////////////////////////////////

#ifndef Packages_h
#define Packages_h

#include "ICommunicator.h"
#include "Framework.h"
#include "Position.h"
#include "Maze.h"
#include "ISubscription.h"
#include "ISubscriber.h"

//Size in number of ints
#define BasePackageBufSize				3   //12 bytes
#define MessagePackageBufSize			5   //20 bytes
#define PositionPackageBufSize			5   //20 bytes
#define MazePositionPackageBufSize		6   //24 bytes
#define MLAgentInfoPackageBufSize		5   //20 bytes
#define RegistrationPackageBufSize		4   //16 bytes
#define MazePackageBufSize				7	//28 bytes
#define EventSubscriptionPackageBufSize	5	//20 bytes
#define MazeLimitPackageBufSize			5	//20 bytes
#define MazeCanMovePackageBufSize		6	//24 bytes
#define MazeCanMoveReplyPackageBufSize	6	//24 bytes
#define MazeAgentPackageBufSize			12	//48 bytes
#define NewMazeAgentPackageBufSize		5	//20 bytes
#define GetObjectOnPackageBufSize		7	//28 bytes

#define IntSize                     sizeof(int)//4
#define DoubleSize					sizeof(double)

namespace Framework
{
	namespace Packages
	{
		namespace PackageType
		{
			enum Package
			{
				NONE = 0,
				AGENT,
				MESSAGE,
				REGISTRATION,
				UNREGISTER,
				CONFIRMATION,
				POSITION,
				MAZE_POSITION,
				MAZE_LIMIT,
				MAZE_CAN_MOVE,
				MAZE_CAN_MOVE_REPLY,
				MAZE_AGENT,
				NEW_MAZE_AGENT,
				AGENT_DONE_EVENT,
				PHYSICAL_PATH_DONE_EVENT,
				GET_OBJ_ON,
				TERMINATE,
				GET_ML_AGENT_INFO,
				ML_AGENT_INFO,
				MAZE,
				EVENT_SUBSCRIPTION,
				MAZE_POSITION_EVENT,
				NODE_READY
			};
		}
	        
		/////////////////////////////////////////////////////////
		//IPackage
		/////////////////////////////////////////////////////////
		class FRAMEWORK_API IPackage
		{
		public:
			virtual ~IPackage() {}
	        
			virtual void SetDestination(unsigned destRank) = 0;
			virtual void SetSource(unsigned sourceRank) = 0;
			virtual unsigned GetDestination() = 0;
			virtual unsigned GetSource() = 0;
			virtual PackageType::Package GetType() = 0;

			//Send and Get are needed to send the actual message across
			virtual void Send(unsigned messageTag, Communications::ICommunicator *comm) = 0;
			virtual void Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm) = 0;

			//This method is needed by the MessagePackage
			virtual void Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm) = 0;
		};

		/////////////////////////////////////////////////////////
		//BasePackage: All packages must derive from this class, and must register
		//with the PackagerFactory.
		/////////////////////////////////////////////////////////
		class FRAMEWORK_API BasePackage : public IPackage
		{
		protected:
			unsigned packageSource;
			unsigned packageDest;
			PackageType::Package myType;
			unsigned buffer[BasePackageBufSize];
			//Communications::ICommunicator *comm;
			unsigned bufferSizeInBytes;

		public:
			BasePackage();

			BasePackage(unsigned packageSource, unsigned packageDest, PackageType::Package type);

			virtual ~BasePackage();                  

			void SetDestination(unsigned destRank);
			void SetSource(unsigned sourceRank);
	        
			unsigned GetDestination();
			unsigned GetSource();

			PackageType::Package GetType();

			virtual void Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm);

			virtual void Send(unsigned messageTag, Communications::ICommunicator *comm);

			static IPackage *CreatePackage(unsigned source, unsigned dest);
		protected:
			//This method is needed by the MessagePackage
			virtual void Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm);
	        

			virtual void Serialize(Communications::ICommunicator *comm);

			virtual void Deserialize(Communications::ICommunicator *comm);

			//Use when creating a child of this class
			void Serialize(void *buf, unsigned bufSize, Communications::ICommunicator *comm);

			//Use when creating a child of this class
			void Deserialize(void *buf, unsigned bufSize, Communications::ICommunicator *comm);
		};

		/////////////////////////////////////////////////////////
		//EmptyPackage
		/////////////////////////////////////////////////////////
		class FRAMEWORK_API EmptyPackage : public BasePackage
		{
		public:
			EmptyPackage();
			EmptyPackage(unsigned packageSource, unsigned packageDest);

			static IPackage *CreatePackage(unsigned source, unsigned dest);
			virtual ~EmptyPackage();
		};

		/////////////////////////////////////////////////////////
		//NodeReadyPackage
		/////////////////////////////////////////////////////////
		class FRAMEWORK_API NodeReadyPackage : public BasePackage
		{
		public:
			NodeReadyPackage();
			NodeReadyPackage(unsigned packageSource, unsigned packageDest);

			static IPackage *CreatePackage(unsigned source, unsigned dest);
			virtual ~NodeReadyPackage();
		};

		/////////////////////////////////////////////////////////
		//TerminatePackage
		/////////////////////////////////////////////////////////
		class FRAMEWORK_API TerminatePackage : public BasePackage
		{
		public:
			TerminatePackage();
			TerminatePackage(unsigned packageSource, unsigned packageDest);

			static IPackage *CreatePackage(unsigned source, unsigned dest);
			virtual ~TerminatePackage();
		};

		/////////////////////////////////////////////////////////
		//MessagePackage
		/////////////////////////////////////////////////////////
		class FRAMEWORK_API MessagePackage : public BasePackage
		{
		private:
			IPackage *message;
			bool deleteMessage;
			unsigned myBuffer[MessagePackageBufSize];
		public:
			MessagePackage();
			MessagePackage(unsigned packageSource, unsigned packageDest);

			MessagePackage(unsigned packageSource, unsigned packageDest, IPackage *msg);

			virtual ~MessagePackage();  
	        
			PackageType::Package GetInnerPackageType();

			static IPackage *CreatePackage(unsigned source, unsigned dest);
			IPackage *GetPackage();
		protected:
			void Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm);
			void Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm);

			void Serialize(Communications::ICommunicator *comm);

			void Deserialize(Communications::ICommunicator *comm);
		};

		/////////////////////////////////////////////////////////
		//RegistrationPackage
		/////////////////////////////////////////////////////////
		class FRAMEWORK_API RegistrationPackage : public BasePackage
		{
		private:
			unsigned id;
			unsigned myBuffer[RegistrationPackageBufSize];
		public:
			RegistrationPackage();
			RegistrationPackage(unsigned packageSource, unsigned packageDest);

			RegistrationPackage(unsigned packageSource, unsigned packageDest, unsigned id);

			virtual ~RegistrationPackage();                        
	        
			static IPackage *CreatePackage(unsigned source, unsigned dest);

			unsigned GetID();
			void SetID(unsigned id);
		protected:
			void Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm);
			void Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm);

			void Serialize(Communications::ICommunicator *comm);

			void Deserialize(Communications::ICommunicator *comm);
		};

		/////////////////////////////////////////////////////////
		//UnregisterPackage
		/////////////////////////////////////////////////////////
		class FRAMEWORK_API UnregisterPackage : public BasePackage
		{
		private:
			unsigned id;
			unsigned myBuffer[RegistrationPackageBufSize];	//Not a bug the size of this package is the same as registration package
		public:
			UnregisterPackage();
			UnregisterPackage(unsigned packageSource, unsigned packageDest);

			UnregisterPackage(unsigned packageSource, unsigned packageDest, unsigned id);

			virtual ~UnregisterPackage();                        
	        
			static IPackage *CreatePackage(unsigned source, unsigned dest);
			unsigned GetID();
		protected:
			void Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm);
			void Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm);

			void Serialize(Communications::ICommunicator *comm);

			void Deserialize(Communications::ICommunicator *comm);
		};

		/////////////////////////////////////////////////////////
		//ConfirmationPackage
		/////////////////////////////////////////////////////////
		class FRAMEWORK_API ConfirmationPackage : public BasePackage
		{
		private:
			unsigned id;
			unsigned myBuffer[RegistrationPackageBufSize];	//Not a bug the size of this package is the same as registration package
		public:
			ConfirmationPackage();
			ConfirmationPackage(unsigned packageSource, unsigned packageDest);

			static IPackage *CreatePackage(unsigned source, unsigned dest);
			virtual ~ConfirmationPackage();
	        
		protected:
			void Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm);
			void Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm);

			void Serialize(Communications::ICommunicator *comm);

			void Deserialize(Communications::ICommunicator *comm);
		};

		/////////////////////////////////////////////////////////
		//PositionPackage: Used by the LinkWeight project.
		/////////////////////////////////////////////////////////
		class FRAMEWORK_API PositionPackage : public BasePackage
		{
		private:
			Position<double> pos;
			double myBuffer[PositionPackageBufSize];
		public:
			PositionPackage();
			PositionPackage(unsigned packageSource, unsigned packageDest);

			PositionPackage(unsigned packageSource, unsigned packageDest, Position<double> newPos);

			static IPackage *CreatePackage(unsigned source, unsigned dest);
			virtual ~PositionPackage();                        
		    
			Position<double> GetPosition();
			void SetPosition(Position<double> newPos);
		protected:
			void Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm);
			void Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm);

			void Serialize(Communications::ICommunicator *comm);

			void Deserialize(Communications::ICommunicator *comm);
		};

		/////////////////////////////////////////////////////////
		//MazePositionPackage
		/////////////////////////////////////////////////////////
		class FRAMEWORK_API MazePositionPackage : public BasePackage
		{
		private:
			unsigned objectOnPos;
			Position<int> pos;
			unsigned myBuffer[MazePositionPackageBufSize];
		public:
			MazePositionPackage();
			MazePositionPackage(unsigned packageSource, unsigned packageDest);

			MazePositionPackage(unsigned packageSource, unsigned packageDest, char object, Position<int> newPos);

			virtual ~MazePositionPackage();                        
		    
			Position<int> GetPosition();
			void SetPosition(Position<int> newPos);

			char GetObject();
			void SetObject(char newObject);

			static IPackage *CreatePackage(unsigned source, unsigned dest);
			MazePositionPackage *Clone();
		protected:
			void Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm);
			void Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm);

			void Serialize(Communications::ICommunicator *comm);

			void Deserialize(Communications::ICommunicator *comm);
		};

		/////////////////////////////////////////////////////////
		//MazeLimitPackage
		/////////////////////////////////////////////////////////
		class FRAMEWORK_API MazeLimitPackage : public BasePackage
		{
		private:
			Position<int> limit;
			unsigned myBuffer[MazeLimitPackageBufSize];

		public:
			MazeLimitPackage();
			MazeLimitPackage(unsigned packageSource, unsigned packageDest);

			MazeLimitPackage(unsigned packageSource, unsigned packageDest, Position<int> newLimit);

			virtual ~MazeLimitPackage();                        
		    
			Position<int> GetLimit();
			void SetLimit(Position<int> limit);

			static IPackage *CreatePackage(unsigned source, unsigned dest);
		protected:
			void Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm);
			void Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm);

			void Serialize(Communications::ICommunicator *comm);

			void Deserialize(Communications::ICommunicator *comm);
		};

		/////////////////////////////////////////////////////////
		//MazeCanMovePackage
		/////////////////////////////////////////////////////////
		class FRAMEWORK_API MazeCanMovePackage : public BasePackage
		{
		private:
			Position<int> pos;
			unsigned agentId;
			unsigned myBuffer[MazeCanMovePackageBufSize];

		public:
			MazeCanMovePackage();
			MazeCanMovePackage(unsigned packageSource, unsigned packageDest);

			MazeCanMovePackage(unsigned packageSource, unsigned packageDest, Position<int> pos, unsigned id);

			virtual ~MazeCanMovePackage();                        
		    
			Position<int> GetPos();
			void SetPos(Position<int> pos);

			unsigned GetAgentId();
			void SetAgentId(unsigned id);

			static IPackage *CreatePackage(unsigned source, unsigned dest);
		protected:
			void Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm);
			void Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm);

			void Serialize(Communications::ICommunicator *comm);

			void Deserialize(Communications::ICommunicator *comm);
		};

		/////////////////////////////////////////////////////////
		//MazeCanMoveReplyPackage
		/////////////////////////////////////////////////////////
		class FRAMEWORK_API MazeCanMoveReplyPackage : public BasePackage
		{
		private:
			bool canMove;
			char newStartPos;
			unsigned agentId;
			unsigned myBuffer[MazeCanMoveReplyPackageBufSize];

		public:
			MazeCanMoveReplyPackage();
			MazeCanMoveReplyPackage(unsigned packageSource, unsigned packageDest);

			MazeCanMoveReplyPackage(unsigned packageSource, unsigned packageDest, bool move, unsigned id, char newStart);

			virtual ~MazeCanMoveReplyPackage();                        
		    
			bool GetCanMove();
			void SetCanMove(bool move);

			char GetStartLoc();
			void SetStartLoc(char startLoc);

			unsigned GetAgentId();
			void SetAgentId(unsigned id);

			static IPackage *CreatePackage(unsigned source, unsigned dest);
		protected:
			void Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm);
			void Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm);

			void Serialize(Communications::ICommunicator *comm);

			void Deserialize(Communications::ICommunicator *comm);
		};

		/////////////////////////////////////////////////////////
		//GetObjectOnPackage
		/////////////////////////////////////////////////////////
		class FRAMEWORK_API GetObjectOnPackage : public BasePackage
		{
		public:
			enum QueryType
			{
				GET_POSITION,
				GET_OBJECT
			};

		private:
			QueryType queryType;
			unsigned objectOnPos;
			Position<int> pos;
			unsigned myBuffer[GetObjectOnPackageBufSize];
		public:
			GetObjectOnPackage();
			GetObjectOnPackage(unsigned packageSource, unsigned packageDest);
			GetObjectOnPackage(unsigned packageSource, unsigned packageDest, QueryType newType, char object);
			GetObjectOnPackage(unsigned packageSource, unsigned packageDest, QueryType newType, Position<int> newPos);
			GetObjectOnPackage(unsigned packageSource, unsigned packageDest, QueryType newType, char object, Position<int> newPos);

			virtual ~GetObjectOnPackage();                        
		    
			Position<int> GetPosition();
			void SetPosition(Position<int> newPos);

			char GetObj();
			void SetObj(char newObject);

			QueryType GetQueryType();
			void SetQueryType(QueryType newType);

			static IPackage *CreatePackage(unsigned source, unsigned dest);
		protected:
			void Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm);
			void Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm);

			void Serialize(Communications::ICommunicator *comm);

			void Deserialize(Communications::ICommunicator *comm);
		};

		/////////////////////////////////////////////////////////
		//NewMazeAgentPackage
		/////////////////////////////////////////////////////////
		class FRAMEWORK_API NewMazeAgentPackage : public BasePackage
		{
		private:
			char start;
			char goal;
			unsigned myBuffer[NewMazeAgentPackageBufSize];

		public:
			NewMazeAgentPackage();
			NewMazeAgentPackage(unsigned packageSource, unsigned packageDest);

			NewMazeAgentPackage(unsigned packageSource, unsigned packageDest, char startLoc, char goalLoc);

			virtual ~NewMazeAgentPackage();                        
		    
			char GetStart();
			void SetStart(char start);

			char GetGoal();
			void SetGoal(char goal);

			static IPackage *CreatePackage(unsigned source, unsigned dest);
		protected:
			void Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm);
			void Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm);

			void Serialize(Communications::ICommunicator *comm);

			void Deserialize(Communications::ICommunicator *comm);
		};

		/////////////////////////////////////////////////////////
		//MazeAgentInfo
		/////////////////////////////////////////////////////////
		typedef struct
		{
			char start;
			char goal;
			unsigned agentId;
			unsigned lastDir;
			unsigned phsPathNum;
			unsigned transferId;
			bool canNotify;
			int totalNumSteps;
			Position<int> prevPos;
		} MazeAgentInfo;

		/////////////////////////////////////////////////////////
		//MazeAgentPackage
		/////////////////////////////////////////////////////////
		class FRAMEWORK_API MazeAgentPackage : public BasePackage
		{
		private:
			MazeAgentInfo agentInfo;
			unsigned myBuffer[MazeAgentPackageBufSize];

		public:
			MazeAgentPackage();
			MazeAgentPackage(unsigned packageSource, unsigned packageDest);

			MazeAgentPackage(unsigned packageSource, unsigned packageDest, MazeAgentInfo agentInformation);

			virtual ~MazeAgentPackage();                        
		    
			MazeAgentInfo GetAgentInfo();
			void SetAgentInfo(MazeAgentInfo agentInfo);

			static IPackage *CreatePackage(unsigned source, unsigned dest);
		protected:
			void Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm);
			void Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm);

			void Serialize(Communications::ICommunicator *comm);

			void Deserialize(Communications::ICommunicator *comm);
		};

		/////////////////////////////////////////////////////////
		//GetMLAgentInfoPackage
		/////////////////////////////////////////////////////////
		class FRAMEWORK_API GetMLAgentInfoPackage : public BasePackage
		{
		private:
			unsigned id;
			unsigned myBuffer[RegistrationPackageBufSize];	//Not a bug the size of this package is the same as registration package
		public:
			GetMLAgentInfoPackage();
			GetMLAgentInfoPackage(unsigned packageSource, unsigned packageDest);
			GetMLAgentInfoPackage(unsigned packageSource, unsigned packageDest, unsigned id);

			virtual ~GetMLAgentInfoPackage();
	        
			static IPackage *CreatePackage(unsigned source, unsigned dest);

			unsigned GetID();
			void SetID(unsigned id);
		protected:
			void Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm);
			void Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm);

			void Serialize(Communications::ICommunicator *comm);

			void Deserialize(Communications::ICommunicator *comm);
		};

		/////////////////////////////////////////////////////////
		//MLAgentInfoPackage
		/////////////////////////////////////////////////////////
		class FRAMEWORK_API MLAgentInfoPackage : public BasePackage
		{
		private:
			MLAgentInfo info;
			unsigned myBuffer[MLAgentInfoPackageBufSize];
		public:
			MLAgentInfoPackage();
			MLAgentInfoPackage(unsigned packageSource, unsigned packageDest);

			MLAgentInfoPackage(unsigned packageSource, unsigned packageDest, MLAgentInfo newInfo);

			virtual ~MLAgentInfoPackage();                        
		    
			static IPackage *CreatePackage(unsigned source, unsigned dest);

			MLAgentInfo GetInfo();
			void SetInfo(MLAgentInfo newInfo);
		protected:
			void Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm);
			void Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm);

			void Serialize(Communications::ICommunicator *comm);

			void Deserialize(Communications::ICommunicator *comm);
		};

		/////////////////////////////////////////////////////////
		//MazePackage
		/////////////////////////////////////////////////////////
		class FRAMEWORK_API MazePackage : public BasePackage
		{
		private:
			bool isResponsible;
			unsigned totalPackageSize;
			Maze *maze;
			unsigned xSize;			//Only used when deserializing
			unsigned ySize;			//Only used when deserializing
			Position<int> offset;

			unsigned firstBuffer[MazePackageBufSize];
			unsigned *secondBuffer;
		public:
			MazePackage();
			MazePackage(unsigned packageSource, unsigned packageDest);
			MazePackage(unsigned packageSource, unsigned packageDest, Maze *newMaze, Position<int> newOffset);

			virtual ~MazePackage();                        
		    
			static IPackage *CreatePackage(unsigned source, unsigned dest);

			void SetMaze(Maze *newMaze);
			Maze *GetMaze();

			void SetOffset(Position<int> newOffset);
			Position<int> GetOffset();
		protected:
			void Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm);
			void Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm);

			void Serialize(Communications::ICommunicator *comm);
			void SerializeMaze(Communications::ICommunicator *comm);

			void Deserialize(Communications::ICommunicator *comm);
			void DeserializeMaze(Communications::ICommunicator *comm);
		};

		/////////////////////////////////////////////////////////
		//EventSubscriptionPackage
		/////////////////////////////////////////////////////////
		class FRAMEWORK_API EventSubscriptionPackage : public BasePackage
		{
		private:
			Eventing::SubscriptionTypes::SubscriptionType subscriptionType;
			Eventing::NotificationType::Life subscriptionLife;

			unsigned myBuffer[EventSubscriptionPackageBufSize];
		public:
			EventSubscriptionPackage();
			EventSubscriptionPackage(unsigned packageSource, unsigned packageDest);
			EventSubscriptionPackage(unsigned packageSource, unsigned packageDest, 
									 Eventing::SubscriptionTypes::SubscriptionType subscription,
									 Eventing::NotificationType::Life life);

			virtual ~EventSubscriptionPackage();                        
		    
			static IPackage *CreatePackage(unsigned source, unsigned dest);

			void SetSubscription(Eventing::SubscriptionTypes::SubscriptionType subscription);
			Eventing::SubscriptionTypes::SubscriptionType GetSubscription();

			void SetSubscriptionLife(Eventing::NotificationType::Life life);
			Eventing::NotificationType::Life GetSubscriptionLife();
		protected:
			void Send(unsigned destRank, unsigned messageTag, Communications::ICommunicator *comm);
			void Get(unsigned sourceRank, unsigned messageTag, Communications::ICommunicator *comm);

			void Serialize(Communications::ICommunicator *comm);
			void Deserialize(Communications::ICommunicator *comm);
		};		
	}
}

#endif  //Packages_h