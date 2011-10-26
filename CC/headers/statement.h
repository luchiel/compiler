#ifndef STATEMENT_H
#define STATEMENT_H

#include <vector>
#include "token.h"
#include "node.h"

using namespace std;

namespace LuCCompiler
{

class Statement: public Node {};

class SelectionStatement: public Statement
{
public:
    Node* _expr;
    Node* _then;
    Node* _else;

    SelectionStatement(): _expr(NULL), _then(NULL), _else(NULL) {}
};

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

class BlockItem: public Statement
{
public:
    BlockItem() {}
};

class CompoundStatement: public Statement
{
public:
    vector<Node*>* _items;
    CompoundStatement() { _items = new vector<Node*>; }
    ~CompoundStatement() { delete _items; }
};

}

#endif
