#include "parser.h"

namespace LuCCompiler
{

double getConstValue(ENode* node);

void Parser::optimize()
{
    SymbolTable& sym(*_symbols->_root);

    Symbol* m = findSymbol("main", NT_NAME);
    if(m == NULL || m->classType != CT_FUNCTION)
        throw NoFunctionDefinition("main");
    SymbolTypeFunction* main = static_cast<SymbolTypeFunction*>(m);

    for(vector<Symbol*>::iterator i = sym.begin(); i != sym.end(); ++i)
        if((*i)->classType == CT_FUNCTION)
        {
            if((*i)->name != "main" && !main->findCall(static_cast<SymbolTypeFunction*>(*i)))
                sym.erase(i);
            else
            {
                SymbolTypeFunction* f = static_cast<SymbolTypeFunction*>(*i);
                if(f->body != NULL)
                    f->body->optimized();
            }
        }
    sym.optimizeInitializers();
}

Node* AssignmentNode::optimized()
{
    left = static_cast<ENode*>(left->optimized());
    right = static_cast<ENode*>(right->optimized());
    return this;
}

Node* CastNode::optimized()
{
    element = static_cast<ENode*>(element->optimized());
    ENode* rep = this;
    if(element->isIntConst())
    {
        if(type->name == "float")
            rep = new FloatNode(static_cast<IntNode*>(element)->value, expType);
        else if(type->name == "int")
            rep = element;
    }
    else if(element->isFloatConst())
    {
        if(type->name == "int")
            rep = new IntNode(static_cast<FloatNode*>(element)->value, expType);
        else if(type->name == "float")
            rep = element;
    }
    return rep;
}

Node* BinaryNode::optimized()
{
    double l, r;
    left = static_cast<ENode*>(left->optimized());
    if(!left->isConst())
        return this;
    if(type == TOK_LOGICAL_AND)
    {
        if(getConstValue(left) == 0)
            return new IntNode(0, expType);
    }
    else if(type == TOK_LOGICAL_OR)
    {
        if(getConstValue(left) != 0)
            return new IntNode(1, expType);
    }
    right = static_cast<ENode*>(right->optimized());
    if(!right->isConst()) //a - 5 - 4 ignored
        return this;

    if(left->isFloatConst())
    {
        l = static_cast<FloatNode*>(left)->value;
        r = static_cast<FloatNode*>(right)->value;
    }
    else
    {
        l = static_cast<IntNode*>(left)->value;
        r = static_cast<IntNode*>(right)->value;
    }
    switch(type)
    {
        case TOK_L:  return new IntNode(l < r, expType);
        case TOK_G:  return new IntNode(l > r, expType);
        case TOK_LE: return new IntNode(l <= r, expType);
        case TOK_GE: return new IntNode(l >= r, expType);
        case TOK_E:  return new IntNode(l == r, expType);
        case TOK_NE: return new IntNode(l != r, expType);
        case TOK_LOGICAL_AND: return new IntNode(l && r, expType);
        case TOK_LOGICAL_OR:  return new IntNode(l || r, expType);
        default:
            break;
    }
    if(left->isIntConst())
    {
        int il = static_cast<IntNode*>(left)->value;
        int ir = static_cast<IntNode*>(right)->value;
        switch(type)
        {
            case TOK_ASTERISK: return new IntNode(il * ir, expType);
            case TOK_DIV:   return new IntNode(il / ir, expType);
            case TOK_MOD:   return new IntNode(il % ir, expType);
            case TOK_PLUS:  return new IntNode(il + ir, expType);
            case TOK_MINUS: return new IntNode(il - ir, expType);
            case TOK_SHL: return new IntNode(il << ir, expType);
            case TOK_SHR: return new IntNode(il >> ir, expType);
            case TOK_AMP: return new IntNode(il & ir, expType);
            case TOK_XOR: return new IntNode(il ^ ir, expType);
            case TOK_OR:  return new IntNode(il | ir, expType);
        }
    }
    else
    {
        switch(type)
        {
            case TOK_ASTERISK: return new FloatNode(l * r, expType);
            case TOK_DIV:   return new FloatNode(l / r, expType);
            case TOK_PLUS:  return new FloatNode(l + r, expType);
            case TOK_MINUS: return new FloatNode(l - r, expType);
        }
    }
    return this;
}

Node* SizeofNode::optimized()
{
    return new IntNode(symbolType != NULL ?
        symbolType->size() * 4 : only->expType->size() * 4, expType);
}

Node* UnaryNode::optimized()
{
    only = static_cast<ENode*>(only->optimized());
    if(type == TOK_PLUS)
        return only;
    if(!only->isConst())
        return this;

    int iValue = 0;
    float fValue = 0;
    if(only->isIntConst())
        iValue = static_cast<IntNode*>(only)->value;
    else
        fValue = static_cast<FloatNode*>(only)->value;
    switch(type)
    {
        case TOK_TILDA:
            return new IntNode(~iValue, expType);
        case TOK_NOT:
            if(only->isIntConst())
                return new IntNode(!iValue, expType);
            else
                return new FloatNode(!fValue, expType);
        case TOK_MINUS:
            if(only->isIntConst())
                return new IntNode(-iValue, expType);
            else
                return new FloatNode(-fValue, expType);
        default: return this;
    }
}

Node* PostfixNode::optimized()
{
    if(tail != NULL)
        tail = static_cast<ENode*>(tail->optimized());
    return this;
}

Node* CompoundStatement::optimized()
{
    locals->optimizeInitializers();
    vector<Node*>::iterator i = items->begin();
    while(i != items->end())
    {
        *i = (*i)->optimized();
        if((*i)->isJump())
        {
            if(++i != items->end())
                i = items->erase(i, items->end());
            break;
        }
        i++;
    }
    return this;
}

Node* ExpressionStatement::optimized()
{
    expr = static_cast<ENode*>(expr->optimized());
    return this;
}

double getConstValue(ENode* node)
{
    return node->isIntConst() ?
        static_cast<IntNode*>(node)->value :
        static_cast<FloatNode*>(node)->value;
}

Node* TernaryNode::optimized()
{
    condition = static_cast<ENode*>(condition->optimized());
    thenOp = static_cast<ENode*>(thenOp->optimized());
    elseOp = static_cast<ENode*>(elseOp->optimized());
    if(!condition->isConst())
        return this;
    return getConstValue(condition) != 0 ? thenOp : elseOp;
}

Node* SelectionStatement::optimized()
{
    cond = static_cast<ENode*>(cond->optimized());
    thenExp = thenExp->optimized();
    if(elseExp != NULL)
        elseExp = elseExp->optimized();
    if(!cond->isConst())
        return this;
    return getConstValue(cond) != 0 ? thenExp :
        elseExp != NULL ? elseExp : new EmptyExpressionStatement();
}

Node* IterationStatement::optimized()
{
    cond = static_cast<ENode*>(cond->optimized());
    loop = loop->optimized();
    if(!cond->isConst())
        return this;
    if(getConstValue(cond) == 0)
        return type == TOK_DO ? loop : new EmptyExpressionStatement();
    return this;
}

Node* ForStatement::optimized()
{
    iterators->optimizeInitializers();
    if(init != NULL)
        init = init->optimized();
    cond = cond->optimized();
    if(mod != NULL)
        mod = mod->optimized();
    loop = loop->optimized();
    ExpressionStatement* tcond = dynamic_cast<ExpressionStatement*>(cond);
    if(tcond == NULL || !tcond->expr->isConst())
        return this;
    if(getConstValue(tcond->expr) == 0)
        return new EmptyExpressionStatement();
    return this;
}

void SymbolTable::optimizeInitializers()
{
    for(vector<Symbol*>::iterator i = _ordered.begin(); i != _ordered.end(); ++i)
        if((*i)->classType == CT_VAR)
        {
            SymbolVariable* v = static_cast<SymbolVariable*>(*i);
            if(v->initializer != NULL)
                v->initializer = v->initializer->optimized();
        }
}

}
