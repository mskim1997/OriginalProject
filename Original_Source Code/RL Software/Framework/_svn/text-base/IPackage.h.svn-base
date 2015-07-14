/////////////////////////////////////////////////////////
//IPackage.h
//Defines the IPackage interface.
//
//Manuel Madera
//5/8/09
/////////////////////////////////////////////////////////

#ifndef IPackage_h
#define IPackage_h

namespace WorldTest
{
    namespace PackageType
    {
        enum Package
        {
            NONE,
            AGENT
        };
    }
        
    class IPackage
    {
    public:
        virtual ~IPackage() {}
                        
        virtual void Serialize() = 0;
        virtual void Deserialize() = 0;
        virtual void Send(int messageTag) = 0;
        virtual void Get(int sourceRank, int messageTag) = 0;
        virtual void SetDestination(int destRank) = 0;
        virtual void SetSource(int sourceRank) = 0;
        virtual int GetDestination() = 0;
        virtual int GetSource() = 0;
        virtual PackageType::Package GetType() = 0;
    };

    class EmptyPackage : public IPackage
    {
    private:
        int sourceRank;
        int destRank;
        PackageType::Package myType;
    public:
        EmptyPackage() {}
        EmptyPackage(int source, int dest) 
        {
            myType = PackageType::NONE;
            sourceRank = source;
            destRank = dest;
        }

        virtual ~EmptyPackage() {}
                        
        void Serialize() {}
        void Deserialize() {}
        void Send(int messageTag) {}
        void Get(int sourceRank, int messageTag) {}
        void SetDestination(int destRank) { this->destRank = destRank; }
        void SetSource(int sourceRank) { this->sourceRank = sourceRank; }
        int GetDestination() { return destRank;}
        int GetSource() { return sourceRank; }
        PackageType::Package GetType() { return myType; }
    };
}

#endif  //IPackage_h