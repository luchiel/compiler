#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <string>
#include "token.h"

using namespace std;

namespace LuCCompiler
{

class Node
{
protected:
    void printRibs(int depth);
    friend void printNodeWithRibs(Node* node, int depth);

public:
    virtual void out(int depth) {}
};

class IdentNode: public Node
{
public:
    string _name;
    IdentNode(const string& name): _name(name) {}

    virtual void out(int depth);
};

class StringNode: public Node
{
public:
    string _value;
    StringNode(const string& value): _value(value) {}

    virtual void out(int depth);
};

class CharNode: public Node
{
public:
    string _value;
    CharNode(const string& value): _value(value) {}

    virtual void out(int depth);
};

class IntNode: public Node
{
public:
    int _value;
    IntNode(const int value): _value(value) {}

    virtual void out(int depth);
};

class FloatNode: public Node
{
public:
    float _value;
    FloatNode(const float value): _value(value) {}

    virtual void out(int depth);
};

class PostfixNode: public Node
{
public:
    TokenType _type;

    Node* _only;
    Node* _tail;
    PostfixNode(): _type(TOK_UNDEF), _only(NULL), _tail(NULL) {}

    virtual void out(int depth);
};

class UnaryNode: public Node
{
public:
    TokenType _type;

    Node* _only;
    UnaryNode(): _type(TOK_UNDEF), _only(NULL) {}

    virtual void out(int depth);
};

class BinaryNode: public Node
{
public:
    TokenType _type;

    Node* _left;
    Node* _right;

    BinaryNode(): _type(TOK_UNDEF), _left(NULL), _right(NULL) {}

    virtual void out(int depth);
};

class TernaryNode: public Node
{
public:
    TokenType _type;

    Node* _if;
    Node* _then;
    Node* _else;

    TernaryNode(): _type(TOK_UNDEF), _if(NULL), _then(NULL), _else(NULL) {}

    virtual void out(int depth);
};

class ExpressionNode: public BinaryNode
{
public:
    ExpressionNode(): BinaryNode() {}
};

}

#endif
