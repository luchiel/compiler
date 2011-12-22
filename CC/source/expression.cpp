#include <stdio.h>
#include <iostream>
#include <vector>
#include "expression.h"
#include "operations.h"

using namespace std;

namespace LuCCompiler
{

void ENode::printExpType(int level)
{
    if(expType != NULL)
        expType->out(level, false);
}

void ENode::makeNodeTop(
    unsigned int depth, vector<bool>* branches, const string& s, int level
)
{
    Node::makeNodeTop(depth, branches, s, level);
    printExpType(level + 1 + depth);
}


void IdentNode::out(unsigned int depth, vector<bool>* branches, int level)
{
    printIndent(depth, branches, level);
    cout << (depth == 0 ? "" : "+-") << "{" << name << "}" << endl;
    printExpType(level + 1 + depth);
}

void StringNode::out(unsigned int depth, vector<bool>* branches, int level)
{
    printIndent(depth, branches, level);
    cout << (depth == 0 ? "" : "+-") << "{string " << _value << "}" << endl;
    printExpType(level + 1 + depth);
}

void CharNode::out(unsigned int depth, vector<bool>* branches, int level)
{
    printIndent(depth, branches, level);
    cout << (depth == 0 ? "" : "+-") << "{char " << _value << "}" << endl;
    printExpType(level + 1 + depth);
}

void IntNode::out(unsigned int depth, vector<bool>* branches, int level)
{
    printIndent(depth, branches, level);
    cout << (depth == 0 ? "" : "+-") << "{int " << value << "}" << endl;
    printExpType(level + 1 + depth);
}

void FloatNode::out(unsigned int depth, vector<bool>* branches, int level)
{
    printIndent(depth, branches, level);
    cout << (depth == 0 ? "" : "+-") << "{float " << _value << "}" << endl;
    printExpType(level + 1 + depth);
}

void PostfixNode::out(unsigned int depth, vector<bool>* branches, int level)
{
    string x(type == TOK_INC || type == TOK_DEC ? "x" : "");
    makeNodeTop(depth, branches, x + operationName(type), level);
    setBranch(depth, branches);

    printIndentBeforeNode(depth, branches, level);

    if(tail.size() == 0)
        (*branches)[depth] = true;

    only->out(depth + 1, branches, level);

    if(tail.size() > 1)
    {
        printIndentBeforeNode(depth, branches, level);
        (*branches)[depth] = true;
        depth++;
        makeNodeTop(depth, branches, ",", level);
    }
    for(unsigned int i = 0; i < tail.size(); ++i)
        printNodeWithIndent(depth, branches, i == tail.size() - 1, tail[i], level);
}

void UnaryNode::out(unsigned int depth, vector<bool>* branches, int level)
{
    string x(type == TOK_INC || type == TOK_DEC ? "x" : "");
    makeNodeTop(depth, branches, operationName(type) + x, level);
    printNodeWithIndent(depth, branches, true, only, level);
}

void BinaryNode::out(unsigned int depth, vector<bool>* branches, int level)
{
    makeNodeTop(depth, branches, operationName(type), level);
    printNodeWithIndent(depth, branches, false, left, level);
    printNodeWithIndent(depth, branches, true, right, level);
}

void TernaryNode::out(unsigned int depth, vector<bool>* branches, int level)
{
    makeNodeTop(depth, branches, operationName(type), level);

    printNodeWithIndent(depth, branches, false, condition, level);
    printNodeWithIndent(depth, branches, false, thenOp, level);
    printNodeWithIndent(depth, branches, true, elseOp, level);
}

void CastNode::out(unsigned int depth, vector<bool>* branches, int level)
{
    makeNodeTop(depth, branches, "cast", level);

    printIndentBeforeNode(depth, branches, level);
    printIndent(depth + 1, branches, level);
    cout << "+-<" << type->name << ">\n";
    printNodeWithIndent(depth, branches, true, element, level);
}

void SizeofNode::out(unsigned int depth, vector<bool>* branches, int level)
{
    makeNodeTop(depth, branches, operationName(type), level);
    if(only != NULL)
        printNodeWithIndent(depth, branches, true, only, level);
    else
    {
        printIndentBeforeNode(depth, branches, level);
        printIndent(depth + 1, branches, level);
        cout << "+-<" << symbolType->name << ">\n";
    }
}

}
