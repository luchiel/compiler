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

class IdentNode: public Node
{
public:
    string _name;
    SymbolVariable* _var;
    IdentNode(const string& name_, SymbolVariable* var_ = NULL): _name(name_), _var(var_) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

class StringNode: public Node
{
public:
    string _value;
    StringNode(const string& value): _value(value) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

class CharNode: public Node
{
public:
    string _value;
    CharNode(const string& value): _value(value) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

class IntNode: public Node
{
public:
    int _value;
    IntNode(const int value): _value(value) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

class FloatNode: public Node
{
public:
    float _value;
    FloatNode(const float value): _value(value) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

class PostfixNode: public Node
{
public:
    TokenType _type;
    Node* _only;
    Node* _tail;
    PostfixNode(): _type(TOK_UNDEF), _only(NULL), _tail(NULL) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

class UnaryNode: public Node
{
public:
    TokenType _type;
    Node* _only;
    UnaryNode(): _type(TOK_UNDEF), _only(NULL) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

class SizeofNode: public UnaryNode
{
public:
    SymbolType* _symbolType;
    SizeofNode(): UnaryNode(), _symbolType(NULL) { _type = TOK_SIZEOF; }
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

class BinaryNode: public Node
{
public:
    TokenType _type;
    Node* _left;
    Node* _right;
    BinaryNode(): _type(TOK_UNDEF), _left(NULL), _right(NULL) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

class TernaryNode: public Node
{
public:
    TokenType _type;
    Node* _if;
    Node* _then;
    Node* _else;
    TernaryNode(): _type(TOK_UNDEF), _if(NULL), _then(NULL), _else(NULL) {}
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

class CastNode: public Node
{
public:
    SymbolType* type;
    Node* element;
    CastNode(SymbolType* type_): Node(), type(type_) {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

}

#endif
