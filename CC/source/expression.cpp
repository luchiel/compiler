#include <stdio.h>
#include <iostream>
#include <vector>
#include "expression.h"
#include "operations.h"

using namespace std;

namespace LuCCompiler
{

void IdentNode::out(unsigned int depth, vector<bool>* branches, int indent)
{
    printRibs(depth, branches, indent);
    cout << (depth == 0 ? "" : "+-") << "{" << _name << "}" << endl;
}

void StringNode::out(unsigned int depth, vector<bool>* branches, int indent)
{
    printRibs(depth, branches, indent);
    cout << (depth == 0 ? "" : "+-") << "{string " << _value << "}" << endl;
}

void CharNode::out(unsigned int depth, vector<bool>* branches, int indent)
{
    printRibs(depth, branches, indent);
    cout << (depth == 0 ? "" : "+-") << "{char " << _value << "}" << endl;
}

void IntNode::out(unsigned int depth, vector<bool>* branches, int indent)
{
    printRibs(depth, branches, indent);
    cout << (depth == 0 ? "" : "+-") << "{int " << _value << "}" << endl;
}

void FloatNode::out(unsigned int depth, vector<bool>* branches, int indent)
{
    printRibs(depth, branches, indent);
    cout << (depth == 0 ? "" : "+-") << "{float " << _value << "}" << endl;
}

void PostfixNode::out(unsigned int depth, vector<bool>* branches, int indent)
{
    string x(_type == TOK_INC || _type == TOK_DEC ? "x" : "");
    makeNodeTop(depth, branches, x + operationName(_type), indent);
    setBranch(depth, branches);

    printRibsBeforeNode(depth, branches, indent);

    if(_tail == NULL)
        (*branches)[depth] = true;

    _only->out(depth + 1, branches, indent);

    if(_tail == NULL)
        return;

    printNodeWithRibs(depth, branches, true, _tail, indent);
}

void UnaryNode::out(unsigned int depth, vector<bool>* branches, int indent)
{
    string x(_type == TOK_INC || _type == TOK_DEC ? "x" : "");
    makeNodeTop(depth, branches, operationName(_type) + x, indent);
    printNodeWithRibs(depth, branches, true, _only, indent);
}

void BinaryNode::out(unsigned int depth, vector<bool>* branches, int indent)
{
    makeNodeTop(depth, branches, operationName(_type), indent);
    printNodeWithRibs(depth, branches, false, _left, indent);
    printNodeWithRibs(depth, branches, true, _right, indent);
}

void TernaryNode::out(unsigned int depth, vector<bool>* branches, int indent)
{
    makeNodeTop(depth, branches, operationName(_type), indent);

    printNodeWithRibs(depth, branches, false, _if, indent);
    printNodeWithRibs(depth, branches, false, _then, indent);
    printNodeWithRibs(depth, branches, true, _else, indent);
}

}
