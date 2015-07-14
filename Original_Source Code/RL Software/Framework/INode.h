/////////////////////////////////////////////////////////
//INode.h
//Defines the INode interface.
//
//Manuel Madera
//5/5/09
/////////////////////////////////////////////////////////

#ifndef INode_h
#define INode_h

#include "Framework.h"
#include "IAgent.h"
#include "IMessageDistributor.h"
#include "IEventDistributor.h"

namespace Framework
{
    class FRAMEWORK_API INode
    {
    public:
        virtual ~INode() {}
        
        virtual void Run() = 0;

        virtual void Wait() = 0;

        virtual void Terminate() = 0;

		virtual void AddAgent(Framework::Agents::IAgent *newAgent) = 0;

		virtual Messaging::IMessageDistributor *GetMessageDistributor() = 0;

		virtual Eventing::IEventDistributor *GetEventDistributor() = 0;
        //virtual void RemoveAgent() = 0;
    };

}

#endif  //INode_h
