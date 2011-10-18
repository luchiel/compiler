#include "expression.h"

namespace LuCCompiler
{

//think about trailing |

void ExpressionNode::printRibs(int depth)
{
    while(depth-- > 2)
        cout << "| ";
}

void IdentNode::out(int depth)
{
    printRibs(depth);

    cout << '+-{' << _name << '}' << endl;
}

void StringNode::out(int depth)
{
    printRibs(depth);

    cout << '+-{' << _value << '}' << endl;
}

void CharNode::out(int depth)
{
    printRibs(depth);

    cout << '+-{' << _value << '}' << endl;
}

void IntNode::out(int depth)
{
    printRibs(depth);

    cout << '+-{' << _value << '}' << endl;
}

void FloatNode::out(int depth)
{
    printRibs(depth);

    cout << '+-{' << _value << '}' << endl;
}

void PostfixNode::out(int depth)
{
    printRibs(depth);

    cout << '+-{' << operationName(_type) << '}' << endl;

    printRibs(depth + 1);
    cout << endl;
    _tail->out(depth + 1);
    printRibs(depth + 1);
    cout << endl;
    _only->out(depth + 1);
}

void UnaryNode::out(int depth)
{
    printRibs(depth);

    cout << '+-{' << operationName(_type) << '}' << endl;

    printRibs(depth + 1);
    cout << endl;
    _only->out(depth + 1);
}

void BinaryNode::out(int depth)
{
    printRibs(depth);

    cout << '+-{' << operationName(_type) << '}' << endl;

    printRibs(depth + 1);
    cout << endl;
    _left->out(depth + 1);
    printRibs(depth + 1);
    cout << endl;
    _right->out(depth + 1);
}

}
