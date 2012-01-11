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
    ENode* cond;
    Node* thenExp;
    Node* elseExp;
    SelectionStatement(): cond(NULL), thenExp(NULL), elseExp(NULL) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
    virtual void gen(AbstractGenerator& g, bool withResult = true);
    virtual Node* tryOptimize();
};

class JumpStatement: public Node
{
public:
    TokenType type;
    JumpStatement(TokenType type_): type(type_) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
    virtual void gen(AbstractGenerator& g, bool withResult = true);
    virtual bool isJump() { return true; }
};

class ReturnStatement: public JumpStatement
{
public:
    ENode* expr;
    ReturnStatement(ENode* expr_): JumpStatement(TOK_RETURN), expr(expr_) {}
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
    ENode* expr;
    ExpressionStatement(ENode* expr_): expr(expr_) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
    virtual void gen(AbstractGenerator& g, bool withResult = true);
    virtual Node* tryOptimize();
};

class IterationStatement: public Node
{
public:
    TokenType type;
    Node* loop;
    ENode* cond;
    IterationStatement(): type(TOK_UNDEF), loop(NULL), cond(NULL) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
    virtual void gen(AbstractGenerator& g, bool withResult = true);
    virtual Node* tryOptimize();
};

class ForStatement: public Node
{
public:
    SymbolTable* iterators;
    TokenType type;
    Node* loop;
    Node* init;
    Node* cond;
    Node* mod;
    ForStatement():
        iterators(new SymbolTable()), type(TOK_FOR),
        loop(NULL), init(NULL), cond(NULL), mod(NULL) {}
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
