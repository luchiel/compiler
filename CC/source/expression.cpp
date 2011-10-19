#include <stdio.h>
#include <iostream>
#include "expression.h"
#include "operations.h"

namespace LuCCompiler
{

void ExpressionNode::printRibs(int depth)
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

    cout << (depth == 0 ? "" : "+-") << "{" << _value << '}' << endl;
}

void CharNode::out(int depth)
{
    printRibs(depth);

    cout << (depth == 0 ? "" : "+-") << "{" << _value << '}' << endl;
}

void IntNode::out(int depth)
{
    printRibs(depth);

    cout << (depth == 0 ? "" : "+-") << "{" << _value << '}' << endl;
}

void FloatNode::out(int depth)
{
    printRibs(depth);

    cout << (depth == 0 ? "" : "+-") << "{" << _value << '}' << endl;
}

void PostfixNode::out(int depth)
{
    printRibs(depth);

    cout << (depth == 0 ? "" : "+-") << "{" << operationName(_type) << '}' << endl;

    printRibs(depth + 2);
    cout << endl;
    _tail->out(depth + 1);
    printRibs(depth + 2);
    cout << endl;
    _only->out(depth + 1);
}

void UnaryNode::out(int depth)
{
    printRibs(depth);

    cout << (depth == 0 ? "" : "+-") << "{" << operationName(_type) << '}' << endl;

    printRibs(depth + 2);
    cout << endl;
    _only->out(depth + 1);
}

void BinaryNode::out(int depth)
{
    printRibs(depth);

    cout << (depth == 0 ? "" : "+-") << "{" << operationName(_type) << '}' << endl;

    printRibs(depth + 2);
    cout << endl;
    _left->out(depth + 1);
    printRibs(depth + 2);
    cout << endl;
    _right->out(depth + 1);
}

}
