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
#include "StatsFileIO.h"

#define ML_PI (float)3.14159
namespace MLContinuous
{
	typedef Framework::Position<float> (*Algorithm)(Framework::Position<float> goal, Framework::Position<float> newPosition, int totalNumVisited, float totalDiscountedReward, std::list<Framework::Position<int> *> obstacles, float absalon);

	class ML_API MLAgent : public Framework::Agents::Agent, public Framework::Eventing::ISubscriber
	{
	private:
		StatsFileIO *statsFile;
		AgentInfo info;
		unsigned nodeTransferedTo;		
		int totalNumberOfStepsTaken;
		IMaze *maze;	
		bool exploreLocalFirst;
		dirVal_t lastDir;
		int currentPhsPathNum;
		bool canNotifyWhenDone;
		bool isGoalOnAnotherNode;
		bool isGoalOnThisNode;
		float lowestRadiusOfPath;
		int bestReward;
		unsigned bestPath;
		float bestRadius;
		unsigned runNumber;
		unsigned transferNumber;
		unsigned currentTransferId;	//used only by transfered agents.
		Framework::Position<int> goalPos;
		Framework::Position<int> prevTransferedPos;
		std::list<Framework::Position<int> *> obstaclePosList; 
		std::list<Framework::Position<float> *> directionVectors;
		action_t moveToGoalNodeAction;
		Framework::Logging::ILog *log;
		sem_t agentTransferedSem;

		sem_t transferedAgentDoneSem;
	public:
		//MLAgent();
		MLAgent(unsigned packageNode, unsigned packageDest, char *agentFile, unsigned ySize, unsigned xSize, IMaze *nodeMaze, state_t **agentBrain, float minRadiusOfCurv, float minTurningAngle, float slidingWindow, unsigned immediateMoves, unsigned runNumber, bool transfered = false, bool localFirst = false);

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
		float storeRewards (float gamma, agent_t *agentPath, state_t **agentBrain, int agentBrainSize, int fVerb, FILE *agentF, int numStepsTaken);
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
		action_t ConvertDirectionToAction(Framework::Position<float> direction);
		action_t takeActionOnGradientDescent (Framework::Position<float> goal, Framework::Position<float>& currentPosition, int totalNumVisited, float totalDiscountedReward, Algorithm algorithm, float stepSize);
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

		bool_t MoveToGoal(agent_t *agent, state_t **agentBrain, agent_t* agentPath, int xSize, int ySize, char goal, state_t* goalLoc, state_t* startLoc, int phsPathNum, FILE* agentF, /*out*/error_t& err);

		error_t TakeStep(agent_t *agent, state_t **agentBrain, agent_t* agentPath, int x, int y, int xSize, int ySize, state_t* startLoc, action_t direction, FILE* agentF, learnMethod_t lrnMethod);

		void GoalFound(agent_t *agent, state_t* goalLoc, bool_t res, error_t err, /*out*/ bool& exploreLocalFirstTemp, /*out*/ bool& stop);

		action_t GetNextMove(Framework::Position<float> currentPos, Framework::Position<float> goalPos, int totalNumVisited, float totalDiscountedReward);

		void GetNextNMoves(agent_t *agent, state_t* goalLoc, state_t* startLoc, unsigned n, state_t **agentBrain, int xSize, int ySize, action_t* actArray, action_t lastAction);

		float CalculateRadiusOfCurvature(Framework::Position<float> prevPos, Framework::Position<float> currPos, Framework::Position<float> nextPos, Framework::Position<float> goalPos, int totalNumVisited, float totalDiscountedReward);

		bool IsTurningAngleAcceptable(Framework::Position<float> vector0, Framework::Position<float> vector1);

		void GetRelaxationSteps(agent_t *agent, state_t* goalLoc, state_t* startLoc, unsigned n, state_t **agentBrain, int xSize, int ySize, action_t* actArray);

		action_t CalculateActionAndStep(Framework::Position<float> currPos, Framework::Position<float> & step);

		static Framework::Position<float> DynamicPolicy(Framework::Position<float> goal, Framework::Position<float> newPosition, int totalNumVisited, float totalDiscountedReward, std::list<Framework::Position<int> *> obstacles, float absalon);

		static float CalculateDistance(Framework::Position<float> pos0, Framework::Position<float> pos1, int optionalExponent = 1);

		static Framework::Position<float> CalculateDerivative(Framework::Position<float> pos0, Framework::Position<float> pos1, float absalonSquared, bool isDerivativePositive, int optionalDistanceExponent = 1);

		static Framework::Position<float> CalculateCenterPoint(Framework::Position<float> start, Framework::Position<float> middle, Framework::Position<float> end);

		static Framework::Position<float> GetNextSubGoal(Framework::Position<float> currentPosition, Framework::Position<float> finalDestination, float lookAhead);

		static Framework::Position<float> CalculateGoalTermSecondDeriv(Framework::Position<float> goalPos, Framework::Position<float> currentPos, float absalonSquared);

		static Framework::Position<float> CalculateObstacleTermSecondDeriv(Framework::Position<float> obstaclePos, Framework::Position<float> currentPos, float absalonSquared);

		static Framework::Position<float> CalculateSecondDeriv(Framework::Position<float> goalPos, Framework::Position<float> currentPos, float absalonSquared, std::list<Framework::Position<int> *> obstacles);

		static Framework::Position<float> CalculateGoalTermThirdDeriv(Framework::Position<float> goalPos, Framework::Position<float> currentPos, float absalonSquared);

		static Framework::Position<float> CalculateObstacleTermThirdDeriv(Framework::Position<float> obstaclePos, Framework::Position<float> currentPos, float absalonSquared);

		static Framework::Position<float> CalculateThirdDeriv(Framework::Position<float> goalPos, Framework::Position<float> currentPos, float absalonSquared, std::list<Framework::Position<int> *> obstacles);

		static float CalculateDifferenceAngle(Framework::Position<float> vector0, Framework::Position<float> vector1);
	};
}

#endif	//MLAgent_h