#include <stdio.h>
#include <iostream>
#include <vector>
#include "expression.h"
#include "dictionaries.h"

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
    cout << (depth == 0 ? "" : "+-") << "{string " << value << "}" << endl;
    printExpType(level + 1 + depth);
}

void CharNode::out(unsigned int depth, vector<bool>* branches, int level)
{
    printIndent(depth, branches, level);
    cout << (depth == 0 ? "" : "+-") << "{char " << char(value) << "}" << endl;
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
    cout << (depth == 0 ? "" : "+-") << "{float " << value << "}" << endl;
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

void IdentNode::gen(AbstractGenerator& g) {}

void IntNode::gen(AbstractGenerator& g)
{
    g.gen(cMov, rEAX, value);
    g.gen(cPush, rEAX);
}

void CharNode::gen(AbstractGenerator& g)
{
    g.gen(cMov, rEAX, value);
    g.gen(cPush, rEAX);
}

void StringNode::gen(AbstractGenerator& g) {}
void FloatNode::gen(AbstractGenerator& g) {}

void PostfixNode::gen(AbstractGenerator& g)
{
    //only->gen(g);
    //vector<ENode*> tail;
    switch(type)
    {
        case TOK_L_BRACKET: //fcall
        case TOK_L_SQUARE: //subscript
        case TOK_DOT: //
        case TOK_ARROW:
        case TOK_INC:
        case TOK_DEC:
            break;
    }
}

void UnaryNode::gen(AbstractGenerator& g)
{
    if(type == TOK_ASTERISK)
    {
        return;
    }
    only->gen(g);
    g.gen(cPop, rEAX);
    switch(type)
    {
        case TOK_AMP: //?
            only->genLValue(g);
            g.gen(cPop, rEAX);
            break;
        case TOK_INC:
        case TOK_DEC:
            only->genLValue(g);
            g.gen(cPop, rEBX);
            g.gen(type == TOK_INC ? cInc : cDec, rEBX + 0);
            g.gen(cMov, rEAX, rEBX + 0);
            break;
        case TOK_NOT:
            g.gen(cXor, rECX, rECX);
            g.gen(cTest, rEAX, rEAX);
            g.gen(cSetZ, rCL);
            g.gen(cMov, rEAX, rECX);
            break;
        case TOK_TILDA: g.gen(cNot, rEAX); break;
        case TOK_MINUS: g.gen(cNeg, rEAX); break;
        case TOK_PLUS:  break;
    }
    g.gen(cPush, rEAX);
}

void BinaryNode::gen(AbstractGenerator& g)
{
    if(type == TOK_LOGICAL_AND || type == TOK_LOGICAL_OR)
    {
        left->gen(g);
        g.gen(cPop, rEAX);
        g.gen(cTest, rEAX, rEAX);
        Argument a = g.label();
        g.gen(type == TOK_LOGICAL_AND ? cJZ : cJNZ, a);

        right->gen(g);
        g.gen(cPop, rEAX);
        g.gen(cTest, rEAX, rEAX);

        g.genLabel(&a);
        g.gen(cPush, rEAX);
    }
    left->gen(g);
    right->gen(g);
    g.gen(cPop, rEBX);
    g.gen(cPop, rEAX);
    if(expType->name == "float")
    {
        ;
    }
    else switch(type)
    {
        case TOK_MOD:
            g.gen(cIdiv, rEBX);
            g.gen(cMov, rEAX, rEDX);
            break;
        case TOK_ASTERISK: g.gen(cImul, rEAX, rEBX); break;
        case TOK_DIV:      g.gen(cIdiv, rEBX); break;
        case TOK_PLUS:     g.gen(cAdd, rEAX, rEBX); break;
        case TOK_MINUS:    g.gen(cSub, rEAX, rEBX); break;
        case TOK_SHL:
        case TOK_SHR:
            g.gen(cMov, rECX, rEBX);
            g.gen(type == TOK_SHL ? cShl : cShr, rCL);
            break;
        case TOK_L:  g.genIntCmp(cSetL); break;
        case TOK_G:  g.genIntCmp(cSetG); break;
        case TOK_LE: g.genIntCmp(cSetLE); break;
        case TOK_GE: g.genIntCmp(cSetGE); break;
        case TOK_E:  g.genIntCmp(cSetE); break;
        case TOK_NE: g.genIntCmp(cSetNE); break;
        case TOK_AMP: g.gen(cAnd, rEAX, rEBX); break;
        case TOK_XOR: g.gen(cXor, rEAX, rEBX); break;
        case TOK_OR:  g.gen(cOr, rEAX, rEBX); break;
    }
    g.gen(cPush, rEAX);
}

void SizeofNode::gen(AbstractGenerator& g)
{
    g.gen(cMov, rEAX, symbolType->size() * 4);
    g.gen(cPush, rEAX);
}

void CastNode::gen(AbstractGenerator& g)
{
    element->gen(g);
}

void AssignmentNode::gen(AbstractGenerator& g) {}
void ExpressionNode::gen(AbstractGenerator& g) {}
void TernaryNode::gen(AbstractGenerator& g) {}

void IdentNode::genLValue(AbstractGenerator& g) {}
void PostfixNode::genLValue(AbstractGenerator& g) {}
void UnaryNode::genLValue(AbstractGenerator& g) {}

}
