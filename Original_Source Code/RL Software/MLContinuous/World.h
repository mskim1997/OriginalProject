/////////////////////////////////////////////////////////
//World.h
//Defines the World class which serves as the entry point
//for all the nodes.
//Manuel Madera
//5/5/09
/////////////////////////////////////////////////////////

#ifndef World_h
#define World_h

#include "Framework/INode.h"
#include "MLTestProject.h"

namespace MLContinuous
{
	typedef struct
	{
		unsigned rank;
		int size;
		unsigned numAgents;
		string statsFile;
		bool allowSharing;
		string agentFile;
		string mazeFile;
		unsigned mazeXSize;
		unsigned mazeYSize;
		float slidingWindowLookAhead;
		float minRadius;
		float minTurningAngle;
		unsigned immediateMoves;
		unsigned runNumber;
	} UserInput;

    class ML_API World
    {        
    public:
        World();
        virtual ~World();
        
        void Run(UserInput userInput);
    };
}
    
#endif  //World_h