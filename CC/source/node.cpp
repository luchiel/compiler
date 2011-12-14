#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include "node.h"

using namespace std;

namespace LuCCompiler
{

void Node::out(unsigned int depth, vector<bool>* branches, int level)
{
    while(level-- > 0)
        cout << '\t';
}

void Node::setBranch(unsigned int depth, vector<bool>* branches)
{
    if(branches->size() < depth + 1)
        branches->push_back(false);
    else
        (*branches)[depth] = false;
}

void Node::printIndentBeforeNode(unsigned int depth, vector<bool>* branches, int level)
{
    printIndent(depth + 2, branches, level);
    cout << endl;
}

void Node::printIndent(unsigned int depth, vector<bool>* branches, int level)
{
    Node::out(depth, branches, level);
    unsigned int i = 0;
    while(depth != 0 && i < depth - 1)
    {
        cout << (branches->size() > i && (*branches)[i] == false ? "| " : "  ");
        i++;
    }
}

void Node::makeNodeTop(
    unsigned int depth, vector<bool>* branches, const string& s, int level
)
{
    setBranch(depth, branches);
    printIndent(depth, branches, level);
    cout << (depth == 0 ? "" : "+-") << "{" << s << "}" << endl;
}

void Node::printNodeWithIndent(
    unsigned int depth, vector<bool>* branches, bool isLast, Node* node, int level
)
{
    printIndentBeforeNode(depth, branches, level);
    (*branches)[depth] = isLast;
    node->out(depth + 1, branches, level);
}

}
