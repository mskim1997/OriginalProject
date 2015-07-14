/////////////////////////////////////////////////////////
//AgentFileIO.h
//This class is responsible for loading agent information from
//the AgentInfo.txt file.
//
//Manuel Madera
//10/31/09
/////////////////////////////////////////////////////////

#include "AgentFileIO.h"
#include <iostream>
#include <fstream>
#include "Framework\Debug.h"

#ifdef PrintMemoryLeak
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif
#endif

namespace MLContinuous
{
	AgentFileIO::AgentFileIO(string fileName)
	{
		unsigned node = 0;
		char start = 'S';
		char goal = 'G';

		ifstream file;
		
		file.open(fileName.c_str());//("AgentInfo.txt");	//open the file

		if (file.is_open())
		{
			while(!file.eof())
			{
				file >> node;

				if (file.eof())
				{
					break;
				}

				file >> start;

				if (file.eof())
				{
					break;
				}

				file >> goal;

				this->agentList.push_back(Framework::MLAgentInfo(start, goal, node));
			}

			file.close();
		}
	}

	AgentFileIO::~AgentFileIO()
	{
		this->agentList.clear();
	}

	bool AgentFileIO::IsEmpty()
	{
		return this->agentList.size() <= 0;
	}

	Framework::MLAgentInfo AgentFileIO::GetAgentInfo()
	{
		Framework::MLAgentInfo agentInfo = this->agentList.front();
		this->agentList.pop_front();

		return agentInfo;
	}
}