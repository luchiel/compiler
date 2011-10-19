#include <stdio.h>
#include <iostream>
#include <vector>
#include "expression.h"
#include "operations.h"

namespace LuCCompiler
{

void setBranch(unsigned int depth, vector<bool>* branches)
{
    if(branches->size() < depth + 1)
        branches->push_back(false);
    else
        (*branches)[depth] = false;
}

void printNodeWithRibs(Node* node, unsigned int depth, vector<bool>* branches)
{
    node->printRibs(depth + 2, branches);
    cout << endl;
    node->out(depth + 1, branches);
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

void IdentNode::out(unsigned int depth, vector<bool>* branches)
{
    printRibs(depth, branches);
    cout << (depth == 0 ? "" : "+-") << "{" << _name << "}" << endl;
}

void StringNode::out(unsigned int depth, vector<bool>* branches)
{
    printRibs(depth, branches);
    cout << (depth == 0 ? "" : "+-") << "{string " << _value << "}" << endl;
}

void CharNode::out(unsigned int depth, vector<bool>* branches)
{
    printRibs(depth, branches);
    cout << (depth == 0 ? "" : "+-") << "{char " << _value << "}" << endl;
}

void IntNode::out(unsigned int depth, vector<bool>* branches)
{
    printRibs(depth, branches);
    cout << (depth == 0 ? "" : "+-") << "{int " << _value << "}" << endl;
}

void FloatNode::out(unsigned int depth, vector<bool>* branches)
{
    printRibs(depth, branches);
    cout << (depth == 0 ? "" : "+-") << "{float " << _value << "}" << endl;
}

void PostfixNode::out(unsigned int depth, vector<bool>* branches)
{
    setBranch(depth, branches);

    printRibs(depth, branches);
    string x(_type == TOK_INC || _type == TOK_DEC ? "x" : "");
    cout << (depth == 0 ? "" : "+-");
    cout << "{" << x << operationName(_type) << "}" << endl;

    printRibs(depth + 2, branches);
    cout << endl;

    if(_tail == NULL)
        (*branches)[depth] = true;

    _only->out(depth + 1, branches);

    if(_tail == NULL)
        return;

    printRibs(depth + 2, branches);
    cout << endl;

    (*branches)[depth] = true;

    _tail->out(depth + 1, branches);
}

void UnaryNode::out(unsigned int depth, vector<bool>* branches)
{
    setBranch(depth, branches);

    printRibs(depth, branches);
    string x(_type == TOK_INC || _type == TOK_DEC ? "x" : "");
    cout << (depth == 0 ? "" : "+-");
    cout << "{" << operationName(_type) << x << "}" << endl;

    printRibs(depth + 2, branches);
    cout << endl;

    (*branches)[depth] = true;

    _only->out(depth + 1, branches);
}

void BinaryNode::out(unsigned int depth, vector<bool>* branches)
{
    setBranch(depth, branches);

    printRibs(depth, branches);
    cout << (depth == 0 ? "" : "+-") << "{" << operationName(_type) << "}" << endl;
    printNodeWithRibs(_left, depth, branches);

    printRibs(depth + 2, branches);
    cout << endl;

    (*branches)[depth] = true;

    _right->out(depth + 1, branches);
}

void TernaryNode::out(unsigned int depth, vector<bool>* branches)
{
    setBranch(depth, branches);

    printRibs(depth, branches);
    cout << (depth == 0 ? "" : "+-") << "{" << operationName(_type) << "}" << endl;
    printNodeWithRibs(_if, depth, branches);
    printNodeWithRibs(_then, depth, branches);

    printRibs(depth + 2, branches);
    cout << endl;

    (*branches)[depth] = true;

    _else->out(depth + 1, branches);
}

}
