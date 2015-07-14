/////////////////////////////////////////////////////////
//LinkedAgent.cpp
//Defines the LinkedAgent class.  This agent runs the 
//Interconnected dynamical system simulation.
//
//Manuel Madera
//7/22/09
/////////////////////////////////////////////////////////

#include "LinkedAgent.h"
#include "Framework\EventDistributor.h"
#include "Framework/MLAgentEvents.h"
#include "Framework/Packages.h"
#include "Framework/PackagerFactory.h"
#include "Framework/Debug.h"
#include "Framework/ITimer.h"
#include "Framework/LogManager.h"

#ifdef WIN32
#include <windows.h>
#endif

using namespace std;
using namespace Framework;
using namespace Framework::Eventing;
using namespace Framework::Agents;
using namespace Framework::Messaging;
using namespace Framework::Timers;
using namespace Framework::Factories;
using namespace Framework::Packages;
using namespace Framework::Logging;

#define MAX_LATENCY_PACKAGES	1000
#define MAX_NUM_TRIES			10

namespace WorldTest
{
	LinkedAgent::LinkedAgent() : Agent(), isReady(false)
	{
		this->kp = 0;		

		Init(Position<double>(0, 0), Position<double>(0, 0));
	}

	LinkedAgent::LinkedAgent(unsigned packageNode, unsigned packageDest, unsigned totalNumNodes, Framework::Position<double> initialPos, Framework::Position<double> finalPos, double kp, double weight, double step, double absalon) 
		: Agent(packageNode, packageDest), delta(step), error(absalon), isReady(false), totalNodes(totalNumNodes)
	{
		//timer = new DummyTimer();
		//this->currPos = initialPos;
		this->kp = kp;
		this->linkWeight = weight;

		Init(initialPos, finalPos);
	}

	void LinkedAgent::Init(Position<double> initalPos, Framework::Position<double> finalPos)
	{
		sem_init(&binSem, 0, 1);	

		PackagerFactory::Instance()->RegisterPackage(PackageType::POSITION, PositionPackage::CreatePackage);
		PackagerFactory::Instance()->RegisterPackage(PackageType::AGENT_DONE_EVENT, AgentDoneEventPackage::CreatePackage);

		if (this->msgDist != NULL)
		{
			this->msgDist->RegisterCallback(PackageType::POSITION, this, PositionPackageCallback);
		}

		currPos.push_back(new Position<double>(initalPos.GetXCoor(), initalPos.GetYCoor()));

		this->finalPos.SetXCoor(finalPos.GetXCoor());
		this->finalPos.SetYCoor(finalPos.GetYCoor());

		index = 0;

		loopCount = 0;

		this->agentDonePublisher = new Publisher(EventDistributor::Instance(), SubscriptionTypes::AGENT_DONE);

		this->log = LogManager::Instance();
		//this->currPos.SetXCoor(0);
		//this->currPos.SetYCoor(0);		
	}

	LinkedAgent::~LinkedAgent()
	{
		if (this->msgDist != NULL)
		{
			this->msgDist->UnregisterCallback(PackageType::POSITION);
		}

		vector<Position<double> *>::iterator iter;

		for (iter = neighborsCurrPos[this->id - 1]->begin(); iter != neighborsCurrPos[this->id - 1]->end(); iter++)
		{
			delete *iter;
			*iter = NULL;
		}

		for (iter = neighborsCurrPos[this->id + 1]->begin(); iter != neighborsCurrPos[this->id + 1]->end(); iter++)
		{
			delete *iter; 
			*iter = NULL;
		}

		delete this->agentDonePublisher;
		this->agentDonePublisher = NULL;

		delete neighborsCurrPos[this->id - 1];
		neighborsCurrPos[this->id - 1] = NULL;

		delete neighborsCurrPos[this->id + 1];
		neighborsCurrPos[this->id + 1] = NULL;
		//delete timer;
	}

	void LinkedAgent::Run()
	{
		sem_wait(&this->binSem);

		//By the time this method is called, each agent should have a valid id assigned.
		neighborsCurrPos[this->id - 1] = new vector<Position<double> *>();
		neighborsCurrPos[this->id + 1] = new vector<Position<double> *>();

		for (int i = 0; i < 2; i++)
		{			
			neighborsCurrPos[this->id - 1]->push_back(new Position<double>(0, 0));
			neighborsCurrPos[this->id + 1]->push_back(new Position<double>(0, 0));
		}
		//timer = new DummyTimer();	

		this->isReady = true;
		sem_post(&this->binSem);

		if (this->currPos[index] != NULL)
		{
#ifdef PrintAgentPosition
			log->Log(&typeid(this), "Agent: %d is at: (%f, %f)", this->id, this->currPos[index]->GetXCoor(), this->currPos[index]->GetYCoor());
#endif	
		}

		//Calculate the round trip message latency
		//This is our link weight, Gi
		CalculateRoundTripMessageLatency();

		while (!IsTaskCompleted())
		{
			#ifdef DebugLinkedAgent
				log->Log(&typeid(this), "Agent: %d is sending its current position ...", this->id);
			#endif

			//Send position to neighbors
			SendCurrentPositionToNeighbors();

			//Introduce delay.
			Sleep(300);

			/*#ifdef DebugLinkedAgent
				log->Log(&typeid(this), "Agent: %d is getting neighbors positions ...", this->id);
			#endif*/

			//Get position from neighbors
			//GetNeighborsPositions();

			#ifdef DebugLinkedAgent
				log->Log(&typeid(this), "Agent: %d is calculating its current position ...", this->id);
			#endif

			//calculate my new position, time?
			CalculateNewPosition();

			//Store my new position and time?
		}

		this->agentDonePublisher->Publish(new AgentDoneEventPackage(this->id, 0, this->id));

		//Persist information
		PersistInformation();
	}

	bool LinkedAgent::IsTaskCompleted()
	{
		bool isDone = false;

		/*if (this->currPos[index] != NULL)
		{
			isDone = (this->currPos[index]->GetXCoor() >= this->finalPos.GetXCoor() - this->error)&&(this->currPos[index]->GetXCoor() <= this->finalPos.GetXCoor() + this->error);
		}

		if (isDone)
		{
			if (this->currPos[index] != NULL)
			{
				isDone = (this->currPos[index]->GetYCoor() >= this->finalPos.GetYCoor() - this->error)&&(this->currPos[index]->GetYCoor() <= this->finalPos.GetYCoor() + this->error);
			}
		}*/

		return isDone;
		//return (loopCount++ >= 20);
	}

	void LinkedAgent::CalculateRoundTripMessageLatency()
	{
		//double leftLinkTimeArray[MAX_LATENCY_PACKAGES];

		//for (int i = 0; i < MAX_LATENCY_PACKAGES; i++)
		//{
		//	IPackage *package = PackagerFactory::CreatePackage(PackageType::POSITION, this->id
		//	this->timer->Start();
		//	this->msgDist->SendPackage(
		//	this->timer->Stop();

		//	leftLinkTimeArray[i] = this->timer->GetElapsed();
		//}
	}

	void LinkedAgent::SendCurrentPositionToNeighbors()
	{
		if (this->id != 1)
		{
			if ((this->msgDist != NULL)&&(this->currPos[index] != NULL))
			{
				this->msgDist->SendPackage(new PositionPackage(this->id, this->id - 1, *(this->currPos[index])));
			}
		}

		if (this->id != this->totalNodes - 1)
		{
			if ((this->msgDist != NULL)&&(this->currPos[index] != NULL))
			{
				this->msgDist->SendPackage(new PositionPackage(this->id, this->id + 1, *(this->currPos[index])));
			}
		}
	}

	void LinkedAgent::GetNeighborsPositions()
	{
		//IPackage *package = 0;
		//bool hasLeftAgentPackage = false;
		//bool hasRightAgentPackage = false;

		//for (int i = 0; i < MAX_NUM_TRIES; i++)
		//{
		//	package = this->myMessageQueue.Dequeue();

		//	if (package != 0)
		//	{
		//		if (package->GetSource() == (this->id - 1))
		//		{
		//			this->leftNeighborCurrPos = static_cast<PositionPackage *>(package)->GetPosition();

		//			delete package; package = NULL;
		//			package = 0;

		//			hasLeftAgentPackage = true;

		//			if (hasRightAgentPackage)
		//			{
		//				break;
		//			}
		//		}
		//		else if (package->GetSource() == (this->id + 1))
		//		{
		//			this->rightNeighborCurrPos = static_cast<PositionPackage *>(package)->GetPosition();

		//			delete package; package = NULL;
		//			package = 0;

		//			hasRightAgentPackage = true;

		//			if (hasLeftAgentPackage)
		//			{
		//				break;
		//			}
		//		}
		//	}

		//	if (!hasLeftAgentPackage)
		//	{
		//		this->leftNeighborCurrPos.SetXCoor(0);
		//		this->leftNeighborCurrPos.SetYCoor(0);
		//	}

		//	if (!hasRightAgentPackage)
		//	{
		//		this->rightNeighborCurrPos.SetXCoor(0);
		//		this->rightNeighborCurrPos.SetYCoor(0);
		//	}
		//}
	}

	void LinkedAgent::CalculateNewPosition()
	{
		sem_wait(&this->binSem);
		Position<double> neighbors;
		Position<double> AXk;
		Position<double> neighbor1Pos;
		Position<double> neighbor2Pos;
		Position<double> step;

		neighbor1Pos.SetXCoor((*this->neighborsCurrPos[this->id - 1])[1]->GetXCoor());
		neighbor1Pos.SetYCoor((*this->neighborsCurrPos[this->id - 1])[1]->GetYCoor());

#ifdef PrintAgentPosition
			log->Log(&typeid(this), "Neighbor 1: %d is at: (%f, %f)", this->id - 1, neighbor1Pos.GetXCoor(), neighbor1Pos.GetYCoor());
#endif

		neighbor2Pos.SetXCoor((*this->neighborsCurrPos[this->id + 1])[1]->GetXCoor());
		neighbor2Pos.SetYCoor((*this->neighborsCurrPos[this->id + 1])[1]->GetYCoor());

#ifdef PrintAgentPosition
			log->Log(&typeid(this), "Neighbor 2: %d is at: (%f, %f)", this->id + 1, neighbor2Pos.GetXCoor(), neighbor2Pos.GetYCoor());
#endif

		neighbors = (neighbor1Pos + neighbor2Pos)*this->linkWeight;
		AXk = (*this->currPos[index] + neighbors)*-kp;

#ifdef PrintAgentPosition
			log->Log(&typeid(this), "AXk = (%f, %f)", AXk.GetXCoor(), AXk.GetYCoor());
#endif

		step = (AXk + this->finalPos)*this->delta;

#ifdef PrintAgentPosition
			log->Log(&typeid(this), "Step = (%f, %f)", step.GetXCoor(), step.GetYCoor());
#endif

		Position<double> *newPos = new Position<double>(step + *this->currPos[index]);//new Position<double>((this->finalPos - AXk)*this->delta + *this->currPos[index]);
		this->currPos.push_back(newPos);

		index++;

		if (this->currPos[index] != NULL)
		{
			cout << "Agent: " << this->id << " is at: (" << this->currPos[index]->GetXCoor() << ", " << this->currPos[index]->GetYCoor() << ")" << endl;

#ifdef PrintAgentPosition
			log->Log(&typeid(this), "Agent: %d is at: (%f, %f)", this->id, this->currPos[index]->GetXCoor(), this->currPos[index]->GetYCoor());
#endif
		}
		sem_post(&this->binSem);
	}

	void LinkedAgent::PersistInformation()
	{
	}

	/*static*/ void LinkedAgent::PositionPackageCallback(void *ownerObject, Packages::IPackage *package)
	{
		LinkedAgent *pThis = static_cast<LinkedAgent *>(ownerObject);
		PositionPackage *pos = static_cast<PositionPackage *>(package);

		if (pThis != NULL)
		{
#ifdef DebugLinkedAgent
			pThis->log->Log(&typeid(pThis), "Agent: %d is getting neighbors positions ...", pThis->id);
#endif

			sem_wait(&pThis->binSem);
			if (pThis->isReady)
			{
				if (pos != NULL)
				{
					if ((pThis->neighborsCurrPos.find(pos->GetSource()) == pThis->neighborsCurrPos.end()) ||
						(pThis->neighborsCurrPos[pos->GetSource()] == NULL))
					{
						pThis->neighborsCurrPos[pos->GetSource()] = new vector<Position<double> *>(); 

						pThis->neighborsCurrPos[pos->GetSource()]->push_back(new Position<double>(0, 0));
						pThis->neighborsCurrPos[pos->GetSource()]->push_back(new Position<double>(0, 0));
					}

					//Take the last known value and store it as the oldest value
					(*pThis->neighborsCurrPos[pos->GetSource()])[0]->SetXCoor((*pThis->neighborsCurrPos[pos->GetSource()])[1]->GetXCoor());
					(*pThis->neighborsCurrPos[pos->GetSource()])[0]->SetYCoor((*pThis->neighborsCurrPos[pos->GetSource()])[1]->GetYCoor());

					//Store the new value as the last known value
					(*pThis->neighborsCurrPos[pos->GetSource()])[1]->SetXCoor(pos->GetPosition().GetXCoor());
					(*pThis->neighborsCurrPos[pos->GetSource()])[1]->SetYCoor(pos->GetPosition().GetYCoor());
				}
			}
			sem_post(&pThis->binSem);
		}

		delete package;
		package = NULL;
	}
}