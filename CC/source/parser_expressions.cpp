#include "parser.h"
#include "token.h"
#include "symbol.h"
#include "complex_symbol.h"
#include "expression.h"
#include "dictionaries.h"

using namespace std;

namespace LuCCompiler
{

void Parser::checkArgumentsArithmetic(BinaryNode* node)
{
    if(!isArithmetic(*node->right->expType))
        throw makeException(
            "Right operand of " + operationName(node->type) +
            " is expected to be of arithmetic type"
        );
    if(!isArithmetic(*node->left->expType))
        throw makeException(
            "Left operand of " + operationName(node->type) +
            " is expected to be of arithmetic type"
        );
}

void Parser::performImplicitCast(BinaryNode* node, bool rightOnly)
{
    if(isDouble(*node->left->expType))
    {
        if(!isDouble(*node->right->expType))
        {
            node->right = new CastNode(_double, node->right);
            node->right->expType = _double;
        }
    }
    else if(isDouble(*node->right->expType))
    {
        if(rightOnly)
            throw makeException("Cannot convert left operand to type double");
        node->left = new CastNode(_double, node->left);
        node->left->expType = _double;
    }
    else
        node->expType = _int;
}

ENode* Parser::parseBinaryExpression(int priority)
{
    BinaryNode* node = NULL;
    BinaryNode* ptrMulNode = NULL;
    IdentNode* tmpl = NULL;
    IdentNode* tmpr = NULL;
    ENode* tmp = priority < 13 ?
        parseBinaryExpression(priority + 1) : parseCastExpression();
    do
    {
        map<TokenType, int>::iterator tok =
            Dictionaries::binaries()->find(tokenType());
        if(tok == Dictionaries::binaries()->end() || tok->second != priority)
            return tmp;

        node = new BinaryNode(tokenType(), tmp);
        _tokens->next();
        node->right = priority < 13 ?
            parseBinaryExpression(priority + 1) : parseCastExpression();

        if(_mode == PM_SYMBOLS)
            switch(node->type)
            {
                case TOK_PLUS:
                case TOK_MINUS:
                    if(node->left->expType->isPointer())
                    {
                        if(!isInt(*node->right->expType))
                            throw makeException(
                                "Right operand of " + operationName(node->type) +
                                " operation with pointer must be of type int"
                            );
                        ptrMulNode = new BinaryNode(TOK_ASTERISK, node->right);
                        ptrMulNode->right = new IntNode(
                            static_cast<SymbolTypePointer*>(node->left->expType)->type->size() * 4
                        );
                        ptrMulNode->expType = ptrMulNode->left->expType;
                        ptrMulNode->varType = ptrMulNode->left->varType;
                        node->right = ptrMulNode;

                        node->expType = node->left->expType;
                        node->varType = node->left->varType;

                        if(node->varType == VT_LOCAL)
                            node->type = node->type == TOK_PLUS ? TOK_MINUS : TOK_PLUS;
                        break;
                    }
                case TOK_ASTERISK:
                case TOK_DIV:
                    checkArgumentsArithmetic(node);
                    node->expType = _double;
                    performImplicitCast(node);
                    break;
                case TOK_E:
                case TOK_NE:
                    node->expType = _int;
                    tmpl = dynamic_cast<IdentNode*>(node->left);
                    tmpr = dynamic_cast<IdentNode*>(node->right);
                    if(tmpl != NULL && tmpl->var == _NULL)
                    {
                        if(!node->right->expType->isPointer())
                            throw makeException("Right operand is expected to be of pointer type");
                        break;
                    }
                    else if(tmpr != NULL && tmpr->var == _NULL)
                    {
                        if(!node->left->expType->isPointer())
                            throw makeException("Left operand is expected to be of pointer type");
                        break;
                    }
                case TOK_L:
                case TOK_G:
                case TOK_LE:
                case TOK_GE:
                    node->expType = _int;
                    if(node->left->expType->isPointer())
                    {
                        if(*node->left->expType != *node->right->expType)
                            throw makeException("Incompatible pointer types");
                        break;
                    }
                    checkArgumentsArithmetic(node);
                    performImplicitCast(node);
                    break;
                case TOK_LOGICAL_AND:
                case TOK_LOGICAL_OR:
                    node->expType = _int;
                    if(!node->left->expType->isPointer() && !isArithmetic(*node->left->expType))
                        throw makeException("Left operand is expected to be of scalar type");
                    if(!node->right->expType->isPointer() && !isArithmetic(*node->right->expType))
                        throw makeException("Right operand is expected to be of scalar type");
                    break;
                default:
                    //TOK_MOD TOK_SHL TOK_SHR TOK_AMP TOK_XOR TOK_OR
                    if(!isInt(*node->left->expType) || !isInt(*node->right->expType))
                        throw makeException("Both operands must be of type int");
                    node->expType = _int;
            }
        tmp = node;
    }
    while(true);

    return node;
}

ENode* Parser::parsePrimaryExpression()
{
    ENode* node = NULL;
    switch(tokenType())
    {
        case TOK_OCT_CONST:
        case TOK_DEC_CONST:
        case TOK_HEX_CONST:
            node = new IntNode(_tokens->get().value.intValue);
            if(_mode == PM_SYMBOLS)
                node->expType = _int;
            break;
        case TOK_CHAR_CONST:
            node = new CharNode(*_tokens->get().value.strValue);
            if(_mode == PM_SYMBOLS)
                node->expType = _int;
            break;
        case TOK_STR_CONST:
            node = new StringNode(*_tokens->get().value.strValue);
            if(_mode == PM_SYMBOLS)
                node->expType = static_cast<SymbolType*>(getSymbol("int*"));
            break;
        case TOK_DOUBLE_CONST:
            node = new DoubleNode(_tokens->get().value.doubleValue);
            if(_mode == PM_SYMBOLS)
                node->expType = _double;
            break;
        case TOK_IDENT:
            if(_mode == PM_NO_SYMBOLS)
                node = new IdentNode(_tokens->get().text);
            else
            {
                Symbol* v = getSymbol(_tokens->get().text);
                if(v->classType != CT_VAR && v->classType != CT_FUNCTION)
                    throw makeException("Unexpected symbol " + _tokens->get().text);
                node = new IdentNode(_tokens->get().text, v);
                if(v->classType == CT_VAR)
                {
                    SymbolVariable* var = static_cast<SymbolVariable*>(v);
                    node->expType = var->type;
                    node->varType = var->varType;
                    var->used = true;
                }
                else
                {
                    node->expType = static_cast<SymbolTypeFunction*>(v);
                    usedFunctions->insert(static_cast<SymbolTypeFunction*>(v));
                }
            }
            break;
        case TOK_L_BRACKET:
            _tokens->next();
            node = parseExpression();
            if(tokenType() != TOK_R_BRACKET)
                throw makeException("')' expected");
            break;
        default:
            throw makeException("Unexpected token " + TOKEN_TYPE_NAME[tokenType()]);
    }
    _tokens->next();
    return node;
}

PostfixNode* Parser::parseFunctionCall(ENode* core)
{
    CallNode* node = new CallNode();
    node->only = core;

    if(_mode == PM_SYMBOLS && !core->expType->isFunction())
        throw makeException("Left must be function");
    _tokens->next();
    while(tokenType() != TOK_R_BRACKET)
    {
        node->params.push_back(parseAssignmentExpression());
        if(tokenType() == TOK_R_BRACKET)
            break;
        consumeTokenOfType(TOK_COMMA, "',' expected");
    }
    consumeTokenOfType(TOK_R_BRACKET, "')' expected");
    if(_mode == PM_NO_SYMBOLS)
        return node;

    SymbolTypeFunction* f =
        static_cast<SymbolTypeFunction*>(core->expType->resolveAlias());
    node->expType = f->type;
    node->varType = VT_LOCAL;

    if(f->name == "printf")
        return node;

    unsigned int j = 0;
    for(unsigned int i = 0; i < f->args->size(); ++i)
    {
        if((*f->args)[i]->classType != CT_VAR)
            continue;
        SymbolVariable* cv = static_cast<SymbolVariable*>((*f->args)[i]);
        if(j >= node->params.size())
            throw makeException("Not enough arguments");
        else if(*cv->type != *node->params[j]->expType)
        {
            if(node->params[j]->expType->castTo != NULL)
            {
                node->params[j] =
                new CastNode(node->params[j]->expType->castTo, node->params[j]);
                static_cast<CastNode*>(node->params[j])->type->castTo = NULL;
                node->params[j]->expType =
                    static_cast<CastNode*>(node->params[j])->type;
            }
            else
                throw makeException(
                    "Incompatible types in function call: argument " + itostr(j + 1)
                );
        }
        j++;
    }
    if(j < node->params.size())
        throw makeException("Too many arguments");

    usedFunctions->insert(f);

    return node;
}

ENode* Parser::parsePostfixExpression()
{
    PostfixNode* node = new PostfixNode();
    ENode* core = parsePrimaryExpression();
    node->only = core;
    while(true)
    {
        node->type = tokenType();
        switch(tokenType())
        {
            case TOK_L_SQUARE:
                if(_mode == PM_SYMBOLS && !core->expType->isPointer())
                    throw makeException("Left must be pointer or array");
                _tokens->next();
                node->tail = parseExpression();
                if(_mode == PM_SYMBOLS && !isInt(*node->tail->expType))
                    throw makeException("expression must be of type int");
                consumeTokenOfType(TOK_R_SQUARE, "']' expected");

                if(_mode == PM_SYMBOLS)
                {
                    node->expType = static_cast<SymbolTypePointer*>(
                        core->expType->resolveAlias()
                    )->type;
                    node->varType = node->only->varType;
                    node->isLValue = true;
                }
                break;
            case TOK_L_BRACKET:
                delete node;
                node = parseFunctionCall(core);
                break;
            case TOK_ARROW:
                if(_mode == PM_SYMBOLS && (
                    !core->expType->isPointer() ||
                    !static_cast<SymbolTypePointer*>(core->expType)->type->isStruct()
                ))
                    throw makeException("Left must be struct");
                _tokens->next();
                if(tokenType() != TOK_IDENT)
                    throw makeException("Identifier expected");
                if(_mode == PM_SYMBOLS)
                    _symbols->push(static_cast<SymbolTypeStruct*>(
                        static_cast<SymbolTypePointer*>(
                            core->expType->resolveAlias()
                    )->type)->fields);
                node->tail = parsePrimaryExpression();
                if(_mode == PM_SYMBOLS)
                {
                    _symbols->pop();
                    node->expType = node->tail->expType;
                    node->varType = node->only->varType;
                    node->isLValue = true;
                }
                break;
            case TOK_DOT:
                if(_mode == PM_SYMBOLS && !core->expType->isStruct())
                    throw makeException("Left must be struct");
                _tokens->next();
                if(tokenType() != TOK_IDENT)
                    throw makeException("Identifier expected");
                if(_mode == PM_SYMBOLS)
                    _symbols->push(static_cast<SymbolTypeStruct*>(
                        core->expType->resolveAlias()
                    )->fields);
                node->tail = parsePrimaryExpression();
                if(_mode == PM_SYMBOLS)
                {
                    _symbols->pop();
                    node->expType = node->tail->expType;
                    node->varType = node->only->varType;
                    node->isLValue = true;
                }
                break;
            case TOK_INC:
            case TOK_DEC:
                if(_mode == PM_SYMBOLS)
                {
                    if(!core->isLValue)
                        throw makeException("Lvalue required as operand");
                    if(core->expType->isFunction() || core->expType->isStruct())
                        throw makeException("Wrong type argument to postfix ++/--");
                    node->expType = core->expType;
                }
                _tokens->next();
                break;
            default:
                return node->only;
        }
        core = node;
        node = new PostfixNode();
        node->only = core;
    }
}

ENode* Parser::parseUnaryExpression()
{
    UnaryNode* node = NULL;
    SizeofNode* size = NULL;
    switch(tokenType())
    {
        case TOK_INC:
        case TOK_DEC:
            node = new UnaryNode();
            node->type = tokenType();
            _tokens->next();
            node->only = parseUnaryExpression();
            if(_mode == PM_SYMBOLS)
            {
                if(!node->only->isLValue)
                    throw makeException("Lvalue required as operand");
                if(node->only->expType->isFunction() || node->only->expType->isStruct())
                    throw makeException("Wrong type argument to unary ++/--");
                node->expType = node->only->expType;
            }
            break;
        case TOK_SIZEOF:
            _tokens->next();
            size = new SizeofNode();
            if(tokenType() != TOK_L_BRACKET)
                size->only = parseUnaryExpression();
            else
            {
                _tokens->next();
                size->symbolType = parseTypeName();
                if(size->symbolType == NULL)
                    size->only = parseUnaryExpression();
                consumeTokenOfType(TOK_R_BRACKET, "')' expected");
            }
            if(_mode == PM_SYMBOLS)
                size->expType = _int;
            return size;
        case TOK_PLUS:
        case TOK_MINUS:
        case TOK_TILDA:
        case TOK_AMP:
        case TOK_ASTERISK:
        case TOK_NOT:
            node = new UnaryNode();
            node->type = tokenType();
            _tokens->next();
            node->only = parseCastExpression();
            if(_mode == PM_SYMBOLS)
                switch(node->type)
                {
                    case TOK_PLUS:
                    case TOK_MINUS:
                        if(!isArithmetic(*node->only->expType))
                            throw makeException("Wrong type argument to unary +/-");
                        node->expType = node->only->expType;
                        break;
                    case TOK_TILDA:
                        if(!isInt(*node->only->expType))
                            throw makeException("Wrong type argument to bit-complement");
                        node->expType = node->only->expType;
                        break;
                    case TOK_AMP:
                        if(!node->only->isLValue)
                            throw makeException("Lvalue required as operand");
                        node->expType = new SymbolTypePointer(node->only->expType, "");
                        node->varType = node->only->varType;
                        break;
                    case TOK_ASTERISK:
                        if(!node->only->expType->isPointer())
                            throw makeException("Wrong type argument to unary *");
                        node->expType = static_cast<SymbolTypePointer*>(
                            node->only->expType->resolveAlias()
                        )->type;
                        node->varType = node->only->varType;
                        node->isLValue = true;
                        break;
                    case TOK_NOT:
                        if(node->only->expType->isStruct())
                            throw makeException("Wrong type argument to logical negation");
                        node->expType = _int;
                        break;
                }
            break;
        default:
            return parsePostfixExpression();
    }
    return node;
}

ENode* Parser::parseCastExpression()
{
    if(tokenType() == TOK_L_BRACKET)
    {
        _tokens->lookForward();
        SymbolType* type = parseTypeName();
        if(type == NULL)
        {
            _tokens->rollBack();
            return parseUnaryExpression();
        }
        consumeTokenOfType(TOK_R_BRACKET, "')' expected");
        ENode* node = NULL;
        if(tokenType() == TOK_L_BRACE)
        {
            _tokens->next();
            node = parseInitializerList();
            consumeTokenOfType(TOK_R_BRACE, "'}' expected");
        }
        else
            node = parseCastExpression();

        CastNode* cast = new CastNode(type, node);
        if(_mode == PM_SYMBOLS)
        {
            cast->expType = cast->type;
            cast->varType = node->varType;
        }
        return cast;
    }
    return parseUnaryExpression();
}

ENode* Parser::parseConditionalExpression()
{
    TernaryNode* node = NULL;
    ENode* tmp = parseBinaryExpression(4);
    if(tokenType() != TOK_QUEST)
        return tmp;
    node = new TernaryNode();
    node->type = TOK_QUEST;
    node->condition = tmp;
    _tokens->next();
    node->thenOp = parseExpression();
    consumeTokenOfType(TOK_COLON, "':' expected");
    node->elseOp = parseConditionalExpression();

    if(_mode == PM_SYMBOLS)
    {
        if(*node->thenOp->expType != *node->elseOp->expType)
        {
            if
            (
                *node->thenOp->expType == *_NULL->type && node->elseOp->expType->isPointer() ||
                *node->elseOp->expType == *_NULL->type && node->thenOp->expType->isPointer()
            )
            {
                node->thenOp->expType = node->elseOp->expType = _NULL->type;
            }
            else if(isDouble(*node->thenOp->expType) && isInt(*node->elseOp->expType))
            {
                node->elseOp = new CastNode(_double, node->elseOp);
                node->elseOp->expType = _double;
            }
            else if(isDouble(*node->elseOp->expType) && isInt(*node->thenOp->expType))
            {
                node->thenOp = new CastNode(_double, node->thenOp);
                node->thenOp->expType = _double;
            }
            else
                throw makeException("Incompatible types in ?:");
        }
        node->expType = node->thenOp->expType;
    }

    return node;
}

ENode* Parser::parseAssignmentExpression()
{
    AssignmentNode* node = NULL;
    BinaryNode* ptrMulNode = NULL;
    ENode* tmp = parseConditionalExpression();
    switch(tokenType())
    {
        case TOK_ASSIGN:
        case TOK_MUL_ASSIGN:
        case TOK_DIV_ASSIGN:
        case TOK_MOD_ASSIGN:
        case TOK_ADD_ASSIGN:
        case TOK_SUB_ASSIGN:
        case TOK_SHL_ASSIGN:
        case TOK_SHR_ASSIGN:
        case TOK_AND_ASSIGN:
        case TOK_XOR_ASSIGN:
        case TOK_OR_ASSIGN:

            if(_mode == PM_SYMBOLS && !tmp->isLValue)
                throw makeException("Lvalue required as left operand in assignment");

            node = new AssignmentNode(tokenType(), tmp);
            _tokens->next();
            node->right = parseAssignmentExpression();

            if(_mode == PM_SYMBOLS)
            {
                switch(node->type)
                {
                    case TOK_ASSIGN:
                        if(node->left->expType->isPointer())
                        {
                            if(node->left->expType->isArray())
                                throw makeException("Incompatible types");
                            if(*node->left->expType != *node->right->expType)
                                if
                                (
                                    *node->right->expType != *_NULL->type &&
                                    *node->left->expType != *_NULL->type
                                )
                                    throw makeException("Incompatible types");
                            break;
                        }
                        else if(!isArithmetic(*node->left->expType))
                        {
                            if(*node->left->expType != *node->right->expType)
                                throw makeException("Incompatible types");
                            break;
                        }
                        checkArgumentsArithmetic(node);
                        performImplicitCast(node, true);
                        break;
                    case TOK_ADD_ASSIGN:
                    case TOK_SUB_ASSIGN:
                        if(node->left->expType->isPointer())
                        {
                            if(!isInt(*node->right->expType))
                                throw makeException("Right operand is expected to be of type int");
                            ptrMulNode = new BinaryNode(TOK_ASTERISK, node->right);
                            ptrMulNode->right = new IntNode(
                                static_cast<SymbolTypePointer*>(node->left->expType)->type->size() * 4
                            );
                            ptrMulNode->expType = ptrMulNode->left->expType;
                            ptrMulNode->varType = ptrMulNode->left->varType;
                            node->right = ptrMulNode;
                            break;
                        }
                    case TOK_MUL_ASSIGN:
                    case TOK_DIV_ASSIGN:
                        checkArgumentsArithmetic(node);
                        performImplicitCast(node, true);
                        break;
                    default:
                        if(!isInt(*node->left->expType) || !isInt(*node->right->expType))
                            throw makeException("Both operands must be of type int");
                }
                node->expType = node->left->expType;
                node->varType = node->left->varType;
            }
            return node;
        default:
            return tmp;
    }
}

ENode* Parser::parseExpression()
{
    ENode* tmp = parseAssignmentExpression();
    if(tokenType() != TOK_COMMA)
        return tmp;
    ExpressionNode* node = new ExpressionNode(TOK_COMMA, tmp);
    _tokens->next();
    node->right = parseExpression();
    if(_mode == PM_SYMBOLS)
        node->expType = node->right->expType;
    return node;
}

}
