#include "parser.h"

namespace LuCCompiler
{

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
                    f->body->tryOptimize();
                //always true/false
                //vars not used?
                //consts
                //wrap initializers
            }
        }
}

Node* AssignmentNode::tryOptimize()
{
    right = static_cast<ENode*>(right->tryOptimize());
    return this;
}

Node* CastNode::tryOptimize()
{
    element = static_cast<ENode*>(element->tryOptimize());
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

Node* BinaryNode::tryOptimize()
{
    left = static_cast<ENode*>(left->tryOptimize());
    right = static_cast<ENode*>(right->tryOptimize());
    if(!left->isConst() || !right->isConst()) //a - 5 - 4 ignored
        return this;

    double l, r;
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

Node* CompoundStatement::tryOptimize()
{
    for(unsigned int i = 0; i < _items->size(); ++i)
        (*_items)[i] = (*_items)[i]->tryOptimize();
    return this;
}

Node* ExpressionStatement::tryOptimize()
{
    _expr = _expr->tryOptimize();
    return this;
}

}
