#include <stdio.h>
#include <iostream>
#include <vector>
#include "expression.h"
#include "operations.h"

using namespace std;

namespace LuCCompiler
{

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

    printRibsBeforeNode(depth, branches);

    if(_tail == NULL)
        (*branches)[depth] = true;

    _only->out(depth + 1, branches);

    if(_tail == NULL)
        return;

    printRibsBeforeNode(depth, branches);
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

    printRibsBeforeNode(depth, branches);
    (*branches)[depth] = true;
    _only->out(depth + 1, branches);
}

void BinaryNode::out(unsigned int depth, vector<bool>* branches)
{
    setBranch(depth, branches);

    printRibs(depth, branches);
    cout << (depth == 0 ? "" : "+-") << "{" << operationName(_type) << "}" << endl;

    printRibsBeforeNode(depth, branches);
    _left->out(depth + 1, branches);
    printRibsBeforeNode(depth, branches);
    (*branches)[depth] = true;
    _right->out(depth + 1, branches);
}

void TernaryNode::out(unsigned int depth, vector<bool>* branches)
{
    setBranch(depth, branches);

    printRibs(depth, branches);
    cout << (depth == 0 ? "" : "+-") << "{" << operationName(_type) << "}" << endl;

    printRibsBeforeNode(depth, branches);
    _if->out(depth + 1, branches);
    printRibsBeforeNode(depth, branches);
    _then->out(depth + 1, branches);
    printRibsBeforeNode(depth, branches);
    (*branches)[depth] = true;
    _else->out(depth + 1, branches);
}

}
