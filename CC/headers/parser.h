#ifndef PARSER_H
#define PARSER_H

#include "iostream"
#include "expression.h"
#include "tokenizer.h"

namespace LuCCompiler
{

class Parser
{
private:
    ExpressionNode* _root;
    Tokenizer* _tokens;

    TokenType tokenType() { return _tokens->get().type; }

public:
    void parse();

    Parser(Tokenizer* tokens);

    ExpressionNode* parsePrimaryExpression();
    ExpressionNode* parsePostfixExpression();
    ExpressionNode* parseUnaryExpression();
    ExpressionNode* parseBinaryExpression(int priority);
    ExpressionNode* parseCastExpression();
    ExpressionNode* parseConditionalExpression();
    ExpressionNode* parseExpression();

    void out();
};

class RightSquareBracketExpected: public exception {};
class RightBracketExpected: public exception {};
class LeftBracketExpected: public exception {};
class RightBraceExpected: public exception {};
class UnexpectedToken: public exception {};
class IdentifierExpected: public exception {};
class ColonExpected: public exception {};

}

#endif
