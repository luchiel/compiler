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
    ENode(): expType(NULL) {}
};

class IdentNode: public ENode
{
public:
    string _name;
    SymbolVariable* _var;
    IdentNode(const string& name_, SymbolVariable* var_ = NULL): ENode(), _name(name_), _var(var_) {}
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
    TokenType _type;
    ENode* _left;
    ENode* _right;
    BinaryNode(): ENode(), _type(TOK_UNDEF), _left(NULL), _right(NULL) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

class TernaryNode: public ENode
{
public:
    TokenType _type;
    ENode* _if;
    ENode* _then;
    ENode* _else;
    TernaryNode(): ENode(), _type(TOK_UNDEF), _if(NULL), _then(NULL), _else(NULL) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

class AssignmentNode: public BinaryNode
{
public:
    AssignmentNode(): BinaryNode() {}
};

class ExpressionNode: public BinaryNode
{
public:
    ExpressionNode(): BinaryNode() {}
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
