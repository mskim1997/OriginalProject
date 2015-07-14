/////////////////////////////////////////////////////////
//MazeReceivedSubscription.cpp
//Defines the MazeReceivedSubscription
//
//Manuel Madera
//8/18/09
/////////////////////////////////////////////////////////

#include "MazeReceivedSubscription.h"
#include "Debug.h"

#ifdef PrintMemoryLeak
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif
#endif

namespace Framework
{   
	namespace Eventing
	{
		MazeReceivedSubscription::MazeReceivedSubscription(Maze *newMaze) 
			: maze(newMaze), type(Eventing::SubscriptionTypes::MAZE_RECEIVED), life(Eventing::SubscriptionTypes::NOTIFY_ONCE)
		{
		}

		MazeReceivedSubscription::~MazeReceivedSubscription() 
		{
		}

		Eventing::SubscriptionTypes::SubscriptionType MazeReceivedSubscription::GetSubscriptionType()
		{
			return this->type;
		}

		Eventing::SubscriptionTypes::SubscriptionLife MazeReceivedSubscription::GetSubscriptionLife()
		{
			return this->life;
		}

		void MazeReceivedSubscription::SetSubscriptionLife(Eventing::SubscriptionTypes::SubscriptionLife life)
		{
			this->life = life;
		}

		Maze *MazeReceivedSubscription::GetMaze() const
		{
			return this->maze;
		}
	}
}