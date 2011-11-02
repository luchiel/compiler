#ifndef STATEMENT_H
#define STATEMENT_H

#include <vector>
#include "token.h"
#include "node.h"

using namespace std;

namespace LuCCompiler
{

class SelectionStatement: public Node
{
public:
    Node* _expr;
    Node* _then;
    Node* _else;
    SelectionStatement(): _expr(NULL), _then(NULL), _else(NULL) {}
    virtual void out(unsigned int depth, vector<bool>* branches);
};

class JumpStatement: public Node
{
public:
    TokenType _type;
    JumpStatement(): _type(TOK_UNDEF) {}
    virtual void out(unsigned int depth, vector<bool>* branches);
};

class ReturnStatement: public JumpStatement
{
public:
    Node* _expr;
    ReturnStatement(): _expr(NULL) { _type = TOK_RETURN; }
    virtual void out(unsigned int depth, vector<bool>* branches);
};

class ExpressionStatement: public Node
{
public:
    Node* _expr;
    ExpressionStatement(): _expr(NULL) {}
    virtual void out(unsigned int depth, vector<bool>* branches);
};

class IterationStatement: public Node
{
public:
    TokenType _type;
    Node* _loop;
    Node* _expr;
    IterationStatement(): _type(TOK_UNDEF), _loop(NULL), _expr(NULL) {}
    virtual void out(unsigned int depth, vector<bool>* branches);
};

class ForStatement: public IterationStatement
{
public:
    Node* _expr2;
    Node* _expr3;
    ForStatement(): IterationStatement(), _expr2(NULL), _expr3(NULL) { _type = TOK_FOR; }
    virtual void out(unsigned int depth, vector<bool>* branches);
};

class CompoundStatement: public Node
{
public:
    vector<Node*>* _items;
    CompoundStatement() { _items = new vector<Node*>; }
    ~CompoundStatement() { delete _items; }
    virtual void out(unsigned int depth, vector<bool>* branches);
};

}

#endif
