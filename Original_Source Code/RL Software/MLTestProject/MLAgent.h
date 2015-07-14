/////////////////////////////////////////////////////////
//MLAgent.h
//Defines the Machine Learning Agent class.
//
//Manuel Madera
//8/8/09
/////////////////////////////////////////////////////////

#ifndef MLAgent_h
#define MLAgent_h

#include "Framework/Agent.h"
#include "ml.h"
#include "MLTestProject.h"
#include "IMaze.h"
#include "Framework\ISubscriber.h"
#include "Framework\Position.h"
#include "Framework\LogManager.h"

namespace MLTest
{
	class ML_API MLAgent : public Framework::Agents::Agent, public Framework::Eventing::ISubscriber
	{
	private:
		AgentInfo info;
		unsigned nodeTransferedTo;
		IMaze *maze;	
		bool exploreLocalFirst;
		dirVal_t lastDir;
		int currentPhsPathNum;
		bool canNotifyWhenDone;
		bool isGoalOnAnotherNode;
		Framework::Position<int> goalPos;
		action_t moveToGoalNodeAction;
		Framework::Logging::ILog *log;
		sem_t agentTransferedSem;

		sem_t transferedAgentDoneSem;
	public:
		//MLAgent();
		MLAgent(unsigned packageNode, unsigned packageDest, char *agentFile, unsigned ySize, unsigned xSize, IMaze *nodeMaze, state_t **agentBrain, bool transfered = false, bool localFirst = false);

		virtual ~MLAgent();

		virtual void Run();

		virtual void SetMaze(int xMazeSize, int yMazeSize, char **maze);

		virtual void Notify(const Framework::Eventing::ISubscription *subscription);

		//This method does not set the agent id.  To do that use Register or RegisterTransferAgent
		void SetAgentInfo(Framework::Packages::MazeAgentInfo agentInfo);

		void SetAgentFile(char *newAgentFile);

		void SignalAgentTransfered();
	private:
		int ml(AgentInfo info);
		bool_t applyML (float alpha, float gamma, float lambda, int taw, int xSize, int ySize, 
                int maxPhsPaths, int maxLrnPaths, learnMethod_t lrnMethod, 
                bool_t cntrlFirstOn,  bool_t initFullExploreOn, bool_t phsGreedyOn, bool_t lrnGreedyOn, 
                bool_t constTawOn, int nAhead, char *agentFile, int pathFileMode, 
                char *cntrlFirstFile, int seedVal, bool_t keepSeed, float w1, float w2, float w3, int gMaxLrnPaths,
				int fVerb, FILE *agentF, char start, char goal, unsigned agentID, state_t **agentBrain);
		action_t getRandomDirection (state_t **agentBrain, state_t *s0, actCheck_t ac, int xSize, int ySize);
		float getDistAB (int xa, int ya, int xb, int yb);
		dirVal_t getDirectionValue (char firstLetter);
		void readMazeIntoMatrix (char **charMaze, int xSize, int ySize, state_t **maze, char start, char goal);
		bool_t findLocation (int xSize, int ySize, state_t **maze, locState_t locState, 
					 state_t *location);
		error_t moveAgent (unsigned id, agent_t *agent, action_t direction, int xSize, int ySize, 
					   state_t **agentBrain, agent_t *agentPath, learnMethod_t lrnMethod, state_t *startLoc, FILE *agentF);
		bool_t isGoal (int x, int y, state_t **agentBrain, char goal);
		bool_t isLocMatch (int x, int y, state_t loc);
		void storeDistances (state_t **agentBrain, int xSize, int ySize, state_t *goalLoc, char start);
		float storeRewards (float gamma, agent_t *agentPath, state_t **agentBrain, int numSteps, int fVerb, FILE *agentF);
		float getQs0a0 (float alpha, float gamma, float lambda, state_t **agentBrain,
						state_t *s, action_t *a, int nAhead, int gLrnPathNum, int gMaxLrnPaths, int fVerb,
						FILE *agentF, FILE *rqRwrdF);
		bool_t doLearning (float alpha, float gamma, float lambda, state_t **agentBrain, 
						   state_t *s0, action_t *a0, int xSize, int ySize, learnMethod_t lrnMethod,
						   int nAhead, state_t startLoc, state_t goalLoc, agent_t *lrnPath,
						   float w1, float w2, float w3, int gLrnPathNum, int gMaxLrnPaths, int fVerb,
						   FILE *agentF, FILE *rqRwrdF);
		side_t whichSide (state_t *s0);
		bool_t isExploredAll (state_t **agentBrain, int xSize, int ySize);
		action_t takeActionOnBoltz (state_t **agentBrain, state_t *s0, bool_t checkPrev, action_t a0Prev, 
               					float taw, int xSize, int ySize, learnMethod_t lrnMethod, 
									float w1, float w2, float w3, int fVerb,
									FILE *agentF);
		action_t takePhsActionVstedDist (state_t **agentBrain, state_t *s0, int xSize, int ySize,
										 state_t *goalLoc, learnMethod_t lrnMethod, int fVerb,
										 FILE *agentF);
		action_t takePhsActionDistVsted (state_t **agentBrain, state_t *s0, int xSize, int ySize,
										 state_t *goalLoc, learnMethod_t lrnMethod, int fVerb, FILE *agentF);
		action_t takeActionFromList (state_t **agentBrain, state_t *s0, int fVerb, FILE *agentF);
		point_t getClosestToStart (state_t **agentBrain, state_t s0, int xSize, int ySize, 
								   state_t *startLoc);
		state_t makeHypoStep (state_t s, action_t a, state_t **agentBrain, 
							  int xSize, int ySize, state_t goalLoc);
		void printHelp (char *execname, const char *message);

		void MoveTo(unsigned dest, char newStartLoc);
	};
}

#endif	//MLAgent_h