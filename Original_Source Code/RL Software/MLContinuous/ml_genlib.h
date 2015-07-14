
/////////////////////////////////////////////////////////////////////////////////
//
// Date        : 03/21/2004
//
// Filename    : ml_genlib.h
//
// Author      : Hani Al-Dayaa
//
// Topic       : Machine Learning
//
// Description : The header file of ml_genlib.c
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

#ifndef ML_GENLIB_H
#define ML_GENLIB_H


/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// HEADER FILES ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

#include "ml_gencon.h"



/////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// TYPE DEFINITIONS ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

// String type.
typedef char string_t[STRING_LEN];
typedef char lstring_t[(STRING_LEN*2)];
typedef char sstring_t[(STRING_LEN/2)];

/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// STRUCTURES ///////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

// Action type structure.
typedef struct {
  dirVal_t val;       // Action value.
  string_t name;      // Action name.
  actionType_t type;  // Action type.
} action_t;


// Reward type structure.
typedef struct {
  float up;         // UP location reward value.
  float down;       // DOWN location reward value.
  float right;      // RIGHT location reward value.
  float left;       // LEFT location reward value.
  float up_right;	// UP_RIGHT location reward value.
  float up_left;	// UP_LEFT location reward value.
  float down_right;	// DOWN_RIGHT location reward value.
  float down_left;	// DOWN_LEFT location reward value.
} reward_t;


// Possible Locations/States type structure.
typedef struct {
  bool_t up;
  bool_t down;
  bool_t right;
  bool_t left;
  bool_t up_right;
  bool_t up_left;
  bool_t down_right;
  bool_t down_left;
} possible_t;
   

// State type structure.
typedef struct { 
  int x;                  // Horizontal coordinate.
  int y;                  // Vertical coordinate.  
  locState_t locSt;       // Location state.
  possible_t barDetect;   // Barrier detection flag.
  bool_t isVsted;         // Physically-Visited flag.  
  bool_t isLrnVsted;      // Hypo.-Visited flag.  
  reward_t imRwrd;        // Immediate reward.
  reward_t dsRwrd;        // Discounted reward.
  reward_t distG;         // Distance from goal.
  int totalNVsted;        // Total number of times state is physically visited.
  reward_t nVsted;        // Number of times state is phys. visited from each direction.
  int totalNLrnVsted;     // Total number of times state is hypo. visited.
  reward_t nLrnVsted;     // Number of time state is hypo. visited.
  reward_t total;         // Total Reward value (Rwrd, distG, nVsted).
  int fromX;
  int fromY;
  int toX;
  int toY;
  possible_t actTaken;    // Actions already taken from current state.
} state_t;


// Point type stucture.
typedef struct {
  int x;
  int y;
} point_t;


// Agent type structure.
typedef struct {
  state_t loc;       // Current location of agent. 
  action_t dir;      // Direction agent moved to reach current location.
  int iter;          // Agent iteration number.
  int step;          // Agent step number.
  bool_t moved;      // Agent flag whether it moved.
} agent_t;


/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// LIST OF FUNCTIONS ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

//action_t getRandomDirection (state_t **agentBrain, state_t *s0, actCheck_t ac, int xSize, int ySize);
//float getDistAB (int xa, int ya, int xb, int yb);
//dirVal_t getDirectionValue (char firstLetter);
//void readMazeIntoMatrix (char **charMaze, int xSize, int ySize, state_t **maze, char start, char goal);
//bool_t findLocation (int xSize, int ySize, state_t **maze, locState_t locState, 
//		     state_t *location);
//error_t moveAgent (unsigned id, agent_t *agent, action_t direction, int xSize, int ySize, 
//	           state_t **maze, state_t **agentBrain, agent_t *agentPath,
//                   learnMethod_t lrnMethod, state_t *startLoc, FILE *agentF);
//bool_t isGoal (int x, int y, state_t **agentBrain, char goal);
//bool_t isLocMatch (int x, int y, state_t loc);
//void storeDistances (state_t **agentBrain, int xSize, int ySize, state_t *goalLoc, char start);
//float storeRewards (float gamma, agent_t *agentPath, state_t **agentBrain, int numSteps, int fVerb, FILE *agentF);
//float getQs0a0 (float alpha, float gamma, float lambda, state_t **agentBrain,
//                state_t *s, action_t *a, int nAhead, int gLrnPathNum, int gMaxLrnPaths, int fVerb,
//				FILE *agentF, FILE *rqRwrdF);
//bool_t doLearning (float alpha, float gamma, float lambda, state_t **agentBrain, 
//                   state_t *s0, action_t *a0, int xSize, int ySize, learnMethod_t lrnMethod,
//                   int nAhead, state_t startLoc, state_t goalLoc, agent_t *lrnPath,
//                   float w1, float w2, float w3, int gLrnPathNum, int gMaxLrnPaths, int fVerb,
//				   FILE *agentF, FILE *rqRwrdF);
//side_t whichSide (state_t *s0);
//bool_t isExploredAll (state_t **agentBrain, int xSize, int ySize);
//action_t takeActionOnBoltz (state_t **agentBrain, state_t *s0, bool_t checkPrev, action_t a0Prev, 
//               		    float taw, int xSize, int ySize, learnMethod_t lrnMethod, 
//                            float w1, float w2, float w3, int fVerb,
//							FILE *agentF);
//action_t takePhsActionVstedDist (state_t **agentBrain, state_t *s0, int xSize, int ySize,
//                                 state_t *goalLoc, learnMethod_t lrnMethod, int fVerb,
//								 FILE *agentF);
//action_t takePhsActionDistVsted (state_t **agentBrain, state_t *s0, int xSize, int ySize,
//                                 state_t *goalLoc, learnMethod_t lrnMethod, int fVerb, FILE *agentF);
//action_t takeActionFromList (state_t **agentBrain, state_t *s0, int fVerb, FILE *agentF);
//point_t getClosestToStart (state_t **agentBrain, state_t s0, int xSize, int ySize, 
//                           state_t *startLoc);
//state_t makeHypoStep (state_t s, action_t a, state_t **agentBrain, 
//                      int xSize, int ySize, state_t goalLoc);
//void printHelp (char *execname, const char *message);


#endif
