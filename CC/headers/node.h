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
    void printRibs(unsigned int depth, vector<bool>* branches, int indent);
    void printRibsBeforeNode(unsigned int depth, vector<bool>* branches, int indent);
    void setBranch(unsigned int depth, vector<bool>* branches);
    void makeNodeTop(unsigned int depth, vector<bool>* branches, const string& s, int indent);
    void printNodeWithRibs(unsigned int depth, vector<bool>* branches, bool isLast, Node* node, int indent);

public:
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
    virtual ~Node() {}
};

}

#endif
