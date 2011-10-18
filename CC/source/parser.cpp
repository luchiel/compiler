#include "parser.h"
#include "expression.h"
#include "tokenizer.h"
#include "operations.h"

namespace LuCCompiler
{

Parser::Parser(Tokenizer* tokens)
{
    _tokens = tokens;
}

ExpressionNode* Parser::parseBinaryExpression(int priority)
{
    ExpressionNode* left = parseBinaryExpression(priority + 1);

    OperationGroups* og = new OperationGroups();

    map<TokenType, int>::iterator tok =
        /*OperationGroups::getInstance()->*/
        og->_binaryOps->find(_tokens->next().type);
    if(
        tok == /*OperationGroups::getInstance()->*/og->_binaryOps->end() ||
        tok->second != priority
    )
        return left;
    BinaryNode* bNode = new BinaryNode();
    bNode->_type = _tokens->get().type;
    bNode->_left = left;
    bNode->_right = parseBinaryExpression(priority);
    return bNode;
}

ExpressionNode* Parser::parsePrimaryExpression()
{
    ExpressionNode* node = NULL;
    switch(tokenType())
    {
        case TOK_OCT_CONST:
        case TOK_DEC_CONST:
        case TOK_HEX_CONST:
            node = new IntNode(_tokens->get().value.intValue); break;
        case TOK_CHAR_CONST:
            node = new CharNode(*_tokens->get().value.strValue); break;
        case TOK_STR_CONST:
            node = new StringNode(*_tokens->get().value.strValue); break;
        case TOK_IDENT:
            node = new IdentNode(_tokens->get().text); break;
        case TOK_L_BRACKET:
            _tokens->next(); //( eaten
            node = parseExpression();
            if(tokenType() != TOK_R_BRACKET)
                throw RightBracketExpected();
            break;
        default:
            throw UnepectedToken();
    }
    _tokens->next(); //token eaten
    return node;
}

ExpressionNode* Parser::parsePostfixExpression()
{
    //'(' type_name ')' '{' initializer_list [','] '}' ;
    PostfixNode* node = new PostfixNode();
    ExpressionNode* core = parsePrimaryExpression();
    node->_only = core;
    while(true)
    {
        node->_type = tokenType();
        _tokens->next();
        switch(node->_type)
        {
            case TOK_L_SQUARE:
                node->_tail = parseExpression();
                if(tokenType() != TOK_R_SQUARE)
                    throw RightSquareBracketExpected();
                _tokens->next();
                break;
            case TOK_L_BRACKET:
                node->_tail = parseExpression(); //List();
                if(tokenType() != TOK_R_BRACKET)
                    throw RightBracketExpected();
                _tokens->next();
                break;
            case TOK_ARROW:
            case TOK_DOT:
                if(tokenType() != TOK_IDENT)
                    throw IdentifierExpected();
                node->_tail = parsePrimaryExpression();
                break;
            case TOK_INC:
            case TOK_DEC:
                break;
            default:
                return node->_only;
        }
        core = node;
        node = new PostfixNode();
    }
}

ExpressionNode* Parser::parseUnaryExpression()
{
    UnaryNode* node = NULL;
    switch(tokenType())
    {
        case TOK_INC:
        case TOK_DEC:
            node = new UnaryNode();
            node->_type = tokenType();
            _tokens->next();
            node->_only = parseUnaryExpression();
            break;
        case TOK_SIZEOF:
            node = new UnaryNode();
            node->_type = tokenType();
            _tokens->next();
            if(tokenType() != TOK_L_BRACKET)
                throw LeftBracketExpected();
            node->_only = parseUnaryExpression();
            if(tokenType() != TOK_R_BRACKET)
                throw RightBracketExpected();
            _tokens->next();
            break;
        case TOK_PLUS:
        case TOK_MINUS:
        case TOK_TILDA:
        case TOK_AMP:
        case TOK_ASTERISK:
        case TOK_NOT:
            node = new UnaryNode();
            node->_type = tokenType();
            _tokens->next();
            node->_only = parseCastExpression();
            break;
        default:
            return parsePostfixExpression();
    }
    return node;
}

ExpressionNode* Parser::parseCastExpression()
{
    //cast_expression = unary_expression | '(' type_name ')' cast_expression ;
    return parsePrimaryExpression();
}

ExpressionNode* Parser::parseConditionalExpression()
{
    return parseBinaryExpression(4);
}

ExpressionNode* Parser::parseExpression()
{
    return parseConditionalExpression();
}

}
