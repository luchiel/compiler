#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <string>

using namespace std;

namespace LuCCompiler
{

class Node {};
class ExpressionNode: public Node
{
    virtual int width() { return 1; }

    virtual void out(int depth) {}
    void printRibs(int depth);
};

class IdentNode: public ExpressionNode
{
public:
    string _name;
    IdentNode(const string& name): _name(name) {}


};

class StringNode: public ExpressionNode
{
public:
    string _value;
    StringNode(const string& value): _value(value) {}

    virtual void out(int depth);
};

class CharNode: public ExpressionNode
{
public:
    string _value;
    CharNode(const string& value): _value(value) {}

    virtual void out(int depth);
};

class IntNode: public ExpressionNode
{
public:
    int _value;
    IntNode(const int value): _value(value) {}

    virtual void out(int depth);
};

class FloatNode: public ExpressionNode
{
public:
    float _value;
    FloatNode(const float value): _value(value) {}

    virtual void out(int depth);
};

class PostfixNode: public ExpressionNode
{
public:
    TokenType _type;

    virtual int width() { return _only->width() + _tail->width() + 1; }

    ExpressionNode* _only;
    ExpressionNode* _tail;
    PostfixNode(): _type(TOK_UNDEF), _only(NULL), _tail(NULL) {}

    virtual void out(int depth);
};

class UnaryNode: public ExpressionNode
{
public:
    TokenType _type;

    virtual int width() { return _only->width(); }

    ExpressionNode* _only;
    UnaryNode(): _type(TOK_UNDEF), _only(NULL) {}

    virtual void out(int depth);
};

class BinaryNode: public ExpressionNode
{
public:
    TokenType _type;

    virtual int width() { return _left->width() + _right->width() + 1; }

    ExpressionNode* _left;
    ExpressionNode* _right;

    BinaryNode(): _type(TOK_UNDEF), _left(NULL), _right(NULL) {}

    virtual void out(int depth);
};

}

#endif
