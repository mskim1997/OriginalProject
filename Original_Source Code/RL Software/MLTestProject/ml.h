/////////////////////////////////////////////////////////
//ml.h
//Defines the Machine Learning algorithm, and data types.
//
//Manuel Madera
//8/8/09
/////////////////////////////////////////////////////////

#ifndef ml_h
#define ml_h

#include "ml_gencon.h"
#include "ml_genlib.h"

typedef struct
{
	int xSize;						// Number of columns in maze matrix.
	int ySize;						// Number of rows in maze matrix.
	char **maze;					// The maze
	//char *mazeFile;					// Maze input file.
	char *agentFile;				// Agent's output file.
	int maxPhsPaths;				// Number of physical paths.
	bool_t maxPhsPathsOn;			// Flag.
	int maxLrnPaths;				// Number of learning paths.
	bool_t maxLrnPathsOn;			// Flag.
	learnMethod_t lrnMethod;        // Method.
	bool_t cntrlFirstOn;			// Control-first-path flag.
	bool_t initFullExploreOn;       // Full-explodation flag.
	bool_t lrnGreedyOn;				// Learning-greedy flag.
	bool_t phsGreedyOn;				// Physical-greedy flag.
	bool_t constTawOn;				// Constant-taw flag.
	bool_t w1On;					// w1 flag.
	bool_t w2On;					// w2 flag.
	bool_t w3On;					// w3 flag.
	int nAhead;						// Number of steps forward.  
	pathFileMode_t pathFileMode;	// Path-file mode.
	char *cntrlFirstFile;			// Controlled First-path filename.
	bool_t seedOn;					// Keep-seed flag.
	int seedVal;					// Seed value.
	float alpha;					// Alpha.
	float gamma;					// Gamma.
	float lambda;					// Lambda.
	float taw;						// Initial Taw.
	float w1;						// Q-Value weigth.
	float w2;						// Distance-to-Goal weight.
	float w3;						// Visited-number weight.
	char *hm;						// Help message.
	int op;							// User's option.
	char start;						// Agent's start character
	char goal;						// Agent's goal character
	unsigned agentID;				// Agent's id
	state_t **agentBrain;			// Agent matrix.
} AgentInfo;

//int ml(AgentInfo info);

#endif	//ml_h