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
    void parseExpression();

    Parser(Tokenizer* tokens);

	ExpressionNode* parsePrimaryExpression();
	ExpressionNode* parsePostfixExpression();
	ExpressionNode* parseUnaryExpression();
	ExpressionNode* parseCastExpression();
	ExpressionNode* parseConditionalExpression();

    TokenType tokenType() { return _tokens->get().type; }
};

}

#endif
