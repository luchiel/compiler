#include <map>
#include "token.h"
#include "operations.h"

namespace LuCCompiler
{

OperationGroups::OperationGroups()
{
    _binaryOps = new map<TokenType, int>;
    map<TokenType, int> bo = *_binaryOps;
    bo[TOK_ASTERISK] = 13; //*a!
    bo[TOK_DIV] = 13;
    bo[TOK_MOD] = 13;
    bo[TOK_PLUS] = 12; //+1
    bo[TOK_MINUS] = 12; //-1
    bo[TOK_SHL] = 11;
    bo[TOK_SHR] = 11;
    bo[TOK_L] = 10;
    bo[TOK_G] = 10;
    bo[TOK_LE] = 10;
    bo[TOK_GE] = 10;
    bo[TOK_EQUAL] = 9;
    bo[TOK_NOT_EQUAL] = 9;
    bo[TOK_AMP] = 8; //&readHere
    bo[TOK_XOR] = 7;
    bo[TOK_OR] = 6;
    bo[TOK_LOGICAl_AND] = 5;
    bo[TOK_LOGICAL_OR] = 4;
    //left to right

    _unaryOps = new map<TokenType, int>;
    map<TokenType, int> uo = *_binaryOps;
    //_unaryOps[(typename)] = 14;
    uo[TOK_ASTERISK] = 15;
    uo[TOK_AMP] = 15;
    uo[TOK_PLUS] = 15;
    uo[TOK_MINUS] = 15;
    uo[TOK_NOT] = 15;
    uo[TOK_TILDA] = 15;
    uo[TOK_SIZEOF] = 15;
    //right to left
}

OperationGroups::~OperationGroups()
{
    //_binaryOps.clear();
    //_unaryOps.clear();
}

string operationName(TokenType type)
{
    switch(type)
    {
        case TOK_L_SQUARE: return "[]";
        case TOK_L_BRACKET: return "()";
        case TOK_ARROW: return "->";
        case TOK_DOT: return ".";
        case TOK_INC: return "++";
        case TOK_DEC: return "--";
        case TOK_SIZEOF: return "[]";
        case TOK_PLUS: return "+";
        case TOK_MINUS: return "-";
        case TOK_TILDA: return "~";
        case TOK_AMP: return "&";
        case TOK_ASTERISK: return "*";
        case TOK_NOT: return "!";

        case TOK_DIV: return "/";
        case TOK_MOD: return "%";
        case TOK_SHL: return "<<";
        case TOK_SHR: return ">>";
        case TOK_L: return "<";
        case TOK_G: return ">";
        case TOK_LE: return "<=";
        case TOK_GE: return ">=";
        case TOK_EQUAL: return "==";
        case TOK_NOT_EQUAL: return "!=";
        case TOK_XOR: return "^";
        case TOK_OR: return "|";
        case TOK_LOGICAl_AND: return "&&";
        case TOK_LOGICAL_OR: return "||";

        default:
            throw OperationExpected();
    }
}

}
