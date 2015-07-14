
/////////////////////////////////////////////////////////////////////////////////
//
// Date        : 03/21/2004
//
// Filename    : ml_genlib.c
//
// Author      : Hani Al-Dayaa
//
// Topic       : Machine Learning
//
// Description : A library with all important functions thas can
//               be used in many applications.
//
//
// Revision History :
// ----------------------------------------------------------------------------
// <Rev> | <Author>  | <Date>   | <Changes Made>
//       |           |          |
// 1.00  |  H.A.     | 03/21/04 | - Initial Revision.
//       |           |          |
// 2.00  |  M.M.     | 08/07/09 | - Modified to meet new ANSI standards
// ----------------------------------------------------------------------------
//
/////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// HEADER FILES ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "ml_genlib.h"
#include "MLAgent.h"

#include "Framework\MessageDistributor.h"
#include "Framework\Position.h"
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

using namespace Framework::Messaging;
using namespace Framework::Packages;
using namespace Framework;

/////////////////////////////////////////////////////////////////////////////////
////////////////////////////// GLOBAL VARIABLES /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

// Action Names table.
string_t actNames[] = 
{
	"NONE",
	"UP",
	"DOWN",
	"RIGHT",
	"LEFT"
};

// Action Types table.
string_t actTypes[] = 
{
	"none",
	"randomly",
	"any-randomly",
	"based on policy",
	"controlled",
	"based on distance",
	"based on distance&visited"
};

/////////////////////////////////////////////////////////////////////////////////
////////////////////////////// TEXT MODE FUNCTIONS //////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

namespace MLTest
{
	/////////////////////////////////////////////////////////////////////////////////
	// FUNCTION
	// Name: getRandomDirection
	// Description: This function returns a random value that can be 1, 2, 3, or 4.
	//              Each value represents a direction.     
	/////////////////////////////////////////////////////////////////////////////////
	action_t
		MLAgent::getRandomDirection (state_t **agentBrain, state_t *s0, actCheck_t ac, int xSize, int ySize)
	{
		action_t a0;

		switch (ac) {
			// Check for limit.
  case CHECK_LIMIT:
	  do {
		  // Return a random value that can be 1, 2, 3, or 4.
		  a0.val = static_cast<dirVal_t>((rand() % 4) + 1);
	  } while (((a0.val == UP) && (s0->y == 0)) ||
		  ((a0.val == DOWN) && (s0->y == ySize-1)) ||
		  ((a0.val == RIGHT) && (s0->x == xSize-1)) ||
		  ((a0.val == LEFT) && (s0->x == 0)));
	  break;  
	  // Check for barrier.
  case CHECK_BARRIER:
	  do {
		  // Return a random value that can be 1, 2, 3, or 4.
		  a0.val = static_cast<dirVal_t>((rand() % 4) + 1);
	  } while (((a0.val == UP) && (agentBrain[s0->y-1][s0->x].locSt == BARRIER)) ||
		  ((a0.val == DOWN) && (agentBrain[s0->y+1][s0->x].locSt == BARRIER)) ||
		  ((a0.val == RIGHT) && (agentBrain[s0->y][s0->x+1].locSt == BARRIER)) ||
		  ((a0.val == LEFT) && (agentBrain[s0->y][s0->x-1].locSt == BARRIER)));
	  break;
	  // Check for limit and barrier.
  case CHECK_BOTH:
	  do {
		  // Return a random value that can be 1, 2, 3, or 4.
		  a0.val = static_cast<dirVal_t>((rand() % 4) + 1);
	  } while (((a0.val == UP) && ((s0->y == 0) || agentBrain[s0->y-1][s0->x].locSt == BARRIER)) ||
		  ((a0.val == DOWN) && ((s0->y == ySize-1) || agentBrain[s0->y+1][s0->x].locSt == BARRIER)) ||
		  ((a0.val == RIGHT) && ((s0->x == xSize-1) || agentBrain[s0->y][s0->x+1].locSt == BARRIER)) ||
		  ((a0.val == LEFT) && ((s0->x == 0) || agentBrain[s0->y][s0->x-1].locSt == BARRIER)));
	  break;
	  // No check.
  case NO_CHECK:
	  // Return a random value that can be 1, 2, 3, or 4.
	  a0.val = static_cast<dirVal_t>((rand() % 4) + 1);
	  break;
  default:
	  if (!Validator::IsNull(log, NAME("log")))
	  {
		log->Log(&typeid(this), "\nError: getRandomDirection: Invalid Action-Check value.\n");
	  }
	  break;
		}  

		a0.type = RAND;
		strcpy (a0.name, actNames[a0.val]);

		return (a0);
	}


	/////////////////////////////////////////////////////////////////////////////////
	// FUNCTION
	// Name: getDirectionValue
	// Description: 
	/////////////////////////////////////////////////////////////////////////////////
	dirVal_t
		MLAgent::getDirectionValue (char firstLetter)
	{
		dirVal_t dir;

		if (firstLetter == 'u') {
			dir = UP;
		}
		else if (firstLetter == 'd') {
			dir = DOWN;
		} 
		else if (firstLetter == 'r') {
			dir = RIGHT;
		}
		else if (firstLetter == 'l') {
			dir = LEFT;
		}
		else {
			if (!Validator::IsNull(log, NAME("log")))
			{
				log->Log(&typeid(this), "Error: getDirectionValue: Invalid direction action name.");
			}
			dir = NO_DIR;
		}

		return (dir);
	}


	/////////////////////////////////////////////////////////////////////////////////
	// FUNCTION
	// Name: getDistAB
	// Description: 
	/////////////////////////////////////////////////////////////////////////////////
	float MLAgent::getDistAB (int xa, int ya, int xb, int yb)
	{
		return ( sqrt (static_cast<double>(SQUARE(xb-xa) + SQUARE(yb-ya))) );
	} 


	/////////////////////////////////////////////////////////////////////////////////
	// FUNCTION
	// Name: readMazeIntoMatrix
	// Description: 
	/////////////////////////////////////////////////////////////////////////////////
	void 
		MLAgent::readMazeIntoMatrix (char **charMaze, int xSize, int ySize, state_t **maze, char start, char goal)
	{
		FILE *mf;        // Maze file descriptor.
		int i = 0;       // Vertical index.
		int j = 0;       // Horizontal index.
		char locState;   // Location value.

		// Fill maze matrix.
		while ((i < ySize) && (j < xSize)) {
			locState = charMaze[i][j];
			// Ignore newlines, carriage returns, tabs, and spaces.
			if ((locState == '\n') || (locState == '\r') || (locState == ' ') || (locState == '\t')) {
				;   
			}
			// Store the location values of the maze in the maze matrix.
			else if ((locState == start) || (locState == goal) || 
				(locState == BARRIER) || (locState == OPEN)) {
					maze[i][j].locSt   = static_cast<locState_t>(locState);
					maze[i][j].isVsted = FALSE_t;
					// Go to next column.
					j++;
					// Go to next row.
					if (j == xSize) {
						j = 0;
						i++;
					}
			}
			//Store other valid characters as OPEN
			else if ((locState >= 'A')&&(locState <= 'Z'))
			{
				maze[i][j].locSt   = OPEN;
				maze[i][j].isVsted = FALSE_t;
				// Go to next column.
				j++;
				// Go to next row.
				if (j == xSize) {
					j = 0;
					i++;
				}
			}
			// Report an error if at least one invalid character exists 
			// in the maze.
			else {
				if (!Validator::IsNull(log, NAME("log")))
				{
					log->Log(&typeid(this), "Error: readMazeIntoMatrix: Invalid maze information.");
				}

				exit (EXIT_FAILURE);
			}
		}

		// Debug Information.
#ifdef DEBUG
		if (!Validator::IsNull(log, NAME("log")))
		{
			log->Log(&typeid(this), "Debug: readMazeIntoMatrix: Input Maze Matrix:");
			for (i = 0; i < ySize; i++) {
				for (j = 0; j < xSize; j++) {
					log->Log(&typeid(this), "%c ", maze[i][j].locSt);
				}
				log->Log(&typeid(this), "\n");
			}
		}
#endif
	}



	/////////////////////////////////////////////////////////////////////////////////
	// FUNCTION
	// Name: findLocation
	// Description: This function finds a location of state "locState" and stores its
	//              x and y values in "location".
	/////////////////////////////////////////////////////////////////////////////////
	bool_t 
		MLAgent::findLocation (int xSize, int ySize, state_t **maze, locState_t locState, 
		state_t *location)
	{
		int i;        // Vertical index.
		int j;        // Horizontal index.

		// Scan the maze matrix.
		for (i = 0; i < ySize; i++) {
			for (j = 0; j < xSize; j++) {
				if (maze[i][j].locSt == locState) {
					location->x     = j;         // j and x are for horizontal.
					location->y     = i;         // i and y are for vertical.
					location->locSt = locState;
					return (TRUE_t);            // Return TRUE when location is found.
				}
			}
		}  

		// Function shouldn't reach here unless the input maze is invalid.
		if (!Validator::IsNull(log, NAME("log")))
		{
			log->Log(&typeid(this), "Error: findLocation: Location with state equal to '%c' not found.", locState);
		}
		return (FALSE_t);
	}



	/////////////////////////////////////////////////////////////////////////////////
	// FUNCTION
	// Name: moveAgent
	// Description: This function moves the agent to location if location is no 
	//              barrier and not outside maze, records location moved to in the 
	//              agent matrix (agentBrain), then records position (direction 
	//              moved, new x, and new y) in the agent path array (agentIters). If 
	//              location to move to is a barrier, do not move agent, and record 
	//              barrier location in the agent matrix (agentBrain).
	/////////////////////////////////////////////////////////////////////////////////
	error_t
		MLAgent::moveAgent (unsigned id, agent_t *agent, action_t a0, int xSize, int ySize, 
		state_t **agentBrain, agent_t *agentPath, 
		learnMethod_t lrnMethod, state_t *startLoc, FILE *agentF)
	{

		int dummy = 0;
		point_t pt;
		bool stay = false;

		// For Debug.
#ifdef DEBUG
		if (!Validator::IsNull(log, NAME("log")))
		{
			if (!Validator::IsNull(agent, NAME("agent")))
			{
				log->Log(&typeid(this), "Debug: moveAgent: Agent Current Iteration: %d", agent->iter);
				log->Log(&typeid(this), "\nDebug: moveAgent: Agent Current Physical Step Number: %d", agent->step);
				log->Log(&typeid(this), "\nDebug: moveAgent: Agent Current Location: [%d,%d]", agent->loc.x, agent->loc.y);
			}
		}
#endif

		Framework::Position<int> limit = this->maze->GetLimits();

		// Stop when agent reaches its maximum number of steps.
		if (!Validator::IsNull(agent, NAME("agent")))
		{
			if (agent->step == MAX_AGENT_STEPS(this->maze->GetXSize(), this->maze->GetYSize()) - 1) {//xSize, ySize) - 1) {
				if (!Validator::IsNull(log, NAME("log")))
				{
					log->Log(&typeid(this), "Warning: moveAgent: Agent reached maximum number of steps before reaching Goal (step = %d).",
						agent->step);
				}

				fprintf (agentF, "\nWarning: moveAgent: Agent reached maximum number of steps before reaching Goal (step = %d).\n",
					agent->step);
				return (REACHMAX_ERR);
				//exit(EXIT_FAILURE);
			}
		}

		switch(this->lastDir)
		{
		case UP:
			if (a0.val == DOWN)
			{
				stay = true;
			}
			break;
		case DOWN:
			if (a0.val == UP)
			{
				stay = true;
			}
			break;
		case RIGHT:
			if (a0.val == LEFT)
			{
				stay = true;
			}
			break;
		case LEFT:
			if (a0.val == RIGHT)
			{
				stay = true;
			}
			break;
		}

		if (!Validator::IsNull(agent, NAME("agent")))
		{
			if (!stay)
			{
				switch (a0.val) 
				{
					// Direction is UP.
				case UP:
					// Check for maze limit first.
					if (this->maze->IsPositionWithinLocalLimits(Framework::Position<int>(agent->loc.x, agent->loc.y - 1)))
					{
						agent->iter = agent->iter + 1;          // This is a new iteration.
						// Check for barrier and move agent up if there is no barrier there.
						if (this->maze->CanMove(this->id, Position<int>(agent->loc.x, agent->loc.y-1)))
						{
							// Add to list.
							if (lrnMethod == PHS_DISTG_NVSTED_LP)
							{
								agentBrain[agent->loc.y][agent->loc.x].toX = agent->loc.x;
								agentBrain[agent->loc.y][agent->loc.x].toY = agent->loc.y - 1;   
								pt = getClosestToStart (agentBrain, agent->loc, xSize, ySize, startLoc);         
								if ((pt.x != -1) && (pt.y != -1))
								{
									agentBrain[agent->loc.y][agent->loc.x].fromX = pt.x;
									agentBrain[agent->loc.y][agent->loc.x].fromY = pt.y;
									agentBrain[pt.y][pt.x].toX = agent->loc.x;
									agentBrain[pt.y][pt.x].toY = agent->loc.y;    
								}
							}  
							else
							{
								agentBrain[agent->loc.y][agent->loc.x].toX = agent->loc.x;
								agentBrain[agent->loc.y][agent->loc.x].toY = agent->loc.y - 1;    
							}
							// Check for circle.        
							if ((lrnMethod == PHS_DISTG_NVSTED) ||
								(lrnMethod == PHS_DISTG_NVSTED_CLO) || (lrnMethod == PHS_DISTG_NVSTED_LP))
							{
								if (agentBrain[agent->loc.y][agent->loc.x].actTaken.up == TRUE_t)
								{
									if (!Validator::IsNull(log, NAME("log")))
									{
										log->Log(&typeid(this), "Information: moveAgent: Goal is Unreachable!");
									}

									fprintf (agentF, "\nInformation: moveAgent: Goal is Unreachable!\n");
									//exit(EXIT_SUCCESS);
									return (UNREACH_ERR);
								}        
							}
							// Make step.
							agentBrain[agent->loc.y][agent->loc.x].actTaken.up = TRUE_t;
							agent->moved = TRUE_t;       
							agent->step  = agent->step + 1;
							agentBrain[agent->loc.y][agent->loc.x].nVsted.up++;             
							agent->loc.y = agent->loc.y - 1;      // Move the agent up.
							agentBrain[agent->loc.y][agent->loc.x].isVsted = TRUE_t;
							agentBrain[agent->loc.y][agent->loc.x].totalNVsted++;
							agentPath[agent->step].dir = a0;
							agentPath[agent->step].loc.x    = agent->loc.x;
							agentPath[agent->step].loc.y    = agent->loc.y;
							agentBrain[agent->loc.y][agent->loc.x].locSt = static_cast<locState_t>(this->maze->GetObjectOn(Position<int>(agent->loc.x, agent->loc.y)));

							this->lastDir = UP;
						}
						// Record barrier.
						else if (static_cast<locState_t>(this->maze->GetObjectOn(Position<int>(agent->loc.x, agent->loc.y-1))) == BARRIER)
						{
							Position<int> pos(agent->loc.x, agent->loc.y-1);
							MessageDistributor::Instance()->Broadcast(id, new MazePositionPackage(id, 0, static_cast<char>(BARRIER), pos));
							agentBrain[agent->loc.y-1][agent->loc.x].locSt = static_cast<locState_t>(this->maze->GetObjectOn(Position<int>(agent->loc.x, agent->loc.y-1)));    
							agentBrain[agent->loc.y-1][agent->loc.x].isVsted = NEVER;        
							agent->moved = FALSE_t; 
							agentPath[agent->step].loc.barDetect.up = TRUE_t;
							// Shadow barrier.      
							agentBrain[agent->loc.y][agent->loc.x].dsRwrd.up = -1 * RWRD_FACTOR;      
							(agent->loc.y >= 2) ? 
								(agentBrain[agent->loc.y-2][agent->loc.x].dsRwrd.down = -1 * RWRD_FACTOR) : (dummy = 1);
							((agent->loc.y >= 1) && (agent->loc.x >= 1)) ?
								(agentBrain[agent->loc.y-1][agent->loc.x-1].dsRwrd.right = -1 * RWRD_FACTOR) : (dummy = 1);
							((agent->loc.y >= 1) && (agent->loc.x < xSize-1)) ?
								(agentBrain[agent->loc.y-1][agent->loc.x+1].dsRwrd.left = -1 * RWRD_FACTOR) : (dummy = 1);	
						}
					}
					else if (this->maze->IsPositionWithinGlobalLimits(Framework::Position<int>(agent->loc.x, agent->loc.y - 1)))
					{
						unsigned node;
						char newStart;

						if (!this->exploreLocalFirst)
						{
							if (this->maze->CanMoveToGlobal(this->id, Framework::Position<int>(agent->loc.x, agent->loc.y), Framework::Position<int>(agent->loc.x, agent->loc.y - 1), node, newStart))
							{
								this->lastDir = UP;

								// Add to list.
								if (lrnMethod == PHS_DISTG_NVSTED_LP)
								{
									agentBrain[agent->loc.y][agent->loc.x].toX = agent->loc.x;
									agentBrain[agent->loc.y][agent->loc.x].toY = agent->loc.y - 1;   
									pt = getClosestToStart (agentBrain, agent->loc, xSize, ySize, startLoc);         
									if ((pt.x != -1) && (pt.y != -1))
									{
										agentBrain[agent->loc.y][agent->loc.x].fromX = pt.x;
										agentBrain[agent->loc.y][agent->loc.x].fromY = pt.y;
										agentBrain[pt.y][pt.x].toX = agent->loc.x;
										agentBrain[pt.y][pt.x].toY = agent->loc.y;    
									}
								}  
								else
								{
									agentBrain[agent->loc.y][agent->loc.x].toX = agent->loc.x;
									agentBrain[agent->loc.y][agent->loc.x].toY = agent->loc.y - 1;    
								}
								// Check for circle.        
								if ((lrnMethod == PHS_DISTG_NVSTED) ||
									(lrnMethod == PHS_DISTG_NVSTED_CLO) || (lrnMethod == PHS_DISTG_NVSTED_LP))
								{
									if (agentBrain[agent->loc.y][agent->loc.x].actTaken.up == TRUE_t)
									{
										if (!Validator::IsNull(log, NAME("log")))
										{
											log->Log(&typeid(this), "Information: moveAgent: Goal is Unreachable!");
										}

										fprintf (agentF, "\nInformation: moveAgent: Goal is Unreachable!\n");
										//exit(EXIT_SUCCESS);
										return (UNREACH_ERR);
									}        
								}

								MoveTo(node, newStart);

								if (this->isGoalOnAnotherNode)
								{
									agentBrain[agent->loc.y][agent->loc.x].actTaken.up = TRUE_t;
									agent->moved = TRUE_t;       
									agent->step  = agent->step + 1;
									agentBrain[agent->loc.y][agent->loc.x].nVsted.up++;             
									//agent->loc.y = agent->loc.y - 1;      // Move the agent up.
									agentPath[agent->step].dir = a0;
									//agentPath[agent->step].loc.x    = agent->loc.x;
									//agentPath[agent->step].loc.y    = agent->loc.y;

									return (MOVED);
								}
								else
								{
									agentBrain[agent->loc.y][agent->loc.x].isVsted = NEVER;
									agent->moved = FALSE_t; 
									agentPath[agent->step].loc.barDetect.up = TRUE_t;
									// Shadow barrier.      
									agentBrain[agent->loc.y][agent->loc.x].dsRwrd.up = -1 * RWRD_FACTOR;  
								}
							}
							else if (static_cast<locState_t>(this->maze->GetObjectOn(Position<int>(agent->loc.x, agent->loc.y - 1))) == BARRIER)
							{
								agentBrain[agent->loc.y][agent->loc.x].isVsted = NEVER;
								agent->moved = FALSE_t; 
								agentPath[agent->step].loc.barDetect.up = TRUE_t;
								// Shadow barrier.      
								agentBrain[agent->loc.y][agent->loc.x].dsRwrd.up = -1 * RWRD_FACTOR;      
								//(agent->loc.y >= 2) ? 
								// (agentBrain[agent->loc.y-2][agent->loc.x].dsRwrd.down = -1 * RWRD_FACTOR) : (dummy = 1);

								//((agent->loc.y >= 1) && (agent->loc.x >= 1)) ?
								// (agentBrain[agent->loc.y-1][agent->loc.x-1].dsRwrd.right = -1 * RWRD_FACTOR) : (dummy = 1);

								//((agent->loc.y >= 1) && (agent->loc.x < xSize-1)) ?
								// (agentBrain[agent->loc.y-1][agent->loc.x+1].dsRwrd.left = -1 * RWRD_FACTOR) : (dummy = 1);
							}
						}
					}
					break;

					// Direction is DOWN.
				case DOWN:
					// Check for maze limit first.
					if (this->maze->IsPositionWithinLocalLimits(Framework::Position<int>(agent->loc.x, agent->loc.y + 1)))
					{
						agent->iter = agent->iter + 1;         // This is a new iteration.
						// Check for barrier and move agent down if there is no barrier there.
						if (this->maze->CanMove(this->id, Position<int>(agent->loc.x, agent->loc.y+1)))
						{
							// Add to list.
							if (lrnMethod == PHS_DISTG_NVSTED_LP)
							{    
								agentBrain[agent->loc.y][agent->loc.x].toX = agent->loc.x;
								agentBrain[agent->loc.y][agent->loc.x].toY = agent->loc.y + 1;               
								pt = getClosestToStart (agentBrain, agent->loc, xSize, ySize, startLoc);
								if ((pt.x != -1) && (pt.y != -1)) {
									agentBrain[agent->loc.y][agent->loc.x].fromX = pt.x;
									agentBrain[agent->loc.y][agent->loc.x].fromY = pt.y;
									agentBrain[pt.y][pt.x].toX = agent->loc.x;
									agentBrain[pt.y][pt.x].toY = agent->loc.y;
								}
							}   
							else
							{
								agentBrain[agent->loc.y][agent->loc.x].toX = agent->loc.x;
								agentBrain[agent->loc.y][agent->loc.x].toY = agent->loc.y + 1;         
							}
							// Check for circle.        
							if ((lrnMethod == PHS_DISTG_NVSTED) ||
								(lrnMethod == PHS_DISTG_NVSTED_CLO) || (lrnMethod == PHS_DISTG_NVSTED_LP))
							{
								if (agentBrain[agent->loc.y][agent->loc.x].actTaken.down == TRUE_t)
								{
									if (!Validator::IsNull(log, NAME("log")))
									{
										log->Log(&typeid(this), "Information: moveAgent: Goal is Unreachable!");
									}

									fprintf (agentF, "\nInformation: moveAgent: Goal is Unreachable!\n");
									//exit(EXIT_SUCCESS);
									return (UNREACH_ERR);
								}
							}    
							// Make step.
							agentBrain[agent->loc.y][agent->loc.x].actTaken.down = TRUE_t;
							agent->moved = TRUE_t; 
							agent->step  = agent->step + 1;
							agentBrain[agent->loc.y][agent->loc.x].nVsted.down++;
							agent->loc.y = agent->loc.y + 1;     // Move the agent down.
							agentBrain[agent->loc.y][agent->loc.x].isVsted = TRUE_t;      
							agentBrain[agent->loc.y][agent->loc.x].totalNVsted++;
							agentPath[agent->step].dir = a0;
							agentPath[agent->step].loc.x    = agent->loc.x;
							agentPath[agent->step].loc.y    = agent->loc.y;	
							agentBrain[agent->loc.y][agent->loc.x].locSt = static_cast<locState_t>(this->maze->GetObjectOn(Position<int>(agent->loc.x, agent->loc.y)));  

							this->lastDir = DOWN;
						}
						// Record barrier.
						else if (static_cast<locState_t>(this->maze->GetObjectOn(Position<int>(agent->loc.x, agent->loc.y+1))) == BARRIER)
						{
							Position<int> pos(agent->loc.x, agent->loc.y+1);
							MessageDistributor::Instance()->Broadcast(id, new MazePositionPackage(id, 0, static_cast<char>(BARRIER), pos));
							agentBrain[agent->loc.y+1][agent->loc.x].locSt = static_cast<locState_t>(this->maze->GetObjectOn(Position<int>(agent->loc.x, agent->loc.y+1)));       
							agentBrain[agent->loc.y+1][agent->loc.x].isVsted = NEVER;
							agent->moved = FALSE_t;
							agentPath[agent->step].loc.barDetect.down = TRUE_t;
							// Shadow barrier.
							agentBrain[agent->loc.y][agent->loc.x].dsRwrd.down = -1 * RWRD_FACTOR;       
							(agent->loc.y < ySize-2) ? 
								(agentBrain[agent->loc.y+2][agent->loc.x].dsRwrd.up = -1 * RWRD_FACTOR) : (dummy = 1);
							((agent->loc.y < ySize-1) && (agent->loc.x >= 1)) ?
								(agentBrain[agent->loc.y+1][agent->loc.x-1].dsRwrd.right = -1 * RWRD_FACTOR) : (dummy = 1);
							((agent->loc.y < ySize-1) && (agent->loc.x < xSize-1)) ?
								(agentBrain[agent->loc.y+1][agent->loc.x+1].dsRwrd.left = -1 * RWRD_FACTOR) : (dummy = 1);
						}
					}
					else if (this->maze->IsPositionWithinGlobalLimits(Framework::Position<int>(agent->loc.x, agent->loc.y + 1)))
					{
						unsigned node;
						char newStart;

						if (!this->exploreLocalFirst)
						{
							if (this->maze->CanMoveToGlobal(this->id, Framework::Position<int>(agent->loc.x, agent->loc.y), Framework::Position<int>(agent->loc.x, agent->loc.y + 1), node, newStart))
							{
								this->lastDir = DOWN;

								// Add to list.
								if (lrnMethod == PHS_DISTG_NVSTED_LP)
								{    
									agentBrain[agent->loc.y][agent->loc.x].toX = agent->loc.x;
									agentBrain[agent->loc.y][agent->loc.x].toY = agent->loc.y + 1;               
									pt = getClosestToStart (agentBrain, agent->loc, xSize, ySize, startLoc);
									if ((pt.x != -1) && (pt.y != -1)) {
										agentBrain[agent->loc.y][agent->loc.x].fromX = pt.x;
										agentBrain[agent->loc.y][agent->loc.x].fromY = pt.y;
										agentBrain[pt.y][pt.x].toX = agent->loc.x;
										agentBrain[pt.y][pt.x].toY = agent->loc.y;
									}
								}   
								else
								{
									agentBrain[agent->loc.y][agent->loc.x].toX = agent->loc.x;
									agentBrain[agent->loc.y][agent->loc.x].toY = agent->loc.y + 1;         
								}
								// Check for circle.        
								if ((lrnMethod == PHS_DISTG_NVSTED) ||
									(lrnMethod == PHS_DISTG_NVSTED_CLO) || (lrnMethod == PHS_DISTG_NVSTED_LP))
								{
									if (agentBrain[agent->loc.y][agent->loc.x].actTaken.down == TRUE_t)
									{
										if (!Validator::IsNull(log, NAME("log")))
										{
											log->Log(&typeid(this), "Information: moveAgent: Goal is Unreachable!");
										}

										fprintf (agentF, "\nInformation: moveAgent: Goal is Unreachable!\n");
										//exit(EXIT_SUCCESS);
										return (UNREACH_ERR);
									}
								} 

								MoveTo(node, newStart);

								if (this->isGoalOnAnotherNode)
								{								
									agentBrain[agent->loc.y][agent->loc.x].actTaken.down = TRUE_t;
									agent->moved = TRUE_t; 
									agent->step  = agent->step + 1;
									agentBrain[agent->loc.y][agent->loc.x].nVsted.down++;
									//agent->loc.y = agent->loc.y + 1;     // Move the agent down.
									agentPath[agent->step].dir = a0;
									//agentPath[agent->step].loc.x    = agent->loc.x;
									//agentPath[agent->step].loc.y    = agent->loc.y;								
									return (MOVED);
								}
								else
								{
									agentBrain[agent->loc.y][agent->loc.x].isVsted = NEVER;
									agent->moved = FALSE_t;
									agentPath[agent->step].loc.barDetect.down = TRUE_t;
									// Shadow barrier.
									agentBrain[agent->loc.y][agent->loc.x].dsRwrd.down = -1 * RWRD_FACTOR; 
								}
							}
							else if (static_cast<locState_t>(this->maze->GetObjectOn(Position<int>(agent->loc.x, agent->loc.y + 1))) == BARRIER)
							{
								agentBrain[agent->loc.y][agent->loc.x].isVsted = NEVER;
								agent->moved = FALSE_t;
								agentPath[agent->step].loc.barDetect.down = TRUE_t;
								// Shadow barrier.
								agentBrain[agent->loc.y][agent->loc.x].dsRwrd.down = -1 * RWRD_FACTOR;       
								//(agent->loc.y < ySize-2) ? 
								// (agentBrain[agent->loc.y+2][agent->loc.x].dsRwrd.up = -1 * RWRD_FACTOR) : (dummy = 1);

								//((agent->loc.y < ySize-1) && (agent->loc.x >= 1)) ?
								// (agentBrain[agent->loc.y+1][agent->loc.x-1].dsRwrd.right = -1 * RWRD_FACTOR) : (dummy = 1);

								//((agent->loc.y < ySize-1) && (agent->loc.x < xSize-1)) ?
								// (agentBrain[agent->loc.y+1][agent->loc.x+1].dsRwrd.left = -1 * RWRD_FACTOR) : (dummy = 1);
							}
						}
					}
					break;
					// Direction is RIGHT.
				case RIGHT:
					// Check for maze limit first.
					if (this->maze->IsPositionWithinLocalLimits(Framework::Position<int>(agent->loc.x + 1, agent->loc.y)))
					{
						agent->iter = agent->iter + 1;         // This is a new iteration.
						// Check for barrier and move agent right if there is no barrier there.
						if (this->maze->CanMove(this->id, Position<int>(agent->loc.x+1, agent->loc.y)))
						{
							// Add to list.
							if (lrnMethod == PHS_DISTG_NVSTED_LP)
							{
								agentBrain[agent->loc.y][agent->loc.x].toX = agent->loc.x + 1;
								agentBrain[agent->loc.y][agent->loc.x].toY = agent->loc.y; 
								pt = getClosestToStart (agentBrain, agent->loc, xSize, ySize, startLoc);
								if ((pt.x != -1) && (pt.y != -1))
								{
									agentBrain[agent->loc.y][agent->loc.x].fromX = pt.x;
									agentBrain[agent->loc.y][agent->loc.x].fromY = pt.y;
									agentBrain[pt.y][pt.x].toX = agent->loc.x;
									agentBrain[pt.y][pt.x].toY = agent->loc.y;
								}
							}   
							else {
								agentBrain[agent->loc.y][agent->loc.x].toX = agent->loc.x + 1;
								agentBrain[agent->loc.y][agent->loc.x].toY = agent->loc.y;         
							}
							// Check for circle.
							if ((lrnMethod == PHS_DISTG_NVSTED) ||
								(lrnMethod == PHS_DISTG_NVSTED_CLO) || (lrnMethod == PHS_DISTG_NVSTED_LP))
							{
								if (agentBrain[agent->loc.y][agent->loc.x].actTaken.right == TRUE_t)
								{
									if (!Validator::IsNull(log, NAME("log")))
									{
										log->Log(&typeid(this), "Information: moveAgent: Goal is Unreachable!");
									}

									fprintf (agentF, "\nInformation: moveAgent: Goal is Unreachable!\n");
									//exit(EXIT_SUCCESS);
									return (UNREACH_ERR);
								}    
							}
							// Make step.
							agentBrain[agent->loc.y][agent->loc.x].actTaken.right = TRUE_t;
							agent->moved = TRUE_t; 
							agent->step  = agent->step + 1;
							agentBrain[agent->loc.y][agent->loc.x].nVsted.right++;
							agent->loc.x = agent->loc.x + 1;     // Move the agent right.
							agentBrain[agent->loc.y][agent->loc.x].isVsted = TRUE_t;       
							agentBrain[agent->loc.y][agent->loc.x].totalNVsted++;
							agentPath[agent->step].dir = a0;
							agentPath[agent->step].loc.x    = agent->loc.x;
							agentPath[agent->step].loc.y    = agent->loc.y;
							agentBrain[agent->loc.y][agent->loc.x].locSt = static_cast<locState_t>(this->maze->GetObjectOn(Position<int>(agent->loc.x, agent->loc.y)));

							this->lastDir = RIGHT;
						}
						// Record barrier.
						else if (static_cast<locState_t>(this->maze->GetObjectOn(Position<int>(agent->loc.x+1, agent->loc.y))) == BARRIER)
						{
							Position<int> pos(agent->loc.x+1, agent->loc.y);
							MessageDistributor::Instance()->Broadcast(id, new MazePositionPackage(id, 0, static_cast<char>(BARRIER), pos));
							agentBrain[agent->loc.y][agent->loc.x+1].locSt = static_cast<locState_t>(this->maze->GetObjectOn(Position<int>(agent->loc.x+1, agent->loc.y)));
							agentBrain[agent->loc.y][agent->loc.x+1].isVsted = NEVER;
							agent->moved = FALSE_t;
							agentPath[agent->step].loc.barDetect.right = TRUE_t;
							// Shadow barrier.
							agentBrain[agent->loc.y][agent->loc.x].dsRwrd.right = -1 * RWRD_FACTOR;       
							(agent->loc.x < xSize-2) ? 
								(agentBrain[agent->loc.y][agent->loc.x+2].dsRwrd.left = -1 * RWRD_FACTOR) : (dummy = 1);
							((agent->loc.y < ySize-1) && (agent->loc.x < xSize-1)) ?
								(agentBrain[agent->loc.y+1][agent->loc.x+1].dsRwrd.up = -1 * RWRD_FACTOR) : (dummy = 1);
							((agent->loc.y >= 1) && (agent->loc.x < xSize-1)) ?
								(agentBrain[agent->loc.y-1][agent->loc.x+1].dsRwrd.down = -1 * RWRD_FACTOR) : (dummy = 1);	
						}
					}
					else if (this->maze->IsPositionWithinGlobalLimits(Framework::Position<int>(agent->loc.x + 1, agent->loc.y)))
					{
						unsigned node;
						char newStart;

						if (!this->exploreLocalFirst)
						{
							if (this->maze->CanMoveToGlobal(this->id, Framework::Position<int>(agent->loc.x, agent->loc.y), Framework::Position<int>(agent->loc.x + 1, agent->loc.y), node, newStart))
							{
								this->lastDir = RIGHT;

								// Add to list.
								if (lrnMethod == PHS_DISTG_NVSTED_LP)
								{
									agentBrain[agent->loc.y][agent->loc.x].toX = agent->loc.x + 1;
									agentBrain[agent->loc.y][agent->loc.x].toY = agent->loc.y; 
									pt = getClosestToStart (agentBrain, agent->loc, xSize, ySize, startLoc);
									if ((pt.x != -1) && (pt.y != -1))
									{
										agentBrain[agent->loc.y][agent->loc.x].fromX = pt.x;
										agentBrain[agent->loc.y][agent->loc.x].fromY = pt.y;
										agentBrain[pt.y][pt.x].toX = agent->loc.x;
										agentBrain[pt.y][pt.x].toY = agent->loc.y;
									}
								}   
								else {
									agentBrain[agent->loc.y][agent->loc.x].toX = agent->loc.x + 1;
									agentBrain[agent->loc.y][agent->loc.x].toY = agent->loc.y;         
								}
								// Check for circle.
								if ((lrnMethod == PHS_DISTG_NVSTED) ||
									(lrnMethod == PHS_DISTG_NVSTED_CLO) || (lrnMethod == PHS_DISTG_NVSTED_LP))
								{
									if (agentBrain[agent->loc.y][agent->loc.x].actTaken.right == TRUE_t)
									{
										if (!Validator::IsNull(log, NAME("log")))
										{
											log->Log(&typeid(this), "Information: moveAgent: Goal is Unreachable!");
										}

										fprintf (agentF, "\nInformation: moveAgent: Goal is Unreachable!\n");
										//exit(EXIT_SUCCESS);
										return (UNREACH_ERR);
									}    
								}

								MoveTo(node, newStart);

								if (this->isGoalOnAnotherNode)
								{	
									agentBrain[agent->loc.y][agent->loc.x].actTaken.right = TRUE_t;
									agent->moved = TRUE_t; 
									agent->step  = agent->step + 1;
									agentBrain[agent->loc.y][agent->loc.x].nVsted.right++;
									//agent->loc.x = agent->loc.x + 1;     // Move the agent right.
									agentPath[agent->step].dir = a0;
									//agentPath[agent->step].loc.x    = agent->loc.x;
									//agentPath[agent->step].loc.y    = agent->loc.y;

									return (MOVED);
								}
								else
								{
									agentBrain[agent->loc.y][agent->loc.x].isVsted = NEVER;
									agent->moved = FALSE_t;
									agentPath[agent->step].loc.barDetect.right = TRUE_t;

									// Shadow barrier.
									agentBrain[agent->loc.y][agent->loc.x].dsRwrd.right = -1 * RWRD_FACTOR;
								}
							}
							else if (static_cast<locState_t>(this->maze->GetObjectOn(Position<int>(agent->loc.x + 1, agent->loc.y))) == BARRIER)
							{
								agentBrain[agent->loc.y][agent->loc.x].isVsted = NEVER;
								agent->moved = FALSE_t;
								agentPath[agent->step].loc.barDetect.right = TRUE_t;

								// Shadow barrier.
								agentBrain[agent->loc.y][agent->loc.x].dsRwrd.right = -1 * RWRD_FACTOR;       
								//(agent->loc.x < xSize-2) ? 
								// (agentBrain[agent->loc.y][agent->loc.x+2].dsRwrd.left = -1 * RWRD_FACTOR) : (dummy = 1);

								//((agent->loc.y < ySize-1) && (agent->loc.x < xSize-1)) ?
								// (agentBrain[agent->loc.y+1][agent->loc.x+1].dsRwrd.up = -1 * RWRD_FACTOR) : (dummy = 1);

								//((agent->loc.y >= 1) && (agent->loc.x < xSize-1)) ?
								// (agentBrain[agent->loc.y-1][agent->loc.x+1].dsRwrd.down = -1 * RWRD_FACTOR) : (dummy = 1);
							}
						}
					}
					break;
					// Direction is LEFT.
				case LEFT:
					// Check for maze limit first.
					if (this->maze->IsPositionWithinLocalLimits(Framework::Position<int>(agent->loc.x - 1, agent->loc.y)))
					{
						agent->iter = agent->iter + 1;         // This is a new iteration.
						// Check for barrier and move agent left if there is no barrier there.
						if (this->maze->CanMove(this->id, Position<int>(agent->loc.x-1, agent->loc.y)))
						{
							// Add to list.
							if (lrnMethod == PHS_DISTG_NVSTED_LP)
							{
								agentBrain[agent->loc.y][agent->loc.x].toX = agent->loc.x - 1;
								agentBrain[agent->loc.y][agent->loc.x].toY = agent->loc.y;
								pt = getClosestToStart (agentBrain, agent->loc, xSize, ySize, startLoc);
								if ((pt.x != -1) && (pt.y != -1))
								{
									agentBrain[agent->loc.y][agent->loc.x].fromX = pt.x;
									agentBrain[agent->loc.y][agent->loc.x].fromY = pt.y;
									agentBrain[pt.y][pt.x].toX = agent->loc.x;
									agentBrain[pt.y][pt.x].toY = agent->loc.y;
								}
							}   
							else {
								agentBrain[agent->loc.y][agent->loc.x].toX = agent->loc.x - 1;
								agentBrain[agent->loc.y][agent->loc.x].toY = agent->loc.y;
							}
							// Check for circle.        
							if ((lrnMethod == PHS_DISTG_NVSTED) ||
								(lrnMethod == PHS_DISTG_NVSTED_CLO) || (lrnMethod == PHS_DISTG_NVSTED_LP))
							{
								if (agentBrain[agent->loc.y][agent->loc.x].actTaken.left == TRUE_t)
								{
									if (!Validator::IsNull(log, NAME("log")))
									{
										log->Log(&typeid(this), "Information: moveAgent: Goal is Unreachable!");
									}

									fprintf (agentF, "\nInformation: moveAgent: Goal is Unreachable!\n");
									//exit(EXIT_SUCCESS);
									return (UNREACH_ERR);
								}    
							}
							// Make step.
							agentBrain[agent->loc.y][agent->loc.x].actTaken.left = TRUE_t;
							agent->moved = TRUE_t; 
							agent->step  = agent->step + 1;
							agentBrain[agent->loc.y][agent->loc.x].nVsted.left++;
							agent->loc.x = agent->loc.x - 1;     // Move the agent left.
							agentBrain[agent->loc.y][agent->loc.x].isVsted = TRUE_t;
							agentBrain[agent->loc.y][agent->loc.x].totalNVsted++;
							agentPath[agent->step].dir = a0;
							agentPath[agent->step].loc.x    = agent->loc.x;
							agentPath[agent->step].loc.y    = agent->loc.y;
							agentBrain[agent->loc.y][agent->loc.x].locSt = static_cast<locState_t>(this->maze->GetObjectOn(Position<int>(agent->loc.x, agent->loc.y)));

							this->lastDir = LEFT;
						}
						// Record barrier.
						else if (static_cast<locState_t>(this->maze->GetObjectOn(Position<int>(agent->loc.x-1, agent->loc.y))) == BARRIER)
						{
							Position<int> pos(agent->loc.x-1, agent->loc.y);
							MessageDistributor::Instance()->Broadcast(id, new MazePositionPackage(id, 0, static_cast<char>(BARRIER), pos));
							agentBrain[agent->loc.y][agent->loc.x-1].locSt = static_cast<locState_t>(this->maze->GetObjectOn(Position<int>(agent->loc.x-1, agent->loc.y)));        
							agentBrain[agent->loc.y][agent->loc.x-1].isVsted = NEVER;
							agent->moved = FALSE_t;
							agentPath[agent->step].loc.barDetect.left = TRUE_t;
							// Shadow barrier.
							agentBrain[agent->loc.y][agent->loc.x].dsRwrd.left = -1 * RWRD_FACTOR;        
							(agent->loc.x >= 2) ? 
								(agentBrain[agent->loc.y][agent->loc.x-2].dsRwrd.right = -1 * RWRD_FACTOR) : (dummy = 1);
							((agent->loc.y < ySize-1) && (agent->loc.x >= 1)) ?
								(agentBrain[agent->loc.y+1][agent->loc.x-1].dsRwrd.up = -1 * RWRD_FACTOR) : (dummy = 1);
							((agent->loc.y >= 1) && (agent->loc.x >= 1)) ?
								(agentBrain[agent->loc.y-1][agent->loc.x-1].dsRwrd.down = -1 * RWRD_FACTOR) : (dummy = 1);
						}
					}
					else if (this->maze->IsPositionWithinGlobalLimits(Framework::Position<int>(agent->loc.x - 1, agent->loc.y)))
					{
						unsigned node;
						char newStart;

						if (!this->exploreLocalFirst)
						{
							if (this->maze->CanMoveToGlobal(this->id, Framework::Position<int>(agent->loc.x, agent->loc.y), Framework::Position<int>(agent->loc.x - 1, agent->loc.y), node, newStart))
							{
								this->lastDir = LEFT;

								// Add to list.
								if (lrnMethod == PHS_DISTG_NVSTED_LP)
								{
									agentBrain[agent->loc.y][agent->loc.x].toX = agent->loc.x - 1;
									agentBrain[agent->loc.y][agent->loc.x].toY = agent->loc.y;
									pt = getClosestToStart (agentBrain, agent->loc, xSize, ySize, startLoc);
									if ((pt.x != -1) && (pt.y != -1))
									{
										agentBrain[agent->loc.y][agent->loc.x].fromX = pt.x;
										agentBrain[agent->loc.y][agent->loc.x].fromY = pt.y;
										agentBrain[pt.y][pt.x].toX = agent->loc.x;
										agentBrain[pt.y][pt.x].toY = agent->loc.y;
									}
								}   
								else {
									agentBrain[agent->loc.y][agent->loc.x].toX = agent->loc.x - 1;
									agentBrain[agent->loc.y][agent->loc.x].toY = agent->loc.y;
								}
								// Check for circle.        
								if ((lrnMethod == PHS_DISTG_NVSTED) ||
									(lrnMethod == PHS_DISTG_NVSTED_CLO) || (lrnMethod == PHS_DISTG_NVSTED_LP))
								{
									if (agentBrain[agent->loc.y][agent->loc.x].actTaken.left == TRUE_t)
									{
										if (!Validator::IsNull(log, NAME("log")))
										{
											log->Log(&typeid(this), "Information: moveAgent: Goal is Unreachable!");
										}

										fprintf (agentF, "\nInformation: moveAgent: Goal is Unreachable!\n");
										//exit(EXIT_SUCCESS);
										return (UNREACH_ERR);
									}    
								}

								MoveTo(node, newStart);

								if (this->isGoalOnAnotherNode)
								{
									agentBrain[agent->loc.y][agent->loc.x].actTaken.left = TRUE_t;
									agent->moved = TRUE_t; 
									agent->step  = agent->step + 1;
									agentBrain[agent->loc.y][agent->loc.x].nVsted.left++;
									//agent->loc.x = agent->loc.x - 1;     // Move the agent left.
									agentPath[agent->step].dir = a0;
									//agentPath[agent->step].loc.x    = agent->loc.x;
									//agentPath[agent->step].loc.y    = agent->loc.y;


									return (MOVED);
								}
								else
								{
									agentBrain[agent->loc.y][agent->loc.x].isVsted = NEVER;
									agent->moved = FALSE_t;
									agentPath[agent->step].loc.barDetect.left = TRUE_t;

									// Shadow barrier.
									agentBrain[agent->loc.y][agent->loc.x].dsRwrd.left = -1 * RWRD_FACTOR;
								}
							}
							else if (static_cast<locState_t>(this->maze->GetObjectOn(Position<int>(agent->loc.x-1, agent->loc.y))) == BARRIER)
							{
								agentBrain[agent->loc.y][agent->loc.x].isVsted = NEVER;
								agent->moved = FALSE_t;
								agentPath[agent->step].loc.barDetect.left = TRUE_t;

								// Shadow barrier.
								agentBrain[agent->loc.y][agent->loc.x].dsRwrd.left = -1 * RWRD_FACTOR;        
								//(agent->loc.x >= 2) ? 
								// (agentBrain[agent->loc.y][agent->loc.x-2].dsRwrd.right = -1 * RWRD_FACTOR) : (dummy = 1);

								//((agent->loc.y < ySize-1) && (agent->loc.x >= 1)) ?
								// (agentBrain[agent->loc.y+1][agent->loc.x-1].dsRwrd.up = -1 * RWRD_FACTOR) : (dummy = 1);

								//((agent->loc.y >= 1) && (agent->loc.x >= 1)) ?
								// (agentBrain[agent->loc.y-1][agent->loc.x-1].dsRwrd.down = -1 * RWRD_FACTOR) : (dummy = 1);
							}
						}
					}
					break;
				case NO_DIR:
					this->lastDir = NO_DIR;

					agent->moved = FALSE_t;
					break;
					// Default case should not happen!
				default:
					if (!Validator::IsNull(log, NAME("log")))
					{
						log->Log(&typeid(this), "Error: agent %d tried to %s, moveAgent: Invalid direction (%d)!", a0.name, a0.val);
					}
					exit (EXIT_FAILURE);
				}
			}
			else
			{
				this->lastDir = NO_DIR;

				agent->moved = FALSE_t;
			}
		}

		if (!Validator::IsNull(agent, NAME("agent")))
		{
			Position<int> pos(agent->loc.x, agent->loc.y);
			MessageDistributor::Instance()->Broadcast(id, new MazePositionPackage(id, 0, static_cast<char>(id | 0x30), pos));
		}

		MazePositionPackage *mazePos = NULL;

		do
		{
			delete mazePos;
			mazePos = NULL;

			mazePos = dynamic_cast<MazePositionPackage *>(this->myMessageQueue.Dequeue());

			if (mazePos != NULL)
			{
				int xCoor = mazePos->GetPosition().GetXCoor();
				int yCoor = mazePos->GetPosition().GetYCoor();

				agentBrain[yCoor][xCoor].locSt = BARRIER;  
				agentBrain[yCoor][xCoor].isVsted = NEVER;
				//agent->moved = FALSE_t;
				//agentPath[agent->step].loc.barDetect.left = FALSE_t;

				//Check that the barrier is not on the left edge
				if (xCoor > 0)
				{
					agentBrain[yCoor][xCoor - 1].dsRwrd.right = -1 * RWRD_FACTOR;
				}

				//Check that the barrier is not on the right edge
				if (xCoor < limit.GetXCoor())
				{
					agentBrain[yCoor][xCoor + 1].dsRwrd.left = -1 * RWRD_FACTOR;
				}

				//Check that the barrier is not on the top edge
				if (yCoor > 0)
				{
					agentBrain[yCoor - 1][xCoor].dsRwrd.down = -1 * RWRD_FACTOR;
				}

				//Check that the barrier is not on the bottom edge
				if (yCoor < limit.GetYCoor())
				{
					agentBrain[yCoor + 1][xCoor].dsRwrd.up = -1 * RWRD_FACTOR;
				}

				#ifdef DebugMLAgent
				if (!Validator::IsNull(log, NAME("log")))
				{
					log->Log(&typeid(this), "Agent: %d, was told that there is a %c on x = %d, y = %d", this->id, BARRIER, mazePos->GetPosition().GetXCoor(), mazePos->GetPosition().GetYCoor());
				}
				#endif
			}

		} while (mazePos != NULL);

		//if (agent->step >= xSize*ySize)
		//{
		//	if (this->exploreLocalFirst)
		//	{
		//		this->exploreLocalFirst = false;
		//	}
		//}

		#ifdef PrintMemoryLeak
			CHECK_HEAP();
		#endif
		return (NO_ERR);
	}



	/////////////////////////////////////////////////////////////////////////////////
	// FUNCTION
	// Name: isLocMatch
	// Description: This function checks if current location [x,y] matches a location
	//              'loc'.
	/////////////////////////////////////////////////////////////////////////////////
	bool_t 
		MLAgent::isLocMatch (int x, int y, state_t loc)
	{
		if ((x == loc.x) && (y == loc.y)) {
			return (TRUE_t);
		}
		else {
			return (FALSE_t);
		}
	}


	/////////////////////////////////////////////////////////////////////////////////
	// FUNCTION
	// Name: isGoal
	// Description: 
	/////////////////////////////////////////////////////////////////////////////////
	bool_t 
		MLAgent::isGoal (int x, int y, state_t **agentBrain, char goal)
	{
		if (agentBrain[y][x].locSt == goal) {
			return (TRUE_t);
		}
		else if ((!this->isTransfered)&&
				 (this->isGoalOnAnotherNode)&&
				 (this->goalPos.GetXCoor() == x)&&
				 (this->goalPos.GetYCoor() == y)){
			return (TRUE_t);
		}
		else {
			return (FALSE_t);
		}
	}



	/////////////////////////////////////////////////////////////////////////////////
	// FUNCTION
	// Name: storeDistances
	// Description: 
	/////////////////////////////////////////////////////////////////////////////////
	void
		MLAgent::storeDistances (state_t **agentBrain, int xSize, int ySize, state_t *goalLoc, char start)
	{
		int i;
		int j;

		// With the Distance-Visited mode, calculate the
		// distance of each state from the goal.
		// NOTE: The distance from a barrier to the goal is
		// very high and exceeds the maximum possible distance. 
		for (i = 0; i < ySize; i++) {
			for (j = 0; j < xSize; j++) {
				// Up.
				if ((i > 0) && (agentBrain[i-1][j].locSt == start)) {
					agentBrain[i][j].distG.up = getDistAB (j, i-1, goalLoc->x, goalLoc->y) + 3.0;
				}
				else if ((i > 0) && agentBrain[i-1][j].locSt != BARRIER) {
					agentBrain[i][j].distG.up = getDistAB (j, i-1, goalLoc->x, goalLoc->y);
				}               
				else {
					agentBrain[i][j].distG.up = getDistAB (0, 0, xSize-1, ySize-1) + 10.0;
				}
				// Down.
				if ((i < ySize-1) && (agentBrain[i+1][j].locSt == start)) {
					agentBrain[i][j].distG.down = getDistAB (j, i+1, goalLoc->x, goalLoc->y) + 3.0;
				}
				else if ((i < ySize-1) && agentBrain[i+1][j].locSt != BARRIER) {
					agentBrain[i][j].distG.down = getDistAB (j, i+1, goalLoc->x, goalLoc->y);
				}
				else {
					agentBrain[i][j].distG.down = getDistAB (0, 0, xSize-1, ySize-1) + 10.0;
				}
				// Right.
				if ((j < xSize-1) && (agentBrain[i][j+1].locSt == start)) {
					agentBrain[i][j].distG.right = getDistAB (j+1, i, goalLoc->x, goalLoc->y) + 3.0;
				}
				else if ((j < xSize-1) && agentBrain[i][j+1].locSt != BARRIER) {
					agentBrain[i][j].distG.right = getDistAB (j+1, i, goalLoc->x, goalLoc->y);
				}
				else {
					agentBrain[i][j].distG.right = getDistAB (0, 0, xSize-1, ySize-1) + 10.0;
				}
				// Left.
				if ((j > 0) && (agentBrain[i][j+1].locSt == start)) {
					agentBrain[i][j].distG.left = getDistAB (j-1, i, goalLoc->x, goalLoc->y) + 3.0;
				}
				else if ((j > 0) && agentBrain[i][j+1].locSt != BARRIER) {
					agentBrain[i][j].distG.left = getDistAB (j-1, i, goalLoc->x, goalLoc->y);
				}
				else {
					agentBrain[i][j].distG.left = getDistAB (0, 0, xSize-1, ySize-1) + 10.0;
				}
			}            
		}
	}


	/////////////////////////////////////////////////////////////////////////////////
	// FUNCTION
	// Name: storeRewards
	// Description: This function stores immediate and discounted reward values in 
	//              their corresponding agentPath and agentBrain locations.
	//              Immediate rewards are 0 for all locations and 1 for only the 
	//              location directly leading to the Goal state. Discounted rewards
	//              rewards are evaluated by back-propagating the path and applying 
	//              GAMMA^i where GAMMA is the discount rate and i is the number of 
	//              the location in the back-path (e.g. i = 0 for the location 
	//              directly leading to the Goal). This function also returns the
	//              Discounted Cumulative Reward.
	/////////////////////////////////////////////////////////////////////////////////
	float 
		MLAgent::storeRewards (float gamma, agent_t *agentPath, state_t **agentBrain, int numSteps, int fVerb, FILE *agentF)
	{
		int i;                   // index.
		float dsReward;          // Discounted reward.
		float dsCmReward = 0;    // Cumulative reward.

		// Exclude the Starting location.
		for (i = numSteps; i >= 1; i--) {
			// Evaluate the discounted reward value for current location.
			dsReward = pow(gamma, numSteps-i) * RWRD_FACTOR;
			dsCmReward += dsReward;
			switch (agentPath[i].dir.val) {
	case UP:
		agentBrain[agentPath[i-1].loc.y][agentPath[i-1].loc.x].dsRwrd.up = 
			MAX(dsReward,agentBrain[agentPath[i-1].loc.y][agentPath[i-1].loc.x].dsRwrd.up);
		// Give an immediate reward of 1 to last location in path before goal.
		if (i == numSteps) {
			agentBrain[agentPath[i-1].loc.y][agentPath[i-1].loc.x].imRwrd.up = 1 * RWRD_FACTOR;
		}
		break;
	case DOWN:
		agentBrain[agentPath[i-1].loc.y][agentPath[i-1].loc.x].dsRwrd.down = 
			MAX(dsReward,agentBrain[agentPath[i-1].loc.y][agentPath[i-1].loc.x].dsRwrd.down);
		// Give an immediate reward of 1 to last location in path before goal.
		if (i == numSteps) {
			agentBrain[agentPath[i-1].loc.y][agentPath[i-1].loc.x].imRwrd.down = 1 * RWRD_FACTOR;
		}
		break;
	case RIGHT:
		agentBrain[agentPath[i-1].loc.y][agentPath[i-1].loc.x].dsRwrd.right = 
			MAX(dsReward,agentBrain[agentPath[i-1].loc.y][agentPath[i-1].loc.x].dsRwrd.right);
		// Give an immediate reward of 1 to last location in path before goal.
		if (i == numSteps) {
			agentBrain[agentPath[i-1].loc.y][agentPath[i-1].loc.x].imRwrd.right = 1 * RWRD_FACTOR;
		}
		break;
	case LEFT:
		agentBrain[agentPath[i-1].loc.y][agentPath[i-1].loc.x].dsRwrd.left = 
			MAX(dsReward,agentBrain[agentPath[i-1].loc.y][agentPath[i-1].loc.x].dsRwrd.left);
		// Give an immediate reward of 1 to last location in path before goal.
		if (i == numSteps) {
			agentBrain[agentPath[i-1].loc.y][agentPath[i-1].loc.x].imRwrd.left = 1 * RWRD_FACTOR;
		}
		break;
	default:
		if (!Validator::IsNull(log, NAME("log")))
		{
			log->Log(&typeid(this), "Error: storeRewards: Invalid direction!");
		}
		break;
			} 

			if (fVerb > 3) {
				fprintf (agentF, "\nstoreRewards: agentBrain[x=%d,y=%d].dsRwrd.up,imRwrd.up = %f,%.0f", 
					agentPath[i-1].loc.x, agentPath[i-1].loc.y, 
					agentBrain[agentPath[i-1].loc.y][agentPath[i-1].loc.x].dsRwrd.up,
					agentBrain[agentPath[i-1].loc.y][agentPath[i-1].loc.x].imRwrd.up);
				fprintf (agentF, "\nstoreRewards: agentBrain[x=%d,y=%d].dsRwrd.down,imRwrd.down = %f,%.0f", 
					agentPath[i-1].loc.x, agentPath[i-1].loc.y, 
					agentBrain[agentPath[i-1].loc.y][agentPath[i-1].loc.x].dsRwrd.down,
					agentBrain[agentPath[i-1].loc.y][agentPath[i-1].loc.x].imRwrd.down);
				fprintf (agentF, "\nstoreRewards: agentBrain[x=%d,y=%d].dsRwrd.right,imRwrd.right = %f,%.0f", 
					agentPath[i-1].loc.x, agentPath[i-1].loc.y, 
					agentBrain[agentPath[i-1].loc.y][agentPath[i-1].loc.x].dsRwrd.right,
					agentBrain[agentPath[i-1].loc.y][agentPath[i-1].loc.x].imRwrd.right);
				fprintf (agentF, "\nstoreRewards: agentBrain[x=%d,y=%d].dsRwrd.left,imRwrd.left = %f,%.0f", 
					agentPath[i-1].loc.x, agentPath[i-1].loc.y, 
					agentBrain[agentPath[i-1].loc.y][agentPath[i-1].loc.x].dsRwrd.left,
					agentBrain[agentPath[i-1].loc.y][agentPath[i-1].loc.x].imRwrd.left);
			}

		}
		return (dsCmReward);
	}


	/////////////////////////////////////////////////////////////////////////////////
	// FUNCTION
	// Name: getQs0a0
	// Description: This functions retuns the Q value based on the Q-Learning 
	//              equation.
	/////////////////////////////////////////////////////////////////////////////////
	float
		MLAgent::getQs0a0 (float alpha, float gamma, float lambda, state_t **agentBrain, 
		state_t *s, action_t *a, int nAhead, int gLrnPathNum, int gMaxLrnPaths, int fVerb,
		FILE *agentF, FILE *rqRwrdF)
	{
		float *rs0a0 = (float *) calloc (nAhead, sizeof(float));        // Immediate reward (r).
		float *Qs0a0 = (float *) calloc (nAhead, sizeof(float));        // Q(s,a).
		float *maxQs1a1 = (float *) calloc (nAhead, sizeof(float));     // max a' Q(s',a').
		float *newQs0a0 = (float *) calloc (nAhead, sizeof(float));     // New Q(s,a).
		float *retQs0a0 = (float *) calloc (nAhead, sizeof(float));     // Return Q(s,a). 
		int i; 

		for (i = 0; i < nAhead; i++) {
			// Initialize values.
			newQs0a0[i] = 0;    
			// Evaluate Q(s,a) value of taking action 'a' in state 's'.
			switch (a[i].val) {
	case UP:
		rs0a0[i]    = agentBrain[s[i].y][s[i].x].imRwrd.up;
		maxQs1a1[i] = MAX(agentBrain[s[i].y-1][s[i].x].dsRwrd.up, 
			(MAX(agentBrain[s[i].y-1][s[i].x].dsRwrd.down, 
			MAX(agentBrain[s[i].y-1][s[i].x].dsRwrd.right, 
			agentBrain[s[i].y-1][s[i].x].dsRwrd.left))));
		Qs0a0[i]    = agentBrain[s[i].y][s[i].x].dsRwrd.up;    
#ifdef DEBUG
		if (agentBrain[s[i].y-1][s[i].x].locSt == BARRIER) {
			if (!Validator::IsNull(log, NAME("log")))
			{
				log->Log(&typeid(this), "Note: getQs0a0: Barrier is Up.");
			}
		}
#endif
		break;

	case DOWN:
		rs0a0[i]    = agentBrain[s[i].y][s[i].x].imRwrd.down;
		maxQs1a1[i] = MAX(agentBrain[s[i].y+1][s[i].x].dsRwrd.up, 
			(MAX(agentBrain[s[i].y+1][s[i].x].dsRwrd.down, 
			MAX(agentBrain[s[i].y+1][s[i].x].dsRwrd.right, 
			agentBrain[s[i].y+1][s[i].x].dsRwrd.left))));
		Qs0a0[i]    = agentBrain[s[i].y][s[i].x].dsRwrd.down;     
#ifdef DEBUG
		if (agentBrain[s[i].y+1][s[i].x].locSt == BARRIER) {
			if (!Validator::IsNull(log, NAME("log")))
			{
				log->Log(&typeid(this), "Note: getQs0a0: Barrier is Down.");
			}
		}
#endif
		break;

	case RIGHT:
		rs0a0[i]    = agentBrain[s[i].y][s[i].x].imRwrd.right;
		maxQs1a1[i] = MAX(agentBrain[s[i].y][s[i].x+1].dsRwrd.up, 
			(MAX(agentBrain[s[i].y][s[i].x+1].dsRwrd.down, 
			MAX(agentBrain[s[i].y][s[i].x+1].dsRwrd.right, 
			agentBrain[s[i].y][s[i].x+1].dsRwrd.left))));
		Qs0a0[i]    = agentBrain[s[i].y][s[i].x].dsRwrd.right;     
#ifdef DEBUG
		if (agentBrain[s[i].y][s[i].x+1].locSt == BARRIER) {
			if (!Validator::IsNull(log, NAME("log")))
			{
				log->Log(&typeid(this), "Note: getQs0a0: Barrier is Right.");
			}
		}
#endif
		break;

	case LEFT:
		rs0a0[i]    = agentBrain[s[i].y][s[i].x].imRwrd.left;
		maxQs1a1[i] = MAX(agentBrain[s[i].y][s[i].x-1].dsRwrd.up, 
			(MAX(agentBrain[s[i].y][s[i].x-1].dsRwrd.down, 
			MAX(agentBrain[s[i].y][s[i].x-1].dsRwrd.right, 
			agentBrain[s[i].y][s[i].x-1].dsRwrd.left))));
		Qs0a0[i]    = agentBrain[s[i].y][s[i].x].dsRwrd.left;     
#ifdef DEBUG
		if (agentBrain[s[i].y][s[i].x-1].locSt == BARRIER) {
			if (!Validator::IsNull(log, NAME("log")))
			{
				log->Log(&typeid(this), "Note: getQs0a0: Barrier is Left.");
			}
		}
#endif
		break;

	default:
		if (!Validator::IsNull(log, NAME("log")))
		{
			log->Log(&typeid(this), "Error: getQs0a0: Invalid direction!\n");
		}
		break;
			}

			// Calculate target value.
			retQs0a0[i] = rs0a0[i] + gamma*maxQs1a1[i];
#ifdef DEBUG    
			if (retQs0a0[i] >= 1) {
				if (!Validator::IsNull(log, NAME("log")))
				{
					log->Log(&typeid(this), "Note: getQs0a0: retQs0a0[%d] = %f >= 1", i, retQs0a0[i]);
				}
			}
#endif

			if (fVerb > 2) {
				fprintf (agentF, "\ngetQs0a0: s[%d]=[x=%d,y=%d], a[%d]=%s, alpha=%f, gamma=%f, lambda=%f", 
					i, s[i].x, s[i].y, i, actNames[a[i].val], alpha, gamma, lambda);
				fprintf (agentF, "\ngetQs0a0: Qs0a0[%d]=%f, rs0a0[%d]=%f, maxQs1a1[%d]=%f, retQs0a0[%d]=%f", 
					i, Qs0a0[i], i, rs0a0[i], i, maxQs1a1[i], i, retQs0a0[i]);
			}

		}

		// EQUATION: N = 1: Q(s,a) <- Q(s,a) + alpha(s,a) + gamma*maxQ(s',a') - Q(s,a)).
		//           N = 1: Q(s,a) <- ...  
		//           N = 3: Q(s,a) <- ...
		//           N = 4: Q(s,a) <- ...
		//           N = 5: Q(s,a) <- ... 
		//           N = 6: Q(s,a) <- ... 
		switch (nAhead) {
			// 1-step.
  case 1:
	  // Already calculated above.
	  newQs0a0[0] = Qs0a0[0] + 
		  alpha*(retQs0a0[0] - 
		  Qs0a0[0]);
	  break;
	  // 2-step.
  case 2:
	  newQs0a0[0] = Qs0a0[0] + 
		  alpha*((1-lambda)*(retQs0a0[0] + lambda*retQs0a0[1]) + 
		  pow(lambda,2)*retQs0a0[1] -
		  Qs0a0[0]);
	  break;
	  // 3-step.
  case 3:
	  newQs0a0[0] = Qs0a0[0] + 
		  alpha*((1-lambda)*(retQs0a0[0] + lambda*retQs0a0[1] + pow(lambda,2)*retQs0a0[2]) 
		  + pow(lambda,3)*retQs0a0[2] -
		  Qs0a0[0]);
	  break;
  case 4:
	  newQs0a0[0] = Qs0a0[0] + 
		  alpha*((1-lambda)*(retQs0a0[0] + lambda*retQs0a0[1] + pow(lambda,2)*retQs0a0[2] + pow(lambda,3)*retQs0a0[3]) 
		  + pow(lambda,4)*retQs0a0[3] -
		  Qs0a0[0]);
	  break;
  case 5:
	  newQs0a0[0] = Qs0a0[0] + 
		  alpha*((1-lambda)*(retQs0a0[0] + lambda*retQs0a0[1] + pow(lambda,2)*retQs0a0[2] + pow(lambda,3)*retQs0a0[3]
	  + pow(lambda,4)*retQs0a0[4]) 
		  + pow(lambda,5)*retQs0a0[4] -
		  Qs0a0[0]);
	  break;
  case 6:
	  newQs0a0[0] = Qs0a0[0] + 
		  alpha*((1-lambda)*(retQs0a0[0] + lambda*retQs0a0[1] + pow(lambda,2)*retQs0a0[2] + pow(lambda,3)*retQs0a0[3]
	  + pow(lambda,4)*retQs0a0[4] + pow(lambda,5)*retQs0a0[5])
		  + pow(lambda,6)*retQs0a0[5] -
		  Qs0a0[0]);
	  break;
  default:
	  if (!Validator::IsNull(log, NAME("log")))
	  {
		log->Log(&typeid(this), "Error: getQs0a0: Unsupported number of lookahead levels.");
	  }
	  break;
		}

#if 1
		// Print to R/Q reward file (...).
		if (gLrnPathNum == (gMaxLrnPaths - 1)) {
			fprintf (rqRwrdF, "%f %f\n", retQs0a0[0], newQs0a0[0]);
		}
#endif

		if (fVerb > 2) {
			fprintf (agentF, "\ngetQs0a0: newQs0a0[0]=%f", newQs0a0[0]);
		}

		float qVal = newQs0a0[0];

		free (rs0a0);
		rs0a0 = NULL;

		free (Qs0a0);
		Qs0a0 = NULL;

		free (maxQs1a1);
		maxQs1a1 = NULL;

		free (newQs0a0);
		newQs0a0 = NULL;

		free (retQs0a0);
		retQs0a0 = NULL;

		#ifdef PrintMemoryLeak
			CHECK_HEAP();
		#endif
		return (qVal);
	}



	/////////////////////////////////////////////////////////////////////////////////
	// FUNCTION
	// Name: doLearning
	// Description:
	/////////////////////////////////////////////////////////////////////////////////
	bool_t
		MLAgent::doLearning (float alpha, float gamma, float lambda, state_t **agentBrain, 
		state_t *s, action_t *a, int xSize, int ySize, learnMethod_t lrnMethod, 
		int nAhead, state_t startLoc, state_t goalLoc, agent_t *lrnPath,
		float w1, float w2, float w3, int gLrnPathNum, int gMaxLrnPaths, int fVerb,
		FILE *agentF, FILE *rqRwrdF)
	{
		bool_t done = FALSE_t;

		Framework::Position<int> limit = this->maze->GetLimits();
		int maxSteps = MAX_AGENT_STEPS(this->maze->GetXSize(), this->maze->GetYSize());

		agentBrain[s[0].y][s[0].x].x = s[0].x;
		agentBrain[s[0].y][s[0].x].y = s[0].y;

		// Initialize lookahead states.
		if (nAhead >= 2) {
			s[1].x = s[0].x;
			s[1].y = s[0].y;
			if (nAhead >= 3) {
				s[2].x = s[1].x;
				s[2].y = s[1].y;
				if (nAhead >= 4) {
					s[3].x = s[2].x;
					s[3].y = s[2].y;
					if (nAhead >= 5) {
						s[4].x = s[3].x;
						s[4].y = s[3].y;
						if (nAhead >= 6) {
							s[5].x = s[4].x;
							s[5].y = s[4].y;
						} 
					} 
				} 
			} 
		} 

		if (fVerb > 0) {
			fprintf (agentF,"\ndoLearning: s[0]=[x=%d,y=%d], a[0]=%s", s[0].x, s[0].y, actNames[a[0].val]);
			fprintf (agentF, "\ndoLearning: Before: total.up=%f, total.down=%f, total.right=%f, total.left=%f",
				agentBrain[s[0].y][s[0].x].total.up, agentBrain[s[0].y][s[0].x].total.down,
				agentBrain[s[0].y][s[0].x].total.right, agentBrain[s[0].y][s[0].x].total.left);
			fprintf (agentF,"\ndoLearning: alpha=%f, gamma=%f, lambda=%f", alpha, gamma, lambda);
		}

		switch(a[0].val) {

  case UP:
	  if (isLocMatch(s[0].x, s[0].y-1, goalLoc) == TRUE_t) {
			  agentBrain[s[0].y][s[0].x].imRwrd.up  = 1 * RWRD_FACTOR;
			  done = TRUE_t;
		  }
		  else {
			  if (nAhead >= 2) {
				  s[1].y--;
			  }
			  agentBrain[s[0].y][s[0].x].imRwrd.up  = 0;     
			  done = FALSE_t;
		  }
		  if (nAhead >= 2) {
			  a[1] = takeActionOnBoltz(agentBrain, &s[1], TRUE_t, a[0], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
			  if (nAhead >= 3) {
				  s[2] = makeHypoStep (s[1], a[1], agentBrain, xSize, ySize, goalLoc);
				  a[2] = takeActionOnBoltz(agentBrain, &s[2], TRUE_t, a[1], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
				  if (nAhead >= 4) {
					  s[3] = makeHypoStep (s[2], a[2], agentBrain, xSize, ySize, goalLoc);
					  a[3] = takeActionOnBoltz(agentBrain, &s[3], TRUE_t, a[2], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
					  if (nAhead >= 5) {
						  s[4] = makeHypoStep (s[3], a[3], agentBrain, xSize, ySize, goalLoc);
						  a[4] = takeActionOnBoltz(agentBrain, &s[4], TRUE_t, a[3], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
						  if (nAhead >= 6) {
							  s[5] = makeHypoStep (s[4], a[4], agentBrain, xSize, ySize, goalLoc);
							  a[5] = takeActionOnBoltz(agentBrain, &s[5], TRUE_t, a[4], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
						  }	
					  }	
				  }	
			  }
		  }
		  /*if (nAhead >= 2) {
			  a[1] = takeActionOnBoltz(agentBrain, &s[1], FALSE_t, a[0], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
			  if (nAhead >= 3) {
				  s[2] = makeHypoStep (s[1], a[1], agentBrain, xSize, ySize, goalLoc);
				  a[2] = takeActionOnBoltz(agentBrain, &s[2], FALSE_t, a[1], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
				  if (nAhead >= 4) {
					  s[3] = makeHypoStep (s[2], a[2], agentBrain, xSize, ySize, goalLoc);
					  a[3] = takeActionOnBoltz(agentBrain, &s[3], FALSE_t, a[2], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
					  if (nAhead >= 5) {
						  s[4] = makeHypoStep (s[3], a[3], agentBrain, xSize, ySize, goalLoc);
						  a[4] = takeActionOnBoltz(agentBrain, &s[4], FALSE_t, a[3], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
						  if (nAhead >= 6) {
							  s[5] = makeHypoStep (s[4], a[4], agentBrain, xSize, ySize, goalLoc);
							  a[5] = takeActionOnBoltz(agentBrain, &s[5], FALSE_t, a[4], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
						  }	
					  }	
				  }	
			  }
		  }*/

		  if (s[0].y - 1 > 0)
		  {
			agentBrain[s[0].y][s[0].x].dsRwrd.up  = getQs0a0(alpha, gamma, lambda, agentBrain, s, a, nAhead, gLrnPathNum, gMaxLrnPaths, fVerb, agentF, rqRwrdF);
		  }
#if 0
		  switch (lrnMethod) {
  case QVAL_ONLY:
	  agentBrain[s[0].y][s[0].x].total.up = 1 * agentBrain[s[0].y][s[0].x].dsRwrd.up;
	  break;
  case DISTG_ONLY:
	  agentBrain[s[0].y][s[0].x].total.up = 1 / (agentBrain[s[0].y][s[0].x].distG.up + 1);
	  break;
  case QVAL_NVSTED:
	  agentBrain[s[0].y][s[0].x].total.up = w1 * agentBrain[s[0].y][s[0].x].dsRwrd.up;
	  break;
  case DISTG_NVSTED:
	  agentBrain[s[0].y][s[0].x].total.up = w2 / (agentBrain[s[0].y][s[0].x].distG.up + 1);
	  break;
  case QVAL_DISTG:
  case COMBINE_ALL:
	  agentBrain[s[0].y][s[0].x].total.up = (w1 * agentBrain[s[0].y][s[0].x].dsRwrd.up) +
		  (w2 / (agentBrain[s[0].y][s[0].x].distG.up + 1));
	  break;
  default:
	  break;
		  }
#endif
		  if (fVerb > 0) {
			  fprintf (agentF, "\ndoLearning: After: total.up=%f, total.down=%f, total.right=%f, total.left=%f",
				  agentBrain[s[0].y][s[0].x].total.up, agentBrain[s[0].y][s[0].x].total.down,
				  agentBrain[s[0].y][s[0].x].total.right, agentBrain[s[0].y][s[0].x].total.left);
		  }     
		  if (fVerb > 1) {
			  if (nAhead >= 2) {
				  fprintf (agentF,"\ndoLearning: s[1]=[x=%d,y=%d], a[1]=%s", s[1].x, s[1].y, actNames[a[1].val]);
				  if (nAhead >= 3) {
					  fprintf (agentF,"\ndoLearning: s[2]=[x=%d,y=%d], a[2]=%s", s[2].x, s[2].y, actNames[a[2].val]);
					  if (nAhead >= 4) {
						  fprintf (agentF,"\ndoLearning: s[3]=[x=%d,y=%d], a[3]=%s", s[3].x, s[3].y, actNames[a[3].val]);
						  if (nAhead >= 5) {
							  fprintf (agentF,"\ndoLearning: s[4]=[x=%d,y=%d], a[4]=%s", s[4].x, s[4].y, actNames[a[4].val]);
							  if (nAhead >= 6) {
								  fprintf (agentF,"\ndoLearning: s[5]=[x=%d,y=%d], a[5]=%s", s[5].x, s[5].y, actNames[a[5].val]);
							  }
						  }
					  }
				  }
			  }
		  }
		  lrnPath[0].step++; 
		  agentBrain[s[0].y][s[0].x].nLrnVsted.up++;
		  s[0].y--; 

		  if (s[0].y > 0)
		  {
			  agentBrain[s[0].y][s[0].x].isLrnVsted = TRUE_t;
		  }		  

		  if (lrnPath[0].step < maxSteps)
		  {
			  lrnPath[lrnPath[0].step].loc.x = s[0].x;
			  lrnPath[lrnPath[0].step].loc.y = s[0].y;
			  lrnPath[lrnPath[0].step].dir = a[0];
		  }
		  else
		  {
			  if (!Validator::IsNull(log, NAME("log")))
			  {
				log->Log(&typeid(this), "ERROR: Number of step exceeds max number of steps");
			  }
		  }
	  break;

  case DOWN:
	  if (isLocMatch(s[0].x, s[0].y+1, goalLoc) == TRUE_t) {
			  agentBrain[s[0].y][s[0].x].imRwrd.down  = 1 * RWRD_FACTOR;
			  done = TRUE_t;
		  }
		  else {
			  if (nAhead >= 2) {
				  s[1].y++;
			  }
			  agentBrain[s[0].y][s[0].x].imRwrd.down  = 0;  
			  done = FALSE_t;
		  }
		  if (nAhead >= 2) {
			  a[1] = takeActionOnBoltz(agentBrain, &s[1], TRUE_t, a[0], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
			  if (nAhead >= 3) {
				  s[2] = makeHypoStep (s[1], a[1], agentBrain, xSize, ySize, goalLoc);
				  a[2] = takeActionOnBoltz(agentBrain, &s[2], TRUE_t, a[1], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
				  if (nAhead >= 4) {
					  s[3] = makeHypoStep (s[2], a[2], agentBrain, xSize, ySize, goalLoc);
					  a[3] = takeActionOnBoltz(agentBrain, &s[3], TRUE_t, a[2], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
					  if (nAhead >= 5) {
						  s[4] = makeHypoStep (s[3], a[3], agentBrain, xSize, ySize, goalLoc);
						  a[4] = takeActionOnBoltz(agentBrain, &s[4], TRUE_t, a[3], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
						  if (nAhead >= 6) {
							  s[5] = makeHypoStep (s[4], a[4], agentBrain, xSize, ySize, goalLoc);
							  a[5] = takeActionOnBoltz(agentBrain, &s[5], TRUE_t, a[4], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
						  }	
					  }
				  }
			  }
		  }
		  /*if (nAhead >= 2) {
			  a[1] = takeActionOnBoltz(agentBrain, &s[1], FALSE_t, a[0], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
			  if (nAhead >= 3) {
				  s[2] = makeHypoStep (s[1], a[1], agentBrain, xSize, ySize, goalLoc);
				  a[2] = takeActionOnBoltz(agentBrain, &s[2], FALSE_t, a[1], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
				  if (nAhead >= 4) {
					  s[3] = makeHypoStep (s[2], a[2], agentBrain, xSize, ySize, goalLoc);
					  a[3] = takeActionOnBoltz(agentBrain, &s[3], FALSE_t, a[2], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
					  if (nAhead >= 5) {
						  s[4] = makeHypoStep (s[3], a[3], agentBrain, xSize, ySize, goalLoc);
						  a[4] = takeActionOnBoltz(agentBrain, &s[4], FALSE_t, a[3], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
						  if (nAhead >= 6) {
							  s[5] = makeHypoStep (s[4], a[4], agentBrain, xSize, ySize, goalLoc);
							  a[5] = takeActionOnBoltz(agentBrain, &s[5], FALSE_t, a[4], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
						  }	
					  }
				  }
			  }
		  }*/

		  if (s[0].y + 1 < limit.GetYCoor())
		  {
			agentBrain[s[0].y][s[0].x].dsRwrd.down  = getQs0a0(alpha, gamma, lambda, agentBrain, s, a, nAhead, gLrnPathNum, gMaxLrnPaths, fVerb, agentF, rqRwrdF);
		  }
#if 0
		  switch (lrnMethod) {
  case QVAL_ONLY:
	  agentBrain[s[0].y][s[0].x].total.down = 1 * agentBrain[s[0].y][s[0].x].dsRwrd.down;
	  break;
  case DISTG_ONLY:
	  agentBrain[s[0].y][s[0].x].total.down = 1 / (agentBrain[s[0].y][s[0].x].distG.down + 1);
	  break;
  case QVAL_NVSTED:
	  agentBrain[s[0].y][s[0].x].total.down = w1 * agentBrain[s[0].y][s[0].x].dsRwrd.down;
	  break;
  case DISTG_NVSTED:
	  agentBrain[s[0].y][s[0].x].total.down = w2 / (agentBrain[s[0].y][s[0].x].distG.down + 1);
	  break;
  case QVAL_DISTG:
  case COMBINE_ALL:
	  agentBrain[s[0].y][s[0].x].total.down = (w1 * agentBrain[s[0].y][s[0].x].dsRwrd.down) +
		  (w2 / (agentBrain[s[0].y][s[0].x].distG.down + 1));
	  break;
  default:
	  break;
		  }
#endif
		  if (fVerb > 0) {
			  fprintf (agentF, "\ndoLearning: After: total.up=%f, total.down=%f, total.right=%f, total.left=%f",
				  agentBrain[s[0].y][s[0].x].total.up, agentBrain[s[0].y][s[0].x].total.down,
				  agentBrain[s[0].y][s[0].x].total.right, agentBrain[s[0].y][s[0].x].total.left);
		  } 
		  if (fVerb > 1) {
			  if (nAhead >= 2) {
				  fprintf (agentF,"\ndoLearning: s[1]=[x=%d,y=%d], a[1]=%s", s[1].x, s[1].y, actNames[a[1].val]);
				  if (nAhead >= 3) {
					  fprintf (agentF,"\ndoLearning: s[2]=[x=%d,y=%d], a[2]=%s", s[2].x, s[2].y, actNames[a[2].val]);
					  if (nAhead >= 4) {
						  fprintf (agentF,"\ndoLearning: s[3]=[x=%d,y=%d], a[3]=%s", s[3].x, s[3].y, actNames[a[3].val]);
						  if (nAhead >= 5) {
							  fprintf (agentF,"\ndoLearning: s[4]=[x=%d,y=%d], a[4]=%s", s[4].x, s[4].y, actNames[a[4].val]);
							  if (nAhead >= 6) {
								  fprintf (agentF,"\ndoLearning: s[5]=[x=%d,y=%d], a[5]=%s", s[5].x, s[5].y, actNames[a[5].val]);
							  }
						  }
					  }
				  }
			  }
		  }

		  lrnPath[0].step++;
		  agentBrain[s[0].y][s[0].x].nLrnVsted.down++;
		  s[0].y++;  

		  if (s[0].y < limit.GetYCoor())
		  {
			agentBrain[s[0].y][s[0].x].isLrnVsted = TRUE_t;
		  }
		  
		  if (lrnPath[0].step < maxSteps)
		  {
			  lrnPath[lrnPath[0].step].loc.x = s[0].x;
			  lrnPath[lrnPath[0].step].loc.y = s[0].y;
			  lrnPath[lrnPath[0].step].dir = a[0];
		  }
		  else
		  {
			  if (!Validator::IsNull(log, NAME("log")))
			  {
				log->Log(&typeid(this), "ERROR: Number of step exceeds max number of steps");
			  }
		  }

	  break;

  case RIGHT:
	  if (isLocMatch(s[0].x+1, s[0].y, goalLoc) == TRUE_t) {
			  agentBrain[s[0].y][s[0].x].imRwrd.right  = 1 * RWRD_FACTOR;
			  done = TRUE_t;
		  }
		  else {
			  if (nAhead >= 2) {
				  s[1].x++;
			  }
			  agentBrain[s[0].y][s[0].x].imRwrd.right  = 0;
			  done = FALSE_t;
		  }
		  if (nAhead >= 2) {
			  a[1] = takeActionOnBoltz(agentBrain, &s[1], TRUE_t, a[0], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
			  if (nAhead >= 3) {
				  s[2] = makeHypoStep (s[1], a[1], agentBrain, xSize, ySize, goalLoc);
				  a[2] = takeActionOnBoltz(agentBrain, &s[2], TRUE_t, a[1], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
				  if (nAhead >= 4) {
					  s[3] = makeHypoStep (s[2], a[2], agentBrain, xSize, ySize, goalLoc);
					  a[3] = takeActionOnBoltz(agentBrain, &s[3], TRUE_t, a[2], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
					  if (nAhead >= 5) {
						  s[4] = makeHypoStep (s[3], a[3], agentBrain, xSize, ySize, goalLoc);
						  a[4] = takeActionOnBoltz(agentBrain, &s[4], TRUE_t, a[3], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
						  if (nAhead >= 6) {
							  s[5] = makeHypoStep (s[4], a[4], agentBrain, xSize, ySize, goalLoc);
							  a[5] = takeActionOnBoltz(agentBrain, &s[5], TRUE_t, a[4], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
						  }	
					  }
				  }
			  }
		  }
		  /*if (nAhead >= 2) {
			  a[1] = takeActionOnBoltz(agentBrain, &s[1], FALSE_t, a[0], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
			  if (nAhead >= 3) {
				  s[2] = makeHypoStep (s[1], a[1], agentBrain, xSize, ySize, goalLoc);
				  a[2] = takeActionOnBoltz(agentBrain, &s[2], FALSE_t, a[1], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
				  if (nAhead >= 4) {
					  s[3] = makeHypoStep (s[2], a[2], agentBrain, xSize, ySize, goalLoc);
					  a[3] = takeActionOnBoltz(agentBrain, &s[3], FALSE_t, a[2], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
					  if (nAhead >= 5) {
						  s[4] = makeHypoStep (s[3], a[3], agentBrain, xSize, ySize, goalLoc);
						  a[4] = takeActionOnBoltz(agentBrain, &s[4], FALSE_t, a[3], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
						  if (nAhead >= 6) {
							  s[5] = makeHypoStep (s[4], a[4], agentBrain, xSize, ySize, goalLoc);
							  a[5] = takeActionOnBoltz(agentBrain, &s[5], FALSE_t, a[4], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
						  }	
					  }
				  }
			  }
		  }*/

		  if (s[0].x + 1 < limit.GetXCoor())
		  {
			agentBrain[s[0].y][s[0].x].dsRwrd.right  = getQs0a0(alpha, gamma, lambda, agentBrain, s, a, nAhead, gLrnPathNum, gMaxLrnPaths, fVerb, agentF, rqRwrdF);
		  }
#if 0
		  switch (lrnMethod) {
  case QVAL_ONLY:
	  agentBrain[s[0].y][s[0].x].total.right = 1 * agentBrain[s[0].y][s[0].x].dsRwrd.right;
	  break;
  case DISTG_ONLY:
	  agentBrain[s[0].y][s[0].x].total.right = 1 / (agentBrain[s[0].y][s[0].x].distG.right + 1);
	  break;
  case QVAL_NVSTED:
	  agentBrain[s[0].y][s[0].x].total.right = w1 * agentBrain[s[0].y][s[0].x].dsRwrd.right;
	  break;
  case DISTG_NVSTED:
	  agentBrain[s[0].y][s[0].x].total.right = w2 / (agentBrain[s[0].y][s[0].x].distG.right + 1);
	  break;
  case QVAL_DISTG:
  case COMBINE_ALL:
	  agentBrain[s[0].y][s[0].x].total.right = (w1 * agentBrain[s[0].y][s[0].x].dsRwrd.right) +
		  (w2 / (agentBrain[s[0].y][s[0].x].distG.right + 1));
	  break;
  default:
	  break;
		  }
#endif
		  if (fVerb > 0) {
			  fprintf (agentF, "\ndoLearning: After: total.up=%f, total.down=%f, total.right=%f, total.left=%f",
				  agentBrain[s[0].y][s[0].x].total.up, agentBrain[s[0].y][s[0].x].total.down,
				  agentBrain[s[0].y][s[0].x].total.right, agentBrain[s[0].y][s[0].x].total.left);
		  }
		  if (fVerb > 1) {
			  if (nAhead >= 2) {
				  fprintf (agentF,"\ndoLearning: s[1]=[x=%d,y=%d], a[1]=%s", s[1].x, s[1].y, actNames[a[1].val]);
				  if (nAhead >= 3) {
					  fprintf (agentF,"\ndoLearning: s[2]=[x=%d,y=%d], a[2]=%s", s[2].x, s[2].y, actNames[a[2].val]);
					  if (nAhead >= 4) {
						  fprintf (agentF,"\ndoLearning: s[3]=[x=%d,y=%d], a[3]=%s", s[3].x, s[3].y, actNames[a[3].val]);
						  if (nAhead >= 5) {
							  fprintf (agentF,"\ndoLearning: s[4]=[x=%d,y=%d], a[4]=%s", s[4].x, s[4].y, actNames[a[4].val]);
							  if (nAhead >= 6) {
								  fprintf (agentF,"\ndoLearning: s[5]=[x=%d,y=%d], a[5]=%s", s[5].x, s[5].y, actNames[a[5].val]);
							  }
						  }
					  }
				  }
			  }
		  }

		  lrnPath[0].step++;    
		  agentBrain[s[0].y][s[0].x].nLrnVsted.right++;
		  s[0].x++;  

		  if (s[0].x < limit.GetXCoor())
		  {
			agentBrain[s[0].y][s[0].x].isLrnVsted = TRUE_t;
		  }

		  if (lrnPath[0].step < maxSteps)
		  {
			  lrnPath[lrnPath[0].step].loc.x = s[0].x;
			  lrnPath[lrnPath[0].step].loc.y = s[0].y;
			  lrnPath[lrnPath[0].step].dir = a[0];
		  }
		  else
		  {
			  if (!Validator::IsNull(log, NAME("log")))
			  {
				  log->Log(&typeid(this), "ERROR: Number of step exceeds max number of steps");
			  }
		  }
		  break;

  case LEFT:
	  if (isLocMatch(s[0].x-1, s[0].y, goalLoc) == TRUE_t) {
			  agentBrain[s[0].y][s[0].x].imRwrd.left  = 1 * RWRD_FACTOR;
			  done = TRUE_t;
		  }
		  else {
			  if (nAhead >= 2) {
				  s[1].x--;
			  }
			  agentBrain[s[0].y][s[0].x].imRwrd.left  = 0;
			  done = FALSE_t;
		  }
		  if (nAhead >= 2) {
			  a[1] = takeActionOnBoltz(agentBrain, &s[1], TRUE_t, a[0], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
			  if (nAhead >= 3) {
				  s[2] = makeHypoStep (s[1], a[1], agentBrain, xSize, ySize, goalLoc);
				  a[2] = takeActionOnBoltz(agentBrain, &s[2], TRUE_t, a[1], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
				  if (nAhead >= 4) {
					  s[3] = makeHypoStep (s[2], a[2], agentBrain, xSize, ySize, goalLoc);
					  a[3] = takeActionOnBoltz(agentBrain, &s[3], TRUE_t, a[2], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
					  if (nAhead >= 5) {
						  s[4] = makeHypoStep (s[3], a[3], agentBrain, xSize, ySize, goalLoc);
						  a[4] = takeActionOnBoltz(agentBrain, &s[4], TRUE_t, a[3], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
						  if (nAhead >= 6) {
							  s[5] = makeHypoStep (s[4], a[4], agentBrain, xSize, ySize, goalLoc);
							  a[5] = takeActionOnBoltz(agentBrain, &s[5], TRUE_t, a[4], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
						  }
					  }
				  }
			  }
		  }
		  /*if (nAhead >= 2) {
			  a[1] = takeActionOnBoltz(agentBrain, &s[1], FALSE_t, a[0], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
			  if (nAhead >= 3) {
				  s[2] = makeHypoStep (s[1], a[1], agentBrain, xSize, ySize, goalLoc);
				  a[2] = takeActionOnBoltz(agentBrain, &s[2], FALSE_t, a[1], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
				  if (nAhead >= 4) {
					  s[3] = makeHypoStep (s[2], a[2], agentBrain, xSize, ySize, goalLoc);
					  a[3] = takeActionOnBoltz(agentBrain, &s[3], FALSE_t, a[2], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
					  if (nAhead >= 5) {
						  s[4] = makeHypoStep (s[3], a[3], agentBrain, xSize, ySize, goalLoc);
						  a[4] = takeActionOnBoltz(agentBrain, &s[4], FALSE_t, a[3], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
						  if (nAhead >= 6) {
							  s[5] = makeHypoStep (s[4], a[4], agentBrain, xSize, ySize, goalLoc);
							  a[5] = takeActionOnBoltz(agentBrain, &s[5], FALSE_t, a[4], GREEDY_TAW, xSize, ySize, lrnMethod, w1, w2, w3, fVerb, agentF);
						  }
					  }
				  }
			  }
		  }*/

		  if (s[0].x - 1 > 0)
		  {
			agentBrain[s[0].y][s[0].x].dsRwrd.left  = getQs0a0(alpha, gamma, lambda, agentBrain, s, a, nAhead, gLrnPathNum, gMaxLrnPaths, fVerb, agentF, rqRwrdF);
		  }
#if 0
		  switch (lrnMethod) {
  case QVAL_ONLY:
	  agentBrain[s[0].y][s[0].x].total.left = 1.0 * agentBrain[s[0].y][s[0].x].dsRwrd.left;
	  break;
  case DISTG_ONLY:
	  agentBrain[s[0].y][s[0].x].total.left = 1.0 / (agentBrain[s[0].y][s[0].x].distG.left + 1);
	  break;
  case QVAL_NVSTED:
	  agentBrain[s[0].y][s[0].x].total.left = w1 * agentBrain[s[0].y][s[0].x].dsRwrd.left;
	  break;
  case DISTG_NVSTED:
	  agentBrain[s[0].y][s[0].x].total.left = w2 / (agentBrain[s[0].y][s[0].x].distG.left + 1);
	  break;
  case QVAL_DISTG:
  case COMBINE_ALL:
	  agentBrain[s[0].y][s[0].x].total.left = (w1 * agentBrain[s[0].y][s[0].x].dsRwrd.left) +
		  (w2 / (agentBrain[s[0].y][s[0].x].distG.left + 1));
	  break;
  default:
	  break;
		  }
#endif
		  if (fVerb > 0) {
			  fprintf (agentF, "\ndoLearning: After: total.up=%f, total.down=%f, total.right=%f, total.left=%f",
				  agentBrain[s[0].y][s[0].x].total.up, agentBrain[s[0].y][s[0].x].total.down,
				  agentBrain[s[0].y][s[0].x].total.right, agentBrain[s[0].y][s[0].x].total.left);
		  }      
		  if (fVerb > 1) {
			  if (nAhead >= 2) {
				  fprintf (agentF,"\ndoLearning: s[1]=[x=%d,y=%d], a[1]=%s", s[1].x, s[1].y, actNames[a[1].val]);
				  if (nAhead >= 3) {
					  fprintf (agentF,"\ndoLearning: s[2]=[x=%d,y=%d], a[2]=%s", s[2].x, s[2].y, actNames[a[2].val]);
					  if (nAhead >= 4) {
						  fprintf (agentF,"\ndoLearning: s[3]=[x=%d,y=%d], a[3]=%s", s[3].x, s[3].y, actNames[a[3].val]);
						  if (nAhead >= 5) {
							  fprintf (agentF,"\ndoLearning: s[4]=[x=%d,y=%d], a[4]=%s", s[4].x, s[4].y, actNames[a[4].val]);
							  if (nAhead >= 6) {
								  fprintf (agentF,"\ndoLearning: s[5]=[x=%d,y=%d], a[5]=%s", s[5].x, s[5].y, actNames[a[5].val]);
							  }
						  }
					  }
				  }
			  }
		  }

		  lrnPath[0].step++;
		  agentBrain[s[0].y][s[0].x].nLrnVsted.left++;
		  s[0].x--; 

		  if (s[0].x > 0)
		  {
			agentBrain[s[0].y][s[0].x].isLrnVsted = TRUE_t;
		  }

		  if (lrnPath[0].step < maxSteps)
		  {
			  lrnPath[lrnPath[0].step].loc.x = s[0].x;
			  lrnPath[lrnPath[0].step].loc.y = s[0].y;
			  lrnPath[lrnPath[0].step].dir = a[0];
		  }
		  else
		  {
			  if (!Validator::IsNull(log, NAME("log")))
			  {
				log->Log(&typeid(this), "ERROR: Number of step exceeds max number of steps");
			  }
		  }
	  break;
  case NO_DIR:
		  //Do nothing.
		  break;
  default:
	  if (!Validator::IsNull(log, NAME("log")))
	  {
		log->Log(&typeid(this), "Error: doLearning: Invalid direction!");
	  }

	  done = TRUE_t;
	  break;  
		}

		if (fVerb > 0) {  
			fprintf (agentF, "\ndoLearning: Agent Learning Step # %d.", lrnPath[0].step);
			fprintf (agentF, "\ndoLearning: Agent hypomoved %s to [x=%d,y=%d], (%s)\n", 
				actNames[a[0].val], s[0].x, s[0].y, actTypes[a[0].type]);  

			if (!Validator::IsNull(log, NAME("log")))
			{
				log->Log(&typeid(this), "doLearning: Agent Learning Step # %d.", lrnPath[0].step);
				log->Log(&typeid(this), "\ndoLearning: Agent hypomoved %s to [x=%d,y=%d], (%s)\n", 
					actNames[a[0].val], s[0].x, s[0].y, actTypes[a[0].type]);
			}
		}  

		return (done);
	}


	/////////////////////////////////////////////////////////////////////////////////
	// FUNCTION
	// Name: isExploredAll
	// Description:
	/////////////////////////////////////////////////////////////////////////////////
	bool_t 
		MLAgent::isExploredAll (state_t **agentBrain, int xSize, int ySize)
	{
		int i;
		int j;

		for (i = 0; i < ySize; i++) {
			for (j = 0; j < xSize; j++) {
				if (agentBrain[i][j].locSt == BLANK) {
					return (FALSE_t);
				}
			}
		}

		return (TRUE_t);
	}


	/////////////////////////////////////////////////////////////////////////////////
	// FUNCTION
	// Name: takeActionOnBoltz
	// Description: This function lets the agent choose an action (direction) that
	//              leads it to the shortest path. This function is to be called
	//              after the first random path is done and learning has been done
	//              too. Note: The agent does not move to a location it has already
	//              visited.
	/////////////////////////////////////////////////////////////////////////////////
	action_t 
		MLAgent::takeActionOnBoltz (state_t **agentBrain, state_t *s0, bool_t checkPrev, 
		action_t a0Prev, float taw, int xSize, int ySize,
		learnMethod_t lrnMethod, float w1, float w2, float w3, int fVerb,
		FILE *agentF)
	{
		action_t a0;

		if (!Validator::IsNull(s0, NAME("s0")))
		{			
			float sumQ = 0;
			float probUp = 0;
			float probDown = 0;
			float probRight = 0;
			float probLeft = 0;
			float maxPas = 0;
			float upVal = 0;
			float downVal = 0;
			float rightVal = 0;
			float leftVal = 0;

#if 1
			if ((s0->y >= 0) &&
				(s0->y < ySize) &&
				(s0->x >= 0) &&
				(s0->x < xSize))
			{
				switch (lrnMethod) 
				{
				case QVAL_ONLY:
					agentBrain[s0->y][s0->x].total.up    = 1.0 * agentBrain[s0->y][s0->x].dsRwrd.up;
					agentBrain[s0->y][s0->x].total.down  = 1.0 * agentBrain[s0->y][s0->x].dsRwrd.down;
					agentBrain[s0->y][s0->x].total.right = 1.0 * agentBrain[s0->y][s0->x].dsRwrd.right;
					agentBrain[s0->y][s0->x].total.left  = 1.0 * agentBrain[s0->y][s0->x].dsRwrd.left;
					break;
				case DISTG_ONLY:
					agentBrain[s0->y][s0->x].total.up    = 1.0 / (agentBrain[s0->y][s0->x].distG.up + 1);
					agentBrain[s0->y][s0->x].total.down  = 1.0 / (agentBrain[s0->y][s0->x].distG.down + 1);
					agentBrain[s0->y][s0->x].total.right = 1.0 / (agentBrain[s0->y][s0->x].distG.right + 1);
					agentBrain[s0->y][s0->x].total.left  = 1.0 / (agentBrain[s0->y][s0->x].distG.left + 1);
					break;
				case QVAL_NVSTED:
					agentBrain[s0->y][s0->x].total.up    = (w1 * agentBrain[s0->y][s0->x].dsRwrd.up) +
						(w3 / (agentBrain[s0->y][s0->x].nVsted.up + 1));
					agentBrain[s0->y][s0->x].total.down  = (w1 * agentBrain[s0->y][s0->x].dsRwrd.down) +
						(w3 / (agentBrain[s0->y][s0->x].nVsted.down + 1));
					agentBrain[s0->y][s0->x].total.right = (w1 * agentBrain[s0->y][s0->x].dsRwrd.right) +
						(w3 / (agentBrain[s0->y][s0->x].nVsted.right + 1));
					agentBrain[s0->y][s0->x].total.left  = (w1 * agentBrain[s0->y][s0->x].dsRwrd.left) +
						(w3 / (agentBrain[s0->y][s0->x].nVsted.left + 1));
					break;
				case DISTG_NVSTED:
					agentBrain[s0->y][s0->x].total.up    = (w2 / (agentBrain[s0->y][s0->x].distG.up + 1)) +
						(w3 / (agentBrain[s0->y][s0->x].nVsted.up + 1));
					agentBrain[s0->y][s0->x].total.down  = (w2 / (agentBrain[s0->y][s0->x].distG.down + 1)) +
						(w3 / (agentBrain[s0->y][s0->x].nVsted.down + 1));
					agentBrain[s0->y][s0->x].total.right = (w2 / (agentBrain[s0->y][s0->x].distG.right + 1)) +
						(w3 / (agentBrain[s0->y][s0->x].nVsted.right + 1));
					agentBrain[s0->y][s0->x].total.left  = (w2 / (agentBrain[s0->y][s0->x].distG.left + 1)) +
						(w3 / (agentBrain[s0->y][s0->x].nVsted.left + 1));
					break;
				case QVAL_DISTG:
					agentBrain[s0->y][s0->x].total.up    = (w1 * agentBrain[s0->y][s0->x].dsRwrd.up) +
						(w2 / (agentBrain[s0->y][s0->x].distG.up + 1));
					agentBrain[s0->y][s0->x].total.down  = (w1 * agentBrain[s0->y][s0->x].dsRwrd.down) +
						(w2 / (agentBrain[s0->y][s0->x].distG.down + 1));
					agentBrain[s0->y][s0->x].total.right = (w1 * agentBrain[s0->y][s0->x].dsRwrd.right) +
						(w2 / (agentBrain[s0->y][s0->x].distG.right + 1));
					agentBrain[s0->y][s0->x].total.left  = (w1 * agentBrain[s0->y][s0->x].dsRwrd.left) +
						(w2 / (agentBrain[s0->y][s0->x].distG.left + 1));
					break;
				case COMBINE_ALL:
					agentBrain[s0->y][s0->x].total.up    = (w1 * agentBrain[s0->y][s0->x].dsRwrd.up) +
						(w2 / (agentBrain[s0->y][s0->x].distG.up + 1)) +
						(w3 / (agentBrain[s0->y][s0->x].nVsted.up + 1));
					agentBrain[s0->y][s0->x].total.down  = (w1 * agentBrain[s0->y][s0->x].dsRwrd.down) +
						(w2 / (agentBrain[s0->y][s0->x].distG.down + 1)) +
						(w3 / (agentBrain[s0->y][s0->x].nVsted.down + 1));
					agentBrain[s0->y][s0->x].total.right = (w1 * agentBrain[s0->y][s0->x].dsRwrd.right) +
						(w2 / (agentBrain[s0->y][s0->x].distG.right + 1)) +
						(w3 / (agentBrain[s0->y][s0->x].nVsted.right + 1));
					agentBrain[s0->y][s0->x].total.left  = (w1 * agentBrain[s0->y][s0->x].dsRwrd.left) +
						(w2 / (agentBrain[s0->y][s0->x].distG.left + 1)) +
						(w3 / (agentBrain[s0->y][s0->x].nVsted.left + 1));
					break;
				default:
					break;
				}
			}
			else
			{
				log->Log(&typeid(this), "Position out of bounds: y = %d, x = %d", s0->y	, s0->x);
			}
#endif

#if 0
			switch (lrnMethod) {
  case QVAL_NVSTED:
  case DISTG_NVSTED:
  case COMBINE_ALL:
	  agentBrain[s0->y][s0->x].total.up    += w3 / (agentBrain[s0->y][s0->x].nVsted.up + 1);
	  agentBrain[s0->y][s0->x].total.down  += w3 / (agentBrain[s0->y][s0->x].nVsted.down + 1);
	  agentBrain[s0->y][s0->x].total.right += w3 / (agentBrain[s0->y][s0->x].nVsted.right + 1);
	  agentBrain[s0->y][s0->x].total.left  += w3 / (agentBrain[s0->y][s0->x].nVsted.left + 1);
	  break;
  default:
	  break;
			}
#endif


			if ((s0->y >= 0) &&
				(s0->y < ySize) &&
				(s0->x >= 0) &&
				(s0->x < xSize))
			{
				upVal    = agentBrain[s0->y][s0->x].total.up;
				downVal  = agentBrain[s0->y][s0->x].total.down;
				rightVal = agentBrain[s0->y][s0->x].total.right;
				leftVal  = agentBrain[s0->y][s0->x].total.left;  
			}


			// Apply Boltzmann distribution.
			// NOTE: "If taw is high, or if Q-values are all the same, this will pick a random action. 
			//       If taw is low and Q-values are different, it will tend to pick the action with 
			//       the highest Q-value. At the start, Q is assumed to be totally inaccurate, so taw 
			//       is high (high exploration), and actions all have a roughly equal chance of being 
			//       executed. taw decreases as time goes on, and it becomes more and more likely to 
			//       pick among the actions with the higher Q-values, until finally, as we assume Q is 
			//       converging to Q*, taw approaches zero (pure exploitation) and we tend to only pick 
			//       the action with the highest Q-value". 
			//       (Source: http://www.compapp.dcu.ie/~humphrys/PhD/ch2.html).
			sumQ = exp(upVal/taw) + exp(downVal/taw) + exp(rightVal/taw) + exp(leftVal/taw);
			probUp    =  exp(upVal/taw)    / sumQ;
			probDown  =  exp(downVal/taw)  / sumQ;
			probRight =  exp(rightVal/taw) / sumQ;
			probLeft  =  exp(leftVal/taw)  / sumQ;

			if ((s0->y >= 0) &&
				(s0->y < ySize) &&
				(s0->x >= 0) &&
				(s0->x < xSize))
			{
				if (fVerb > 1) {
					fprintf (agentF, "\ntakeActionOnBoltz: Exploration Parameter taw = %f", taw);
					fprintf (agentF, "\ntakeAction: s0=[x=%d,y=%d], total.up=%f, total.down=%f, total.right=%f, total.left=%f",
						s0->x, s0->y, agentBrain[s0->y][s0->x].total.up, agentBrain[s0->y][s0->x].total.down,
						agentBrain[s0->y][s0->x].total.right, agentBrain[s0->y][s0->x].total.left);
					fprintf (agentF, "\ntakeActionOnBoltz: SumQ=%f, Probs: up=%f, down=%f, right=%f, left=%f", 
						sumQ, probUp, probDown, probRight, probLeft);
				}
			}

			//if (probUp == probDown == probRight == probLeft) {
			if ((taw != GREEDY_TAW) &&
				((ABS(probUp - probDown) <= VERY_SMALL_VAL) && 
				(ABS(probDown - probRight) <= VERY_SMALL_VAL) &&
				(ABS(probRight - probLeft) <= VERY_SMALL_VAL) && 
				(ABS(probLeft - probUp) <= VERY_SMALL_VAL))) {
					a0 = getRandomDirection(agentBrain, s0, CHECK_BOTH, xSize, ySize);
					a0.type = ANY_RAND;
			}
			else {
				// Get the maximum P(a,s) probability.
				maxPas = MAX(probUp, MAX(probDown, MAX(probRight, probLeft)));

				// If checkPrev is TRUE, avoid going back to the previous state.
				// NOTE: If going back to the previous state (by taking the opposite
				// of the previous action a[0]) gives the maximum reward, a[1] will 
				// be the opposite of a[0].
				if (checkPrev == TRUE_t) {
					if (fVerb > 2) {
						fprintf (agentF, "\ntakeActionOnBoltz: Previous act = %s", a0Prev.name);
					}
					if (a0Prev.val == UP) {
						maxPas = MAX(probUp, MAX(probRight, probLeft));
					}
					else if (a0Prev.val == DOWN) {
						maxPas = MAX(probDown, MAX(probRight, probLeft));
					}
					else if (a0Prev.val == RIGHT) {
						maxPas = MAX(probRight, MAX(probUp, probDown));
						//maxPas = MAX(probUp, MAX(probRight, probDown));
					}
					else if (a0Prev.val == LEFT) {
						maxPas = MAX(probLeft, MAX(probUp, probDown));
						//maxPas = MAX(probUp, MAX(probDown, probLeft));
					}
				}

				// Move in the direction that has the maximum probability.
				if ((maxPas == probUp)&&(s0->y > 0)) {
					a0.val = UP;
					a0.type = POLICY;
				}
				else if ((maxPas == probDown)&&(s0->y < ySize)) { 
					a0.val = DOWN;
					a0.type = POLICY;
				}
				else if ((maxPas == probRight)&&(s0->x < xSize)) {
					a0.val = RIGHT;
					a0.type = POLICY;
				}
				else if ((maxPas == probLeft)&&(s0->x > 0)) {
					a0.val = LEFT;
					a0.type = POLICY;
				}
				else {
					if (a0Prev.val == NO_DIR)
					{
						a0 = getRandomDirection(agentBrain, s0, CHECK_BOTH, xSize, ySize);
						a0.type = ANY_RAND;
					}
					else
					{
						a0.val = NO_DIR;
						a0.type = POLICY;

#ifdef DebugMLAgent
						if (!Validator::IsNull(log, NAME("log")))
						{
							log->Log(&typeid(this), "Note: takeActiononBoltz: Agent is stuck!");
							//fprintf (stderr, "\nNote: takeActiononBoltz: Agent is stuck!");
						}
#endif
					}
				}

				// Store action name and policy.
				strcpy (a0.name, actNames[a0.val]);			 
			}

			if (fVerb > 1) {
				fprintf (agentF, "\ntakeActionOnBoltz: Action: %s (%s)", a0.name, actTypes[a0.type]);
			}
		}
		else
		{
			a0.val = NO_DIR;
			a0.type = RAND;
			strcpy (a0.name, actNames[a0.val]);	
		}

		// Return action.
		return (a0);
	}

	/////////////////////////////////////////////////////////////////////////////////
	// FUNCTION
	// Name: takePhsActionVstedDist
	// Description: This function lets the agent choose an action: The closest to
	//              the goal among the least visited.
	/////////////////////////////////////////////////////////////////////////////////
	action_t 
		MLAgent::takePhsActionVstedDist (state_t **agentBrain, state_t *s0, int xSize, int ySize,
		state_t *goalLoc, learnMethod_t lrnMethod, int fVerb, FILE *agentF)
	{
		action_t a0; 
		float maxDistG;  
		float probUp;
		float probDown;
		float probRight;
		float probLeft;
		float maxPas;
		int totalNVstedUp;
		int totalNVstedDown;
		int totalNVstedRight;
		int totalNVstedLeft;
		int minTotalNVsted;
		float resActs[NUM_DIR_VALS];

		maxDistG = getDistAB (0, 0, xSize-1, ySize-1);

#if 1
		// If all possible states are either visited, barriers, or nomores, 
		// set current state to nomore.
		if (((s0->y > 0) && (agentBrain[s0->y-1][s0->x].isVsted != FALSE_t)) &&
			((s0->y < ySize-1) && (agentBrain[s0->y+1][s0->x].isVsted != FALSE_t)) &&
			((s0->x < xSize-1) && (agentBrain[s0->y][s0->x+1].isVsted != FALSE_t)) &&
			((s0->x > 0) && (agentBrain[s0->y][s0->x-1].isVsted != FALSE_t))
			) {     
				agentBrain[s0->y][s0->x].isVsted = NOMORE;
				//fprintf (agentF, "\ntakePhsAction: x=%d,y=%d. NOMORE.", s0->x, s0->y);      
		}
#endif

		// Set distances as moving....
		// Up.
		if ((s0->y > 0) && (agentBrain[s0->y-1][s0->x].locSt != BARRIER)) {
			agentBrain[s0->y][s0->x].distG.up = getDistAB (s0->x, s0->y-1, goalLoc->x, goalLoc->y);
		}  
		else {
			agentBrain[s0->y][s0->x].distG.up = maxDistG + 10.0;    
		}
		// Down.
		if ((s0->y < ySize-1) && (agentBrain[s0->y+1][s0->x].locSt != BARRIER)) {
			agentBrain[s0->y][s0->x].distG.down = getDistAB (s0->x, s0->y+1, goalLoc->x, goalLoc->y);
		}
		else {
			agentBrain[s0->y][s0->x].distG.down = maxDistG + 10.0;    
		}
		// Right.
		if ((s0->x < xSize-1) && (agentBrain[s0->y][s0->x+1].locSt != BARRIER)) {
			agentBrain[s0->y][s0->x].distG.right = getDistAB (s0->x+1, s0->y, goalLoc->x, goalLoc->y);
		}
		else {
			agentBrain[s0->y][s0->x].distG.right = maxDistG + 10.0;    
		}
		// Left
		if ((s0->x > 0) && (agentBrain[s0->y][s0->x-1].locSt != BARRIER)) {
			agentBrain[s0->y][s0->x].distG.left = getDistAB (s0->x-1, s0->y, goalLoc->x, goalLoc->y);
		}
		else {
			agentBrain[s0->y][s0->x].distG.left = maxDistG + 10.0;     
		}

		if (lrnMethod == PHS_DISTG_ONLY) {       
			agentBrain[s0->y][s0->x].total.up    = 1 / (agentBrain[s0->y][s0->x].distG.up + 1);
			agentBrain[s0->y][s0->x].total.down  = 1 / (agentBrain[s0->y][s0->x].distG.down + 1);
			agentBrain[s0->y][s0->x].total.right = 1 / (agentBrain[s0->y][s0->x].distG.right + 1);
			agentBrain[s0->y][s0->x].total.left  = 1 / (agentBrain[s0->y][s0->x].distG.left + 1);    
		}

		else if ((lrnMethod == PHS_DISTG_NVSTED) || 
			(lrnMethod == PHS_DISTG_NVSTED_CLO) || 
			(lrnMethod == PHS_DISTG_NVSTED_LP)) {
				// Up.
				if ((s0->y > 0) && (agentBrain[s0->y-1][s0->x].isVsted == NOMORE)) {
					totalNVstedUp = MAX_AGENT_STEPS(xSize,ySize) + 5;
				}
				else if ((s0->y > 0) && (agentBrain[s0->y-1][s0->x].locSt != BARRIER)) {
					totalNVstedUp = agentBrain[s0->y-1][s0->x].totalNVsted;
				}    
				else {
					totalNVstedUp = MAX_AGENT_STEPS(xSize,ySize) + 10; 
				}
				// Down.
				if ((s0->y < ySize-1) && (agentBrain[s0->y+1][s0->x].isVsted == NOMORE)) {
					totalNVstedDown = MAX_AGENT_STEPS(xSize,ySize) + 5;
				}
				else if ((s0->y < ySize-1) && (agentBrain[s0->y+1][s0->x].locSt != BARRIER)) {
					totalNVstedDown = agentBrain[s0->y+1][s0->x].totalNVsted;
				}    
				else {    
					totalNVstedDown = MAX_AGENT_STEPS(xSize,ySize) + 10;
				}
				// Right.    
				if ((s0->x < xSize-1) && (agentBrain[s0->y][s0->x+1].isVsted == NOMORE)) {
					totalNVstedRight = MAX_AGENT_STEPS(xSize,ySize) + 5;
				}
				else if ((s0->x < xSize-1) && (agentBrain[s0->y][s0->x+1].locSt != BARRIER)) {
					totalNVstedRight = agentBrain[s0->y][s0->x+1].totalNVsted;
				}
				else {
					totalNVstedRight = MAX_AGENT_STEPS(xSize,ySize) + 10;
				}
				// Left
				if ((s0->x > 0) && (agentBrain[s0->y][s0->x-1].isVsted == NOMORE)) {
					totalNVstedLeft = MAX_AGENT_STEPS(xSize,ySize) + 5;
				}
				else if ((s0->x > 0) && (agentBrain[s0->y][s0->x-1].locSt != BARRIER)) {     
					totalNVstedLeft = agentBrain[s0->y][s0->x-1].totalNVsted;
				}   
				else {      
					totalNVstedLeft = MAX_AGENT_STEPS(xSize,ySize) + 10;
				}

				minTotalNVsted = MIN (totalNVstedUp, 
					MIN (totalNVstedDown, 
					MIN (totalNVstedRight, 
					totalNVstedLeft)));

				if (minTotalNVsted == totalNVstedUp) {
					resActs[UP] = agentBrain[s0->y][s0->x].distG.up;
				}
				else {
					resActs[UP] =  maxDistG + 10.0;
				}
				if (minTotalNVsted == totalNVstedDown) {
					resActs[DOWN] = agentBrain[s0->y][s0->x].distG.down;
				}
				else {
					resActs[DOWN] =  maxDistG + 10.0;
				}
				if (minTotalNVsted == totalNVstedRight) {
					resActs[RIGHT] = agentBrain[s0->y][s0->x].distG.right;
				}
				else {
					resActs[RIGHT] =  maxDistG + 10.0;
				}
				if (minTotalNVsted == totalNVstedLeft) {
					resActs[LEFT] = agentBrain[s0->y][s0->x].distG.left;
				}
				else {
					resActs[LEFT] =  maxDistG + 10.0;
				}

				agentBrain[s0->y][s0->x].total.up    = 1 / (resActs[UP] + 1);
				agentBrain[s0->y][s0->x].total.down  = 1 / (resActs[DOWN] + 1);
				agentBrain[s0->y][s0->x].total.right = 1 / (resActs[RIGHT] + 1);
				agentBrain[s0->y][s0->x].total.left  = 1 / (resActs[LEFT] + 1);       
		}

		probUp    = agentBrain[s0->y][s0->x].total.up;
		probDown  = agentBrain[s0->y][s0->x].total.down;
		probRight = agentBrain[s0->y][s0->x].total.right;
		probLeft  = agentBrain[s0->y][s0->x].total.left;

		// Get the maximum P(a,s) probability.
		maxPas = MAX(probUp, MAX(probDown, MAX(probRight, probLeft)));  

		if (fVerb > 1) {
			fprintf (agentF, "\ntakePhsAction: Probs: up=%f, down=%f, right=%f, left=%f", 
				probUp, probDown, probRight, probLeft);
		}

		// Move in the direction that has the maximum probability.
		if (maxPas == probUp) {
			a0.val = UP;
		}
		else if (maxPas == probDown) { 
			a0.val = DOWN;
		}
		else if (maxPas == probRight) {
			a0.val = RIGHT;
		}
		else if (maxPas == probLeft) {
			a0.val = LEFT;
		}
		else {
			a0.val = NO_DIR;
			if (!Validator::IsNull(log, NAME("log")))
			{
				log->Log(&typeid(this), "Note: takePhsAction: Agent is stuck!");
			}
		}

		strcpy (a0.name, actNames[a0.val]);
		a0.type = POLICY; 

		if (fVerb > 1) {
			fprintf (agentF, "\ntakePhsAction: Action: %s (%s)", a0.name, actTypes[a0.type]);
		}

		// Return action.
		return (a0);
	}






	/////////////////////////////////////////////////////////////////////////////////
	// FUNCTION
	// Name: takePhsActionDistVsted
	// Description: This function lets the agent choose an action: The least visited
	//              among the closest to the goal.
	/////////////////////////////////////////////////////////////////////////////////
	action_t 
		MLAgent::takePhsActionDistVsted (state_t **agentBrain, state_t *s0, int xSize, int ySize,
		state_t *goalLoc, learnMethod_t lrnMethod, int fVerb, FILE *agentF)
	{
		action_t a0; 
		float maxDistG;  
		float probUp;
		float probDown;
		float probRight;
		float probLeft;
		float maxPas;
		int totalNVstedUp;
		int totalNVstedDown;
		int totalNVstedRight;
		int totalNVstedLeft;
		float minTotalDistG;
		float resActs[NUM_DIR_VALS];

		maxDistG = getDistAB (0, 0, xSize-1, ySize-1);

#if 1
		// In this case, a visited state is set to nomore.
		if (agentBrain[s0->y][s0->x].isVsted != FALSE_t) {     
			agentBrain[s0->y][s0->x].isVsted = NOMORE;
		}
#endif

		// Up.
		if ((s0->y > 0) && (agentBrain[s0->y-1][s0->x].locSt != BARRIER)) {
			totalNVstedUp = agentBrain[s0->y-1][s0->x].totalNVsted;
		}
		else {
			totalNVstedUp = MAX_AGENT_STEPS(xSize,ySize) + 10; 
		}
		// Down.
		if ((s0->y < ySize-1) && (agentBrain[s0->y+1][s0->x].locSt != BARRIER)) {
			totalNVstedDown = agentBrain[s0->y+1][s0->x].totalNVsted;
		}
		else {    
			totalNVstedDown = MAX_AGENT_STEPS(xSize,ySize) + 10;
		}
		// Right.
		if ((s0->x < xSize-1) && (agentBrain[s0->y][s0->x+1].locSt != BARRIER)) {
			totalNVstedRight = agentBrain[s0->y][s0->x+1].totalNVsted;
		}
		else {
			totalNVstedRight = MAX_AGENT_STEPS(xSize,ySize) + 10;
		}
		// Left
		if ((s0->x > 0) && (agentBrain[s0->y][s0->x-1].locSt != BARRIER)) {     
			totalNVstedLeft = agentBrain[s0->y][s0->x-1].totalNVsted;
		}
		else {      
			totalNVstedLeft = MAX_AGENT_STEPS(xSize,ySize) + 10;
		}

		if (lrnMethod == PHS_DISTG_ONLY) {       
			agentBrain[s0->y][s0->x].total.up    = 1 / (totalNVstedUp + 1);
			agentBrain[s0->y][s0->x].total.down  = 1 / (totalNVstedDown + 1);
			agentBrain[s0->y][s0->x].total.right = 1 / (totalNVstedRight + 1);
			agentBrain[s0->y][s0->x].total.left  = 1 / (totalNVstedLeft + 1); 
		}

		else if ((lrnMethod == PHS_DISTG_NVSTED) || 
			(lrnMethod == PHS_DISTG_NVSTED_CLO) || 
			(lrnMethod == PHS_DISTG_NVSTED_LP)) {
				// Set distances as moving....
				// Up.
				if ((s0->y > 0) && (agentBrain[s0->y-1][s0->x].isVsted == NOMORE)) {
					agentBrain[s0->y][s0->x].distG.up = maxDistG + 5.0;
				}  
				else if ((s0->y > 0) && (agentBrain[s0->y-1][s0->x].locSt != BARRIER)) {
					agentBrain[s0->y][s0->x].distG.up = getDistAB (s0->x, s0->y-1, goalLoc->x, goalLoc->y);
				}     
				else {
					agentBrain[s0->y][s0->x].distG.up = maxDistG + 10.0;    
				}
				// Down.
				if ((s0->y < ySize-1) && (agentBrain[s0->y+1][s0->x].isVsted == NOMORE)) {
					agentBrain[s0->y][s0->x].distG.down = maxDistG + 5.0;
				}
				else if ((s0->y < ySize-1) && (agentBrain[s0->y+1][s0->x].locSt != BARRIER)) {
					agentBrain[s0->y][s0->x].distG.down = getDistAB (s0->x, s0->y+1, goalLoc->x, goalLoc->y);
				}     
				else {
					agentBrain[s0->y][s0->x].distG.down = maxDistG + 10.0; 
				}
				// Right.
				if ((s0->x < xSize-1) && (agentBrain[s0->y][s0->x+1].isVsted == NOMORE)) {
					agentBrain[s0->y][s0->x].distG.right = maxDistG + 5.0;
				}
				else if ((s0->x < xSize-1) && (agentBrain[s0->y][s0->x+1].locSt != BARRIER)) {
					agentBrain[s0->y][s0->x].distG.right = getDistAB (s0->x+1, s0->y, goalLoc->x, goalLoc->y);
				}    
				else {
					agentBrain[s0->y][s0->x].distG.right = maxDistG + 10.0;    
				}
				// Left
				if ((s0->x > 0) && (agentBrain[s0->y][s0->x-1].isVsted == NOMORE)) {
					agentBrain[s0->y][s0->x].distG.left = maxDistG + 5.0;
				}
				else if ((s0->x > 0) && (agentBrain[s0->y][s0->x-1].locSt != BARRIER)) {
					agentBrain[s0->y][s0->x].distG.left = getDistAB (s0->x-1, s0->y, goalLoc->x, goalLoc->y);
				}   
				else {
					agentBrain[s0->y][s0->x].distG.left = maxDistG + 10.0;     
				}

				minTotalDistG = MIN (agentBrain[s0->y][s0->x].distG.up, 
					MIN (agentBrain[s0->y][s0->x].distG.down, 
					MIN (agentBrain[s0->y][s0->x].distG.right, 
					agentBrain[s0->y][s0->x].distG.left)));

				if (minTotalDistG == agentBrain[s0->y][s0->x].distG.up) {
					resActs[UP] = totalNVstedUp;
				}
				else {
					resActs[UP] =  MAX_AGENT_STEPS(xSize,ySize) + 10;
				}
				if (minTotalDistG == agentBrain[s0->y][s0->x].distG.down) {
					resActs[DOWN] = totalNVstedDown;
				}
				else {
					resActs[DOWN] =  MAX_AGENT_STEPS(xSize,ySize) + 10;
				}
				if (minTotalDistG == agentBrain[s0->y][s0->x].distG.right) {
					resActs[RIGHT] = totalNVstedRight;
				}
				else {
					resActs[RIGHT] =  MAX_AGENT_STEPS(xSize,ySize) + 10;
				}
				if (minTotalDistG == agentBrain[s0->y][s0->x].distG.left) {
					resActs[LEFT] = totalNVstedLeft;
				}
				else {
					resActs[LEFT] =  MAX_AGENT_STEPS(xSize,ySize) + 10;
				}

				agentBrain[s0->y][s0->x].total.up    = 1 / (resActs[UP] + 1);
				agentBrain[s0->y][s0->x].total.down  = 1 / (resActs[DOWN] + 1);
				agentBrain[s0->y][s0->x].total.right = 1 / (resActs[RIGHT] + 1);
				agentBrain[s0->y][s0->x].total.left  = 1 / (resActs[LEFT] + 1);       
		}

		probUp    = agentBrain[s0->y][s0->x].total.up;
		probDown  = agentBrain[s0->y][s0->x].total.down;
		probRight = agentBrain[s0->y][s0->x].total.right;
		probLeft  = agentBrain[s0->y][s0->x].total.left;

		// Get the maximum P(a,s) probability.
		maxPas = MAX(probUp, MAX(probDown, MAX(probRight, probLeft)));  

		if (fVerb > 1) {
			fprintf (agentF, "\ntakePhsAction2: Probs: up=%f, down=%f, right=%f, left=%f", 
				probUp, probDown, probRight, probLeft);
		}

		// Move in the direction that has the maximum probability.
		if (maxPas == probUp) {
			a0.val = UP;
		}
		else if (maxPas == probDown) { 
			a0.val = DOWN;
		}
		else if (maxPas == probRight) {
			a0.val = RIGHT;
		}
		else if (maxPas == probLeft) {
			a0.val = LEFT;
		}
		else {
			a0.val = NO_DIR;
			if (!Validator::IsNull(log, NAME("log")))
			{
				log->Log(&typeid(this), "Note: takePhsAction2: Agent is stuck!");
			}
		}

		strcpy (a0.name, actNames[a0.val]);
		a0.type = POLICY; 

		if (fVerb > 1) {
			fprintf (agentF, "\ntakePhsAction2: x=%d,y=%d. Action: %s (%s)", s0->x, s0->y,
				a0.name, actTypes[a0.type]);
		}

		// Return action.
		return (a0);
	}



	/////////////////////////////////////////////////////////////////////////////////
	// FUNCTION
	// Name: takeActionFromList
	// Description: This function lets the agent choose an action....
	////////////////////////////////////////////////////////////////////////////////
	action_t 
		MLAgent::takeActionFromList (state_t **agentBrain, state_t *s0, int fVerb, FILE *agentF)
	{
		action_t a0; 

		if (agentBrain[s0->y][s0->x].toY == s0->y - 1) {
			a0.val = UP;    
		}
		else if (agentBrain[s0->y][s0->x].toY == s0->y + 1) {
			a0.val = DOWN;
		}
		else if (agentBrain[s0->y][s0->x].toX == s0->x + 1) {
			a0.val = RIGHT;
		}
		else if (agentBrain[s0->y][s0->x].toX == s0->x - 1) {
			a0.val = LEFT;
		}
		else {
			a0.val = NO_DIR;
			if (!Validator::IsNull(log, NAME("log")))
			{
				log->Log(&typeid(this), "Note: takeActionFromList: Agent is stuck at [x=%d, y=%d]!", s0->x, s0->y);
			}
		}

		strcpy (a0.name, actNames[a0.val]);
		a0.type = POLICY; 

		if (fVerb > 1) {
			fprintf (agentF, "\ntakeActionFromList: Action: %s (%s)", a0.name, actTypes[a0.type]);
		}

		// Return action.
		return (a0);
	}


	/////////////////////////////////////////////////////////////////////////////////
	// FUNCTION
	// Name: getClosestToStart
	// Description: This function....
	/////////////////////////////////////////////////////////////////////////////////
	point_t 
		MLAgent::getClosestToStart (state_t **agentBrain, state_t s0, int xSize, int ySize, 
		state_t *startLoc)
	{
		point_t pt;
		float maxDistS;  
		float minDistS;
		float distSUp;
		float distSDown;
		float distSRight;
		float distSLeft;

		maxDistS = getDistAB (0, 0, xSize-1, ySize-1);
		pt.x = -1;
		pt.y = -1;

		// Up.
		if ((s0.y > 0) && (agentBrain[s0.y-1][s0.x].totalNVsted > 0)) {
			distSUp = getDistAB (s0.x, s0.y-1, startLoc->x, startLoc->y);
		}
		else {
			distSUp = maxDistS + 5.0;
		}   
		// Down.
		if ((s0.y < ySize-1) && (agentBrain[s0.y+1][s0.x].totalNVsted > 0)) {
			distSDown = getDistAB (s0.x, s0.y+1, startLoc->x, startLoc->y);
		}
		else {
			distSDown = maxDistS + 5.0;
		}    
		// Right.
		if ((s0.x < xSize-1) && (agentBrain[s0.y][s0.x+1].totalNVsted > 0)) {
			distSRight = getDistAB (s0.x+1, s0.y, startLoc->x, startLoc->y);
		}
		else {
			distSRight = maxDistS + 5.0;
		}    
		// Left.
		if ((s0.x > 0) && (agentBrain[s0.y][s0.x-1].totalNVsted > 0)) {
			distSLeft = getDistAB (s0.x-1, s0.y, startLoc->x, startLoc->y);
		}
		else {
			distSLeft = maxDistS + 5.0;
		}

		minDistS = MIN (distSUp, 
			MIN (distSDown,
			MIN (distSRight, distSLeft)));

		if (minDistS == distSUp) {
			pt.x = s0.x;
			pt.y = s0.y-1;
		}
		else if (minDistS == distSDown) {
			pt.x = s0.x; 
			pt.y = s0.y+1;
		}
		else if (minDistS == distSRight) {
			pt.x = s0.x+1;
			pt.y = s0.y;
		}
		else if (minDistS == distSLeft) {
			pt.x = s0.x-1;
			pt.y = s0.y;
		}

		// Don't apply it for the starting state to avoid infinite paths.
		if ((s0.x == startLoc->x) && (s0.y == startLoc->y)) {
			pt.x = -1;
			pt.y = -1;
		}

		//fprintf (stdout, "\nx=%d,y=%d  pt.x=%d,pt.y=%d", s0.x,s0.y, pt.x,pt.y);

		return (pt);  

	}


	/////////////////////////////////////////////////////////////////////////////////
	// FUNCTION
	// Name: makeHypoStep
	// Description:
	//              
	/////////////////////////////////////////////////////////////////////////////////
	state_t 
		MLAgent::makeHypoStep (state_t s, action_t a, state_t **agentBrain, 
		int xSize, int ySize, state_t goalLoc)
	{
		// Initialize new hypo state.
		state_t newSta = s;

		switch (a.val) {
  case UP:
	  newSta.y = (isLocMatch(s.x, s.y-1, goalLoc) == FALSE_t) ?
		  s.y-1 : s.y;
	  break;
  case DOWN:
	  newSta.y = (isLocMatch(s.x, s.y+1, goalLoc) == FALSE_t) ?
		  s.y+1 : s.y;
	  break;
  case RIGHT:
	  newSta.x = (isLocMatch(s.x+1, s.y, goalLoc) == FALSE_t) ?
		  s.x+1 : s.x;
	  break;
  case LEFT:
	  newSta.x = (isLocMatch(s.x-1, s.y, goalLoc) == FALSE_t) ?
		  s.x-1 : s.x;
	  break;
  default:
	  if (!Validator::IsNull(log, NAME("log")))
	  {
		log->Log(&typeid(this), "Error: makeHypoStep: Invalid direction!");
	  }
	  break;
		}

		return (newSta);
	}


	/////////////////////////////////////////////////////////////////////////////////
	// FUNCTION
	// Name: PrintHelp
	// Description: This function displays the help information that shows the usage 
	//              of the executable.
	/////////////////////////////////////////////////////////////////////////////////
	void 
		MLAgent::printHelp (char *execname, const char *message) 
	{
		printf ("%s", message);

		printf ("\nUsage: %s <options>", execname);
		printf ("\nOptions:");
		printf ("\n-i <file>       Filename of input maze matrix");
		printf ("\n-x <integer>    Maze X size (number of columns in maze matrix)");
		printf ("\n-y <integer>    Maze Y size (number of rows in maze matrix)");
		printf ("\n-o <file>       Filename of agent output log");
		printf ("\n-p <integer>    Number of physical paths");
		printf ("\n-l <integer>    Number of learning paths per physical path");
		printf ("\n[-m <integer>]  Method (default=0)");
		printf ("\n                  0: Learning with Q-value only");
		printf ("\n                  1: Learning with Distance-to-Goal Only");
		printf ("\n                  2: Learning with Q-value and Frequency (need w1 & w3)");
		printf ("\n                  3: Learning with Distance and Frequency (w2 & w3)");
		printf ("\n                  4: Learning with Q-value and Distance (need w1 & w3)");
		printf ("\n                  5: Learning with Q-value, Distance, and Frequency (need w1, w2, w3)");
		printf ("\n                 10: Physical with Distance only");
		printf ("\n                 11: Physical with Distance and Frequency");
		printf ("\n                 12: Physical with Distance and Frequency using Closed-Loop Omitting");
		printf ("\n                 13: Physical with Distance and Frequency using Loop Pinching");
		printf ("\n[-n <integer>]  Number of lookahead levels N (default=1)");
		printf ("\n[-c <file>]     File where controlled first-path actions are");
		printf ("\n[-f <integer>]  Path-file mode (default=0)");
		printf ("\n                  0: No path files");
		printf ("\n                  1: Physical path files only");
		printf ("\n                  2: Learning path files only (including the lrn num and R/Q rwrd files)");
		printf ("\n                  3: Physical and learning path files (including the lrn num and R/Q rwrd files)");
		printf ("\n[-v <integer>]  Level of verbose in agent output log file");
		printf ("\n[-e]            Initial full-exploration flag");
		printf ("\n[-g]            Physical-Greedy flag");
		printf ("\n[-r]            Learning-Greedy flag");
		printf ("\n[-A <float>]    Step-size parameter ALPHA (default=%.3f)", D_ALPHA);
		printf ("\n[-G <float>]    Discount rate GAMMA (default=%.3f)", D_GAMMA);
		printf ("\n[-L <float>]    Lookahead weight factor LAMBDA (default=%.3f)", D_LAMBDA);
		printf ("\n[-T <integer>]  Initial Boltzmann constant TAW (default=%d)", INIT_TAW);
		printf ("\n[-1 <float>]    Q-value weight w1 (default=0.000)");
		printf ("\n[-2 <float>]    Distance-to-Goal weight w2 (default=0.000)");
		printf ("\n[-3 <float>]    Visited-Number weight w3 (default=0.000)");
		printf ("\n[-a]            Print information about %s", execname);
		printf ("\n[-h]            Print this Help Table");
		printf ("\n");
	}
}
