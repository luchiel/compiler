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
    if(locals->size() != 0)
        locals->out(level + 1);
    if(items->size() == 0)
    {
        (*branches)[depth] = true;
        return;
    }
    for(unsigned int i = 0; i < items->size() - 1; ++i)
        printNodeWithIndent(depth, branches, false, (*items)[i], level);
    printNodeWithIndent(depth, branches, true, (*items)[items->size() - 1], level);
}

void CompoundStatement::gen(AbstractGenerator& g, bool withResult)
{
    g.gen(cSub, rESP, (locals->offset() - locals->parent->offset()) * 4);
    for(unsigned int i = 0; i < items->size(); ++i)
        (*items)[i]->gen(g, false);
    g.gen(cAdd, rESP, (locals->offset() - locals->parent->offset()) * 4);
}

void ReturnStatement::gen(AbstractGenerator& g, bool withResult)
{
    expr->gen(g);
    g.gen(cMov, rEAX, rEBP);
    g.gen(cAdd, rEAX, 4 * (g.currentParamSize + 2));
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

void LoopStatement::genCheckCondition(AbstractGenerator& g, Node* cond)
{
    cond->gen(g);
    g.gen(cPop, rEAX);
    g.gen(cTest, rEAX, rEAX);
}

void LoopStatement::genLoop(AbstractGenerator& g)
{
    g.pushJumpLabels(breakLabel, continueLabel);
    loop->gen(g, false);
    g.popJumpLabels();
}

void IterationStatement::gen(AbstractGenerator& g, bool withResult)
{
    continueLabel = g.label();
    breakLabel = g.label();
    if(type == TOK_DO)
    {
        Argument* startLabel = g.label();
        g.genLabel(startLabel);
        genLoop(g);
        g.genLabel(continueLabel);
        genCheckCondition(g, cond);
        g.gen(cJNZ, *startLabel);
    }
    else
    {
        g.genLabel(continueLabel);
        genCheckCondition(g, cond);
        g.gen(cJZ, *breakLabel);
        genLoop(g);
        g.gen(cJmp, *continueLabel);
    }
    g.genLabel(breakLabel);
}

void ForStatement::gen(AbstractGenerator& g, bool withResult)
{
    g.gen(cSub, rESP, (iterators->offset() - iterators->parent->offset()) * 4);
    if(init != NULL)
        init->gen(g, false);
    Argument* startLabel = g.label();
    continueLabel = g.label();
    breakLabel = g.label();
    g.genLabel(startLabel);
    if(dynamic_cast<EmptyExpressionStatement*>(cond) == NULL)
    {
        genCheckCondition(g, cond);
        g.gen(cJZ, *breakLabel);
    }
    genLoop(g);
    g.genLabel(continueLabel);
    if(mod != NULL)
        mod->gen(g, false);
    g.gen(cJmp, *startLabel);
    g.genLabel(breakLabel);
    g.gen(cAdd, rESP, (iterators->offset() - iterators->parent->offset()) * 4);
}

void InitStatement::gen(AbstractGenerator& g, bool withResult)
{
    if(static_cast<ENode*>(var->initializer)->isDoubleConst())
    {
        string s = g.addDoubleConstant(
            static_cast<DoubleNode*>(var->initializer)->value
        );
        g.gen(cMovsd, rXMM0, s + Offset(0) + swQword);
        g.gen(cMovsd, rEBP + Offset(-(var->offset + 2) * 4) + swQword, rXMM0);
    }
    else if(static_cast<ENode*>(var->initializer)->isIntConst())
        g.gen(
            cMov, rEBP + Offset(-(var->offset + 1) * 4),
            static_cast<IntNode*>(var->initializer)->value
        );
    else if(static_cast<ENode*>(var->initializer)->isNULL())
        g.gen(cMov, rEBP + Offset(-(var->offset + 1) * 4), 0);
    else
    {
        var->initializer->gen(g);
        if(var->type->name == "double")
        {
            g.genDoublePop(rXMM0);
            g.gen(cMovsd, rEBP + Offset(-(var->offset + 2) * 4) + swQword, rXMM0);
        }
        else
            for(int i = 0; i < var->type->size(); ++i)
            {
                g.gen(cPop, rEBX);
                g.gen(cMov, rEBP + Offset(-(var->offset + i + 1) * 4), rEBX);
            }
    }
}

void PackedInitStatement::gen(AbstractGenerator& g, bool withResult)
{
    for(unsigned int i = 0; i < inits.size(); ++i)
        inits[i]->gen(g);
}

}
