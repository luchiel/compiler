#ifndef NODE_H
#define NODE_H

#include <vector>

using namespace std;

namespace LuCCompiler
{

class Node
{
protected:
    void printRibs(unsigned int depth, vector<bool>* branches);
    void printRibsBeforeNode(unsigned int depth, vector<bool>* branches);
    void setBranch(unsigned int depth, vector<bool>* branches);

public:
    virtual void out(unsigned int depth, vector<bool>* branches) {}
    virtual ~Node() {}
};

}

#endif