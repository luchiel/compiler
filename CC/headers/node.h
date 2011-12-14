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
    void printIndent(unsigned int depth, vector<bool>* branches, int level);
    void printIndentBeforeNode(unsigned int depth, vector<bool>* branches, int level);
    void setBranch(unsigned int depth, vector<bool>* branches);
    void makeNodeTop(unsigned int depth, vector<bool>* branches, const string& s, int level);
    void printNodeWithIndent(unsigned int depth, vector<bool>* branches, bool isLast, Node* node, int level);

public:
    virtual void out(unsigned int depth, vector<bool>* branches, int level = 0);
    virtual ~Node() {}
};

}

#endif
