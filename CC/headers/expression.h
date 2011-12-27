#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <string>
#include <vector>
#include "token.h"
#include "node.h"
#include "symbol.h"
#include "abstract_generator.h"

using namespace std;

namespace LuCCompiler
{

class ENode: public Node
{
protected:
    void printExpType(int level);
    virtual void makeNodeTop(
        unsigned int depth, vector<bool>* branches, const string& s, int level
    );
public:
    SymbolType* expType;
    bool isLValue;
    ENode(): expType(NULL), isLValue(false) {}
    virtual void genLValue(AbstractGenerator& g) {}
};

class IdentNode: public ENode
{
public:
    string name;
    Symbol* var;
    IdentNode(const string& name_, Symbol* var_ = NULL):
        ENode(), name(name_), var(var_) { isLValue = true; }
    virtual void out(unsigned int depth, vector<bool>* branches, int level = 0);

    virtual void gen(AbstractGenerator& g, bool withResult = true);
    virtual void genLValue(AbstractGenerator& g);
};

class StringNode: public ENode
{
public:
    string value;
    StringNode(const string& value_): ENode(), value(value_) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int level = 0);

    virtual void gen(AbstractGenerator& g, bool withResult = true);
};

class CharNode: public ENode
{
public:
    int value;
    CharNode(const string& value_): ENode(), value(value_[0]) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int level = 0);

    virtual void gen(AbstractGenerator& g, bool withResult = true);
};

class IntNode: public ENode
{
public:
    int value;
    IntNode(const int value_): ENode(), value(value_) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int level = 0);

    virtual void gen(AbstractGenerator& g, bool withResult = true);
};

class FloatNode: public ENode
{
public:
    float value;
    FloatNode(const float value_): ENode(), value(value_) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int level = 0);

    virtual void gen(AbstractGenerator& g, bool withResult = true);
};

class PostfixNode: public ENode
{
public:
    TokenType type;
    ENode* only;
    ENode* tail;
    PostfixNode(): ENode(), type(TOK_UNDEF), only(NULL), tail(NULL) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int level = 0);

    virtual void gen(AbstractGenerator& g, bool withResult = true);
    virtual void genLValue(AbstractGenerator& g);
    void performCommonGenPart(AbstractGenerator& g);
};

class CallNode: public PostfixNode
{
public:
    vector<ENode*> params;
    CallNode(): PostfixNode() { type = TOK_L_BRACKET; }
    virtual void out(unsigned int depth, vector<bool>* branches, int level = 0);

    virtual void gen(AbstractGenerator& g, bool withResult = true);
};

class UnaryNode: public ENode
{
public:
    TokenType type;
    ENode* only;
    UnaryNode(): ENode(), type(TOK_UNDEF), only(NULL) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int level = 0);

    virtual void gen(AbstractGenerator& g, bool withResult = true);
    virtual void genLValue(AbstractGenerator& g);
};

class SizeofNode: public UnaryNode
{
public:
    SymbolType* symbolType;
    SizeofNode(): UnaryNode(), symbolType(NULL) { type = TOK_SIZEOF; }
    virtual void out(unsigned int depth, vector<bool>* branches, int level = 0);

    virtual void gen(AbstractGenerator& g, bool withResult = true);
};

class BinaryNode: public ENode
{
public:
    TokenType type;
    ENode* left;
    ENode* right;
    BinaryNode(TokenType type_, ENode* left_): ENode(), type(type_), left(left_), right(NULL) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int level = 0);

    virtual void gen(AbstractGenerator& g, bool withResult = true);
};

class TernaryNode: public ENode
{
public:
    TokenType type;
    ENode* condition;
    ENode* thenOp;
    ENode* elseOp;
    TernaryNode(): ENode(), type(TOK_UNDEF), condition(NULL), thenOp(NULL), elseOp(NULL) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int level = 0);

    virtual void gen(AbstractGenerator& g, bool withResult = true);
};

class AssignmentNode: public BinaryNode
{
public:
    AssignmentNode(TokenType type_, ENode* left_): BinaryNode(type_, left_) {}

    virtual void gen(AbstractGenerator& g, bool withResult = true);
};

class ExpressionNode: public BinaryNode
{
public:
    ExpressionNode(TokenType type_, ENode* left_): BinaryNode(type_, left_) {}

    virtual void gen(AbstractGenerator& g, bool withResult = true);
};

class CastNode: public ENode
{
public:
    SymbolType* type;
    ENode* element;
    CastNode(SymbolType* type_, ENode* element_): ENode(), type(type_), element(element_) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int level = 0);

    virtual void gen(AbstractGenerator& g, bool withResult = true);
};

}

#endif
