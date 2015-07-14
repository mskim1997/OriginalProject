/////////////////////////////////////////////////////////
//AgentFileIO.h
//This class is responsible for loading agent information from
//the AgentInfo.txt file.
//
//Manuel Madera
//10/31/09
/////////////////////////////////////////////////////////

#ifndef AgentFileIO_h
#define AgentFileIO_h

#include "MLTestProject.h"
#include "Framework\Maze.h"
#include <list>

using namespace std;

namespace MLTest
{
	class ML_API AgentFileIO
	{
	private:
		list<Framework::MLAgentInfo> agentList;

	public:
		AgentFileIO(string fileName);
		virtual ~AgentFileIO();

		Framework::MLAgentInfo GetAgentInfo();
		bool IsEmpty();
	};
}

#endif	//AgentFileIO_h