#include <vector>
#include "parser.h"
#include "token.h"
#include "statement.h"
#include "symbol.h"

using namespace std;

namespace LuCCompiler
{

Node* Parser::parseExpressionStatement()
{
    if(tokenType() == TOK_SEP)
    {
        _tokens->next();
        return new EmptyExpressionStatement();
    }
    ExpressionStatement* node = new ExpressionStatement(parseExpression());
    consumeTokenOfType(TOK_SEP, "';' expected");
    return node;
}

Node* Parser::parseJumpStatement()
{
    if(tokenType() == TOK_RETURN)
    {
        _tokens->next();
        if(tokenType() == TOK_SEP)
        {
            if(_mode == PM_SYMBOLS && *_expectedReturnType != *getSymbol("void"))
                throw makeException("Function is expected to return value");
            _tokens->next();
            return new JumpStatement(TOK_RETURN);
        }
        ReturnStatement* node = new ReturnStatement(parseExpression());
        if(_mode == PM_SYMBOLS && *node->expr->expType != *_expectedReturnType)
            throw makeException("Function type and value type in return statement do not match");
        consumeTokenOfType(TOK_SEP, "';' expected");
        return node;
    }
    else if(tokenType() == TOK_CONTINUE || tokenType() == TOK_BREAK)
    {
        if(_mode == PM_SYMBOLS && _jumpAllowed == 0)
            throw makeException("Jump statement not within loop");
        JumpStatement* node = new JumpStatement(tokenType());
        _tokens->next();
        consumeTokenOfType(TOK_SEP, "';' expected");
        return node;
    }
    return NULL;
}

Node* Parser::parseBlockItem()
{
    if(!parseDeclaration(false))
        return parseStatement();
    return NULL;
}

CompoundStatement* Parser::parseCompoundStatement()
{
    if(tokenType() == TOK_L_BRACE)
    {
        CompoundStatement* node = new CompoundStatement();
        _symbols->push(node->_locals);
        _tokens->next();
        bool statementsStarted = false;
        while(tokenType() != TOK_R_BRACE)
        {
            Node* bi = parseBlockItem();
            if(bi != NULL)
            {
                statementsStarted = true;
                node->_items->push_back(bi);
            }
            //else if(statementsStarted)
            //    throw makeException("Declaration after statements");
        }
        _symbols->pop();
        consumeTokenOfType(TOK_R_BRACE, "'}' expected");
        return node;
    }
    return NULL;
}

Node* Parser::parseSelectionStatement()
{
    if(tokenType() == TOK_IF)
    {
        SelectionStatement* node = new SelectionStatement();
        _tokens->next();
        consumeTokenOfType(TOK_L_BRACKET, "'(' expected");
        node->_expr = parseExpression();
        consumeTokenOfType(TOK_R_BRACKET, "')' expected");
        node->_then = parseStatement();
        if(tokenType() == TOK_ELSE)
        {
            _tokens->next();
            node->_else = parseStatement();
        }
        return node;
    }
    return NULL;
}

Node* Parser::parseIterationStatement()
{
    IterationStatement* node = NULL;
    if(tokenType() == TOK_WHILE)
    {
        node = new IterationStatement();
        node->_type = tokenType();
        _tokens->next();
        consumeTokenOfType(TOK_L_BRACKET, "'(' expected");
        node->_expr = parseExpression();
        consumeTokenOfType(TOK_R_BRACKET, "')' expected");

        _jumpAllowed++;
        node->_loop = parseStatement();
        _jumpAllowed--;

        return node;
    }
    else if(tokenType() == TOK_DO)
    {
        node = new IterationStatement();
        node->_type = tokenType();
        _tokens->next();

        _jumpAllowed++;
        node->_loop = parseStatement();
        _jumpAllowed--;

        consumeTokenOfType(TOK_WHILE, "'while' expected");
        consumeTokenOfType(TOK_L_BRACKET, "'(' expected");
        node->_expr = parseExpression();
        consumeTokenOfType(TOK_R_BRACKET, "')' expected");
        consumeTokenOfType(TOK_SEP, "';' expected");
        return node;
    }
    else if(tokenType() == TOK_FOR)
    {
        ForStatement* forNode = new ForStatement();
        _tokens->next();
        consumeTokenOfType(TOK_L_BRACKET, "'(' expected");
        _symbols->push(forNode->_iterators);
        if(!parseDeclaration(false))
            forNode->_expr = parseExpressionStatement();
        forNode->_expr2 = parseExpressionStatement();
        if(tokenType() != TOK_R_BRACKET)
            forNode->_expr3 = parseExpression();
        consumeTokenOfType(TOK_R_BRACKET, "')' expected");

        _jumpAllowed++;
        forNode->_loop = parseStatement();
        _jumpAllowed--;

        _symbols->pop();
        return forNode;
    }
    return NULL;
}

Node* Parser::parseStatement()
{
    Node* r = parseCompoundStatement();
    if(r != NULL)
        return r;
    r = parseSelectionStatement();
    if(r != NULL)
        return r;
    r = parseIterationStatement();
    if(r != NULL)
        return r;
    r = parseJumpStatement();
    if(r != NULL)
        return r;
    return parseExpressionStatement();
}

}
