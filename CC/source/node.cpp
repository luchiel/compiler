#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include "node.h"

using namespace std;

namespace LuCCompiler
{

void Node::setBranch(unsigned int depth, vector<bool>* branches)
{
    if(branches->size() < depth + 1)
        branches->push_back(false);
    else
        (*branches)[depth] = false;
}

void Node::printRibsBeforeNode(unsigned int depth, vector<bool>* branches)
{
    printRibs(depth + 2, branches);
    cout << endl;
}

void Node::printRibs(unsigned int depth, vector<bool>* branches)
{
    unsigned int i = 0;
    while(depth != 0 && i < depth - 1)
    {
        cout << (branches->size() > i && (*branches)[i] == false ? "| " : "  ");
        i++;
    }
}

void Node::makeNodeTop(unsigned int depth, vector<bool>* branches, const string& s)
{
    setBranch(depth, branches);
    printRibs(depth, branches);
    cout << (depth == 0 ? "" : "+-") << "{" << s << "}" << endl;
}

void Node::printNodeWithRibs(unsigned int depth, vector<bool>* branches, bool isLast, Node* node)
{
    printRibsBeforeNode(depth, branches);
    (*branches)[depth] = isLast;
    node->out(depth + 1, branches);
}

}
