/////////////////////////////////////////////////////////
//MazeReceivedSubscription.h
//Defines the MazeReceivedSubscription
//
//Manuel Madera
//8/18/09
/////////////////////////////////////////////////////////

#include "ISubscription.h"
#include "Maze.h"

namespace Framework
{   
	namespace Eventing
	{
		//MazeReceived Subscription
		class FRAMEWORK_API MazeReceivedSubscription : public Eventing::ISubscription
		{
		private:
			Maze *maze;

			Eventing::SubscriptionTypes::SubscriptionType type;
			Eventing::SubscriptionTypes::SubscriptionLife life;

		public:
			MazeReceivedSubscription(Maze *newMaze);
			virtual ~MazeReceivedSubscription();

			Eventing::SubscriptionTypes::SubscriptionType GetSubscriptionType();

			Eventing::SubscriptionTypes::SubscriptionLife GetSubscriptionLife();

			void SetSubscriptionLife(Eventing::SubscriptionTypes::SubscriptionLife life);

			Maze *GetMaze() const;
		};
	}
}