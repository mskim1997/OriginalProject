/////////////////////////////////////////////////////////
//LinkNode.h
//Defines the specific Node implementations.
//
//Manuel Madera
//7/10/10
/////////////////////////////////////////////////////////

#ifndef LinkNode_h
#define LinkNode_h

#include "Framework\Nodes.h"
#include "IEventDistributor.h"
#include <list>
#include <pthread.h>
#include <map>

using namespace std;

namespace Framework
{
	class LinkNode : public Framework::Node
    {
    private:
		sem_t doneSem;

    public:
		LinkNode(unsigned rank, bool allowSharing);
		LinkNode(unsigned rank, bool allowSharing, Agents::IAgent *newAgent);
        virtual ~LinkNode();

		virtual void Wait();

	private:
		void Init();

		static void TerminateCallback(void *ownerObject, Framework::Packages::IPackage *package);
    };

}

#endif  //LinkNode_h