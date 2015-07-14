/////////////////////////////////////////////////////////
//Maze.h
//Defines the Maze class.
//
//Manuel Madera
//8/22/09
/////////////////////////////////////////////////////////

#ifndef Maze_h
#define Maze_h

#include "Framework.h"
#include "Debug.h"
#include "Validator.h"
#include <iostream>

using namespace std;

namespace Framework
{
	/////////////////////////////////////////////////////////
	//MLAgentInfo
	/////////////////////////////////////////////////////////
	class FRAMEWORK_API MLAgentInfo
	{
	private:
		char start;
		char goal;
		unsigned ownerNodeId;

	public:
		MLAgentInfo()
		{
			start = 'S';
			goal = 'G';
		}

		MLAgentInfo(char newStart, char newGoal, unsigned nodeId) : start(newStart), goal(newGoal), ownerNodeId(nodeId)
		{
		}

		virtual ~MLAgentInfo()
		{
		}

		void SetStart(char newStart)
		{
			this->start = newStart;
		}

		void SetGoal(char newGoal)
		{
			this->goal = newGoal;
		}

		void SetNodeId(unsigned newNodeId)
		{
			this->ownerNodeId = newNodeId;
		}

		char GetStart()
		{
			return this->start;
		}

		char GetGoal()
		{
			return this->goal;
		}

		unsigned GetNodeId()
		{
			return this->ownerNodeId;
		}
	};

	/////////////////////////////////////////////////////////
	//Maze
	/////////////////////////////////////////////////////////
	class FRAMEWORK_API Maze
	{
	private:
		char **maze;

		unsigned xSize;
		unsigned ySize;

	public:
		Maze() : ySize(0), xSize(0), maze(NULL)
		{
		}

		Maze(unsigned newYSize, unsigned newXSize, char **newMaze) : ySize(newYSize), xSize(newXSize), maze(newMaze)
		{
		}

		virtual ~Maze()
		{
			for (unsigned i = 0; i < this->ySize; i++)
			{
				delete[] maze[i];
				maze[i] = NULL;
			}

			delete maze; 
			maze = NULL;
		}

		void SetMaze(unsigned newYSize, unsigned newXSize, char **newMaze)
		{
			this->ySize = newYSize;
			this->xSize = newXSize;

			this->maze = newMaze;
		}

		char **Clone()
		{
			char **tempMaze = new char *[this->ySize];

			for (unsigned i = 0; i < this->ySize; i++)
			{
				tempMaze[i] = new char[this->xSize];
			}

			for (unsigned i = 0; i < this->ySize; i++)
			{
				for (unsigned j = 0; j < this->xSize; j++)
				{
					tempMaze[i][j] = this->maze[i][j];
				}
			}

			return tempMaze;
		}

		char **GetMaze()
		{
			return this->maze;
		}

		unsigned GetYSize()
		{
			return this->ySize;
		}

		unsigned GetXSize()
		{
			return this->xSize;
		}

		void PrintMaze()
		{
			for (unsigned i = 0; i < this->ySize; i++)
			{
				for (unsigned j = 0; j < this->xSize; j++)
				{
					cout << this->maze[i][j];
				}

				cout << endl;
			}
		}
	};
}

#endif	//Maze_h