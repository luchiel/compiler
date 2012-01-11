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
    printNodeWithIndent(depth, branches, false, cond, level);
    printNodeWithIndent(depth, branches, elseExp == NULL, thenExp, level);
    if(elseExp != NULL)
        printNodeWithIndent(depth, branches, true, elseExp, level);
}

void JumpStatement::out(unsigned int depth, vector<bool>* branches, int level)
{
    makeNodeTop(depth, branches,
        type == TOK_BREAK ? "break" : type == TOK_CONTINUE ? "continue" : "return",
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
    printNodeWithIndent(depth, branches, true, expr, level);
}

void ExpressionStatement::gen(AbstractGenerator& g, bool withResult)
{
    expr->gen(g, withResult);
}

void IterationStatement::out(unsigned int depth, vector<bool>* branches, int level)
{
    makeNodeTop(depth, branches, type == TOK_WHILE ? "while" : "do", level);
    if(type == TOK_WHILE)
    {
        printNodeWithIndent(depth, branches, false, cond, level);
        printNodeWithIndent(depth, branches, true, loop, level);
    }
    else
    {
        printNodeWithIndent(depth, branches, false, loop, level);
        printNodeWithIndent(depth, branches, true, cond, level);
    }
}

void ForStatement::out(unsigned int depth, vector<bool>* branches, int level)
{
    makeNodeTop(depth, branches, "for", level);
    if(init != NULL)
        printNodeWithIndent(depth, branches, false, init, level);
    else
        iterators->out(level + 1);
    printNodeWithIndent(depth, branches, false, cond, level);
    if(mod != NULL)
        printNodeWithIndent(depth, branches, false, mod, level);
    printNodeWithIndent(depth, branches, true, loop, level);
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
    g.gen(cMov, rEAX, rEBP);
    g.gen(cAdd, rEAX, 4 * (g.currentParamSize + 3));
    for(unsigned int i = 0; i < expr->expType->size(); ++i)
    {
        g.gen(cPop, rEBX);
        g.gen(cMov, rEAX + Offset(i * 4), rEBX);
    }
    g.gen(cJmp, g.returnLabel());
}

void SelectionStatement::gen(AbstractGenerator& g, bool withResult)
{
    cond->gen(g);
    g.gen(cPop, rEAX);
    Argument* l1 = g.label();
    Argument* l2 = g.label();
    g.gen(cTest, rEAX, rEAX);
    g.gen(cJZ, *l1);
    thenExp->gen(g, false);
    g.gen(cJmp, *l2);
    g.genLabel(l1);
    if(elseExp != NULL)
        elseExp->gen(g, false);
    g.genLabel(l2);
}

void JumpStatement::gen(AbstractGenerator& g, bool withResult)
{
    if(type == TOK_BREAK)
        g.gen(cJmp, g.breakLabel());
    else if(type == TOK_CONTINUE)
        g.gen(cJmp, g.continueLabel());
    else
        g.gen(cJmp, g.returnLabel());
}

void IterationStatement::gen(AbstractGenerator& g, bool withResult)
{
    if(type == TOK_DO)
    {
        Argument* l1 = g.label();
        Argument* l2 = g.label(); //continue label
        Argument* l3 = g.label(); //break label
        g.genLabel(l1);
        g.pushJumpLabels(l3, l2);
        loop->gen(g, false);
        g.popJumpLabels();
        g.genLabel(l2);
        cond->gen(g);
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
        cond->gen(g);
        g.gen(cPop, rEAX);
        g.gen(cTest, rEAX, rEAX);
        g.gen(cJZ, *l2);
        g.pushJumpLabels(l2, l1);
        loop->gen(g, false);
        g.popJumpLabels();
        g.gen(cJmp, *l1);
        g.genLabel(l2);
    }
}

void ForStatement::gen(AbstractGenerator& g, bool withResult)
{
    g.gen(cSub, rESP, (iterators->offset() - iterators->parent->offset()) * 4);
    iterators->genInitLocals(g);
    if(init != NULL)
        init->gen(g, false);
    Argument* l1 = g.label();
    Argument* l2 = g.label(); //continue label
    Argument* l3 = g.label(); //break label
    g.genLabel(l1);
    if(dynamic_cast<EmptyExpressionStatement*>(cond) == NULL)
    {
        cond->gen(g);
        g.gen(cPop, rEAX);
        g.gen(cTest, rEAX, rEAX);
        g.gen(cJZ, *l3);
    }
    g.pushJumpLabels(l3, l2);
    loop->gen(g, false);
    g.popJumpLabels();
    g.genLabel(l2);
    if(mod != NULL)
        mod->gen(g, false);
    g.gen(cJmp, *l1);
    g.genLabel(l3);
    g.gen(cAdd, rESP, (iterators->offset() - iterators->parent->offset()) * 4);
}

}
