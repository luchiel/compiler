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
        _symbols->push(node->locals);
        _tokens->next();
        while(tokenType() != TOK_R_BRACE)
        {
            initialized.clear();
            Node* bi = parseBlockItem();
            if(bi != NULL)
                node->items->push_back(bi);
            else
                for(unsigned int i = 0; i < initialized.size(); ++i)
                    node->items->push_back(new InitStatement(initialized[i]));
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
        node->cond = parseExpression();
        if(_mode == PM_SYMBOLS && node->cond->expType != _int)
            throw makeException("expression must be of type int");
        consumeTokenOfType(TOK_R_BRACKET, "')' expected");
        node->thenExp = parseStatement();
        if(tokenType() == TOK_ELSE)
        {
            _tokens->next();
            node->elseExp = parseStatement();
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
        node->type = tokenType();
        _tokens->next();
        consumeTokenOfType(TOK_L_BRACKET, "'(' expected");
        node->cond = parseExpression();
        if(_mode == PM_SYMBOLS && node->cond->expType != _int)
            throw makeException("expression must be of type int");
        consumeTokenOfType(TOK_R_BRACKET, "')' expected");

        _jumpAllowed++;
        node->loop = parseStatement();
        _jumpAllowed--;

        return node;
    }
    else if(tokenType() == TOK_DO)
    {
        node = new IterationStatement();
        node->type = tokenType();
        _tokens->next();

        _jumpAllowed++;
        node->loop = parseStatement();
        _jumpAllowed--;

        consumeTokenOfType(TOK_WHILE, "'while' expected");
        consumeTokenOfType(TOK_L_BRACKET, "'(' expected");
        node->cond = parseExpression();
        if(_mode == PM_SYMBOLS && node->cond->expType != _int)
            throw makeException("expression must be of type int");
        consumeTokenOfType(TOK_R_BRACKET, "')' expected");
        consumeTokenOfType(TOK_SEP, "';' expected");
        return node;
    }
    else if(tokenType() == TOK_FOR)
    {
        ForStatement* forNode = new ForStatement();
        _tokens->next();
        consumeTokenOfType(TOK_L_BRACKET, "'(' expected");
        _symbols->push(forNode->iterators);
        if(!parseDeclaration(false))
            forNode->init = parseExpressionStatement();
        forNode->cond = parseExpressionStatement();
        if(tokenType() != TOK_R_BRACKET)
            forNode->mod = parseExpression();
        consumeTokenOfType(TOK_R_BRACKET, "')' expected");

        _jumpAllowed++;
        forNode->loop = parseStatement();
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
