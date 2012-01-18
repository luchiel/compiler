#include <stdio.h>
#include <iostream>
#include <vector>
#include <assert.h>
#include "expression.h"
#include "dictionaries.h"
#include "complex_symbol.h"

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

void DoubleNode::out(unsigned int depth, vector<bool>* branches, int level)
{
    printIndent(depth, branches, level);
    cout << (depth == 0 ? "" : "+-") << "{double " << value << "}" << endl;
    printExpType(level + 1 + depth);
}

void PostfixNode::out(unsigned int depth, vector<bool>* branches, int level)
{
    string x(type == TOK_INC || type == TOK_DEC ? "x" : "");
    makeNodeTop(depth, branches, x + operationName(type), level);
    setBranch(depth, branches);
    printIndentBeforeNode(depth, branches, level);

    if(tail == NULL)
        (*branches)[depth] = true;

    only->out(depth + 1, branches, level);

    if(tail != NULL)
        printNodeWithIndent(depth, branches, true, tail, level);
}

void CallNode::out(unsigned int depth, vector<bool>* branches, int level)
{
    makeNodeTop(depth, branches, operationName(type), level);
    setBranch(depth, branches);
    printIndentBeforeNode(depth, branches, level);

    if(params.size() == 0)
        (*branches)[depth] = true;

    only->out(depth + 1, branches, level);

    if(params.size() > 1)
    {
        printIndentBeforeNode(depth, branches, level);
        (*branches)[depth] = true;
        depth++;
        makeNodeTop(depth, branches, ",", level);
    }
    for(unsigned int i = 0; i < params.size(); ++i)
        printNodeWithIndent(depth, branches, i == params.size() - 1, params[i], level);
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

void IdentNode::gen(AbstractGenerator& g, bool withResult)
{
    if(var->classType == CT_FUNCTION)
    {
        //
        return;
    }
    SymbolVariable* v = static_cast<SymbolVariable*>(var);

    int od = expType->name == "double" ? 1 : 0;

    switch(v->varType)
    {
        case VT_LOCAL:  g.gen(cLea, rEAX, rEBP + Offset(-(var->offset + od + 1) * 4)); break;
        case VT_PARAM:  g.gen(cLea, rEAX, rEBP + Offset((var->offset + 2) * 4)); break;
        case VT_GLOBAL: g.gen(cMov, rEAX, "v_" + var->name); break;
    }
    if(!withResult)
        return;

    if(od)
    {
        g.gen(cMovsd, rXMM0, rEAX + Offset(0) + swQword);
        g.genDoublePush(rXMM0);
    }
    else
        for(int i = v->type->size() - 1; i >= 0; --i)
            g.gen(cPush, rEAX + Offset(i * 4 * (varType == VT_LOCAL ? -1 : 1)));
}

void IntNode::gen(AbstractGenerator& g, bool withResult)
{
    if(withResult)
    {
        g.gen(cMov, rEAX, value);
        g.gen(cPush, rEAX);
    }
}

void CharNode::gen(AbstractGenerator& g, bool withResult)
{
    if(withResult)
    {
        g.gen(cMov, rEAX, value);
        g.gen(cPush, rEAX);
    }
}

void StringNode::gen(AbstractGenerator& g, bool withResult)
{
    if(withResult)
    {
        string s = g.addConstant(value);
        g.gen(cMov, rEAX, s);
        g.gen(cPush, rEAX);
    }
}

void DoubleNode::gen(AbstractGenerator& g, bool withResult)
{
    if(withResult)
    {
        string s = g.addDoubleConstant(value);
        g.gen(cMovsd, rXMM0, s + Offset(0) + swQword);
        g.gen(cSub, rESP, 8);
        g.gen(cMovsd, rESP + Offset(0) + swQword, rXMM0);
    }
}

void PostfixNode::gen(AbstractGenerator& g, bool withResult)
{
    switch(type)
    {
        case TOK_INC:
        case TOK_DEC:
            if(expType->name == "double")
            {
                only->gen(g);
                only->genLValue(g);
                g.gen(cPop, rEBX);
                g.genDoublePop(rXMM0);
                g.gen(cMovsd, rXMM1, rXMM0);

                g.gen(
                    type == TOK_INC ? cAddsd : cSubsd,
                    rXMM0, string("d_one") + Offset(0) + swQword
                );
                g.gen(cMovsd, rEBX + Offset(0) + swQword, rXMM0);
                if(withResult)
                    g.genDoublePush(rXMM1);
            }
            else
            {
                only->genLValue(g);
                g.gen(cPop, rEBX);
                g.gen(cMov, rEAX, rEBX + Offset(0));
                g.gen(type == TOK_INC ? cInc : cDec, rEBX + Offset(0));
                if(withResult)
                    g.gen(cPush, rEAX);
            }
            return;
        default:
            performCommonGenPart(g);
    }
    if(withResult)
        for(int i = expType->size() - 1; i >= 0; --i)
            g.gen(cPush, rEAX + Offset(i * 4));
}

void CallNode::gen(AbstractGenerator& g, bool withResult)
{
    bool isPrintf = only->expType->name == "printf";
    SymbolTypeFunction* f = static_cast<SymbolTypeFunction*>(only->expType->resolveAlias());
    int retTypeSize = f->type->size();
    if(!isPrintf)
        g.gen(cSub, rESP, retTypeSize * 4);

    f->args->offset();
    for(int j = params.size() - 1; j >= 0; --j)
        params[j]->gen(g);

    g.gen(cCall, "f_" + only->expType->name);

    if(!isPrintf)
    {
        g.gen(cAdd, rESP, f->args->offset() * 4);
        if(!withResult)
            g.gen(cAdd, rESP, retTypeSize * 4);
    }
    else
    {
        g.gen(cAdd, rESP, params.size() * 4);
        if(withResult)
            g.gen(cPush, rEAX);
    }
}

void UnaryNode::gen(AbstractGenerator& g, bool withResult)
{
    if(type == TOK_ASTERISK)
    {
        only->genLValue(g);

        g.gen(cPop, rEAX);
        if(!only->expType->isArray())
            g.gen(cMov, rEAX, rEAX + Offset(0));
        if(withResult)
            if(expType->name == "double")
            {
                g.gen(cMovsd, rXMM0, rEAX + Offset(0) + swQword);
                g.genDoublePush(rXMM0);
            }
            else
                g.gen(cPush, rEAX + Offset(0));
        return;
    }
    else if(type == TOK_AMP)
    {
        only->genLValue(g);
        if(!withResult)
            g.gen(cPop, rEAX);
        return;
    }
    only->gen(g);
    if(only->expType->name == "double")
    {
        g.genDoublePop(rXMM0);
        switch(type)
        {
            case TOK_NOT:
                g.gen(cXor, rECX, rECX);
                g.gen(cComisd, rXMM0, string("d_zero") + Offset(0) + swQword);
                g.gen(cSetZ, rCL);
                if(withResult)
                    g.gen(cPush, rECX);
                return;
            case TOK_INC:
            case TOK_DEC:
                only->genLValue(g);
                g.gen(cPop, rEBX);
                g.gen(
                    type == TOK_INC ? cAddsd : cSubsd,
                    rXMM0, string("d_one") + Offset(0) + swQword
                );
                g.gen(cMovsd, rEBX + Offset(0) + swQword, rXMM0);
                break;
            case TOK_MINUS:
                g.gen(cMulsd, rXMM0, string("d_minus_one") + Offset(0) + swQword);
                break;
        }
        if(withResult)
            g.genDoublePush(rXMM0);
    }
    else
    {
        g.gen(cPop, rEAX);
        switch(type)
        {
            case TOK_INC:
            case TOK_DEC:
                only->genLValue(g);
                g.gen(cPop, rEBX);
                g.gen(type == TOK_INC ? cInc : cDec, rEBX + Offset(0));
                g.gen(cMov, rEAX, rEBX + Offset(0));
                break;
            case TOK_NOT:
                g.gen(cXor, rECX, rECX);
                g.gen(cTest, rEAX, rEAX);
                g.gen(cSetZ, rCL);
                g.gen(cMov, rEAX, rECX);
                break;
            case TOK_TILDA: g.gen(cNot, rEAX); break;
            case TOK_MINUS: g.gen(cNeg, rEAX); break;
        }
        if(withResult)
            g.gen(cPush, rEAX);
    }
}

void genCmp0Int(AbstractGenerator& g)
{
    g.gen(cPop, rEAX);
    g.gen(cXor, rECX, rECX);
    g.gen(cTest, rEAX, rEAX);
}

void genCmp0Double(AbstractGenerator& g)
{
    g.gen(cMovsd, rXMM0, rESP + Offset(0) + swQword);
    g.gen(cXor, rECX, rECX);
    g.gen(cComisd, rXMM0, string("d_zero") + Offset(0) + swQword);
}

void BinaryNode::gen(AbstractGenerator& g, bool withResult)
{
    bool leftIsDouble = left->expType->name == "double";
    if(type == TOK_LOGICAL_AND || type == TOK_LOGICAL_OR)
    {
        left->gen(g);
        if(leftIsDouble)
            genCmp0Double(g);
        else
            genCmp0Int(g);

        g.gen(cSetNZ, rCL);
        Argument* a = g.label();
        g.gen(type == TOK_LOGICAL_AND ? cJZ : cJNZ, *a);

        right->gen(g);
        if(right->expType->name == "double")
            genCmp0Double(g);
        else
            genCmp0Int(g);
        g.gen(cSetNZ, rCL);

        g.genLabel(a);
        if(withResult)
            g.gen(cPush, rECX);
        return;
    }
    left->gen(g);
    right->gen(g);
    if(leftIsDouble)
    {
        g.gen(cMovsd, rXMM1, rESP + Offset(0) + swQword);
        g.gen(cMovsd, rXMM0, rESP + Offset(8) + swQword);
        g.gen(cAdd, rESP, 16);

        switch(type)
        {
            case TOK_L:  g.genDoubleCmp(cSetB); break;
            case TOK_G:  g.genDoubleCmp(cSetA); break;
            case TOK_LE: g.genDoubleCmp(cSetBE); break;
            case TOK_GE: g.genDoubleCmp(cSetAE); break;
            case TOK_E:  g.genDoubleCmp(cSetE); break;
            case TOK_NE: g.genDoubleCmp(cSetNE); break;
            default:
                switch(type)
                {
                    case TOK_DIV:      g.gen(cDivsd, rXMM0, rXMM1); break;
                    case TOK_ASTERISK: g.gen(cMulsd, rXMM0, rXMM1); break;
                    case TOK_PLUS:     g.gen(cAddsd, rXMM0, rXMM1); break;
                    case TOK_MINUS:    g.gen(cSubsd, rXMM0, rXMM1); break;
                }
                if(withResult)
                {
                    g.gen(cSub, rESP, 8);
                    g.gen(cMovsd, rESP + Offset(0) + swQword, rXMM0);
                }
                return;
        }
    }
    else
    {
        g.gen(cPop, rEBX);
        g.gen(cPop, rEAX);

        switch(type)
        {
            case TOK_DIV:
            case TOK_MOD:
                g.gen(cCdq);
                g.gen(cIdiv, rEBX);
                if(type == TOK_MOD)
                    g.gen(cMov, rEAX, rEDX);
                break;
            case TOK_ASTERISK: g.gen(cImul, rEAX, rEBX); break;
            case TOK_PLUS:     g.gen(cAdd, rEAX, rEBX); break;
            case TOK_MINUS:    g.gen(cSub, rEAX, rEBX); break;
            case TOK_SHL:
            case TOK_SHR:
                g.gen(cMov, rECX, rEBX);
                g.gen(type == TOK_SHL ? cShl : cShr, rEAX, rCL);
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
    }
    if(withResult)
        g.gen(cPush, rEAX);
}

void SizeofNode::gen(AbstractGenerator& g, bool withResult)
{
    //this will never be used
    if(withResult)
    {
        if(symbolType != NULL)
            g.gen(cMov, rEAX, symbolType->size() * 4);
        else
            g.gen(cMov, rEAX, only->expType->size() * 4);
        g.gen(cPush, rEAX);
    }
}

void CastNode::gen(AbstractGenerator& g, bool withResult)
{
    element->gen(g, withResult);
    if(expType->name == "int" && element->expType->name == "double")
    {
        g.genDoublePop(rXMM0);
        g.gen(cCvttsd2si, rEAX, rXMM0);
        g.gen(cPush, rEAX);
    }
    else if(expType->name == "double" && element->expType->name == "int")
    {
        g.gen(cPop, rEAX);
        g.gen(cCvtsi2sd, rXMM0, rEAX);
        g.genDoublePush(rXMM0);
    }
}

void AssignmentNode::gen(AbstractGenerator& g, bool withResult)
{
    if(left->expType->name == "double")
    {
        left->genLValue(g);
        right->gen(g);

        if(type != TOK_ASSIGN)
        {
            left->gen(g);
            g.genDoublePop(rXMM0);
            g.genDoublePop(rXMM1);

            switch(type)
            {
                case TOK_ADD_ASSIGN: g.gen(cAddsd, rXMM0, rXMM1); break;
                case TOK_SUB_ASSIGN: g.gen(cSubsd, rXMM0, rXMM1); break;
                case TOK_MUL_ASSIGN: g.gen(cMulsd, rXMM0, rXMM1); break;
                case TOK_DIV_ASSIGN: g.gen(cDivsd, rXMM0, rXMM1); break;
            }
        }
        else
            g.genDoublePop(rXMM0);

        g.gen(cPop, rEAX);
        g.gen(cMovsd, rEAX + Offset(0) + swQword, rXMM0);
        if(withResult)
            g.genDoublePush(rXMM0);

        return;
    }

    if
    (
        type == TOK_ADD_ASSIGN || type == TOK_SUB_ASSIGN ||
        type == TOK_AND_ASSIGN || type == TOK_XOR_ASSIGN ||
        type == TOK_OR_ASSIGN || type == TOK_ASSIGN
    )
    {
        right->gen(g);
        left->genLValue(g);
        g.gen(cPop, rEAX);

        if(type != TOK_ASSIGN)
            g.gen(cPop, rEBX);
        switch(type)
        {
            case TOK_ADD_ASSIGN: g.gen(cAdd, rEAX + Offset(0), rEBX); break;
            case TOK_SUB_ASSIGN: g.gen(cSub, rEAX + Offset(0), rEBX); break;
            case TOK_AND_ASSIGN: g.gen(cAnd, rEAX + Offset(0), rEBX); break;
            case TOK_XOR_ASSIGN: g.gen(cXor, rEAX + Offset(0), rEBX); break;
            case TOK_OR_ASSIGN:  g.gen(cOr, rEAX + Offset(0), rEBX); break;
            default:
                for(unsigned int i = 0; i < right->expType->size(); ++i)
                {
                    g.gen(cPop, rEBX);
                    g.gen(cMov, rEAX + Offset(i * 4 * (varType == VT_LOCAL ? -1 : 1)), rEBX);
                }
        }
        if(withResult)
            for(int i = expType->size() - 1; i >= 0; --i)
                g.gen(cPush, rEAX + Offset(i * 4));
        return;
    }
    left->genLValue(g);
    left->gen(g);
    right->gen(g);
    g.gen(cPop, rEBX);
    g.gen(cPop, rEAX);
    switch(type)
    {
        case TOK_MUL_ASSIGN:
            g.gen(cImul, rEAX, rEBX);
            break;
        case TOK_MOD_ASSIGN:
        case TOK_DIV_ASSIGN:
            g.gen(cCdq);
            g.gen(cIdiv, rEBX);
            if(type == TOK_MOD_ASSIGN)
                g.gen(cMov, rEAX, rEDX);
            break;
        case TOK_SHL_ASSIGN:
        case TOK_SHR_ASSIGN:
            g.gen(cMov, rECX, rEBX);
            g.gen(type == TOK_SHL_ASSIGN ? cShl : cShr, rEAX, rCL);
            break;
    }
    g.gen(cPop, rECX);
    g.gen(cMov, rECX + Offset(0), rEAX);
    if(withResult)
        g.gen(cPush, rECX);
}

void ExpressionNode::gen(AbstractGenerator& g, bool withResult)
{
    left->gen(g, false);
    right->gen(g);
}

void TernaryNode::gen(AbstractGenerator& g, bool withResult)
{
    condition->gen(g);
    g.gen(cPop, rEAX);
    Argument* l1 = g.label();
    Argument* l2 = g.label();
    g.gen(cTest, rEAX, rEAX);
    g.gen(cJZ, *l1);
    thenOp->gen(g, withResult);
    g.gen(cJmp, *l2);
    g.genLabel(l1);
    elseOp->gen(g, withResult);
    g.genLabel(l2);
    //result will be in stack after execution
}

void IdentNode::genLValue(AbstractGenerator& g)
{
    assert(isLValue);
    SymbolVariable* v = static_cast<SymbolVariable*>(var);

    int od = expType->name == "double" ? 1 : 0;
    SizeInWords sw = od == 1 ? swQword : swDword;

    switch(v->varType)
    {
        case VT_LOCAL: g.gen(cLea, rEAX, rEBP + Offset(-(var->offset + od + 1) * 4) + sw); break;
        case VT_PARAM: g.gen(cLea, rEAX, rEBP + Offset((var->offset + 2) * 4) + sw); break;
        case VT_GLOBAL: g.gen(cPush, "v_" + var->name); return;
    }
    g.gen(cPush, rEAX);
}

void PostfixNode::performCommonGenPart(AbstractGenerator& g)
{
    TypedSymbolType* t = NULL;
    switch(type)
    {
        case TOK_L_SQUARE:
            only->genLValue(g);
            tail->gen(g);
            g.gen(cPop, rEBX);
            g.gen(cPop, rEAX);
            t = static_cast<TypedSymbolType*>(only->expType);
            g.gen(cImul, rEBX, t->type->size() * 4);
            g.gen(varType == VT_LOCAL ? cSub : cAdd, rEAX, rEBX);
            break;
        case TOK_DOT:
        case TOK_ARROW:
            SymbolTypeStruct* s = static_cast<SymbolTypeStruct*>(
                type == TOK_DOT ?
                    only->expType :
                    static_cast<SymbolTypePointer*>(only->expType)->type
            );
            s->calculateOffsets();
            only->genLValue(g);
            g.gen(cPop, rEAX);
            if(type == TOK_ARROW && !only->expType->isArray())
                g.gen(cMov, rEAX, rEAX + Offset(0));
            g.gen(
                only->varType == VT_LOCAL ? cSub : cAdd,
                rEAX, static_cast<IdentNode*>(tail)->var->offset * 4
            );
            break;
    }
}

void PostfixNode::genLValue(AbstractGenerator& g)
{
    assert(isLValue);
    performCommonGenPart(g);
    g.gen(cPush, rEAX);
}

void BinaryNode::genLValue(AbstractGenerator& g)
{
    left->genLValue(g);
    right->gen(g);
    g.gen(cPop, rEBX);
    g.gen(cPop, rEAX);
    switch(type)
    {
        case TOK_PLUS:  g.gen(cAdd, rEAX, rEBX); break;
        case TOK_MINUS: g.gen(cSub, rEAX, rEBX); break;
        default:
            assert(true);
    }
    g.gen(cPush, rEAX);
}

void UnaryNode::genLValue(AbstractGenerator& g)
{
    assert(isLValue);
    only->genLValue(g);
    if(type != TOK_ASTERISK || !only->expType->isArray())
    {
        g.gen(cPop, rEAX);
        g.gen(cMov, rEAX, rEAX + Offset(0));
        g.gen(cPush, rEAX);
    }
}

}
