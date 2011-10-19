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
    Node* _root;
    Tokenizer* _tokens;

    TokenType tokenType() { return _tokens->get().type; }

public:
    void parse();

    Parser(Tokenizer* tokens);

    Node* parsePrimaryExpression();
    Node* parsePostfixExpression();
    Node* parseUnaryExpression();
    Node* parseBinaryExpression(int priority);
    Node* parseCastExpression();
    Node* parseConditionalExpression();
    Node* parseAssignmentExpression();
    Node* parseExpression();

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
