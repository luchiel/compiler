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
    string x(_type == TOK_INC || _type == TOK_DEC ? "x" : "");
    makeNodeTop(depth, branches, x + operationName(_type));
    setBranch(depth, branches);

    printRibsBeforeNode(depth, branches);

    if(_tail == NULL)
        (*branches)[depth] = true;

    _only->out(depth + 1, branches);

    if(_tail == NULL)
        return;

    printNodeWithRibs(depth, branches, true, _tail);
}

void UnaryNode::out(unsigned int depth, vector<bool>* branches)
{
    string x(_type == TOK_INC || _type == TOK_DEC ? "x" : "");
    makeNodeTop(depth, branches, operationName(_type) + x);
    printNodeWithRibs(depth, branches, true, _only);
}

void BinaryNode::out(unsigned int depth, vector<bool>* branches)
{
    makeNodeTop(depth, branches, operationName(_type));
    printNodeWithRibs(depth, branches, false, _left);
    printNodeWithRibs(depth, branches, true, _right);
}

void TernaryNode::out(unsigned int depth, vector<bool>* branches)
{
    makeNodeTop(depth, branches, operationName(_type));

    printNodeWithRibs(depth, branches, false, _if);
    printNodeWithRibs(depth, branches, false, _then);
    printNodeWithRibs(depth, branches, true, _else);
}

}
