#include <stdio.h>
#include <iostream>
#include <vector>
#include "statement.h"

using namespace std;

namespace LuCCompiler
{

void SelectionStatement::out(unsigned int depth, vector<bool>* branches, int indent)
{
    makeNodeTop(depth, branches, "if", indent);
    printNodeWithRibs(depth, branches, false, _expr, indent);
    printNodeWithRibs(depth, branches, _else == NULL, _then, indent);
    if(_else != NULL)
        printNodeWithRibs(depth, branches, true, _else, indent);
}

void JumpStatement::out(unsigned int depth, vector<bool>* branches, int indent)
{
    makeNodeTop(depth, branches,
        _type == TOK_BREAK ? "break" : _type == TOK_CONTINUE ? "continue" : "return",
        indent
    );
    (*branches)[depth] = true;
}

void ReturnStatement::out(unsigned int depth, vector<bool>* branches, int indent)
{
    makeNodeTop(depth, branches, "return", indent);
    printNodeWithRibs(depth, branches, true, _expr, indent);
}

void EmptyExpressionStatement::out(unsigned int depth, vector<bool>* branches, int indent)
{
    makeNodeTop(depth, branches, ";", indent);
    (*branches)[depth] = true;
}

void ExpressionStatement::out(unsigned int depth, vector<bool>* branches, int indent)
{
    makeNodeTop(depth, branches, ";", indent);
    printNodeWithRibs(depth, branches, true, _expr, indent);
}

void IterationStatement::out(unsigned int depth, vector<bool>* branches, int indent)
{
    makeNodeTop(depth, branches, _type == TOK_WHILE ? "while" : "do", indent);
    if(_type == TOK_WHILE)
    {
        printNodeWithRibs(depth, branches, false, _expr, indent);
        printNodeWithRibs(depth, branches, true, _loop, indent);
    }
    else
    {
        printNodeWithRibs(depth, branches, false, _loop, indent);
        printNodeWithRibs(depth, branches, true, _expr, indent);
    }
}

void ForStatement::out(unsigned int depth, vector<bool>* branches, int indent)
{
    makeNodeTop(depth, branches, "for", indent);
    if(_expr != NULL)
        printNodeWithRibs(depth, branches, false, _expr, indent);
    else
        _iterators->out(indent + 1);
    printNodeWithRibs(depth, branches, false, _expr2, indent);
    if(_expr3 != NULL)
        printNodeWithRibs(depth, branches, false, _expr3, indent);
    printNodeWithRibs(depth, branches, true, _loop, indent);
}

void CompoundStatement::out(unsigned int depth, vector<bool>* branches, int indent)
{
    makeNodeTop(depth, branches, "{}", indent);
    if(_locals->size() != 0)
        _locals->out(indent + 1);
    if(_items->size() == 0)
    {
        (*branches)[depth] = true;
        return;
    }
    for(unsigned int i = 0; i < _items->size() - 1; ++i)
        printNodeWithRibs(depth, branches, false, (*_items)[i], indent);
    printNodeWithRibs(depth, branches, true, (*_items)[_items->size() - 1], indent);
}

}
