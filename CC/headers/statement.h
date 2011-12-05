#ifndef STATEMENT_H
#define STATEMENT_H

#include <vector>
#include "token.h"
#include "node.h"
#include "symbol_table.h"

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
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

class JumpStatement: public Node
{
public:
    TokenType _type;
    JumpStatement(TokenType type): _type(type) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

class ReturnStatement: public Node
{
public:
    Node* _expr;
    ReturnStatement(Node* expr): _expr(expr) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

class EmptyExpressionStatement: public Node
{
public:
    EmptyExpressionStatement() {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

class ExpressionStatement: public Node
{
public:
    Node* _expr;
    ExpressionStatement(Node* expr): _expr(expr) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

class IterationStatement: public Node
{
public:
    TokenType _type;
    Node* _loop;
    Node* _expr;
    IterationStatement(): _type(TOK_UNDEF), _loop(NULL), _expr(NULL) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

class ForStatement: public IterationStatement
{
public:
    SymbolTable* _iterators;
    Node* _expr2;
    Node* _expr3;
    ForStatement():
        IterationStatement(), _iterators(new SymbolTable()),
        _expr2(NULL), _expr3(NULL) { _type = TOK_FOR; }
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

class CompoundStatement: public Node
{
public:
    SymbolTable* _locals;
    vector<Node*>* _items;
    CompoundStatement(): _locals(new SymbolTable()), _items(new vector<Node*>) {}
    ~CompoundStatement() { delete _items; }
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

}

#endif
