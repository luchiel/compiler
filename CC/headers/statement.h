#ifndef STATEMENT_H
#define STATEMENT_H

#include "token.h"
#include "node.h"

using namespace std;

namespace LuCCompiler
{

class Statement: public Node {};

/*
selection_statement
iteration_statement
jump_statement
*/

class JumpStatement: public Statement
{
public:
    TokenType _type;

    JumpStatement(): _type(TOK_UNDEF) {}
};

class ReturnStatement: public Statement
{
public:
    Node* _expr;

    ReturnStatement(): _expr(NULL) {}
};

class ExpressionStatement: public Statement
{
public:
    Node* _expr;

    ExpressionStatement(): _expr(NULL) {}
};

class IterationStatement: public Statement
{
public:
    TokenType _type;
    Node* _loop;
    Node* _expr;

    IterationStatement(): _type(TOK_UNDEF), _loop(NULL), _expr(NULL) {}
};

class ForStatement: public IterationStatement
{
public:
    Node* _expr2;
    Node* _expr3;

    ForStatement(): IterationStatement(), _expr2(NULL), _expr3(NULL) { _type = TOK_FOR; }
};

class CompoundStatement: public Node {};

}

#endif
