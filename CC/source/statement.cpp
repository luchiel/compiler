#include <stdio.h>
#include <iostream>
#include <vector>
#include "statement.h"

using namespace std;

namespace LuCCompiler
{

void SelectionStatement::out(unsigned int depth, vector<bool>* branches)
{
    makeNodeTop(depth, branches, "if");
    printNodeWithRibs(depth, branches, false, _expr);
    printNodeWithRibs(depth, branches, _else == NULL, _then);
    if(_else != NULL)
        printNodeWithRibs(depth, branches, true, _else);
}

void JumpStatement::out(unsigned int depth, vector<bool>* branches)
{
    makeNodeTop(depth, branches, _type == TOK_BREAK ? "break" : "continue");
    (*branches)[depth] = true;
}

void ReturnStatement::out(unsigned int depth, vector<bool>* branches)
{
    makeNodeTop(depth, branches, "return");

    if(_expr != NULL)
        printRibsBeforeNode(depth, branches);

    (*branches)[depth] = true;

    if(_expr != NULL)
        _expr->out(depth + 1, branches);
}

void ExpressionStatement::out(unsigned int depth, vector<bool>* branches)
{
    makeNodeTop(depth, branches, ";");

    if(_expr != NULL)
        printRibsBeforeNode(depth, branches);

    (*branches)[depth] = true;

    if(_expr != NULL)
        _expr->out(depth + 1, branches);
}

void IterationStatement::out(unsigned int depth, vector<bool>* branches)
{
    makeNodeTop(depth, branches, _type == TOK_WHILE ? "while" : "do");
    if(_type == TOK_WHILE)
    {
        printNodeWithRibs(depth, branches, false, _expr);
        printNodeWithRibs(depth, branches, true, _loop);
    }
    else
    {
        printNodeWithRibs(depth, branches, false, _loop);
        printNodeWithRibs(depth, branches, true, _expr);
    }
}

void ForStatement::out(unsigned int depth, vector<bool>* branches)
{
    makeNodeTop(depth, branches, "for");
    printNodeWithRibs(depth, branches, false, _expr);
    printNodeWithRibs(depth, branches, false, _expr2);
    if(_expr3 != NULL)
        printNodeWithRibs(depth, branches, false, _expr3);
    printNodeWithRibs(depth, branches, true, _loop);
}

void CompoundStatement::out(unsigned int depth, vector<bool>* branches)
{
    makeNodeTop(depth, branches, "{}");
    for(unsigned int i = 0; i < _items->size() - 1; ++i)
        printNodeWithRibs(depth, branches, false, (*_items)[i]);
    printNodeWithRibs(depth, branches, true, (*_items)[_items->size() - 1]);
}

}
