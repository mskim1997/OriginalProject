/////////////////////////////////////////////////////////////////////////////////
//
// Date        : 03/21/2004
// 
// Filename    : ml.c
//
// Author      : Hani Al-Dayaa
//
// Topic       : Reinforcement Learning
//
// Description : This program implements the Machine Learning suite.
//              
// Revision History :
// ----------------------------------------------------------------------------
// <Rev> | <Author>  | <Date>   | <Changes Made>
//       |           |          |
// 1.00  |  H.A.     | 03/21/04 | - Initial Revision.
//       |           |          |
// 2.00  |  M.M.     | 08/07/09 | - Modified to meet new ANSI standards
// ----------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// HEADERS /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>

// Custom Header files.
#include "ml_gencon.h"
#include "ml_genlib.h"
#include "ml.h"
#include "MLAgent.h"
#include "Framework/Position.h"
#include "Framework\Debug.h"
#include "DateTime.h"

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
using namespace Framework::Eventing;

/////////////////////////////////////////////////////////////////////////////////
////////////////////////////// GLOBAL VARIABLES /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

extern string_t actNames[];     // Action names.
extern string_t actTypes[];     // Action Types.

namespace MLTest
{
	/////////////////////////////////////////////////////////////////////////////////
	// FUNCTION
	// Name: applyML
	// Description: This function applies agent navigation in the maze, using
	//              Machine Learning.
	/////////////////////////////////////////////////////////////////////////////////

	bool_t MLAgent::applyML (float alpha, float gamma, float lambda, int taw, int xSize, int ySize, 
		int maxPhsPaths, int maxLrnPaths, learnMethod_t lrnMethod, 
		bool_t cntrlFirstOn,  bool_t initFullExploreOn, bool_t phsGreedyOn, bool_t lrnGreedyOn, 
		bool_t constTawOn, int nAhead, char *agentFile, int pathFileMode, 
		char *cntrlFirstFile, int seedVal, bool_t keepSeed, float w1, float w2, float w3, int gMaxLrnPaths,
		int fVerb, FILE *agentF, char start, char goal, unsigned agentID, state_t **agentBrain)
	{  
		int i;                       // Vertical index.
		int j;                       // Horizontal index.
		int k;                       // Miscellaneous counter.
		bool_t res;                  // Location-Match result.
		error_t err = NO_ERR;        // Error result.
		time_t seed;                 // Random seed value.
		int phsPathNum = 0;              // Physical path number.
		int lrnPathNum = 0;              // Learning path number.
		int totalLrnPathNum = 0;     // Learning path number.
		float phsTaw = INIT_TAW;     // Physical Taw.
		bool_t lrnPathDone;          // Done flag for each learning path.
		agent_t *agent = NULL;              // Agent position.  
		state_t *startLoc = NULL;           // Start location.
		state_t *tempStartLoc = NULL;       // Temp Start location.
		state_t *goalLoc = NULL;            // Goal location.
		state_t *knownGoalLoc = NULL;       // Known Goal location.
		//state_t **maze;              // Maze matrix.
		//state_t **agentBrain;        // Agent matrix.
		agent_t *agentPath = NULL;          // Agent physical path.
		agent_t *lrnPath = NULL;            // Agent learning path.
		int phsStep = 0;             // Physical step.
		action_t *act = NULL;               // Action (Direction).
		state_t *sta = NULL;                // Q-Learning state (location).
		FILE **phsPathFs = NULL;            // Agent physical path files.  
		char **phsPathFnames = NULL;        // Agent physical path filenames.
		FILE **lrnPathFs = NULL;            // Agent learning path files.  
		char **lrnPathFnames = NULL;        // Agent learning path filenames.
		FILE *lrnNumsF = NULL;              // Agent learning numbers file.  
		char *lrnNumsFname = NULL;          // Agent learning numbers filename.
		char *rqRwrdFname = NULL;           // R/Q reward filename.
		FILE *cntrlFirstF = NULL;           // Control-first-path file.
		char *tempStr1 = NULL;
		char *tempStr2 = NULL;
		char *tempStr3 = NULL;
		//char *tempStr4;
		char tempChar;
		int gLrnPathNum = lrnPathNum;
		FILE *rqRwrdF = NULL;
		struct timeval time1, time2;   // Time elements.
		double t1, t2, delta_t;

		Framework::Position<int> limit = this->maze->GetLimits();

		int maxSteps = MAX_AGENT_STEPS(this->maze->GetXSize(), this->maze->GetYSize());


		/*****************************************************
		* Allocate memory for different arrays and pointers. 
		*****************************************************/

		// Allocate memory start and goal locations.
		startLoc     = (state_t *) calloc (1, sizeof(state_t));
		tempStartLoc     = (state_t *) calloc (1, sizeof(state_t));
		goalLoc      = (state_t *) calloc (1, sizeof(state_t));
		knownGoalLoc = (state_t *) calloc (1, sizeof(state_t));
		// Allocate memory for agent path arrays, agent, actions, and states.
		agentPath  = (agent_t *) calloc (maxSteps + 1, sizeof(agent_t));
		lrnPath    = (agent_t *) calloc (maxSteps + 1, sizeof(agent_t));
		agent      = (agent_t *) calloc (1, sizeof(agent_t));
		act        = (action_t *) calloc (nAhead, sizeof(action_t));
		sta        = (state_t *) calloc (nAhead, sizeof(state_t));    

		// Allocate memory for the path files and their names.  
		tempStr1       = (char *) calloc (STRING_LEN*2, sizeof(char *));
		tempStr2       = (char *) calloc (STRING_LEN/2, sizeof(char *));
		tempStr3       = (char *) calloc (STRING_LEN/2, sizeof(char *));
		//tempStr4       = (char *) calloc (STRING_LEN/2, sizeof(char *));
		if ((pathFileMode == BOTH_PATH_FILE) || (pathFileMode == PHS_PATH_FILE)) {
			phsPathFs      = (FILE **) calloc (maxPhsPaths, sizeof(FILE *));  
			phsPathFnames  = (char **) calloc (maxPhsPaths, sizeof(char *));
			for (i = 0; i < maxPhsPaths; i++) {
				phsPathFnames[i] = (char *) calloc (STRING_LEN*2, sizeof(char));
			}
		}
		if ((pathFileMode == BOTH_PATH_FILE) || (pathFileMode == LRN_PATH_FILE)) {
			lrnPathFs      = (FILE **) calloc (maxLrnPaths*(maxPhsPaths-1), sizeof(FILE *));  
			lrnPathFnames  = (char **) calloc (maxLrnPaths*(maxPhsPaths-1), sizeof(char *));
			for (k = 0; k < (maxLrnPaths*(maxPhsPaths-1)); k++) {
				lrnPathFnames[k] = (char *) calloc (STRING_LEN*2, sizeof(char));
			}
		}
		lrnNumsFname = (char *) calloc (STRING_LEN*2, sizeof(char *));
		rqRwrdFname  = (char *) calloc (STRING_LEN*2, sizeof(char *));

		// Generate 16-bit Random Seed.
		// Use the srand() function to seed the random number generator. 
		(keepSeed == TRUE) ? (seed = seedVal) : (seed = time(NULL) % 65536);
		srand ((unsigned int) seed);

		// Print the random seed value, so the user can easily get
		// it and use it in the next run if needed.
		//printf ("\nRandom seed value = %d\n", (unsigned int)seed);

		// Find Start location.
		Position<int> startPos = this->maze->GetPosition(start);
		startLoc->x = startPos.GetXCoor();
		startLoc->y = startPos.GetYCoor();
		startLoc->locSt = static_cast<locState_t>(start);

		/***************************
		* Initial Full Exploation. 
		***************************/
		if (initFullExploreOn == TRUE) {
			for (i = 0; i < ySize; i++) {
				for (j = 0; j < xSize; j++) {
					agentBrain[i][j].locSt = static_cast<locState_t>(this->maze->GetObjectOn(Position<int>(j, i)));//maze[i][j].locSt;
				}
			}
		}


		/***************************************
		* Setup path files and check for them.
		***************************************/

		if ((pathFileMode == NO_PATH_FILE) || (pathFileMode == PHS_PATH_FILE) || 
			(pathFileMode == LRN_PATH_FILE) || (pathFileMode == BOTH_PATH_FILE)) {
				if ((pathFileMode == PHS_PATH_FILE) || (pathFileMode == BOTH_PATH_FILE)) {
					for (i = 0; i < maxPhsPaths; i++) {   
						strcpy (tempStr1, "Out\\.phs_m");
						sprintf(tempStr2, "%d", lrnMethod);
						strcpy(tempStr1, strcat(tempStr1, tempStr2));  
						strcpy(tempStr1, strcat(tempStr1, "_n"));
						sprintf(tempStr2, "%d", nAhead);
						strcpy(tempStr1, strcat(tempStr1, tempStr2));
						strcpy(tempStr1, strcat(tempStr1, "_p"));
						sprintf(tempStr2, "%d", i+1);
						strcpy(tempStr1, strcat(tempStr1, tempStr2));
						strcpy (tempStr2, ".pth");
						strcpy(tempStr1, strcat(tempStr1, tempStr2));
						strcat(tempStr1, ".");

						sprintf(tempStr2, "%c", this->info.start);
						strcat(tempStr1, tempStr2);
						sprintf(tempStr2, "%c", this->info.goal);
						strcat(tempStr1, tempStr2);

						strcpy(phsPathFnames[i], tempStr1);
						if ( (phsPathFs[i] = fopen(phsPathFnames[i], "w")) == NULL ) {
							fprintf (stderr, "\nError: applyML: Cannot open file %s for writing.\n", phsPathFnames[i]);
							exit (EXIT_FAILURE);
						}
					}
				}
				if ((pathFileMode == LRN_PATH_FILE) || (pathFileMode == BOTH_PATH_FILE)) {
					strcpy (tempStr1, "Out\\.lrn_m");
					sprintf(tempStr2, "%d", lrnMethod);
					strcpy(tempStr1, strcat(tempStr1, tempStr2)); 
					strcpy(tempStr1, strcat(tempStr1, "_n"));
					sprintf(tempStr2, "%d", nAhead);           
					strcpy(tempStr1, strcat(tempStr1, tempStr2));
					strcpy (tempStr2, "_L");
					strcpy (tempStr1, strcat(tempStr1, tempStr2));
					sprintf(tempStr2, "%1.2f", lambda);  
					strcpy (tempStr1, strcat(tempStr1, tempStr2));
					strcpy(tempStr3, tempStr1);     
					strcpy (tempStr2, ".num");
					strcpy(tempStr1, strcat(tempStr1, tempStr2));  
					strcat(tempStr1, ".");
					sprintf(tempStr2, "%c", this->info.start);
					strcat(tempStr1, tempStr2);
					sprintf(tempStr2, "%c", this->info.goal);
					strcat(tempStr1, tempStr2);
					strcpy(lrnNumsFname, tempStr1);
					if ( (lrnNumsF = fopen(lrnNumsFname, "w")) == NULL ) {
						fprintf (stderr, "\nError: applyML: Cannot open file %s for writing.\n", lrnNumsFname);
						exit (EXIT_FAILURE);
					}
#if 1
					strcpy (tempStr2, ".rwrd");
					strcpy(tempStr3, strcat(tempStr3, tempStr2));  
					strcat(tempStr3, ".");
					sprintf(tempStr2, "%c", this->info.start);
					strcat(tempStr3, tempStr2);
					sprintf(tempStr2, "%c", this->info.goal);
					strcat(tempStr3, tempStr2);
					strcpy(rqRwrdFname, tempStr3);
					if ( (rqRwrdF = fopen(rqRwrdFname, "w")) == NULL ) {
						fprintf (stderr, "\nError: applyML: Cannot open file %s for writing.\n", rqRwrdFname);
						exit (EXIT_FAILURE);
					}
#endif

					k = 0;
					for (i = 0; i < maxPhsPaths-1; i++) {  
						for (j = 0; j < maxLrnPaths; j++) {  
							strcpy (tempStr1, "Out\\.lrn_m");
							sprintf(tempStr2, "%d", lrnMethod);
							strcpy(tempStr1, strcat(tempStr1, tempStr2));
							strcpy(tempStr1, strcat(tempStr1, "_n"));
							sprintf(tempStr2, "%d", nAhead);
							strcpy(tempStr1, strcat(tempStr1, tempStr2));
							strcpy(tempStr1, strcat(tempStr1, "_p"));
							sprintf(tempStr2, "%d", i+1);
							strcpy(tempStr1, strcat(tempStr1, tempStr2));
							strcpy(tempStr1, strcat(tempStr1, "_l"));
							sprintf(tempStr2, "%d", j+1);
							strcpy(tempStr1, strcat(tempStr1, tempStr2));
							strcpy (tempStr2, ".pth");
							strcpy(tempStr1, strcat(tempStr1, tempStr2));  
							strcat(tempStr1, ".");
							sprintf(tempStr2, "%c", this->info.start);
							strcat(tempStr1, tempStr2);
							sprintf(tempStr2, "%c", this->info.goal);
							strcat(tempStr1, tempStr2);
							strcpy(lrnPathFnames[k], tempStr1);
							if ( (lrnPathFs[k] = fopen(lrnPathFnames[k], "w")) == NULL ) {
								fprintf (stderr, "\nError: applyML: Cannot open file %s for writing.\n", lrnPathFnames[k]);
								exit (EXIT_FAILURE);
							}
							k++;
						}
					}
				}
		}
		else {
			printf ("\nWarning: applyML: Only Physical Path File mode is supported. No path files will be written out.");  
		}

		// Open control-first-path file for reading.
		if (cntrlFirstOn == TRUE) {
			if ( (cntrlFirstF = fopen(cntrlFirstFile, "r")) == NULL ) {
				fprintf (stderr, "\nError: applyML: Cannot open file %s for reading.\n", cntrlFirstFile);
				exit(EXIT_FAILURE);
			}          
		}

		fprintf (agentF, "\n>>> Values:");
		fprintf (agentF, "\n Random Seed Value = %d", (int)seed);
		fprintf (agentF, "\n Number of Physical Paths = %d", maxPhsPaths);
		fprintf (agentF, "\n Number of Learning Paths per Physical Path = %d", maxLrnPaths);
		fprintf (agentF, "\n Method = %d", lrnMethod);
		fprintf (agentF, "\n Number of Lookahead Levels = %d", nAhead);
		fprintf (agentF, "\n Path-File Mode = %d", pathFileMode);
		fprintf (agentF, "\n Step-Size Parameter ALPHA = %f", alpha);
		fprintf (agentF, "\n Discount Rate GAMMA = %f", gamma);
		fprintf (agentF, "\n Lookahead Weight Factor LAMBDA = %f", lambda);
		fprintf (agentF, "\n Q-value Weight Factor w1 = %f", w1);
		fprintf (agentF, "\n Distance-to-Goal Weight Factor w2 = %f", w2);
		(initFullExploreOn == TRUE) ? 
			fprintf (agentF, "\n Initial Full Exploration is ON") :
		fprintf (agentF, "\n Initial Full Exploration is OFF");
		fprintf (agentF, "\n Visited-Number Weight factor w3 = %f", w3);
		(lrnGreedyOn == TRUE) ?
			fprintf (agentF, "\n TAW has greedy-mode value = %d", GREEDY_TAW) :
		((constTawOn == TRUE) ?
			fprintf (agentF, "\n TAW is constant = %d", taw) :
		fprintf (agentF, "\n TAW is variable (initial = %d)", taw));

		fprintf(agentF, "\n Star is %c at location [%d, %d]", startLoc->locSt, startLoc->x, startLoc->y);
		fprintf(agentF, "\n Goal is %c", goal);

		// Display Initial Information in Agent Output File.
		// Display maze in agent output file.
		fprintf (agentF, "\n\n>>> Input Maze:\n");
		for (i = 0; i < ySize; i++) {
			for (j = 0; j < xSize; j++) {
				fprintf (agentF, " %c ", static_cast<locState_t>(this->maze->GetObjectOn(Position<int>(j, i))));//maze[i][j].locSt);
			}
			fprintf (agentF, "\n");
		}  


		/******************************
		* Agent physical experiences.
		******************************/
		bool stop = false;
		bool exploreLocalFirstTemp = this->exploreLocalFirst;

		for (phsPathNum = 1; phsPathNum <= maxPhsPaths; phsPathNum++) 
		{
			if (this->isTransfered)
			{
				sem_wait(&this->agentTransferedSem);
			}

			stop = false;
			this->exploreLocalFirst = exploreLocalFirstTemp;
			err = NO_ERR;

			do
			{
				if ((!this->exploreLocalFirst)&&((err == REACHMAX_ERR)||(err == UNREACH_ERR)))
				{
					stop = true;
				}

				if (!this->isTransfered)
				{
					this->currentPhsPathNum = phsPathNum;
				}
				else
				{
					if (phsPathNum != this->currentPhsPathNum)
					{
						phsPathNum = this->currentPhsPathNum;
					}
				}

				// Get starting time in micro-seconds.
				gettimeofday(&time1, NULL);
				t1 = time1.tv_sec + (time1.tv_usec/1000000.0);

				// Clear the number of visited times before each physical path.
				for (i = 0; i < ySize; i++) {
					for (j = 0; j < xSize; j++) {
						agentBrain[i][j].locSt = BLANK;
						agentBrain[i][j].totalNVsted  = 0;
						agentBrain[i][j].nVsted.up    = 0;
						agentBrain[i][j].nVsted.down  = 0;
						agentBrain[i][j].nVsted.right = 0;
						agentBrain[i][j].nVsted.left  = 0;
						agentBrain[i][j].totalNLrnVsted  = 0;
						agentBrain[i][j].nLrnVsted.up    = 0;
						agentBrain[i][j].nLrnVsted.down  = 0;
						agentBrain[i][j].nLrnVsted.right = 0;
						agentBrain[i][j].nLrnVsted.left  = 0;
						agentBrain[i][j].actTaken.up    = FALSE_t;
						agentBrain[i][j].actTaken.down  = FALSE_t;
						agentBrain[i][j].actTaken.right = FALSE_t;
						agentBrain[i][j].actTaken.left  = FALSE_t;
						if (lrnMethod == PHS_DISTG_NVSTED_LP) {
							agentBrain[i][j].fromX = -1;
							agentBrain[i][j].fromY = -1;         
						}
					}
				}

				fprintf (agentF, "\n\n>>> Agent Physical Path %d:\n", phsPathNum);
				// Place the agent at the starting point.
				if ((!Validator::IsNull(agent, NAME("agent"))) &&
					(!Validator::IsNull(startLoc, NAME("startLoc"))) &&
					(!Validator::IsNull(goalLoc, NAME("goalLoc"))))
				{
					agent->iter                                  = 0;
					agent->step                                  = 0;
					agent->loc.x                                 = startLoc->x;
					agent->loc.y                                 = startLoc->y;
					agentPath[agent->step].loc.x                 = startLoc->x;
					agentPath[agent->step].loc.y                 = startLoc->y;
					agentBrain[agent->loc.y][agent->loc.x].locSt = static_cast<locState_t>(start);
#if 1
					// Starting location is already visited!
					agentBrain[startLoc->y][startLoc->x].isVsted = TRUE_t;
					agentBrain[startLoc->y][startLoc->x].totalNVsted = 1;
					if (startLoc->y > 0) {
						agentBrain[startLoc->y-1][startLoc->x].nVsted.down  = 1;
					}
					if (startLoc->y < ySize-1) {
						agentBrain[startLoc->y+1][startLoc->x].nVsted.up    = 1;
					}
					if (startLoc->x > 0) {
						agentBrain[startLoc->y][startLoc->x-1].nVsted.right = 1;
					}
					if (startLoc->x < xSize-1) {
						agentBrain[startLoc->y][startLoc->x+1].nVsted.left  = 1;
					}

					if ((pathFileMode == PHS_PATH_FILE) || (pathFileMode == BOTH_PATH_FILE)) {    
						fprintf (phsPathFs[phsPathNum-1], "S %d %d %d %d %d %d %d\n",
							agentPath[agent->step].loc.x, agentPath[agent->step].loc.y, NO_TYPE,
							agentPath[agent->step].loc.barDetect.up, agentPath[agent->step].loc.barDetect.down,
							agentPath[agent->step].loc.barDetect.right, agentPath[agent->step].loc.barDetect.left);
					}
#endif    

					// Use random or controlled navigation in the initial path 
					// and intelligent navigation (after the learning) in the 
					// other physical paths.
					if (!Validator::IsNull(log, NAME("log")))
					{
						log->Log(&typeid(this), "Agent %d: Doing physical path %d...", this->id, phsPathNum);
					}

					printf("\nAgent %d: Doing physical path %d...", this->id, phsPathNum);

					// Calculate physical taw.
					if ((lrnMethod != PHS_DISTG_ONLY) && (lrnMethod != PHS_DISTG_NVSTED) &&
						(lrnMethod != PHS_DISTG_NVSTED_CLO) && (lrnMethod != PHS_DISTG_NVSTED_LP)) {
							if (phsPathNum < maxPhsPaths) {
								(phsGreedyOn == TRUE) ? (phsTaw = GREEDY_TAW) : (phsTaw = taw);
							}
							else {
								phsTaw = GREEDY_TAW;
							}
					}

					// First physical path is random.
					if (cntrlFirstOn == FALSE) {
						// Implement the physical (learning-as-moving) methods.
						if ((lrnMethod == PHS_DISTG_ONLY) || (lrnMethod == PHS_DISTG_NVSTED) ||
							(lrnMethod == PHS_DISTG_NVSTED_CLO) || (lrnMethod == PHS_DISTG_NVSTED_LP)) {
								// Agent knows where the goal is but doesn't know what is in between
								// the starting point the goal.
								if (phsPathNum == 1) {
									Position<int> goalPos = this->maze->GetPosition(goal);
									knownGoalLoc->x = goalPos.GetXCoor();
									knownGoalLoc->y = goalPos.GetYCoor();
									knownGoalLoc->locSt = static_cast<locState_t>(goal);

									agentBrain[knownGoalLoc->y][knownGoalLoc->x].locSt = static_cast<locState_t>(goal);           
								}
								while ((res = isGoal(agent->loc.x, agent->loc.y, agentBrain, goal)) == FALSE_t) { 
									if (phsPathNum == 1) {
										act[0] = takePhsActionVstedDist (agentBrain, &agent->loc, xSize, ySize, knownGoalLoc, lrnMethod, fVerb, agentF);
									}
									else { 
										act[0] = takeActionFromList (agentBrain, &agent->loc, fVerb, agentF);
									}
									if ((err = moveAgent (agentID, agent, act[0], xSize, ySize, agentBrain, agentPath, lrnMethod, startLoc, agentF)) != 
										NO_ERR) {
											break;
									}            
								}
						}      
						else {
							action_t lastAction;
							int lastNumStep = 0;

							while ((res = isGoal(agent->loc.x, agent->loc.y, agentBrain, goal)) == FALSE) {             
								if (phsPathNum == 1) {
									act[0] = getRandomDirection(agentBrain, &agent->loc, NO_CHECK, xSize, ySize);
								}
								else {
									act[0] = takeActionOnBoltz(agentBrain, &agent->loc, TRUE_t, act[0], phsTaw, xSize, ySize, 
										lrnMethod, w1, w2, w3, fVerb, agentF);
									/*act[0] = takeActionOnBoltz(agentBrain, &agent->loc, FALSE_t, act[0], phsTaw, xSize, ySize, 
									lrnMethod, w1, w2, w3, fVerb, agentF);*/
								}  

								if (agent->step == lastNumStep)
								{
									if (act[0].val == lastAction.val)
									{
										while (lastAction.val == act[0].val)
										{
											act[0] = getRandomDirection(agentBrain, &agent->loc, NO_CHECK, xSize, ySize);
										}
									}								

								}
								else
								{
									lastNumStep = agent->step;
								}

								strcpy (lastAction.name, act[0].name);
								lastAction.type = act[0].type;
								lastAction.val = act[0].val;

								if ((err = moveAgent (agentID, agent, act[0], xSize, ySize, agentBrain, agentPath, lrnMethod, startLoc, agentF)) != 
									NO_ERR) {
										break;
								}
							}
						}

						if ((!this->isTransfered)&&(this->isGoalOnAnotherNode)&&((err != REACHMAX_ERR)||(err != UNREACH_ERR)))
						{
							stop = true;	//Don't execute the error loop.

							if (phsPathNum == 1)
							{
								this->goalPos.SetXCoor(agent->loc.x);
								this->goalPos.SetYCoor(agent->loc.y);

								this->moveToGoalNodeAction.val = this->lastDir;
								strcpy (this->moveToGoalNodeAction.name, actNames[this->moveToGoalNodeAction.val]);
								this->moveToGoalNodeAction.type = NO_TYPE;

								exploreLocalFirstTemp = false;
							}
							else
							{
								this->exploreLocalFirst = false;
								err = moveAgent (agentID, agent, this->moveToGoalNodeAction, xSize, ySize, agentBrain, agentPath, lrnMethod, startLoc, agentF);
							}
						}
						else if (!this->isTransfered && (res == TRUE_t) && ((err != REACHMAX_ERR)||(err != UNREACH_ERR)))
						{
							stop = true;
						}
						else if ((this->isTransfered)&&((err == REACHMAX_ERR)||(err == UNREACH_ERR)))
						{
							this->isGoalOnAnotherNode = false;
						}
						else if (this->isTransfered)
						{
							this->isGoalOnAnotherNode = true;
							stop = true;	//Don't execute the error loop.
						}

						// The last location of the agent is usually the goal!
						goalLoc->x = agent->loc.x;
						goalLoc->y = agent->loc.y;				
					}

					// First physical path is user-controlled.
					else if (cntrlFirstOn == TRUE) {      
						if (phsPathNum == 1) {	
							while (!feof(cntrlFirstF)) {
								fread (&tempChar, sizeof(char), 1, cntrlFirstF);
								if (tempChar != '\n') {	   
									act[0].val = getDirectionValue(tempChar);
									strcpy (act[0].name, actNames[act[0].val]);
									act[0].type = CNTRL;
									if ((err = moveAgent (agentID, agent, act[0], xSize, ySize, agentBrain, agentPath, lrnMethod, startLoc, agentF)) != 
										NO_ERR) {
											break;
									}
								}
							}
						}
						else {
							action_t lastAction;
							int lastNumStep = 0;

							while ((res = isGoal(agent->loc.x, agent->loc.y, agentBrain, goal)) == FALSE_t) { 
								act[0] = takeActionOnBoltz(agentBrain, &agent->loc, TRUE_t, act[0], phsTaw, xSize, ySize, 
									lrnMethod, w1, w2, w3, fVerb, agentF);
								/*act[0] = takeActionOnBoltz(agentBrain, &agent->loc, FALSE_t, act[0], phsTaw, xSize, ySize, 
								lrnMethod, w1, w2, w3, fVerb, agentF);*/
								if (agent->step == lastNumStep)
								{
									if (act[0].val == lastAction.val)
									{
										while (lastAction.val == act[0].val)
										{
											act[0] = getRandomDirection(agentBrain, &agent->loc, NO_CHECK, xSize, ySize);
										}
									}
								}
								else
								{
									lastNumStep = agent->step;
								}

								strcpy (lastAction.name, act[0].name);
								lastAction.type = act[0].type;
								lastAction.val = act[0].val;

								if ((err = moveAgent (agentID, agent, act[0], xSize, ySize, agentBrain, agentPath, lrnMethod, startLoc, agentF)) != 
									NO_ERR) {
										break;
								}
							}
						}

						if ((!this->isTransfered)&&(this->isGoalOnAnotherNode)&&((err != REACHMAX_ERR)||(err != UNREACH_ERR)))
						{
							stop = true;	//Don't execute the error loop.

							if (phsPathNum == 1)
							{
								this->goalPos.SetXCoor(agent->loc.x);
								this->goalPos.SetYCoor(agent->loc.y);

								this->moveToGoalNodeAction.val = this->lastDir;
								strcpy (this->moveToGoalNodeAction.name, actNames[this->moveToGoalNodeAction.val]);
								this->moveToGoalNodeAction.type = NO_TYPE;

								exploreLocalFirstTemp = false;
							}
							else
							{
								this->exploreLocalFirst = false;
								err = moveAgent (agentID, agent, this->moveToGoalNodeAction, xSize, ySize, agentBrain, agentPath, lrnMethod, startLoc, agentF);
							}
						}
						else if (!this->isTransfered && (res == TRUE_t) && ((err != REACHMAX_ERR)||(err != UNREACH_ERR)))
						{
							stop = true;
						}
						else if ((this->isTransfered)&&((err == REACHMAX_ERR)||(err == UNREACH_ERR)))
						{
							this->isGoalOnAnotherNode = false;
						}
						else if (this->isTransfered)
						{
							this->isGoalOnAnotherNode = true;
							stop = true;	//Don't execute the error loop.
						}

						// The last location of the agent is usually the goal!
						goalLoc->x = agent->loc.x;
						goalLoc->y = agent->loc.y;				
					}

					// Physical path done.
					//printf (" Done.\n");

					if ((err != REACHMAX_ERR)&&(err != UNREACH_ERR))
					{
						if (!Validator::IsNull(log, NAME("log")))
						{
							log->Log(&typeid(this), "Agent %d, found the goal = %c from start = %c", info.agentID, info.goal, info.start);
						}

						printf("\nAgent %d, found the goal = %c from start = %c", info.agentID, info.goal, info.start);

						// Get end time in micro-seconds.
						gettimeofday(&time2, NULL);
						t2 = time2.tv_sec + (time2.tv_usec/1000000.0);

						// Calculate delta time.
						delta_t = t2 - t1;

						if (!Validator::IsNull(log, NAME("log")))
						{
							log->Log(&typeid(this), "Agent %d: Physical path %d, Time: %f s.", this->id, phsPathNum, delta_t);
						}

						printf("\nAgentd %d: Physical path %d, Time: %f s.", this->id, phsPathNum, delta_t);
						fprintf (agentF, "\n>>> Physical path %d, Time: %f s.\n", phsPathNum, delta_t);

						PhysicalPathEventInfo agentInfo;
						agentInfo.agentId = this->id;
						agentInfo.start = this->info.start;
						agentInfo.goal = this->info.goal;
						agentInfo.pathNum = phsPathNum;
						agentInfo.pathTime = delta_t;
						agentInfo.numSteps = agent->step;

						this->maze->NotifyPhysicalPathDone(agentInfo);

						// Give rewards to different locations of maze experienced 
						// by the agent in the physical path.
						if ((lrnMethod != PHS_DISTG_ONLY) && (lrnMethod != PHS_DISTG_NVSTED) &&
							(lrnMethod != PHS_DISTG_NVSTED_CLO) && (lrnMethod != PHS_DISTG_NVSTED_LP)) {
								storeRewards (gamma, agentPath, agentBrain, agent->step, fVerb, agentF);
						}

						// Store distances.
						if ((lrnMethod != PHS_DISTG_ONLY) && (lrnMethod != PHS_DISTG_NVSTED) &&
							(lrnMethod != PHS_DISTG_NVSTED_CLO) && (lrnMethod != PHS_DISTG_NVSTED_LP)) {
								storeDistances (agentBrain, xSize, ySize, goalLoc, start);
						}

						// Output the Agent Path array to a file.
						for (phsStep = 1; phsStep <= agent->step; phsStep++) {
							if ((fVerb > 0) && (phsStep == 1)) {
								fprintf (agentF, "\n");
							} 
							fprintf (agentF, " Step %3d: Agent moved %5s to location [x=%d,y=%d], %s.\n", 
								phsStep, agentPath[phsStep].dir.name, agentPath[phsStep].loc.x, 
								agentPath[phsStep].loc.y, actTypes[agentPath[phsStep].dir.type]); 
							// Print to physical-path file.
							if ((pathFileMode == PHS_PATH_FILE) || (pathFileMode == BOTH_PATH_FILE)){
								fprintf (phsPathFs[phsPathNum-1], "%c %d %d %d %d %d %d %d\n", agentPath[phsStep].dir.name[0], 
									agentPath[phsStep].loc.x, agentPath[phsStep].loc.y, agentPath[phsStep].dir.type,
									agentPath[phsStep].loc.barDetect.up, agentPath[phsStep].loc.barDetect.down,
									agentPath[phsStep].loc.barDetect.right, agentPath[phsStep].loc.barDetect.left);
							}
						}
						// Display the Agent brain matrix in the agent output file.
						fprintf (agentF, "\n>>> Agent Brain:\n");
						for (i = 0; i < ySize; i++) {
							for (j = 0; j < xSize; j++) {
								fprintf (agentF, " %c ", agentBrain[i][j].locSt);
							}
							fprintf (agentF, "\n");
						}


						/************************
						* Agent learning paths.
						************************/

						/* Do Learning. */
						// NOTE: There is no need to do learning after the last physical path.
						if (phsPathNum < maxPhsPaths) {
							if (maxLrnPaths > 0) {
								sta[0].x = startLoc->x;
								sta[0].y = startLoc->y;
								lrnPathNum = 0;
								lrnPathDone = FALSE_t;
								// Do 'maxLrnPaths' learning paths.
								// NOTE: 'maxLrnPaths' is input by user.
								while (lrnPathNum < maxLrnPaths) {
									if ((lrnPathDone == TRUE_t) || (lrnPath[0].step == (maxSteps - 1))) {
										lrnPathNum++;
										totalLrnPathNum++;

										if(!Validator::IsNull(log, NAME("log")))
										{
											log->Log(&typeid(this), "Completed learning path %d.", lrnPathNum);
										}

										// Decrease taw as the agent learns more, to increase exploitation.
										if (constTawOn == FALSE_t) {
											(taw > D_TAW_DEC) ? (taw = taw - D_TAW_DEC) : (taw = GREEDY_TAW); 
										}
										fprintf (agentF, "\n>>> Agent Completed Learning Path %d / %d after Physical Path %d / %d.\n", 
											lrnPathNum, maxLrnPaths, phsPathNum, maxPhsPaths);

										if ((pathFileMode == LRN_PATH_FILE) || (pathFileMode == BOTH_PATH_FILE)) {
											fprintf (lrnPathFs[totalLrnPathNum-1], "S %d %d %d\n", 
												startLoc->x, startLoc->y, NO_TYPE);
										}                  

										for (k = 1; k <= lrnPath[0].step; k++) {
											fprintf (agentF, " Learning Step %3d: Agent hypomoved %5s to location [x=%d,y=%d], %s.\n", 
												k, lrnPath[k].dir.name, lrnPath[k].loc.x, lrnPath[k].loc.y,
												actTypes[lrnPath[k].dir.type]);
											// Print to learning-path file (Direction X Y Type)
											if ((pathFileMode == LRN_PATH_FILE) || (pathFileMode == BOTH_PATH_FILE)) {
												fprintf (lrnPathFs[totalLrnPathNum-1], "%c %d %d %d\n", lrnPath[k].dir.name[0], 
													lrnPath[k].loc.x, lrnPath[k].loc.y, lrnPath[k].dir.type);	  
											}
										}
										// Print to learning-number file (Physical-Path-# Learning-Path-# #-of-Steps)
										fprintf (lrnNumsF, "%d %d %d\n", phsPathNum, lrnPathNum, lrnPath[0].step);
										gLrnPathNum = lrnPathNum;

										// Return to Start location for a new learning path.
										sta[0].x = startLoc->x;
										sta[0].y = startLoc->y;
										lrnPathDone = FALSE_t;
										lrnPath[0].step = 0;
										// Reset learning-visited information after each learning path.
										for (i = 0; i < ySize; i++) {
											for (j = 0; j < xSize; j++) {
												agentBrain[i][j].isLrnVsted = FALSE_t;
											}
										}
									}
									else {
										// This is : direction.val = a <- Policy.
										if ((sta[0].y >= 0) &&
											(sta[0].x >= 0) &&
											(sta[0].y < ySize) &&
											(sta[0].x < xSize))
										{
											(lrnGreedyOn == TRUE_t) ?
												(act[0] = takeActionOnBoltz(agentBrain, &sta[0], TRUE_t, act[0], GREEDY_TAW, xSize, ySize, 
												lrnMethod, w1, w2, w3, fVerb, agentF)) :
											(act[0] = takeActionOnBoltz(agentBrain, &sta[0], TRUE_t, act[0], taw, xSize, ySize, 
												lrnMethod, w1, w2, w3, fVerb, agentF));	    
											lrnPathDone = doLearning (alpha, gamma, lambda, agentBrain, sta, act, xSize, 
												ySize, lrnMethod, nAhead, *startLoc, *goalLoc, lrnPath,
												w1, w2, w3, gLrnPathNum, gMaxLrnPaths, fVerb, agentF, rqRwrdF);
										}
										else
										{
											lrnPathDone = TRUE_t;
										}
										/*(lrnGreedyOn == TRUE_t) ?
										(act[0] = takeActionOnBoltz(agentBrain, &sta[0], FALSE_t, act[0], GREEDY_TAW, xSize, ySize, 
										lrnMethod, w1, w2, w3, fVerb, agentF)) :
										(act[0] = takeActionOnBoltz(agentBrain, &sta[0], FALSE_t, act[0], taw, xSize, ySize, 
										lrnMethod, w1, w2, w3, fVerb, agentF));	    
										lrnPathDone = doLearning (alpha, gamma, lambda, agentBrain, sta, act, xSize, 
										ySize, lrnMethod, nAhead, *startLoc, *goalLoc, lrnPath,
										w1, w2, w3, gLrnPathNum, gMaxLrnPaths, fVerb, agentF, rqRwrdF);*/
									}
								}	
							}
						}	
					}
					else
					{
						this->exploreLocalFirst = false;
					}
				}
			} while (!stop);

			if (this->isTransfered && this->canNotifyWhenDone)
			{
				this->maze->NotifyAgentIsDone(this->info.agentID, this->isTransfered, this->isGoalOnAnotherNode);
			}
		}


		/*******************************
		* Close files and free memory.
		*******************************/

		// Close files.
		//fclose (agentF);
		if (cntrlFirstOn == TRUE) {
			fclose (cntrlFirstF);
			cntrlFirstF = NULL;
		}

		// Free memory allocated for arrays.
		free (startLoc);
		startLoc = NULL;

		free (tempStartLoc);
		tempStartLoc = NULL;

		free (goalLoc);
		goalLoc = NULL;

		free (knownGoalLoc);
		knownGoalLoc = NULL;

		free (agentPath);
		agentPath = NULL;

		free (lrnPath);	//Here
		lrnPath = NULL;

		free (agent);
		agent = NULL;

		free (act);
		act = NULL;

		free (sta);
		sta = NULL;

		free (tempStr1);
		tempStr1 = NULL;

		free (tempStr2);
		tempStr2 = NULL;

		free (tempStr3);
		tempStr3 = NULL;

		// Close/free additional files/memory.
		if ((pathFileMode == BOTH_PATH_FILE) || (pathFileMode == PHS_PATH_FILE)) {
			for (i = 0; i < maxPhsPaths; i++) {
				fclose (phsPathFs[i]);
				phsPathFs[i] = NULL;

				free (phsPathFnames[i]);
				phsPathFnames[i] = NULL;
			}
			free (phsPathFs);
			phsPathFs = NULL;

			free (phsPathFnames);
			phsPathFnames = NULL;
		}
		if ((pathFileMode == BOTH_PATH_FILE) || (pathFileMode == LRN_PATH_FILE)) {
			for (k = 0; k < (lrnPathNum*(maxPhsPaths-1)); k++) {
				fclose (lrnPathFs[k]);
				lrnPathFs[k] = NULL;

				free (lrnPathFnames[k]);
				lrnPathFnames[k] = NULL;
			}    
			free (lrnPathFs);
			lrnPathFs = NULL;

			free (lrnPathFnames);
			lrnPathFnames = NULL;

			fclose (lrnNumsF);
			lrnNumsF = NULL;

			free (lrnNumsFname);
			lrnNumsFname = NULL;
#if 1
			fclose (rqRwrdF);
			rqRwrdF = NULL;

			free (rqRwrdFname);
			rqRwrdFname = NULL;
#endif
		}

		#ifdef PrintMemoryLeak
			CHECK_HEAP();
		#endif
		return res;//(TRUE_t);	
	}



	/////////////////////////////////////////////////////////////////////////////////
	// MAIN FUNCTION
	// Name: main
	// Description: 
	/////////////////////////////////////////////////////////////////////////////////

	int MLAgent::ml (AgentInfo info) 
	{
		try
		{
			struct timeval time1, time2;   // Time elements.
			double t1, t2, delta_t;
			int fVerb = 0;
			FILE *agentF = NULL;

			// Validate values of options.
			if ((info.xSize <= 0) || (info.ySize <= 0)) {
				printf ("\nError: main: X and Y sizes of the input matrix must be positive.\n");
				exit (EXIT_FAILURE);
			}
			else if ((info.nAhead <= 0) || (info.nAhead > 6)) {
				printf ("\nError: main: Invalid number of lookahead levels (only N=1,2,3,4,5,6 are supported).\n");
				exit (EXIT_FAILURE);
			}
			else if ((info.pathFileMode != NO_PATH_FILE) && (info.pathFileMode != PHS_PATH_FILE) && 
				(info.pathFileMode != LRN_PATH_FILE) && (info.pathFileMode != BOTH_PATH_FILE)) {
					printf ("\nError: main: Invalid 'path-file mode' input value.\n");
					exit (EXIT_FAILURE);
			}
			else if ((info.lrnMethod != QVAL_ONLY) && (info.lrnMethod != DISTG_ONLY) && 
				(info.lrnMethod != QVAL_NVSTED) && (info.lrnMethod != DISTG_NVSTED) &&
				(info.lrnMethod != QVAL_DISTG) && (info.lrnMethod != COMBINE_ALL) &&
				(info.lrnMethod != PHS_DISTG_ONLY) && (info.lrnMethod != PHS_DISTG_NVSTED) &&
				(info.lrnMethod != PHS_DISTG_NVSTED_CLO) && (info.lrnMethod != PHS_DISTG_NVSTED_LP)) {
					printf ("\nError: main: Invalid 'learning method' input value.\n");
					exit (EXIT_FAILURE);
			}
			else if (((info.lrnMethod == PHS_DISTG_ONLY) || (info.lrnMethod == PHS_DISTG_NVSTED)) &&
				(info.maxPhsPaths != 1)) {
					printf ("\nError: main: Only 1 physical path should take place with Methods 10 and 11.\n");
					exit (EXIT_FAILURE);
			}    
			else if ((info.alpha < 0) || (info.alpha > 1)) {
				printf ("\nError: main: Invalid ALPHA value.\n");
				exit (EXIT_FAILURE);
			}
			else if ((info.gamma < 0) || (info.gamma > 1)) {
				printf ("\nError: main: Invalid GAMMA value.\n");
				exit (EXIT_FAILURE);
			}
			else if ((info.lambda < 0) || (info.lambda > 1)) {
				printf ("\nError: main: Invalid LAMBDA value.\n");
				exit (EXIT_FAILURE);
			}
			else if (info.xSize <= 0) {
				printf ("\nError: main: Invalid or Unspecified X-size value.\n");
				exit (EXIT_FAILURE);
			}
			else if (info.ySize <= 0) {
				printf ("\nError: main: Invalid or Unspecified Y-size value.\n");
				exit (EXIT_FAILURE);
			}

			// Check for the Agent File.
			if ( (agentF = fopen(info.agentFile, "w")) == NULL ) {
				fprintf (stderr, "\nError: applyML: Cannot open file %s for writing.\n", info.agentFile);
				exit (EXIT_FAILURE);
			}

			if (this->maze != NULL)
			{
				// Get starting time in micro-seconds.
				gettimeofday(&time1, NULL);
				t1 = time1.tv_sec + (time1.tv_usec/1000000.0);

				// Call the applyML function.  
				applyML (info.alpha, 
					info.gamma, 
					info.lambda, 
					info.taw, 
					info.xSize, 
					info.ySize,
					info.maxPhsPaths, 
					info.maxLrnPaths, 
					info.lrnMethod, 
					info.cntrlFirstOn, 
					info.initFullExploreOn, 
					info.phsGreedyOn, 
					info.lrnGreedyOn, 
					info.constTawOn,
					info.nAhead, 
					info.agentFile, 
					info.pathFileMode, 
					info.cntrlFirstFile, 
					info.seedVal, 
					info.seedOn, 
					info.w1, 
					info.w2, 
					info.w3,
					info.maxLrnPaths,
					fVerb,
					agentF,
					info.start,
					info.goal,
					info.agentID,
					info.agentBrain);	

				if (!this->isTransfered)
				{
					this->maze->NotifyAgentIsDone(this->info.agentID, this->isTransfered, this->isGoalOnAnotherNode);
				}

				//printf("\nAgent %d, found the goal = %c\n", info.agentID, info.goal);

				// Get end time in micro-seconds.
				gettimeofday(&time2, NULL);
				t2 = time2.tv_sec + (time2.tv_usec/1000000.0);

				// Calculate delta time.
				delta_t = t2 - t1;
				if (!Validator::IsNull(log, NAME("log")))
				{
					log->Log(&typeid(this), "Process Time: %f s.", delta_t);
				}

				fprintf (agentF, "\n>>> Process Time: %f s.\n", delta_t);
			}

#ifdef DEBUG
			if (!Validator::IsNull(log, NAME("log")))
			{
				log->Log(&typeid(this), "Closing file ...");
			}
#endif
			fclose (agentF);
		}
		catch(...)
		{
			throw;
		}

		#ifdef PrintMemoryLeak
			CHECK_HEAP();
		#endif
		return (NO_ERR);
	}
}