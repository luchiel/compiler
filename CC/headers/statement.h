#ifndef STATEMENT_H
#define STATEMENT_H

#include <vector>
#include "token.h"
#include "node.h"
#include "expression.h"
#include "symbol_table.h"
#include "abstract_generator.h"

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
    virtual Node* optimized();
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
    virtual Node* optimized();
};

class LoopStatement: public Node
{
protected:
    Argument* breakLabel;
    Argument* continueLabel;
    void genCheckCondition(AbstractGenerator& g, Node* cond);
    void genLoop(AbstractGenerator& g);

public:
    TokenType type;
    Node* loop;
    LoopStatement():
        breakLabel(NULL), continueLabel(NULL),
        type(TOK_UNDEF), loop(NULL) {}
};

class IterationStatement: public LoopStatement
{
public:
    ENode* cond;
    IterationStatement(): LoopStatement(), cond(NULL) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
    virtual void gen(AbstractGenerator& g, bool withResult = true);
    virtual Node* optimized();
};

class ForStatement: public LoopStatement
{
public:
    SymbolTable* iterators;
    Node* cond;
    Node* init;
    Node* mod;
    ForStatement():
        LoopStatement(), iterators(new SymbolTable()), cond(NULL),
        init(NULL), mod(NULL) { type = TOK_FOR; }
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
    virtual void gen(AbstractGenerator& g, bool withResult = true);
    virtual Node* optimized();
};

class CompoundStatement: public Node
{
public:
    SymbolTable* locals;
    vector<Node*>* items;
    CompoundStatement(): locals(new SymbolTable()), items(new vector<Node*>) {}
    ~CompoundStatement() { delete items; }
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
    virtual void gen(AbstractGenerator& g, bool withResult = true);
    virtual Node* optimized();
};

}

#endif
