/////////////////////////////////////////////////////////
//INodeBrain.h
//Defines the INodeBrain interface.
//
//Manuel Madera
//10/27/09
/////////////////////////////////////////////////////////

#ifndef INodeBrain_h
#define INodeBrain_h

#include "MLTestProject.h"
#include "ml_genlib.h"
#include "Framework\Position.h"

namespace MLContinuous
{
	class ML_API INodeBrain
	{
	public:
		virtual ~INodeBrain() {}

		virtual Framework::Position<int> GetLimits() = 0;

		virtual state_t **GetBrain() = 0;

		virtual void SetLocationState(state_t newState) = 0;
	};
}

#endif	//INodeBrain_h