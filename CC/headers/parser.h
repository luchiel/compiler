#ifndef PARSER_H
#define PARSER_H

#include "expression.h"
#include "tokenizer.h"

namespace LuCCompiler
{

class Parser
{
private:
    BinaryNode* _root;
    Tokenizer* _tokens;

public:
    ExpressionNode* parseExpression();

    Parser(Tokenizer* tokens);

    ExpressionNode* parsePrimaryExpression();
    ExpressionNode* parsePostfixExpression();
    ExpressionNode* parseUnaryExpression();
    ExpressionNode* parseBinaryExpression(int priority);
    ExpressionNode* parseCastExpression();
    ExpressionNode* parseConditionalExpression();

    TokenType tokenType() { return _tokens->get().type; }
};

class RightSquareBracketExpected: public exception {};
class RightBracketExpected: public exception {};
class LeftBracketExpected: public exception {};
class RightBraceExpected: public exception {};
class UnepectedToken: public exception {};
class IdentifierExpected: public exception {};

}

#endif
