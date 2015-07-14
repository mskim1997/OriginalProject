/////////////////////////////////////////////////////////
//IMaze.h
//Defines the IMaze interface.
//
//Manuel Madera
//10/10/09
/////////////////////////////////////////////////////////

#ifndef IMaze_h
#define IMaze_h

#include "MLTestProject.h"
#include "Framework/Position.h"
#include "Framework/MLAgentEvents.h"

namespace MLTest
{
	class ML_API IMaze
	{
	public:
		virtual ~IMaze() {}
		virtual char GetObjectOn(Framework::Position<int> pos) = 0;

		virtual Framework::Position<int> GetPosition(char object) = 0;

		//Assumes that if the return is true, the agent will move to the specified position
		virtual bool CanMove(unsigned agentID, Framework::Position<int> pos) = 0;
		virtual bool CanMoveToGlobal(unsigned agentID, Framework::Position<int> movingFomPos, Framework::Position<int> movingToPos, unsigned &dest, char &newStartLoc) = 0;

		virtual unsigned GetXSize() = 0;
		virtual unsigned GetYSize() = 0;

		virtual void NotifyPhysicalPathDone(Framework::Eventing::PhysicalPathEventInfo agentInfo) = 0;

		virtual Framework::Position<int> GetLimits() = 0;

		virtual bool IsPositionWithinLocalLimits(Framework::Position<int> pos) = 0;

		virtual bool IsPositionWithinGlobalLimits(Framework::Position<int> pos) = 0;

		virtual void NotifyAgentIsDone(unsigned agentId, bool isTransferedAgent = false, bool hasFoundGoal = true) = 0;
	};
}

#endif	//IMaze_h