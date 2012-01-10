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
    printNodeWithIndent(depth, branches, true, expr, level);
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

void ExpressionStatement::gen(AbstractGenerator& g, bool withResult)
{
    _expr->gen(g, withResult);
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

void CompoundStatement::gen(AbstractGenerator& g, bool withResult)
{
    g.gen(cSub, rESP, (_locals->offset() - _locals->parent->offset()) * 4);
    _locals->genInitLocals(g);
    for(unsigned int i = 0; i < _items->size(); ++i)
        (*_items)[i]->gen(g, false);
    g.gen(cAdd, rESP, (_locals->offset() - _locals->parent->offset()) * 4);
}

void ReturnStatement::gen(AbstractGenerator& g, bool withResult)
{
    expr->gen(g);
    g.gen(cPop, rEBX);
    g.gen(cMov, rEAX, rEBP);
    g.gen(cAdd, rEAX, 4 * (g.currentParamSize + 3));
    for(unsigned int i = 0; i < expr->expType->size(); ++i)
        g.gen(cMov, rEAX + Offset(i * 4), rEBX);
    //g.gen(cAdd, rESP, expr->expType->size() * 4); //not required
    g.gen(cJmp, g.returnLabel());
}

void SelectionStatement::gen(AbstractGenerator& g, bool withResult)
{
    _expr->gen(g);
    g.gen(cPop, rEAX);
    Argument* l1 = g.label();
    Argument* l2 = g.label();
    g.gen(cTest, rEAX, rEAX);
    g.gen(cJZ, *l1);
    _then->gen(g, false);
    g.gen(cJmp, *l2);
    g.genLabel(l1);
    if(_else != NULL)
        _else->gen(g, false);
    g.genLabel(l2);
}

void JumpStatement::gen(AbstractGenerator& g, bool withResult)
{
    if(_type == TOK_BREAK)
        g.gen(cJmp, g.breakLabel());
    else if(_type == TOK_CONTINUE)
        g.gen(cJmp, g.continueLabel());
    else
        g.gen(cJmp, g.returnLabel());
}

void IterationStatement::gen(AbstractGenerator& g, bool withResult)
{
    if(_type == TOK_DO)
    {
        Argument* l1 = g.label();
        Argument* l2 = g.label(); //continue label
        Argument* l3 = g.label(); //break label
        g.genLabel(l1);
        g.pushJumpLabels(l3, l2);
        _loop->gen(g, false);
        g.popJumpLabels();
        g.genLabel(l2);
        _expr->gen(g);
        g.gen(cPop, rEAX);
        g.gen(cTest, rEAX, rEAX);
        g.gen(cJNZ, *l1);
        g.genLabel(l3);
    }
    else
    {
        Argument* l1 = g.label(); //continue label
        Argument* l2 = g.label(); //break label
        g.genLabel(l1);
        _expr->gen(g);
        g.gen(cPop, rEAX);
        g.gen(cTest, rEAX, rEAX);
        g.gen(cJZ, *l2);
        g.pushJumpLabels(l2, l1);
        _loop->gen(g, false);
        g.popJumpLabels();
        g.gen(cJmp, *l1);
        g.genLabel(l2);
    }
}

void ForStatement::gen(AbstractGenerator& g, bool withResult)
{
    g.gen(cSub, rESP, (_iterators->offset() - _iterators->parent->offset()) * 4);
    _iterators->genInitLocals(g);
    if(_expr != NULL)
        _expr->gen(g, false);
    Argument* l1 = g.label();
    Argument* l2 = g.label(); //continue label
    Argument* l3 = g.label(); //break label
    g.genLabel(l1);
    if(dynamic_cast<EmptyExpressionStatement*>(_expr2) == NULL)
    {
        _expr2->gen(g);
        g.gen(cPop, rEAX);
        g.gen(cTest, rEAX, rEAX);
        g.gen(cJZ, *l3);
    }
    g.pushJumpLabels(l3, l2);
    _loop->gen(g, false);
    g.popJumpLabels();
    g.genLabel(l2);
    if(_expr3 != NULL)
        _expr3->gen(g, false);
    g.gen(cJmp, *l1);
    g.genLabel(l3);
    g.gen(cAdd, rESP, (_iterators->offset() - _iterators->parent->offset()) * 4);
}

}
