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
#include <map>

// Custom Header files.
#include "ml_gencon.h"
#include "ml_genlib.h"
#include "ml.h"
#include "MLAgent.h"
#include "Framework/Position.h"
#include "Framework\Debug.h"
#include "DateTime.h"

#define MAX_VAL (float)10000

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

namespace MLContinuous
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

				// Find Start location.
				Position<int> startPos = this->maze->GetPosition(start);
				startLoc->x = startPos.GetXCoor();
				startLoc->y = startPos.GetYCoor();
				startLoc->locSt = static_cast<locState_t>(start);

				if (phsPathNum != this->currentPhsPathNum)
				{
					phsPathNum = this->currentPhsPathNum;
				}

				if (phsPathNum > 1)
				{
					// Only explore local first on the first run.
					this->exploreLocalFirst = false;
				}
			}
			else
			{
				this->transferNumber = 0;
				this->totalNumberOfStepsTaken = 0;
				this->currentPhsPathNum = phsPathNum;
			}

			stop = false;
			this->exploreLocalFirst = exploreLocalFirstTemp;
			this->isGoalOnThisNode = false;
			err = NO_ERR;

			// Get starting time in micro-seconds.
			gettimeofday(&time1, NULL);
			t1 = time1.tv_sec + (time1.tv_usec/1000000.0);

			list<string> elements;

			// Clear the number of visited times before each physical path.
			for (i = 0; i < ySize; i++) {
				for (j = 0; j < xSize; j++) {
					if ((phsPathNum == 2) || agentBrain[i][j].locSt != BARRIER)
					{
						agentBrain[i][j].locSt = BLANK;
					}

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
					agentBrain[i][j].fromX = 0;
					agentBrain[i][j].fromY = 0;
					if (lrnMethod == PHS_DISTG_NVSTED_LP) {
						agentBrain[i][j].fromX = -1;
						agentBrain[i][j].fromY = -1;         
					}
				}
			}

			if (phsPathNum == 2)
			{
				for (std::list<Position<int> *>::iterator iter = this->obstaclePosList.begin(); iter != this->obstaclePosList.end(); ++iter)
				{
					delete *iter;
				}

				this->obstaclePosList.clear();
			}

			elements.clear();

			elements.push_back("");
			elements.push_back("");

			if (this->statsFile != NULL)
			{
				this->statsFile->WriteRow(elements);

				elements.clear();
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
				agent->loc.fromX							 = 0;
				agent->loc.fromY							 = 0;
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
			}

			do
			{
				agent->iter                                  = 0;
				agent->step                                  = 0;

				if ((!this->exploreLocalFirst)&&((err == REACHMAX_ERR)||(err == UNREACH_ERR)))
				{
					stop = true;
				}								

				// Place the agent at the starting point.
				if ((!Validator::IsNull(agent, NAME("agent"))) &&
					(!Validator::IsNull(startLoc, NAME("startLoc"))) &&
					(!Validator::IsNull(goalLoc, NAME("goalLoc"))))
				{
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
									
									elements.push_back(StatsFileIO::ConvertToString(agent->loc.x));
									elements.push_back(StatsFileIO::ConvertToString(agent->loc.y));

									if (this->statsFile != NULL)
									{
										this->statsFile->WriteRow(elements);
										elements.clear();
									}

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
						else 
						{
							res = this->MoveToGoal(agent, agentBrain, agentPath, xSize, ySize, goal, goalLoc, startLoc, phsPathNum, agentF, err);
						}

						this->GoalFound(agent, goalLoc, res, err, exploreLocalFirstTemp, stop);
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
						else
						{
							res = this->MoveToGoal(agent, agentBrain, agentPath, xSize, ySize, goal, goalLoc, startLoc, phsPathNum, agentF, err);
						}

						this->GoalFound(agent, goalLoc, res, err, exploreLocalFirstTemp, stop);
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
						fprintf (agentF, "\n>>> Physical path %d, Time: %f s, Min radius: %f Total Steps: %d.\n", phsPathNum, delta_t, this->lowestRadiusOfPath, this->totalNumberOfStepsTaken);

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
								if (!Validator::IsNull(log, NAME("log")))
								{
									log->Log(&typeid(this), "Agent %d, found the goal in %d total steps.", info.agentID, this->totalNumberOfStepsTaken);
								}

								storeRewards (gamma, agentPath, agentBrain, agent->step, fVerb, agentF, this->totalNumberOfStepsTaken);
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
						//if (phsPathNum < maxPhsPaths) {
						//	if (maxLrnPaths > 0) {
						//		sta[0].x = startLoc->x;
						//		sta[0].y = startLoc->y;
						//		lrnPathNum = 0;
						//		lrnPathDone = FALSE_t;
						//		// Do 'maxLrnPaths' learning paths.
						//		// NOTE: 'maxLrnPaths' is input by user.
						//		while (lrnPathNum < maxLrnPaths) {
						//			if ((lrnPathDone == TRUE_t) || (lrnPath[0].step == (maxSteps - 1))) {
						//				lrnPathNum++;
						//				totalLrnPathNum++;

						//				if(!Validator::IsNull(log, NAME("log")))
						//				{
						//					log->Log(&typeid(this), "Completed learning path %d.", lrnPathNum);
						//				}

						//				// Decrease taw as the agent learns more, to increase exploitation.
						//				if (constTawOn == FALSE_t) {
						//					(taw > D_TAW_DEC) ? (taw = taw - D_TAW_DEC) : (taw = GREEDY_TAW); 
						//				}
						//				fprintf (agentF, "\n>>> Agent Completed Learning Path %d / %d after Physical Path %d / %d.\n", 
						//					lrnPathNum, maxLrnPaths, phsPathNum, maxPhsPaths);

						//				if ((pathFileMode == LRN_PATH_FILE) || (pathFileMode == BOTH_PATH_FILE)) {
						//					fprintf (lrnPathFs[totalLrnPathNum-1], "S %d %d %d\n", 
						//						startLoc->x, startLoc->y, NO_TYPE);
						//				}                  

						//				for (k = 1; k <= lrnPath[0].step; k++) {
						//					fprintf (agentF, " Learning Step %3d: Agent hypomoved %5s to location [x=%d,y=%d], %s.\n", 
						//						k, lrnPath[k].dir.name, lrnPath[k].loc.x, lrnPath[k].loc.y,
						//						actTypes[lrnPath[k].dir.type]);
						//					// Print to learning-path file (Direction X Y Type)
						//					if ((pathFileMode == LRN_PATH_FILE) || (pathFileMode == BOTH_PATH_FILE)) {
						//						fprintf (lrnPathFs[totalLrnPathNum-1], "%c %d %d %d\n", lrnPath[k].dir.name[0], 
						//							lrnPath[k].loc.x, lrnPath[k].loc.y, lrnPath[k].dir.type);	  
						//					}
						//				}
						//				// Print to learning-number file (Physical-Path-# Learning-Path-# #-of-Steps)
						//				fprintf (lrnNumsF, "%d %d %d\n", phsPathNum, lrnPathNum, lrnPath[0].step);
						//				gLrnPathNum = lrnPathNum;

						//				// Return to Start location for a new learning path.
						//				sta[0].x = startLoc->x;
						//				sta[0].y = startLoc->y;
						//				lrnPathDone = FALSE_t;
						//				lrnPath[0].step = 0;
						//				// Reset learning-visited information after each learning path.
						//				for (i = 0; i < ySize; i++) {
						//					for (j = 0; j < xSize; j++) {
						//						agentBrain[i][j].isLrnVsted = FALSE_t;
						//					}
						//				}
						//			}
						//			else {
						//				// This is : direction.val = a <- Policy.
						//				if ((sta[0].y >= 0) &&
						//					(sta[0].x >= 0) &&
						//					(sta[0].y < ySize) &&
						//					(sta[0].x < xSize))
						//				{
						//					(lrnGreedyOn == TRUE_t) ?
						//						(act[0] = takeActionOnBoltz(agentBrain, &sta[0], TRUE_t, act[0], GREEDY_TAW, xSize, ySize, 
						//						lrnMethod, w1, w2, w3, fVerb, agentF)) :
						//					(act[0] = takeActionOnBoltz(agentBrain, &sta[0], TRUE_t, act[0], taw, xSize, ySize, 
						//						lrnMethod, w1, w2, w3, fVerb, agentF));	    
						//					lrnPathDone = doLearning (alpha, gamma, lambda, agentBrain, sta, act, xSize, 
						//						ySize, lrnMethod, nAhead, *startLoc, *goalLoc, lrnPath,
						//						w1, w2, w3, gLrnPathNum, gMaxLrnPaths, fVerb, agentF, rqRwrdF);
						//				}
						//				else
						//				{
						//					lrnPathDone = TRUE_t;
						//				}
						//			}
						//		}	
						//	}
						//}	
					}
					else
					{
						this->exploreLocalFirst = false;

						if (!Validator::IsNull(log, NAME("log")))
						{
							log->Log(&typeid(this), "Agent %d, didn't find the goal = %c from start = %c", info.agentID, info.goal, info.start);
						}

						printf("\nAgent %d, didn't find the goal = %c from start = %c", info.agentID, info.goal, info.start);

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
						fprintf (agentF, "\n>>> Physical path %d, Time: %f s, Min radius: %f Total Steps: %d.\n", phsPathNum, delta_t, this->lowestRadiusOfPath, this->totalNumberOfStepsTaken);

						PhysicalPathEventInfo agentInfo;
						agentInfo.agentId = this->id;
						agentInfo.start = this->info.start;
						agentInfo.goal = this->info.goal;
						agentInfo.pathNum = phsPathNum;
						agentInfo.pathTime = delta_t;
						agentInfo.numSteps = agent->step;

						this->maze->NotifyPhysicalPathDone(agentInfo);
					}
				}
			} while (!stop);

			if (this->isTransfered && this->canNotifyWhenDone)
			{
				this->maze->NotifyAgentIsDone(this->info.agentID, this->totalNumberOfStepsTaken, this->lowestRadiusOfPath, goalLoc->x, goalLoc->y, this->isTransfered, this->isGoalOnAnotherNode, this->currentTransferId);
			}
			
			// If you are transfered and you didn't find anything, go ahead die
			if ((this->isTransfered) && (err == REACHMAX_ERR)&&(err == UNREACH_ERR))
			{
				break;
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

			//free (lrnPathFnames);
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
					fprintf (agentF, "\n>>> Best Path: %d.\n", this->bestPath);

					this->maze->NotifyAgentIsDone(this->info.agentID, this->totalNumberOfStepsTaken, this->lowestRadiusOfPath, this->goalPos.GetXCoor(), this->goalPos.GetYCoor(), this->isTransfered, this->isGoalOnAnotherNode, this->currentTransferId);
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

	float MLAgent::CalculateRadiusOfCurvature(Framework::Position<float> prevPos, Framework::Position<float> currPos, Framework::Position<float> nextPos, Framework::Position<float> goalPos, int totalNumVisited, float totalDiscountedReward)
	{
		//Framework::Logging::ILog *log = Framework::Logging::LogManager::Instance();

		//float radius = MLAgent::CalculateDistance(currPos,
		//										  MLAgent::CalculateCenterPoint(prevPos, currPos, nextPos));

		//if (!Validator::IsNull(log, NAME("log")))
		//{
		//	log->Log(&typeid(MLAgent), "Radius of curvature is %f for: [previous position: (%f, %f)], [Current position: (%f, %f)], [New position: (%f, %f)]", radius, prevPos.GetXCoor(), prevPos.GetYCoor(), currPos.GetXCoor(), currPos.GetYCoor(), nextPos.GetXCoor(), nextPos.GetYCoor());
		//}

		//// If the radius is 0, it means that the curve of the agent's path is a straight line.  And it is always ok, to move in a straight line.
		//if (radius == 0)
		//{
		//	return true;
		//}

		Framework::Logging::ILog *log = Framework::Logging::LogManager::Instance();
		Framework::Position<float> firstDervStep;
		Framework::Position<float> secondDervStep;
		Framework::Position<float> thirdDervStep;

		/*Position<float> firstDerv = MLAgent::DynamicPolicy(goalPos, nextPos, totalNumVisited, totalDiscountedReward, this->obstaclePosList, this->info.absalonSquared);
		Position<float> secondDerv = MLAgent::CalculateSecondDeriv(goalPos, nextPos, this->info.absalonSquared, this->obstaclePosList);*/
		MLAgent::CalculateActionAndStep(MLAgent::DynamicPolicy(goalPos, currPos, totalNumVisited, totalDiscountedReward, this->obstaclePosList, this->info.absalonSquared),
										firstDervStep);
		MLAgent::CalculateActionAndStep(MLAgent::CalculateSecondDeriv(goalPos, nextPos, this->info.absalonSquared, this->obstaclePosList),
										secondDervStep);
		Position<float> firstDerv = currPos - 
									(firstDervStep * this->info.gradientStepSize) -
									(secondDervStep * this->info.gradientStepSize);

		MLAgent::CalculateActionAndStep(MLAgent::DynamicPolicy(goalPos, prevPos, totalNumVisited, totalDiscountedReward, this->obstaclePosList, this->info.absalonSquared),
										firstDervStep);
		MLAgent::CalculateActionAndStep(MLAgent::CalculateSecondDeriv(goalPos, currPos, this->info.absalonSquared, this->obstaclePosList),
										secondDervStep);
		MLAgent::CalculateActionAndStep(MLAgent::CalculateThirdDeriv(goalPos, nextPos, this->info.absalonSquared, this->obstaclePosList),
										thirdDervStep);
		Position<float> secondDerv = prevPos -
									 (firstDervStep * this->info.gradientStepSize) -
									 (secondDervStep * this->info.gradientStepSize) -
									 (thirdDervStep * this->info.gradientStepSize);

		Position<float> firstDervSquared = firstDerv * firstDerv;
		float firstTerm = firstDerv.GetXCoor() * secondDerv.GetYCoor();
		float secondTerm = firstDerv.GetYCoor() * secondDerv.GetXCoor();
		float thirdTerm = sqrt( pow ( (firstDervSquared.GetXCoor() + firstDervSquared.GetYCoor()), 3 ));

		float radius = 0;
		
		if ((firstTerm - secondTerm) != 0)
		{
			radius = thirdTerm / (abs(firstTerm - secondTerm));//which is the same as: 1 / (abs(firstTerm - secondTerm) / thirdTerm);
		}
		else
		{
			radius = MAX_VAL;
		}

		if (!Validator::IsNull(log, NAME("log")))
		{
			log->Log(&typeid(MLAgent), "Radius of curvature is %f ", radius);
		}

		return radius;
	}

	bool MLAgent::IsTurningAngleAcceptable(Framework::Position<float> vector0, Framework::Position<float> vector1)
	{
		float angle = MLAgent::CalculateDifferenceAngle(vector0, vector1);

		if (!Validator::IsNull(log, NAME("log")))
		{
			log->Log(&typeid(MLAgent), "Turning angle = %f, min angle = %f ", angle * (180 / ML_PI), this->info.minTurningAngle * (180 / ML_PI));
		}

		return  angle <= this->info.minTurningAngle;
	}

	void MLAgent::GoalFound(agent_t *agent, state_t* goalLoc, bool_t res, error_t err, /*out*/ bool& exploreLocalFirstTemp, /*out*/ bool& stop)
	{
		list<string> elements;

		if (res == TRUE_t)
		{
			this->isGoalOnThisNode = true;

			// The last location of the agent is usually the goal!
			goalLoc->x = agent->loc.x;
			goalLoc->y = agent->loc.y;
		}

		if ((!this->isTransfered)&&(this->isGoalOnAnotherNode)&&((err != REACHMAX_ERR)||(err != UNREACH_ERR)))
		{
			stop = true;	//Don't execute the error loop.

			exploreLocalFirstTemp = false;

			goalLoc->x = this->goalPos.GetXCoor();
			goalLoc->y = this->goalPos.GetYCoor();
		}
		else if (!this->isTransfered && (res == TRUE_t) && ((err != REACHMAX_ERR)||(err != UNREACH_ERR)))
		{
			stop = true;

			exploreLocalFirstTemp = false;

			// The last location of the agent is usually the goal!
			goalLoc->x = agent->loc.x;
			goalLoc->y = agent->loc.y;
		}
		else if ((this->isTransfered)&&((err == REACHMAX_ERR)||(err == UNREACH_ERR)))
		{
			this->isGoalOnAnotherNode = false;

			exploreLocalFirstTemp = false;
		}
		else if (this->isTransfered)
		{
			this->isGoalOnAnotherNode = true;
			exploreLocalFirstTemp = false;
			stop = true;	//Don't execute the error loop.
		}
		else if ((err == REACHMAX_ERR)||(err == UNREACH_ERR))
		{
			exploreLocalFirstTemp = false;
		}

		if (!this->isTransfered && this->currentPhsPathNum > 1)
		{
			// Since the goal has been found, check to see if the path found is the best so far by looking at the rewards (radius and steps)
			if (this->lowestRadiusOfPath >= this->info.minRadiusOfCurvature)
			{
				if (this->bestRadius < this->info.minRadiusOfCurvature)
				{
					this->bestReward = this->totalNumberOfStepsTaken;
					this->bestPath = this->currentPhsPathNum;
					this->bestRadius = this->lowestRadiusOfPath;
				}
				else if (this->totalNumberOfStepsTaken < this->bestReward)
				{
					this->bestReward = this->totalNumberOfStepsTaken;
					this->bestPath = this->currentPhsPathNum;
					this->bestRadius = this->lowestRadiusOfPath;
				}
				else if ((agent->step == this->bestReward) &&
						 (this->lowestRadiusOfPath > this->bestRadius))
				{
					this->bestReward = this->totalNumberOfStepsTaken;
					this->bestPath = this->currentPhsPathNum;
					this->bestRadius = this->lowestRadiusOfPath;
				}
			}
			else
			{
				if (this->lowestRadiusOfPath > this->bestRadius)
				{
					this->bestReward = this->totalNumberOfStepsTaken;
					this->bestPath = this->currentPhsPathNum;
					this->bestRadius = this->lowestRadiusOfPath;
				}
				else if ((this->lowestRadiusOfPath == this->bestRadius) &&
						 (this->totalNumberOfStepsTaken < this->bestReward))
				{
					this->bestReward = this->totalNumberOfStepsTaken;
					this->bestPath = this->currentPhsPathNum;
					this->bestRadius = this->lowestRadiusOfPath;
				}
			}
		}

		// At this point the agent is on the goal position, store that as the last position
		elements.push_back(StatsFileIO::ConvertToString(goalLoc->x));
		elements.push_back(StatsFileIO::ConvertToString(goalLoc->y));

		if (this->statsFile != NULL)
		{
			this->statsFile->WriteRow(elements);
			elements.clear();
		}
	}

	action_t MLAgent::GetNextMove(Framework::Position<float> currentPos, Framework::Position<float> goalPos, int totalNumVisited, float totalDiscountedReward)
	{
		return this->takeActionOnGradientDescent(goalPos, currentPos, totalNumVisited, totalDiscountedReward, MLAgent::DynamicPolicy, this->info.gradientStepSize);
	}

	void MLAgent::GetNextNMoves(agent_t *agent, state_t* goalLoc, state_t* startLoc, unsigned n, state_t **agentBrain, int xSize, int ySize, action_t* actArray, action_t lastAction)
	{
		state_t nextPos;
		/*act[0] = takeActionOnBoltz(agentBrain, &agent->loc, TRUE_t, act[0], phsTaw, xSize, ySize, 
		lrnMethod, w1, w2, w3, fVerb, agentF);*/
		Position<float> currentPos(agent->loc.x, agent->loc.y);
		Position<float> prevPos(agent->loc.fromX, agent->loc.fromY);
		Position<float> goal(goalLoc->x, goalLoc->y);
		Position<float> subGoal = MLAgent::GetNextSubGoal(currentPos, goal, this->info.slidingWindowLookAhead);
		Position<float> prevVector(currentPos.GetXCoor() - prevPos.GetXCoor(), currentPos.GetYCoor() - prevPos.GetYCoor());
		std::map<Position<float> *, float> vectorToRadiusMap;
		std::list<Position<float> *> radiusAcceptableVectors;
		std::list<Position<float> *> turningAngleAcceptableVectors;
		Position<float> temp;
		float tempRadius = 0;
		float tempAngle = 0;
		float minAngle = 10000;	// Some big random number

		nextPos = agent->loc;
		int totalNumVisited = 0;
		float totalDiscountedReward = 0; 

		//Figure out the next n moves.
		for (unsigned i = 0; i < n; i++)
		{
			// Do a turning angle test, and select all the vectors that pass.
			for (std::list<Position<float> *>::iterator iter = this->directionVectors.begin(); iter != this->directionVectors.end(); ++iter)
			{
				if (MLAgent::IsTurningAngleAcceptable(prevVector, **iter))
				{
					// Add the direction if the agent moved on its last step, or if it didn't move that the new direction doesn't match the last.
					if (agent->moved == TRUE_t ||
						(agent->moved == FALSE_t && lastAction.val != MLAgent::ConvertDirectionToAction(**iter).val))
					{
						turningAngleAcceptableVectors.push_back(*iter);
					}
				}
			}

			// Do a radius of curvature test and select all the vectors that pass.
			for (std::list<Position<float> *>::iterator iter = turningAngleAcceptableVectors.begin(); iter != turningAngleAcceptableVectors.end(); ++iter)
			{
				temp = currentPos + **iter;

				tempRadius = MLAgent::CalculateRadiusOfCurvature(prevPos, currentPos, temp, subGoal, totalNumVisited, totalDiscountedReward);

				vectorToRadiusMap[*iter] = tempRadius;

				if (tempRadius >= this->info.minRadiusOfCurvature)
				{
					radiusAcceptableVectors.push_back(*iter);
				}
			}

			// If none of the directions passed the turning angle, and radius of curvature test, then pick the direction with the max radius
			if (radiusAcceptableVectors.size() == 0)
			{
				tempRadius = 0;

				for (std::map<Position<float> *, float>::iterator iter = vectorToRadiusMap.begin(); iter != vectorToRadiusMap.end(); ++iter)
				{
					if (iter->second >= tempRadius)
					{
						tempRadius = iter->second;

						temp.SetXCoor(iter->first->GetXCoor());
						temp.SetYCoor(iter->first->GetYCoor());
					}
				}
			}
			else	// If there are directions that passed both tests, then get the direction closest to the one yielded by the policy
			{
				temp = currentPos;
				MLAgent::takeActionOnGradientDescent(subGoal, temp, totalNumVisited, totalDiscountedReward, MLAgent::DynamicPolicy, this->info.gradientStepSize);

				// Get the vector
				Position<float> tempVector = temp - currentPos;				 

				// Select the possible direction closest to the desired direction
				for (std::list<Position<float> *>::iterator iter = radiusAcceptableVectors.begin(); iter != radiusAcceptableVectors.end(); ++iter)
				{
					tempAngle = abs(MLAgent::CalculateDifferenceAngle(**iter, tempVector));
					if (tempAngle <= minAngle)
					{
						minAngle = tempAngle;

						temp.SetXCoor((*iter)->GetXCoor());
						temp.SetYCoor((*iter)->GetYCoor());
					}
				}
			}

			// Get the first action
			actArray[i] = this->ConvertDirectionToAction(temp);

			turningAngleAcceptableVectors.clear();
			vectorToRadiusMap.clear();
			radiusAcceptableVectors.clear();

			tempRadius = 0;
			tempAngle = 0;
			minAngle = 10000;

			// Update the current and previous positions;
			prevPos = currentPos;
			currentPos = prevPos + temp;
			prevVector = currentPos - prevPos;
			//totalNumVisited = nextPos.totalNVsted;
			//totalDiscountedReward = nextPos.dsRwrd.down + nextPos.dsRwrd.up + nextPos.dsRwrd.left + nextPos.dsRwrd.right; 

			//actArray[i] = this->GetNextMove(currentPos, subGoal, totalNumVisited, totalDiscountedReward);

			//// Move the agent logically, not physically.
			//nextPos = makeHypoStep(nextPos, actArray[i], agentBrain, xSize, ySize, *goalLoc);

			//currentPos.SetXCoor(nextPos.x);
			//currentPos.SetYCoor(nextPos.y);
		}
	}

	/*static*/ float MLAgent::CalculateDifferenceAngle(Framework::Position<float> vector0, Framework::Position<float> vector1)
	{
		Position<float> vector0Normalised = vector0 * (1 / (sqrt((vector0.GetXCoor() * vector0.GetXCoor()) + (vector0.GetYCoor() * vector0.GetYCoor()))));
		Position<float> vector1Normalised = vector1 * (1 / (sqrt((vector1.GetXCoor() * vector1.GetXCoor()) + (vector1.GetYCoor() * vector1.GetYCoor()))));

		float dotProduct = (vector0Normalised.GetXCoor() * vector1Normalised.GetXCoor()) + (vector0Normalised.GetYCoor() * vector1Normalised.GetYCoor());

		return acos(dotProduct);
		
		//return abs(ML_PI - atan2(vector1.GetYCoor(), vector1.GetXCoor()) - atan2(vector0.GetYCoor(), vector0.GetXCoor()));
	}

	action_t MLAgent::CalculateActionAndStep(Framework::Position<float> currPos, Framework::Position<float> & step)
	{
		float closestAngle = 180;
		float angle = 180;

		for (std::list<Position<float> *>::iterator iter = this->directionVectors.begin(); iter != this->directionVectors.end(); ++iter)
		{
			angle = abs(MLAgent::CalculateDifferenceAngle(**iter, currPos)); 
			if (angle <= closestAngle)
			{
				closestAngle = angle;

				step.SetXCoor((*iter)->GetXCoor());
				step.SetYCoor((*iter)->GetYCoor());
			}
		}

		return this->ConvertDirectionToAction(step);
	}

	void MLAgent::GetRelaxationSteps(agent_t *agent, state_t* goalLoc, state_t* startLoc, unsigned n, state_t **agentBrain, int xSize, int ySize, action_t* actArray)
	{
		//First calculate the radius of all the near points in all directions.

		//Second calculate all the angles for all the near points in all directions.
		//use [arctan2(vector2.y, vector2.x) - arctan2(vector1.y, vector1.x)] * (180/pi)?

		//Choose pos with min radius >= constraint radius, and smallest angle.
	}

	bool_t MLAgent::MoveToGoal(agent_t *agent, state_t **agentBrain, agent_t* agentPath, int xSize, int ySize, char goal, state_t* goalLoc, state_t* startLoc, int phsPathNum, FILE* agentF, /*out*/error_t& err)
	{
		bool_t res;
		bool generateFirstStepRandomly = true;
		action_t act;
		action_t* actArray = new action_t[this->info.immediateMoves];
		list<string> elements;
		unsigned i = 0;
		unsigned tries = 0;
		float currentRadius = MAX_VAL;
		this->lowestRadiusOfPath = MAX_VAL;
		Position<float> oldPos;
		Position<float> prevPos;
		Position<float> currentPos;
		Position<float> goalPos;

		// Set the goal if this is the second trial, because it means we found the goal the first time.
		if (phsPathNum > 1)
		{
			goalPos.SetXCoor(this->goalPos.GetXCoor());
			goalPos.SetYCoor(this->goalPos.GetYCoor());
		}

		if (this->isTransfered)
		{
			Position<int> prevPos = this->prevTransferedPos - this->msgDist->GetOffset();

			agent->loc.fromX = prevPos.GetXCoor();
			agent->loc.fromY = prevPos.GetYCoor();
		}

		action_t lastAction;
		int lastNumStep = 0;

		while ((res = isGoal(agent->loc.x, agent->loc.y, agentBrain, goal)) == FALSE) {								

			elements.push_back(StatsFileIO::ConvertToString(agent->loc.x));
			elements.push_back(StatsFileIO::ConvertToString(agent->loc.y));

			if (this->statsFile != NULL)
			{
				this->statsFile->WriteRow(elements);
				elements.clear();
			}

			if (phsPathNum == 1) {
				act = getRandomDirection(agentBrain, &agent->loc, NO_CHECK, xSize, ySize);
			}
			else {
				/*if (i == this->info.immediateMoves)
				{
					this->GetRelaxationSteps(agent, goalLoc, startLoc, this->info.immediateMoves, agentBrain, xSize, ySize, actArray);
				}*/				

				if (!this->isTransfered && generateFirstStepRandomly)
				{
					actArray[i] = getRandomDirection(agentBrain, &agent->loc, NO_CHECK, xSize, ySize);
				}
				//Re-calculate the immediate moves if we've executed the last action OR if our last action was changed OR if we didn't move
				else if ((i == this->info.immediateMoves) || (act.val != actArray[i - 1].val) || (agent->moved == FALSE_t))
				{
					this->GetNextNMoves(agent, goalLoc, startLoc, this->info.immediateMoves, agentBrain, xSize, ySize, actArray, actArray[i]);
					
					// If the agent didn't move on the last move, increase the number of tries.  Else, if it did move, clear it back to 0.
					if (agent->moved == FALSE_t)
					{
						tries++;
					}
					else
					{
						tries = 0;
					}

					//We have a new set of moves, start from the beginning of the array.
					i = 0;
				}

				////and delete these:
				////Re-calculate the immediate moves if we've executed the last action OR if our last action was changed OR if we didn't move
				//if ((i == this->info.immediateMoves) || (act.val != actArray[i].val) || (agent->moved == FALSE_t))
				//{
				//	this->GetNextNMoves(agent, goalLoc, startLoc, this->info.immediateMoves, agentBrain, xSize, ySize, actArray);
				//	
				//	//We have a new set of moves, start from the beginning of the array.
				//	i = 0;
				//}

				act = actArray[i];
			}  

			if (agent->step == lastNumStep)
			{
				// If the agent has tried to move, but has failed 3 times, then, generate a random move.
				if (tries >= 3)
				{
					strcpy (lastAction.name, act.name);
					lastAction.type = act.type;
					lastAction.val = act.val;
				}

				if (act.val == lastAction.val)
				{
					while (lastAction.val == act.val)
					{
						act = getRandomDirection(agentBrain, &agent->loc, NO_CHECK, xSize, ySize);
					}
				}								

			}
			else
			{
				lastNumStep = agent->step;
			}

			strcpy (lastAction.name, act.name);
			lastAction.type = act.type;
			lastAction.val = act.val;

			// Update the position three steps back
			oldPos.SetXCoor(agent->loc.fromX);
			oldPos.SetYCoor(agent->loc.fromY);

			if ((err = moveAgent (this->info.agentID, agent, act, xSize, ySize, agentBrain, agentPath, this->info.lrnMethod, startLoc, agentF)) != 
				NO_ERR) {
					break;
			}
			else
			{
				if (generateFirstStepRandomly && agent->moved == TRUE_t)
				{
					generateFirstStepRandomly = false;

					i = 0;
				}
				else if (agent->moved == TRUE_t)
				{
					//If there was no error, we've successfully taken a step, increment i;
					i++;

					// Calculate radius and check to see if it is the lowest so far.
					prevPos.SetXCoor(agent->loc.fromX);
					prevPos.SetYCoor(agent->loc.fromY);

					currentPos.SetXCoor(agent->loc.x);
					currentPos.SetYCoor(agent->loc.y);

					currentRadius = MLAgent::CalculateRadiusOfCurvature(oldPos, prevPos, currentPos, goalPos, 0, 0);

					if (currentRadius < this->lowestRadiusOfPath)
					{
						this->lowestRadiusOfPath = currentRadius;
					}
				}
			}
		}

		delete [] actArray;

		return res;
	}

	/*static*/ Framework::Position<float> MLAgent::CalculateGoalTermSecondDeriv(Framework::Position<float> goalPos, Framework::Position<float> currentPos, float absalonSquared)
	{
		Position<float> diff = goalPos - currentPos;
		Position<float> diffSquared = diff * diff;
		float distance = MLAgent::CalculateDistance(goalPos, currentPos);
		float gamma = MLAgent::CalculateDistance(goalPos, currentPos, 3);
		Position<float> secondDerv(MAX_VAL, MAX_VAL);
		
		if (distance != 0 && gamma != 0)
		{
			float firstTerm = 1 / distance;
			Position<float> secondTerm = diffSquared * (-1 / gamma);

			secondDerv = (secondTerm + firstTerm) * (absalonSquared / 2);
		}

		return secondDerv;
	}

	/*static*/ Framework::Position<float> MLAgent::CalculateObstacleTermSecondDeriv(Framework::Position<float> obstaclePos, Framework::Position<float> currentPos, float absalonSquared)
	{
		Position<float> diff = obstaclePos - currentPos;
		Position<float> diffSquared = diff * diff;
		float gamma0 = MLAgent::CalculateDistance(obstaclePos, currentPos, 3);
		float gamma1 = MLAgent::CalculateDistance(obstaclePos, currentPos, 5);
		Position<float> secondDerv(MAX_VAL, MAX_VAL);
		
		if (gamma0 != 0 && gamma1 != 0)
		{
			float secondTerm = 1 / gamma0;
			Position<float> firstTerm = diffSquared * 3 * (1 / gamma1);

			secondDerv = (firstTerm - secondTerm) * (absalonSquared / 2);
		}

		if (gamma0 == 0)
		{
			secondDerv = secondDerv * -1;
		}

		return secondDerv;
	}

	/*static*/ Framework::Position<float> MLAgent::CalculateSecondDeriv(Framework::Position<float> goalPos, Framework::Position<float> currentPos, float absalonSquared, std::list<Framework::Position<int> *> obstacles)
	{
		Position<float> goalTerm = MLAgent::CalculateGoalTermSecondDeriv(goalPos, currentPos, absalonSquared);
		Position<float> obstacleTerm(0,0);
		
		for (std::list<Position<int> *>::iterator iter = obstacles.begin(); iter != obstacles.end(); ++iter)
		{
			Position<float> obstacle((*iter)->GetXCoor(), (*iter)->GetYCoor());
			obstacleTerm = obstacleTerm + MLAgent::CalculateObstacleTermSecondDeriv(obstacle, currentPos, absalonSquared);
		}

		return goalTerm + obstacleTerm;
	}

	/*static*/ Framework::Position<float> MLAgent::CalculateGoalTermThirdDeriv(Framework::Position<float> goalPos, Framework::Position<float> currentPos, float absalonSquared)
	{
		Position<float> diff = goalPos - currentPos;
		Position<float> diffCubed = diff * diff * diff;
		float gamma0 = MLAgent::CalculateDistance(goalPos, currentPos, 3);
		float gamma1 = MLAgent::CalculateDistance(goalPos, currentPos, 5);
		Position<float> thirdDerv(MAX_VAL, MAX_VAL);
		
		if (gamma0 != 0 && gamma1 != 0)
		{
			Position<float> firstTerm = diff * (1 / gamma0);
			Position<float> secondTerm = diffCubed * (-1 / gamma1);

			thirdDerv = (secondTerm + firstTerm) * ((3 * absalonSquared) / 2);
		}

		if (gamma1 == 0)
		{
			thirdDerv = thirdDerv * -1;
		}

		return thirdDerv;
	}

	/*static*/ Framework::Position<float> MLAgent::CalculateObstacleTermThirdDeriv(Framework::Position<float> obstaclePos, Framework::Position<float> currentPos, float absalonSquared)
	{
		Position<float> diff = obstaclePos - currentPos;
		Position<float> diffCubed = diff * diff * diff;
		float gamma0 = MLAgent::CalculateDistance(obstaclePos, currentPos, 5);
		float gamma1 = MLAgent::CalculateDistance(obstaclePos, currentPos, 7);
		Position<float> thirdDerv(MAX_VAL, MAX_VAL);
		
		if (gamma0 != 0 && gamma1 != 0)
		{
			Position<float> secondTerm = diff * 3 * (1 / gamma0);
			Position<float> firstTerm = diffCubed * 5 * (1 / gamma1);

			thirdDerv = (firstTerm - secondTerm) * ((3 * absalonSquared) / 2);
		}

		if (gamma0 == 0)
		{
			thirdDerv = thirdDerv * -1;
		}

		return thirdDerv;
	}

	/*static*/ Framework::Position<float> MLAgent::CalculateThirdDeriv(Framework::Position<float> goalPos, Framework::Position<float> currentPos, float absalonSquared, std::list<Framework::Position<int> *> obstacles)
	{
		Position<float> goalTerm = MLAgent::CalculateGoalTermThirdDeriv(goalPos, currentPos, absalonSquared);
		Position<float> obstacleTerm(0,0);
		
		for (std::list<Position<int> *>::iterator iter = obstacles.begin(); iter != obstacles.end(); ++iter)
		{
			Position<float> obstacle((*iter)->GetXCoor(), (*iter)->GetYCoor());
			obstacleTerm = obstacleTerm + MLAgent::CalculateObstacleTermThirdDeriv(obstacle, currentPos, absalonSquared);
		}

		return goalTerm + obstacleTerm;
	}

	/*static*/ Framework::Position<float> MLAgent::DynamicPolicy(Framework::Position<float> goal, Framework::Position<float> newPosition, int totalNumVisited, float totalDiscountedReward, std::list<Framework::Position<int> *> obstacles, float absalon)
	{
		Framework::Logging::ILog *log = Framework::Logging::LogManager::Instance();
		Position<float> goalTerm;
		Position<float> obstacleTerm;
		float xDistance = goal.GetXCoor() - newPosition.GetXCoor();
		float yDistance = goal.GetYCoor() - newPosition.GetYCoor();
		float euclideanDistance = 0;
		float goalXTerm = 0;
		float goalYTerm = 0;
		float obstacleXTerm = 0;
		float obstacleYTerm = 0;
		float xDistanceToObstacle = 0;
		float yDistanceToObstacle = 0;
		float obstacleSqrTerm = 0;
		float visitedTerm = 0;
		float learningTerm = 0;

		//Position<float> centerPoint;
		//MLAgent::CalculateCenterPoint(start, newPosition, goal, centerPoint);
		//float curvatureRadiusTerm = -1 / MLAgent::CalculateDistance(centerPoint, newPosition);

		Position<float> temp;

		// Calculate the portion of the policy coming from the static obstacles
		for (std::list<Position<int> *>::iterator iter = obstacles.begin(); iter != obstacles.end(); ++iter)
		{
			Position<float> obstacle((*iter)->GetXCoor(), (*iter)->GetYCoor());
			obstacleTerm = obstacleTerm + MLAgent::CalculateDerivative(obstacle, newPosition, absalon, true, 3);
		}

		if (totalNumVisited != 0)
		{
			visitedTerm = 0;//absalon / totalNumVisited;
		}

		if (totalDiscountedReward != 0)
		{
			learningTerm = 0;//- absalon / totalDiscountedReward;
		}

		goalTerm = MLAgent::CalculateDerivative(goal, newPosition, absalon, false);

		temp = goalTerm + obstacleTerm + visitedTerm + learningTerm;

		if (!Validator::IsNull(log, NAME("log")))
		{
			log->Log(&typeid(MLAgent), "Calculating policy: [goal position: (%f, %f)], [Current position: (%f, %f)], [New position: (%f, %f)]", goal.GetXCoor(), goal.GetYCoor(), newPosition.GetXCoor(), newPosition.GetYCoor(), temp.GetXCoor(), temp.GetYCoor());
		}

		return temp;
	}

	/*static*/ float MLAgent::CalculateDistance(Framework::Position<float> pos0, Framework::Position<float> pos1, int optionalExponent)
	{
		float xDistance = pos0.GetXCoor() - pos1.GetXCoor();
		float yDistance = pos0.GetYCoor() - pos1.GetYCoor();
		float euclideanDistance = 0;

		// Make sure we don't take the square root of 0
		if (xDistance != 0)
		{
			euclideanDistance = sqrt( pow ( (xDistance * xDistance) + (yDistance * yDistance), optionalExponent ) );
		}
		else
		{
			if (yDistance != 0)
			{
				euclideanDistance = sqrt( pow ( (xDistance * xDistance) + (yDistance * yDistance), optionalExponent ) );
			}
		}

		return euclideanDistance;
	}
	
	/*static*/ Framework::Position<float> MLAgent::CalculateDerivative(Framework::Position<float> pos0, Framework::Position<float> pos1, float absalonSquared, bool isDerivativePositive, int optionalDistanceExponent)
	{
		float xDistance = pos0.GetXCoor() - pos1.GetXCoor();
		float yDistance = pos0.GetYCoor() - pos1.GetYCoor();
		float euclideanDistance = 0;
		float goalXTerm = 0;
		float goalYTerm = 0; 

		Position<float> temp(MAX_VAL, MAX_VAL);

		// Make sure we don't take the square root of 0
		euclideanDistance = MLAgent::CalculateDistance(pos0, pos1, optionalDistanceExponent); 

		if (euclideanDistance != 0)
		{
			if (xDistance != 0)
			{
				goalXTerm = ( ( absalonSquared * xDistance ) / ( 2 * euclideanDistance ) );			
			}
			else
			{
				goalXTerm = 0;
			}

			if (yDistance != 0)
			{
				goalYTerm = ( ( absalonSquared * yDistance ) / ( 2 * euclideanDistance ) );			
			}
			else
			{
				goalYTerm = 0;
			}

			if (isDerivativePositive)
			{
				temp.SetXCoor( goalXTerm );
				temp.SetYCoor( goalYTerm );
			}
			else
			{
				temp.SetXCoor( -goalXTerm );
				temp.SetYCoor( -goalYTerm );
			}
		}

		return temp;
	}

	// Getting the distance from the center point to any of the given points gives you the radius of curvature
	/*static*/ Framework::Position<float> MLAgent::CalculateCenterPoint(Framework::Position<float> start, Framework::Position<float> middle, Framework::Position<float> end)
	{
		float deltaY1 = middle.GetYCoor() - start.GetYCoor();
		float deltaY2 = end.GetYCoor() - middle.GetYCoor();
		float deltaX1 = middle.GetXCoor() - start.GetXCoor();
		float deltaX2 = end.GetXCoor() - middle.GetXCoor();

		float slope1 = 0;
		float slope2 = 0;

		// Set the initial radius to the middle point.  If the curve created by the 3 points happens to be a straight line than, the
		// center point is the middle point.
		float xc = middle.GetXCoor();
		float yc = middle.GetYCoor();

		if (deltaY1 != 0)
		{
			if (deltaX1 == 0)
			{
				slope1 = MAX_VAL;
			}
			else
			{
				slope1 = deltaY1/deltaX1;
			}
		}

		if (deltaY2 != 0)
		{
			if (deltaX2 == 0)
			{
				slope2 = MAX_VAL;
			}
			else
			{
				slope2 = deltaY2/deltaX2;
			}
		}

		// If the slopes are the same, then the agent is moving on a straight line.
		if (slope1 != slope2)
		{
			//Calculate the x portion of the center point using the equation:
			//			m1m2(y1 - y3) + m2(x1 + x2) - m1(x2 + x3)
			// Xc = -----------------------------------------------------
			//							2(m2 - m1)
			//
			// where m1 = slope1, and m2 = slope2
			float firstTerm = slope1*slope2*(start.GetYCoor() - end.GetYCoor());
			float secondTerm = slope2*(start.GetXCoor() + middle.GetXCoor());
			float thirdTerm = slope1*(middle.GetXCoor() + end.GetXCoor());

			xc = 0;

			if (slope1 != slope2)
			{
				xc = (firstTerm + secondTerm - thirdTerm) / (2*(slope2 - slope1));
			}
			else
			{
				xc = MAX_VAL;
			}

			//Calculate the y portion of the center point using the equation:
			//		 -1	  |       x1 + x2   |    y1 + y2
			// Yc = ----- | Xc - ---------- | + ---------
			//		  m1  |			 2	    |		2
			if (slope1 != 0)
			{
				firstTerm = -1/slope1;
			}
			else
			{
				firstTerm = -MAX_VAL;
			}

			secondTerm = xc - ((start.GetXCoor() + middle.GetXCoor()) / 2);
			thirdTerm = (start.GetYCoor() + middle.GetYCoor()) / 2;

			yc = (firstTerm * secondTerm) + thirdTerm;
		}

		return Position<float>(xc, yc);
	}

	/*static*/ Framework::Position<float> MLAgent::GetNextSubGoal(Framework::Position<float> currentPosition, Framework::Position<float> finalDestination, float lookAhead)
	{
		Position<float> subGoalPos;

		float diffX = abs(finalDestination.GetXCoor() - currentPosition.GetXCoor());
		float diffY = abs(finalDestination.GetYCoor() - currentPosition.GetYCoor());
		float xSign = 1;
		float ySign = 1;

		if (finalDestination.GetXCoor() < currentPosition.GetXCoor())
		{
			xSign = -1;
		}

		if (finalDestination.GetYCoor() < currentPosition.GetYCoor())
		{
			ySign = -1;
		}

		// Figure out the x and y positions.
		// If the difference in x is divisible by the look ahead, then
		if ((diffX / lookAhead) > 1)
		{
			// Add look ahead to x.
			subGoalPos.SetXCoor(currentPosition.GetXCoor() + xSign * lookAhead);

			// If the difference in y is divisible by the look ahead, then add look ahead to y.
			if ((diffY / lookAhead) > 1)
			{
				subGoalPos.SetYCoor(currentPosition.GetYCoor() + ySign * lookAhead);
			}
			else // If it isn't, then add half of the difference.
			{
				subGoalPos.SetYCoor(currentPosition.GetYCoor() + ySign * (diffY / 2));
			}
		}
		else // If the difference in x is not divisible, then check to see if the difference in y is divisible.
		{
			// If it is, then add look ahead to y and add half of the difference on x.
			if ((diffY / lookAhead) > 1)
			{
				subGoalPos.SetYCoor(currentPosition.GetYCoor() + ySign * lookAhead);
				subGoalPos.SetXCoor(currentPosition.GetXCoor() + xSign * (diffX / 2));
			}
			else // If neither of them are divisible by look ahead, then just take the final destination as the goal, since it should be within less than look ahead steps
			{
				subGoalPos.SetXCoor(finalDestination.GetXCoor());
				subGoalPos.SetYCoor(finalDestination.GetYCoor());
			}			
		}

		Framework::Logging::ILog *log = Framework::Logging::LogManager::Instance();

		if (!Validator::IsNull(log, NAME("log")))
		{
			log->Log(&typeid(MLAgent), "Calculating Next goal: (%f, %f), for Agent on: (%f, %f)", subGoalPos.GetXCoor(), subGoalPos.GetYCoor(), currentPosition.GetXCoor(), currentPosition.GetYCoor());
		}

		return subGoalPos;
	}
}