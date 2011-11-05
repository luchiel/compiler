#ifndef NODE_H
#define NODE_H

#include <vector>
#include <string>

using namespace std;

namespace LuCCompiler
{

class Node
{
protected:
    void printRibs(unsigned int depth, vector<bool>* branches);
    void printRibsBeforeNode(unsigned int depth, vector<bool>* branches);
    void setBranch(unsigned int depth, vector<bool>* branches);
    void makeNodeTop(unsigned int depth, vector<bool>* branches, const string& s);
    void printNodeWithRibs(unsigned int depth, vector<bool>* branches, bool isLast, Node* node);

public:
    virtual void out(unsigned int depth, vector<bool>* branches) {}
    virtual ~Node() {}
};

}

#endif
