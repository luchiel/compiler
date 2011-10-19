#include <iostream>
#include "parser.h"
#include "expression.h"
#include "tokenizer.h"
#include "token.h"
#include "operations.h"

namespace LuCCompiler
{

void Parser::out()
{
    _root->out(0);
}

Parser::Parser(Tokenizer* tokens)
{
    _tokens = tokens;
}

void Parser::parse()
{
    _root = parseExpression();
}

Node* Parser::parseBinaryExpression(int priority)
{
    Node* left = priority < 13 ?
        parseBinaryExpression(priority + 1) :
        parseCastExpression();

    OperationGroups* og = new OperationGroups();

    map<TokenType, int>::iterator tok = og->_binaryOps->find(_tokens->get().type);
    if(tok == og->_binaryOps->end() || tok->second != priority)
        return left;
    BinaryNode* bNode = new BinaryNode();
    bNode->_type = _tokens->get().type;
    bNode->_left = left;
    _tokens->next();
    bNode->_right = parseBinaryExpression(priority);
    return bNode;
}

Node* Parser::parsePrimaryExpression()
{
    Node* node = NULL;
    switch(tokenType())
    {
        case TOK_OCT_CONST:
        case TOK_DEC_CONST:
        case TOK_HEX_CONST:
            node = new IntNode(_tokens->get().value.intValue); break;
        case TOK_CHAR_CONST:
            node = new CharNode(*_tokens->get().value.strValue); break;
        case TOK_STR_CONST:
            node = new StringNode(*_tokens->get().value.strValue); break;
        case TOK_FLOAT_CONST:
            node = new FloatNode(_tokens->get().value.floatValue); break;
        case TOK_IDENT:
            node = new IdentNode(_tokens->get().text); break;
        case TOK_L_BRACKET:
            _tokens->next(); //( eaten
            node = parseExpression();
            if(tokenType() != TOK_R_BRACKET)
                throw makeException("')' expected");
            break;
        default:
            throw makeException("Unexpected token " + TOKEN_TYPE_NAME[tokenType()]);
    }
    _tokens->next(); //token eaten
    return node;
}

Node* Parser::parsePostfixExpression()
{
    //'(' type_name ')' '{' initializer_list [','] '}' ;
    PostfixNode* node = new PostfixNode();
    Node* core = parsePrimaryExpression();
    node->_only = core;
    while(true)
    {
        node->_type = tokenType();
        switch(tokenType())
        {
            case TOK_L_SQUARE:
                _tokens->next();
                node->_tail = parseExpression();
                if(tokenType() != TOK_R_SQUARE)
                    throw makeException("']' expected");
                _tokens->next();
                break;
            case TOK_L_BRACKET:
                _tokens->next();
                node->_tail = parseExpression(); //List();
                if(tokenType() != TOK_R_BRACKET)
                    throw makeException("')' expected");
                _tokens->next();
                break;
            case TOK_ARROW:
            case TOK_DOT:
                _tokens->next();
                if(tokenType() != TOK_IDENT)
                    throw makeException("Identifier expected");
                node->_tail = parsePrimaryExpression();
                break;
            case TOK_INC:
            case TOK_DEC:
                _tokens->next();
                break;
            default:
                return node->_only;
        }
        core = node;
        node = new PostfixNode();
        node->_only = core;
    }
}

Node* Parser::parseUnaryExpression()
{
    UnaryNode* node = NULL;
    switch(tokenType())
    {
        case TOK_INC:
        case TOK_DEC:
            node = new UnaryNode();
            node->_type = tokenType();
            _tokens->next();
            node->_only = parseUnaryExpression();
            break;
        case TOK_SIZEOF:
            node = new UnaryNode();
            node->_type = tokenType();
            _tokens->next();
            if(tokenType() != TOK_L_BRACKET)
                throw makeException("'(' expected");
            node->_only = parseUnaryExpression();
            if(tokenType() != TOK_R_BRACKET)
                throw makeException("')' expected");
            _tokens->next();
            break;
        case TOK_PLUS:
        case TOK_MINUS:
        case TOK_TILDA:
        case TOK_AMP:
        case TOK_ASTERISK:
        case TOK_NOT:
            node = new UnaryNode();
            node->_type = tokenType();
            _tokens->next();
            node->_only = parseCastExpression();
            break;
        default:
            return parsePostfixExpression();
    }
    return node;
}

Node* Parser::parseCastExpression()
{
    //cast_expression = unary_expression | '(' type_name ')' cast_expression ;
    return parseUnaryExpression();
}

Node* Parser::parseConditionalExpression()
{
    TernaryNode* node = NULL;
    Node* tmp = parseBinaryExpression(4);
    if(tokenType() != TOK_QUEST)
        return tmp;
    node = new TernaryNode();
    node->_type = TOK_QUEST;
    node->_if = tmp;
    _tokens->next();
    node->_then = parseExpression();
    if(tokenType() != TOK_COLON)
        throw makeException("':' expected");
    _tokens->next();
    node->_else = parseConditionalExpression();
    return node;
}

Node* Parser::parseAssignmentExpression()
{
    /*
    assignment_expression =
        conditional_expression |
        unary_expression assignment_operator assignment_expression ;
    */
    return parseConditionalExpression();
}

Node* Parser::parseExpression()
{
    ExpressionNode* node = new ExpressionNode();
    node->_left = parseConditionalExpression();
    if(tokenType() != TOK_COMMA)
        return node->_left;
    node->_type = TOK_COMMA;
    _tokens->next();
    node->_right = parseExpression();
    return node;
}

ParserException Parser::makeException(const string& e)
{
    return ParserException(_tokens->get().line, _tokens->get().col + 1, e);
}


}
