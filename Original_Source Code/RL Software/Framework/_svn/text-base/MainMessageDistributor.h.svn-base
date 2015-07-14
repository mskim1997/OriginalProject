/////////////////////////////////////////////////////////
//MainMessageDistributor.h
//Defines the MainMessageDistributor, who's responsible
//for distributing message and storing global data.
//
//Manuel Madera
//6/26/09
/////////////////////////////////////////////////////////

#ifndef MainMessageDistributor_h
#define MainMessageDistributor_h

#include "IMessageDistributor.h"
#include <pthread.h>
#include "ICommMgr.h"
#include <map>
#include "Framework.h"
#include "Maze.h"
#include <list>
#include <semaphore.h>
#include "Position.h"
#include "LogManager.h"

using namespace std;

namespace Framework
{
	namespace Messaging
	{
		class FRAMEWORK_API RegistryID
		{
		private:
			unsigned currID;
		public:
			RegistryID();
			virtual ~RegistryID();

			unsigned GetID();
			void FreeID(unsigned id);
		};

		class FRAMEWORK_API MainMessageDistributor : public IMessageDistributor
		{
		private:
			typedef struct
			{
				void *ownerObject;
				MessageCallback callback;
			} Callback;

			RegistryID registryID;
			Communications::ICommMgr *pMyCommMgr;
			sem_t binSem;
			pthread_t mainThread;
			bool terminate;
			unsigned myRank;
			unsigned totalNumNode;
			unsigned terminatedNodesCount;
			map<unsigned, unsigned> agentsRegistry;
			//map<unsigned, list<MLAgentInfo>> mlAgentsInfo;
			map<Packages::PackageType::Package, Callback> callbackMap;
			Logging::ILog *log;

		public:
			MainMessageDistributor(unsigned rank, unsigned totalNumNode);
			virtual ~MainMessageDistributor();

			void Wait();
			void Terminate();
	                
			//These methods do nothing
			void Register(Agents::IAgent *agent, Agents::AgentQueue *agentQ);
			void Unregister(Agents::IAgent *agent);

			void RegisterCallback(Packages::PackageType::Package type, void *ownerObject, MessageCallback callback);
			void UnregisterCallback(Packages::PackageType::Package type);

			void SendPackage(Packages::IPackage *package);

			void Broadcast(unsigned senderID, Framework::Packages::MazePositionPackage *mazePosPackage);

			Position<int> GetOffset(){ return Position<int>(0,0); }
		private:
			static void DistributorCallback(void *ownerObject, Packages::IPackage *msg);

			void Send(Packages::IPackage *package, unsigned dest);
			static void *MainThread(void *arg);
		};
	}
}

#endif  //MainMessageDistributor_h