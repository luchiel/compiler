#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <string>

namespace LuCCompiler
{

class Node {};
class ExpressionNode: public Node {};

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
};

class CharNode: public ExpressionNode
{
public:
    string _value;
    CharNode(const string& value): _value(value) {}
};

class IntNode: public ExpressionNode
{
public:
    int _value;
    IntNode(const int value): _value(value) {}
};

class FloatNode: public ExpressionNode
{
public:
    float _value;
    FloatNode(const float value): _value(value) {}
};

class PostfixNode: public ExpressionNode
{
public:
    TokenType _type;

    ExpressionNode* _only;
    ExpressionNode* _tail;
    PostfixNode(): _type(TOK_UNDEF), _only(NULL), _tail(NULL) {}
};

class UnaryNode: public ExpressionNode
{
public:
    TokenType _type;

    ExpressionNode* _only;
    UnaryNode(): _type(TOK_UNDEF), _only(NULL) {}
};

class BinaryNode: public ExpressionNode
{
public:
	TokenType _type;

    ExpressionNode* _left;
    ExpressionNode* _right;

    BinaryNode(): _type(TOK_UNDEF), _left(NULL), _right(NULL) {}
};

}

#endif
