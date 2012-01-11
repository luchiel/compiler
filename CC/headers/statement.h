#ifndef STATEMENT_H
#define STATEMENT_H

#include <vector>
#include "token.h"
#include "node.h"
#include "expression.h"
#include "symbol_table.h"

using namespace std;

namespace LuCCompiler
{

class SelectionStatement: public Node
{
public:
    ENode* _expr;
    Node* thenExp;
    Node* elseExp;
    SelectionStatement(): _expr(NULL), thenExp(NULL), elseExp(NULL) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
    virtual void gen(AbstractGenerator& g, bool withResult = true);
    virtual Node* tryOptimize();
};

class JumpStatement: public Node
{
public:
    TokenType _type;
    JumpStatement(TokenType type): _type(type) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
    virtual void gen(AbstractGenerator& g, bool withResult = true);
};

class ReturnStatement: public Node
{
public:
    ENode* expr;
    ReturnStatement(ENode* expr_): expr(expr_) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
    virtual void gen(AbstractGenerator& g, bool withResult = true);
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
    virtual void gen(AbstractGenerator& g, bool withResult = true);
    virtual Node* tryOptimize();
};

class IterationStatement: public Node
{
public:
    TokenType _type;
    Node* _loop;
    ENode* _expr;
    IterationStatement(): _type(TOK_UNDEF), _loop(NULL), _expr(NULL) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
    virtual void gen(AbstractGenerator& g, bool withResult = true);
    virtual Node* tryOptimize();
};

class ForStatement: public Node
{
public:
    SymbolTable* _iterators;
    TokenType _type;
    Node* _loop;
    Node* _expr;
    Node* _expr2;
    Node* _expr3;
    ForStatement():
        _iterators(new SymbolTable()), _type(TOK_FOR),
        _loop(NULL), _expr(NULL), _expr2(NULL), _expr3(NULL) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
    virtual void gen(AbstractGenerator& g, bool withResult = true);
    virtual Node* tryOptimize();
};

class CompoundStatement: public Node
{
public:
    SymbolTable* _locals;
    vector<Node*>* _items;
    CompoundStatement(): _locals(new SymbolTable()), _items(new vector<Node*>) {}
    ~CompoundStatement() { delete _items; }
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
    virtual void gen(AbstractGenerator& g, bool withResult = true);
    virtual Node* tryOptimize();
};

}

#endif
