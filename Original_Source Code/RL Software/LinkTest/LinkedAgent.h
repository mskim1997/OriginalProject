/////////////////////////////////////////////////////////
//LinkedAgent.h
//Defines the LinkedAgent class.  This agent runs the 
//Interconnected dynamical system simulation.
//
//Manuel Madera
//7/22/09
/////////////////////////////////////////////////////////

#ifndef LinkedAgent_h
#define LinkedAgent_h

#include "Framework/Agent.h"
#include "Framework/Position.h"
#include "Framework/ITimer.h"
#include <semaphore.h>
#include <vector>
#include <map>
#include "Framework/ILog.h"
#include "Framework\Publisher.h"

using namespace std;

namespace WorldTest
{
	class LinkedAgent : public Framework::Agents::Agent
	{
	private:
		vector<Framework::Position<double> *> currPos;
		Framework::Eventing::IPublisher *agentDonePublisher;
		map<unsigned, vector<Framework::Position<double> *> *> neighborsCurrPos;//leftNeighborCurrPos;
		Framework::Position<double> finalPos;
		double delta;
		double error;
		//list<Framework::Position *> rightNeighborCurrPos;

		Framework::Timers::ITimer *timer;

		double linkWeight;

		double kp;
		Framework::Logging::ILog *log;
		bool isReady;

		unsigned index;
		sem_t binSem;
		int loopCount;

		unsigned totalNodes;
	public:
		LinkedAgent();
		LinkedAgent(unsigned packageNode, unsigned packageDest, unsigned totalNumNodes, Framework::Position<double> initialPos, Framework::Position<double> finalPos, double kp, double weight, double step, double absalon);

		virtual ~LinkedAgent();

		virtual void Run();

	protected:
		//virtual void Init();

		//virtual void Send(unsigned destRank, unsigned messageTag);
		//virtual void Get(unsigned sourceRank, unsigned messageTag);

		//virtual void Serialize();

		//virtual void Deserialize();
	private:
		void Init(Framework::Position<double> initalPos, Framework::Position<double> finalPos);

		static void PositionPackageCallback(void *ownerObject, Framework::Packages::IPackage *package);

		void CalculateRoundTripMessageLatency();

		void SendCurrentPositionToNeighbors();

		void GetNeighborsPositions();

		void CalculateNewPosition();

		bool IsTaskCompleted();

		void PersistInformation();
	};
}

#endif	//LinkedAgent_h