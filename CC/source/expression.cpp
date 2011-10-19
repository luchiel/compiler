#include <stdio.h>
#include <iostream>
#include "expression.h"
#include "operations.h"

namespace LuCCompiler
{

void printNodeWithRibs(Node* node, int depth)
{
    node->printRibs(depth + 2);
    cout << endl;
    node->out(depth + 1);
}

void Node::printRibs(int depth)
{
    while(depth-- > 1)
        cout << "| ";
}

void IdentNode::out(int depth)
{
    printRibs(depth);
    cout << (depth == 0 ? "" : "+-") << "{" << _name << '}' << endl;
}

void StringNode::out(int depth)
{
    printRibs(depth);
    cout << (depth == 0 ? "" : "+-") << "{string " << _value << '}' << endl;
}

void CharNode::out(int depth)
{
    printRibs(depth);
    cout << (depth == 0 ? "" : "+-") << "{char " << _value << '}' << endl;
}

void IntNode::out(int depth)
{
    printRibs(depth);
    cout << (depth == 0 ? "" : "+-") << "{int " << _value << '}' << endl;
}

void FloatNode::out(int depth)
{
    printRibs(depth);
    cout << (depth == 0 ? "" : "+-") << "{float " << _value << '}' << endl;
}

void PostfixNode::out(int depth)
{
    printRibs(depth);
    cout << (depth == 0 ? "" : "+-") << "{" << operationName(_type) << '}' << endl;
    printNodeWithRibs(_only, depth);
    if(_tail != NULL)
        printNodeWithRibs(_tail, depth);
}

void UnaryNode::out(int depth)
{
    printRibs(depth);
    cout << (depth == 0 ? "" : "+-") << "{" << operationName(_type) << '}' << endl;
    printNodeWithRibs(_only, depth);
}

void BinaryNode::out(int depth)
{
    printRibs(depth);
    cout << (depth == 0 ? "" : "+-") << "{" << operationName(_type) << '}' << endl;
    printNodeWithRibs(_left, depth);
    printNodeWithRibs(_right, depth);
}

void TernaryNode::out(int depth)
{
    printRibs(depth);
    cout << (depth == 0 ? "" : "+-") << "{" << operationName(_type) << '}' << endl;
    printNodeWithRibs(_if, depth);
    printNodeWithRibs(_then, depth);
    printNodeWithRibs(_else, depth);
}

}
