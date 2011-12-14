#include <stdio.h>
#include <iostream>
#include <vector>
#include "statement.h"

using namespace std;

namespace LuCCompiler
{

void SelectionStatement::out(unsigned int depth, vector<bool>* branches, int level)
{
    makeNodeTop(depth, branches, "if", level);
    printNodeWithIndent(depth, branches, false, _expr, level);
    printNodeWithIndent(depth, branches, _else == NULL, _then, level);
    if(_else != NULL)
        printNodeWithIndent(depth, branches, true, _else, level);
}

void JumpStatement::out(unsigned int depth, vector<bool>* branches, int level)
{
    makeNodeTop(depth, branches,
        _type == TOK_BREAK ? "break" : _type == TOK_CONTINUE ? "continue" : "return",
        level
    );
    (*branches)[depth] = true;
}

void ReturnStatement::out(unsigned int depth, vector<bool>* branches, int level)
{
    makeNodeTop(depth, branches, "return", level);
    printNodeWithIndent(depth, branches, true, _expr, level);
}

void EmptyExpressionStatement::out(unsigned int depth, vector<bool>* branches, int level)
{
    makeNodeTop(depth, branches, ";", level);
    (*branches)[depth] = true;
}

void ExpressionStatement::out(unsigned int depth, vector<bool>* branches, int level)
{
    makeNodeTop(depth, branches, ";", level);
    printNodeWithIndent(depth, branches, true, _expr, level);
}

void IterationStatement::out(unsigned int depth, vector<bool>* branches, int level)
{
    makeNodeTop(depth, branches, _type == TOK_WHILE ? "while" : "do", level);
    if(_type == TOK_WHILE)
    {
        printNodeWithIndent(depth, branches, false, _expr, level);
        printNodeWithIndent(depth, branches, true, _loop, level);
    }
    else
    {
        printNodeWithIndent(depth, branches, false, _loop, level);
        printNodeWithIndent(depth, branches, true, _expr, level);
    }
}

void ForStatement::out(unsigned int depth, vector<bool>* branches, int level)
{
    makeNodeTop(depth, branches, "for", level);
    if(_expr != NULL)
        printNodeWithIndent(depth, branches, false, _expr, level);
    else
        _iterators->out(level + 1);
    printNodeWithIndent(depth, branches, false, _expr2, level);
    if(_expr3 != NULL)
        printNodeWithIndent(depth, branches, false, _expr3, level);
    printNodeWithIndent(depth, branches, true, _loop, level);
}

void CompoundStatement::out(unsigned int depth, vector<bool>* branches, int level)
{
    makeNodeTop(depth, branches, "{}", level);
    if(_locals->size() != 0)
        _locals->out(level + 1);
    if(_items->size() == 0)
    {
        (*branches)[depth] = true;
        return;
    }
    for(unsigned int i = 0; i < _items->size() - 1; ++i)
        printNodeWithIndent(depth, branches, false, (*_items)[i], level);
    printNodeWithIndent(depth, branches, true, (*_items)[_items->size() - 1], level);
}

}
