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

    printRibsBeforeNode(depth, branches);
    _expr->out(depth + 1, branches);
    printRibsBeforeNode(depth, branches);
    if(_else == NULL)
        (*branches)[depth] = true;
    _then->out(depth + 1, branches);

    if(_else != NULL)
    {
        printRibsBeforeNode(depth, branches);
        (*branches)[depth] = true;
        _else->out(depth + 1, branches);
    }
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
        printRibsBeforeNode(depth, branches);
        _expr->out(depth + 1, branches);
        printRibsBeforeNode(depth, branches);
        (*branches)[depth] = true;
        _loop->out(depth + 1, branches);
    }
    else
    {
        printRibsBeforeNode(depth, branches);
        _loop->out(depth + 1, branches);
        printRibsBeforeNode(depth, branches);
        (*branches)[depth] = true;
        _expr->out(depth + 1, branches);
    }
}

void ForStatement::out(unsigned int depth, vector<bool>* branches)
{
    makeNodeTop(depth, branches, "for");

    printRibsBeforeNode(depth, branches);
    _expr->out(depth + 1, branches);
    printRibsBeforeNode(depth, branches);
    _expr2->out(depth + 1, branches);
    if(_expr3 != NULL)
    {
        printRibsBeforeNode(depth, branches);
        _expr3->out(depth + 1, branches);
    }
    printRibsBeforeNode(depth, branches);
    (*branches)[depth] = true;
    _loop->out(depth + 1, branches);
}

void CompoundStatement::out(unsigned int depth, vector<bool>* branches)
{
    makeNodeTop(depth, branches, "{}");

    for(unsigned int i = 0; i < _items->size() - 1; ++i)
    {
        printRibsBeforeNode(depth, branches);
        (*_items)[i]->out(depth + 1, branches);
    }
    printRibsBeforeNode(depth, branches);
    (*branches)[depth] = true;
    (*_items)[_items->size() - 1]->out(depth + 1, branches);
}

}
