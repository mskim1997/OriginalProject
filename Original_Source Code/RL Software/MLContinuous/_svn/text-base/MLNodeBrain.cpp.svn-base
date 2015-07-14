/////////////////////////////////////////////////////////
//MLNodeBrain.cpp
//Defines the MLNodeBrain class.  This class is
//responsible for managing the node's brain.
//
//Manuel Madera
//10/27/09
/////////////////////////////////////////////////////////

#include "MLNodeBrain.h"
#include <stdlib.h>
#include "Framework\Debug.h"

#ifdef PrintMemoryLeak
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#define calloc DEBUG_CALLOC
#define free DEBUG_FREE
#endif
#endif
#endif

using namespace Framework;

namespace MLContinuous
{
	MLNodeBrain::MLNodeBrain(unsigned y, unsigned x) : ySize(y), xSize(x), agentBrain(0)
	{
		sem_init(&binSem, 0, 1);

		unsigned i = 0;
		unsigned j = 0;

		agentBrain = new state_t *[ySize];//(state_t **) calloc (ySize, sizeof(state_t *));
		for (i = 0; i < ySize; i++) {
			agentBrain[i] = new state_t [xSize];//(state_t *) calloc (xSize, sizeof(state_t));
		}

		/**************************
		* Initialize Agent Brain.
		**************************/

		// - Initialize the agent brain matrix to all Blank ('?').
		//   This means the agent starts with an empty brain, having no
		//   knowledge about the environment it is in.
		// - Initialize the reward values to all 0.
		for (i = 0; i < ySize; i++) {
			for (j = 0; j < xSize; j++) {
				agentBrain[i][j].locSt = BLANK;
				// Initialize rewards.
				agentBrain[i][j].imRwrd.up    = 0;
				agentBrain[i][j].imRwrd.down  = 0;
				agentBrain[i][j].imRwrd.right = 0;
				agentBrain[i][j].imRwrd.left  = 0;
				// Initialize side discounted rewards to a large negative value,
				// so they don't have any weight on the action policy.
				(i == 0) ? agentBrain[i][j].dsRwrd.up          = -1 * RWRD_FACTOR : 0;
				(i == ySize-1) ? agentBrain[i][j].dsRwrd.down  = -1 * RWRD_FACTOR : 0;
				(j == xSize-1) ? agentBrain[i][j].dsRwrd.right = -1 * RWRD_FACTOR : 0;
				(j == 0) ? agentBrain[i][j].dsRwrd.left        = -1 * RWRD_FACTOR : 0;
				agentBrain[i][j].distG.up     = 0;
				agentBrain[i][j].distG.down   = 0;
				agentBrain[i][j].distG.right  = 0;
				agentBrain[i][j].distG.left   = 0;
				agentBrain[i][j].nVsted.up    = 0;
				agentBrain[i][j].nVsted.down  = 0;
				agentBrain[i][j].nVsted.right = 0;
				agentBrain[i][j].nVsted.left  = 0;
				agentBrain[i][j].total.up     = 0;
				agentBrain[i][j].total.down   = 0;
				agentBrain[i][j].total.right  = 0;
				agentBrain[i][j].total.left   = 0;
				agentBrain[i][j].isVsted      = FALSE_t;
				agentBrain[i][j].isLrnVsted   = FALSE_t;     
			}
		}
	}

	MLNodeBrain::~MLNodeBrain()
	{
		for (unsigned i = 0; i < ySize; i++) {
			delete [] this->agentBrain[i];
			this->agentBrain[i] = NULL;
			//free (this->agentBrain[i]);
		}

		delete [] this->agentBrain;
		this->agentBrain = NULL;
		//free (this->agentBrain);

		sem_destroy(&this->binSem);

		#ifdef PrintMemoryLeak
			CHECK_HEAP();
		#endif
	}

	Framework::Position<int> MLNodeBrain::GetLimits()
	{
		return Position<int>(this->xSize, this->ySize);
	}

	state_t **MLNodeBrain::GetBrain()
	{
		sem_wait(&this->binSem);
		state_t **newAgentBrain = new state_t *[ySize];//(state_t **) calloc (ySize, sizeof(state_t *));
		for (unsigned i = 0; i < ySize; i++) {
			newAgentBrain[i] = new state_t [xSize];//(state_t *) calloc (xSize, sizeof(state_t));
		}

		for (unsigned i = 0; i < ySize; i++)
		{
			for (unsigned j = 0; j < xSize; j++)
			{
				CopyState(agentBrain[i][j], newAgentBrain[i][j]);
			}
		}
		sem_post(&this->binSem);

		return newAgentBrain;
	}

	void MLNodeBrain::SetLocationState(state_t newState)
	{
		sem_wait(&this->binSem);
		for (unsigned i = 0; i < ySize; i++)
		{
			for (unsigned j = 0; j < xSize; j++)
			{
				if ((newState.y == i)&&(newState.x == j))
				{
					CopyState(newState, agentBrain[i][j]);
				}
			}
		}
		sem_post(&this->binSem);
	}

	void MLNodeBrain::CopyState(state_t source, state_t &dest)
	{
		dest.x = source.x;
		dest.y = source.y;
		dest.locSt = source.locSt;
		dest.barDetect.down = source.barDetect.down;
		dest.barDetect.left = source.barDetect.left;
		dest.barDetect.right = source.barDetect.right;
		dest.barDetect.up = source.barDetect.up;
		dest.isVsted = source.isVsted;
		dest.isLrnVsted = source.isLrnVsted;
		dest.totalNVsted = source.totalNVsted;
		dest.totalNLrnVsted = source.totalNLrnVsted;
		dest.fromX = source.fromX;
		dest.fromY = source.fromY;
		dest.toX = source.toX;
		dest.toY = source.toY;
		dest.actTaken.down = source.actTaken.down;
		dest.actTaken.left = source.actTaken.left;
		dest.actTaken.right = source.actTaken.right;
		dest.actTaken.up = source.actTaken.up;
			dest.imRwrd.down = source.imRwrd.down;
			dest.imRwrd.left = source.imRwrd.left;
			dest.imRwrd.right = source.imRwrd.right;
			dest.imRwrd.up = source.imRwrd.up;
		dest.dsRwrd.down = source.dsRwrd.down;
		dest.dsRwrd.left = source.dsRwrd.left;
		dest.dsRwrd.right = source.dsRwrd.right;
		dest.dsRwrd.up = source.dsRwrd.up;
		dest.distG.down = source.distG.down;
		dest.distG.left = source.distG.left;
		dest.distG.right = source.distG.right;
		dest.distG.up = source.distG.up;
		dest.nVsted.down = source.nVsted.down;
		dest.nVsted.left = source.nVsted.left;
		dest.nVsted.right = source.nVsted.right;
		dest.nVsted.up = source.nVsted.up;
		dest.nLrnVsted.down = source.nLrnVsted.down;
		dest.nLrnVsted.left = source.nLrnVsted.left;
		dest.nLrnVsted.right = source.nLrnVsted.right;
		dest.nLrnVsted.up = source.nLrnVsted.up;
		dest.total.down = source.total.down;
		dest.total.left = source.total.left;
		dest.total.right = source.total.right;
		dest.total.up = source.total.up;
	}
}