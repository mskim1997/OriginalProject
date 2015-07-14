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

namespace WorldTest
{
    class World
    {
    private:
		Framework::INode *pRootNode;
        
    public:
        World();
        virtual ~World();
        
        void Run(unsigned rank, int size);
    };
}
    
#endif  //World_h
