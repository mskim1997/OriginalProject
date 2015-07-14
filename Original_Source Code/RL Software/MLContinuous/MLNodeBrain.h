/////////////////////////////////////////////////////////
//MLNodeBrain.h
//Defines the MLNodeBrain class.  This class is
//responsible for managing the node's brain.
//
//Manuel Madera
//10/27/09
/////////////////////////////////////////////////////////

#ifndef MLNodeBrain_h
#define MLNodeBrain_h

#include "MLTestProject.h"
#include "INodeBrain.h"
#include "ml_genlib.h"
#include "Framework\Position.h"
#include <semaphore.h>

namespace MLContinuous
{
	class ML_API MLNodeBrain : public INodeBrain
	{
	private:
		sem_t binSem;
		unsigned ySize;
		unsigned xSize;
		state_t **agentBrain;

	public:
		MLNodeBrain(unsigned y, unsigned x);

		virtual ~MLNodeBrain();

		virtual Framework::Position<int> GetLimits();

		virtual state_t **GetBrain();

		virtual void SetLocationState(state_t newState);

	private:
		void CopyState(state_t source, state_t &dest);
	};
}

#endif	//MLNodeBrain_h