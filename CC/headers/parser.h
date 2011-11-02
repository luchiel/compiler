#ifndef PARSER_H
#define PARSER_H

#include "iostream"
#include "expression.h"
#include "tokenizer.h"
#include "exception.h"

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
    void parseExpr();

    Parser(Tokenizer* tokens);

    Node* parsePrimaryExpression();
    Node* parsePostfixExpression();
    Node* parseUnaryExpression();
    Node* parseBinaryExpression(int priority);
    Node* parseCastExpression();
    Node* parseConditionalExpression();
    Node* parseAssignmentExpression();
    Node* parseExpression();

    Node* parseDeclaration();

    Node* parseBlockItem();
    Node* parseCompoundStatement();
    Node* parseSelectionStatement();
    Node* parseJumpStatement();
    Node* parseExpressionStatement();
    Node* parseIterationStatement();
    Node* parseStatement();

    Node* parseIdentifierList();

    ParserException makeException(const string& e);
    void consumeTokenOfType(TokenType type, const string& except);

    void out();
};

}

#endif
