#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <string>
#include <vector>
#include "token.h"
#include "node.h"
#include "symbol.h"

using namespace std;

namespace LuCCompiler
{

class ENode: public Node //aka ExpressionNode
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
};

class IdentNode: public ENode
{
public:
    string name;
    SymbolVariable* var;
    IdentNode(const string& name_, SymbolVariable* var_ = NULL):
        ENode(), name(name_), var(var_) { isLValue = true; }
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

class StringNode: public ENode
{
public:
    string _value;
    StringNode(const string& value): ENode(), _value(value) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

class CharNode: public ENode
{
public:
    string _value;
    CharNode(const string& value): ENode(), _value(value) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

class IntNode: public ENode
{
public:
    int _value;
    IntNode(const int value): ENode(), _value(value) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

class FloatNode: public ENode
{
public:
    float _value;
    FloatNode(const float value): ENode(), _value(value) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

class PostfixNode: public ENode
{
public:
    TokenType type;
    ENode* only;
    vector<ENode*> tail;
    PostfixNode(): ENode(), type(TOK_UNDEF), only(NULL) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

class UnaryNode: public ENode
{
public:
    TokenType type;
    ENode* only;
    UnaryNode(): ENode(), type(TOK_UNDEF), only(NULL) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

class SizeofNode: public UnaryNode
{
public:
    SymbolType* symbolType;
    SizeofNode(): UnaryNode(), symbolType(NULL) { type = TOK_SIZEOF; }
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

class BinaryNode: public ENode
{
public:
    TokenType type;
    ENode* left;
    ENode* right;
    BinaryNode(TokenType type_, ENode* left_): ENode(), type(type_), left(left_), right(NULL) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

class TernaryNode: public ENode
{
public:
    TokenType type;
    ENode* condition;
    ENode* thenOp;
    ENode* elseOp;
    TernaryNode(): ENode(), type(TOK_UNDEF), condition(NULL), thenOp(NULL), elseOp(NULL) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

class AssignmentNode: public BinaryNode
{
public:
    AssignmentNode(TokenType type_, ENode* left_): BinaryNode(type_, left_) {}
};

class ExpressionNode: public BinaryNode
{
public:
    ExpressionNode(TokenType type_, ENode* left_): BinaryNode(type_, left_) {}
};

class CastNode: public ENode
{
public:
    SymbolType* type;
    ENode* element;
    CastNode(SymbolType* type_, ENode* element_): ENode(), type(type_), element(element_) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

}

#endif
