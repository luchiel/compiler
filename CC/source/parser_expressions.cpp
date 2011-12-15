#include "parser.h"
#include "token.h"
#include "symbol.h"
#include "complex_symbol.h"
#include "expression.h"
#include "operations.h"

using namespace std;

namespace LuCCompiler
{

ENode* Parser::parseBinaryExpression(int priority)
{
    TokenType opType = TOK_UNDEF;
    BinaryNode* node = NULL;
    ENode* tmp = priority < 13 ?
        parseBinaryExpression(priority + 1) : parseCastExpression();
    do
    {
        if(opType == TOK_UNDEF)
            opType = tokenType();
        map<TokenType, int>::iterator tok =
            OperationGroups::binaries()->find(tokenType());
        if(tok == OperationGroups::binaries()->end() || tok->second != priority)
            return tmp;

        node = new BinaryNode();
        node->_type = tokenType();
        node->_left = tmp;
        _tokens->next();
        node->_right = priority < 13 ?
            parseBinaryExpression(priority + 1) : parseCastExpression();
        tmp = node;
    }
    while(opType == tokenType());

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
                node->expType = static_cast<SymbolType*>(getSymbol("int"));
            break;
        case TOK_CHAR_CONST:
            node = new CharNode(*_tokens->get().value.strValue);
            if(_mode == PM_SYMBOLS)
                node->expType = static_cast<SymbolType*>(getSymbol("int"));
            break;
        case TOK_STR_CONST:
            node = new StringNode(*_tokens->get().value.strValue);
            if(_mode == PM_SYMBOLS)
                node->expType = static_cast<SymbolType*>(getSymbol("int*"));
            break;
        case TOK_FLOAT_CONST:
            node = new FloatNode(_tokens->get().value.floatValue);
            if(_mode == PM_SYMBOLS)
                node->expType = static_cast<SymbolType*>(getSymbol("float"));
            break;
        case TOK_IDENT:
            if(_mode == PM_NO_SYMBOLS)
                node = new IdentNode(_tokens->get().text);
            else
            {
                Symbol* v = getSymbol(_tokens->get().text);
                if(v->classType != CT_VAR && v->classType != CT_FUNCTION)
                    throw makeException("Unexpected symbol " + _tokens->get().text);
                node = new IdentNode(_tokens->get().text, static_cast<SymbolVariable*>(v));
                if(v->classType == CT_VAR)
                    node->expType = static_cast<SymbolVariable*>(v)->type;
                else
                    node->expType = static_cast<SymbolTypeFunction*>(v);
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
                if
                (
                    _mode == PM_SYMBOLS &&
                    core->expType->classType != CT_POINTER &&
                    core->expType->classType != CT_ARRAY
                )
                    throw makeException("Left must be pointer or array");
                _tokens->next();
                node->tail.push_back(parseExpression());
                if
                (
                    _mode == PM_SYMBOLS &&
                    node->tail[node->tail.size() - 1]->expType != getSymbol("int")
                )
                    throw makeException("expression must be of type int");
                consumeTokenOfType(TOK_R_SQUARE, "']' expected");

                if(_mode == PM_SYMBOLS)
                    node->expType = static_cast<SymbolTypePointer*>(core->expType)->type;
                break;
            case TOK_L_BRACKET:
                if(_mode == PM_SYMBOLS && core->expType->classType != CT_FUNCTION)
                    throw makeException("Left must be function");
                _tokens->next();
                while(tokenType() != TOK_R_BRACKET)
                {
                    node->tail.push_back(parseAssignmentExpression());
                    if(tokenType() == TOK_R_BRACKET)
                        break;
                    consumeTokenOfType(TOK_COMMA, "',' expected");
                }
                if(_mode == PM_SYMBOLS)
                {
                    node->expType = static_cast<SymbolTypeFunction*>(core->expType)->type;

                    SymbolTypeFunction* f = static_cast<SymbolTypeFunction*>(core->expType);
                    unsigned int j = 0;
                    for(unsigned int i = 0; i < f->args->size(); ++i)
                    {
                        if((*f->args)[i]->classType != CT_VAR)
                            continue;
                        SymbolVariable* cv = static_cast<SymbolVariable*>((*f->args)[i]);
                        if(j >= node->tail.size())
                            throw makeException("Not enough arguments");
                        else if(*cv->type != *node->tail[j]->expType)
                            throw makeException(
                                "Incompatible types in function call: argument " + itostr(j + 1)
                            );
                        j++;
                    }
                    if(j < node->tail.size())
                        throw makeException("Too many arguments");
                }
                consumeTokenOfType(TOK_R_BRACKET, "')' expected");
                break;
            case TOK_ARROW:
                if(_mode == PM_SYMBOLS && (
                    core->expType->classType != CT_POINTER ||
                    static_cast<SymbolTypePointer*>(core->expType)->type->classType != CT_STRUCT
                ))
                    throw makeException("Left must be struct");
                _tokens->next();
                if(tokenType() != TOK_IDENT)
                    throw makeException("Identifier expected");
                if(_mode == PM_SYMBOLS)
                    _symbols->push(
                        static_cast<SymbolTypeStruct*>(
                            static_cast<SymbolTypePointer*>(core->expType)->type
                        )->fields
                    );
                node->tail.push_back(parsePrimaryExpression());
                if(_mode == PM_SYMBOLS)
                {
                    _symbols->pop();
                    node->expType = node->tail[0]->expType;
                }
                break;
            case TOK_DOT:
                if(_mode == PM_SYMBOLS && core->expType->classType != CT_STRUCT)
                    throw makeException("Left must be struct");
                _tokens->next();
                if(tokenType() != TOK_IDENT)
                    throw makeException("Identifier expected");
                if(_mode == PM_SYMBOLS)
                    _symbols->push(static_cast<SymbolTypeStruct*>(core->expType)->fields);
                node->tail.push_back(parsePrimaryExpression());
                if(_mode == PM_SYMBOLS)
                {
                    _symbols->pop();
                    node->expType = node->tail[0]->expType;
                }
                break;
            case TOK_INC:
            case TOK_DEC:
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
                node->expType = node->only->expType;
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
                size->expType = static_cast<SymbolType*>(getSymbol("int"));
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
                        node->expType = node->only->expType;
                        break;
                    case TOK_TILDA:
                        break;
                    case TOK_AMP:
                        node->expType = new SymbolTypePointer(node->only->expType, "");
                        break;
                    case TOK_ASTERISK:
                        break;
                    case TOK_NOT:
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

        CastNode* cast = new CastNode(type);
        cast->element = node;
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
    node->_type = TOK_QUEST;
    node->_if = tmp;
    _tokens->next();
    node->_then = parseExpression();
    consumeTokenOfType(TOK_COLON, "':' expected");
    node->_else = parseConditionalExpression();
    return node;
}

ENode* Parser::parseAssignmentExpression()
{
    AssignmentNode* node = NULL;
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
            node = new AssignmentNode();
            node->_left = tmp;
            node->_type = tokenType();
            _tokens->next();
            node->_right = parseAssignmentExpression();
            return node;
        default:
            return tmp;
    }
}

ENode* Parser::parseExpression()
{
    ExpressionNode* node = new ExpressionNode();
    node->_left = parseAssignmentExpression();
    if(tokenType() != TOK_COMMA)
        return node->_left;
    node->_type = TOK_COMMA;
    _tokens->next();
    node->_right = parseExpression();
    return node;
}

}
