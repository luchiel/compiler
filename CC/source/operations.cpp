#include <map>
#include "token.h"
#include "operations.h"

namespace LuCCompiler
{

OperationGroups::OperationGroups()
{
    _binaryOps = new map<TokenType, int>;
    (*_binaryOps)[TOK_ASTERISK] = 13; //*a!
    (*_binaryOps)[TOK_DIV] = 13;
    (*_binaryOps)[TOK_MOD] = 13;
    (*_binaryOps)[TOK_PLUS] = 12; //+1
    (*_binaryOps)[TOK_MINUS] = 12; //-1
    (*_binaryOps)[TOK_SHL] = 11;
    (*_binaryOps)[TOK_SHR] = 11;
    (*_binaryOps)[TOK_L] = 10;
    (*_binaryOps)[TOK_G] = 10;
    (*_binaryOps)[TOK_LE] = 10;
    (*_binaryOps)[TOK_GE] = 10;
    (*_binaryOps)[TOK_EQUAL] = 9;
    (*_binaryOps)[TOK_NOT_EQUAL] = 9;
    (*_binaryOps)[TOK_AMP] = 8;
    (*_binaryOps)[TOK_XOR] = 7;
    (*_binaryOps)[TOK_OR] = 6;
    (*_binaryOps)[TOK_LOGICAl_AND] = 5;
    (*_binaryOps)[TOK_LOGICAL_OR] = 4;
    //left to right

    _unaryOps = new map<TokenType, int>;
    //_unaryOps[(typename)] = 14;
    (*_unaryOps)[TOK_ASTERISK] = 15;
    (*_unaryOps)[TOK_AMP] = 15;
    (*_unaryOps)[TOK_PLUS] = 15;
    (*_unaryOps)[TOK_MINUS] = 15;
    (*_unaryOps)[TOK_NOT] = 15;
    (*_unaryOps)[TOK_TILDA] = 15;
    (*_unaryOps)[TOK_SIZEOF] = 15;
    //right to left
}

OperationGroups::~OperationGroups()
{
    delete _binaryOps;
    delete _unaryOps;
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

        case TOK_QUEST: return "?:";

        case TOK_COMMA: return ",";

        default:
            throw OperationExpected();
    }
}

}
