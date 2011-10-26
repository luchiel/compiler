#include <iostream>
#include <vector>
#include "parser.h"
#include "expression.h"
#include "statement.h"
#include "tokenizer.h"
#include "token.h"
#include "operations.h"

namespace LuCCompiler
{

void Parser::out()
{
    vector<bool> finishedBranches;
    _root->out(0, &finishedBranches);
}

Parser::Parser(Tokenizer* tokens)
{
    _tokens = tokens;
}

void Parser::parse()
{
    _root = parseExpression();
}

void Parser::consumeTokenOfType(TokenType type, const string& except)
{
    if(tokenType() != type)
        throw makeException(except);
    _tokens->next();
}

Node* Parser::parseDeclaration()
{
    //not true
    return parseExpression();
}

Node* Parser::parseBinaryExpression(int priority)
{
    TokenType opType = TOK_UNDEF;
    BinaryNode* node = NULL;
    Node* tmp = priority < 13 ?
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
                consumeTokenOfType(TOK_R_SQUARE, "']' expected");
                break;
            case TOK_L_BRACKET:
                _tokens->next();
                if(tokenType() != TOK_R_BRACKET)
                {
                    node->_tail = parseExpression(); //List();
                    if(tokenType() != TOK_R_BRACKET)
                        throw makeException("')' expected");
                }
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
            consumeTokenOfType(TOK_L_BRACKET, "'(' expected");
            node->_only = parseUnaryExpression();
            consumeTokenOfType(TOK_R_BRACKET, "')' expected");
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
    consumeTokenOfType(TOK_COLON, "':' expected");
    node->_else = parseConditionalExpression();
    return node;
}

Node* Parser::parseAssignmentExpression()
{
    AssignmentNode* node = NULL;
    Node* tmp = parseConditionalExpression();
    //cout << (tokenType() == EOF ? "qu" : operationName(tokenType())) << " ";
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

Node* Parser::parseExpression()
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

ParserException Parser::makeException(const string& e)
{
    return ParserException(_tokens->get().line, _tokens->get().col + 1, e);
}

Node* Parser::parseExpressionStatement()
{
    ExpressionStatement* node = new ExpressionStatement();
    if(tokenType() == TOK_SEP)
    {
        _tokens->next();
        return node;
    }
    node->_expr = parseExpression();
    consumeTokenOfType(TOK_SEP, "';' expected");
    return node;
}

Node* Parser::parseJumpStatement()
{
    if(tokenType() == TOK_RETURN)
    {
        ReturnStatement* node = new ReturnStatement();
        _tokens->next();
        if(tokenType() == TOK_SEP)
        {
            _tokens->next();
            return node;
        }
        node->_expr = parseExpression();
        consumeTokenOfType(TOK_SEP, "';' expected");
        return node;
    }
    else if(tokenType() == TOK_CONTINUE || tokenType() == TOK_BREAK)
    {
        JumpStatement* node = new JumpStatement();
        node->_type = tokenType();
        _tokens->next();
        return node;
    }
    return NULL;
}

Node* Parser::parseIterationStatement()
{
    IterationStatement* node = NULL;
    ForStatement* forNode = NULL;
    if(tokenType() == TOK_WHILE)
    {
        node = new IterationStatement();
        node->_type = tokenType();
        _tokens->next();
        consumeTokenOfType(TOK_L_BRACKET, "'(' expected");
        node->_expr = parseExpression();
        consumeTokenOfType(TOK_R_BRACKET, "')' expected");
        node->_loop = parseStatement();
        return node;
    }
    else if(tokenType() == TOK_DO)
    {
        node = new IterationStatement();
        node->_type = tokenType();
        _tokens->next();
        node->_loop = parseStatement();
        consumeTokenOfType(TOK_WHILE, "'while' expected");
        consumeTokenOfType(TOK_L_BRACKET, "'(' expected");
        node->_expr = parseExpression();
        consumeTokenOfType(TOK_R_BRACKET, "')' expected");
        consumeTokenOfType(TOK_SEP, "';' expected");
        return node;
    }
    else if(tokenType() == TOK_FOR)
    {
        forNode = new ForStatement();
        _tokens->next();
        consumeTokenOfType(TOK_L_BRACKET, "'(' expected");
        //declaration expression_statement
        //declaration expression_statement expression
        forNode->_expr = parseExpressionStatement();
        forNode->_expr2 = parseExpressionStatement();
        if(tokenType() != TOK_R_BRACKET)
            forNode->_expr3 = parseExpression();
        consumeTokenOfType(TOK_R_BRACKET, "')' expected");
        forNode->_loop = parseStatement();
        return forNode;
    }
    return NULL;
}

Node* Parser::parseStatement()
{
    return NULL;
}

}
