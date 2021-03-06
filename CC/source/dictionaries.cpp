#include <map>
#include "token.h"
#include "dictionaries.h"

namespace LuCCompiler
{

Dictionaries* Dictionaries::_instance = NULL;

Dictionaries* Dictionaries::getInstance()
{
    if(_instance == NULL)
        _instance = new Dictionaries();
    return _instance;
}

map<TokenType, int>* Dictionaries::binaries()
{
    return getInstance()->_binaryOps;
}

Dictionaries::Dictionaries()
{
    _binaryOps = new map<TokenType, int>;
    (*_binaryOps)[TOK_ASTERISK] = 13;
    (*_binaryOps)[TOK_DIV] = 13;
    (*_binaryOps)[TOK_MOD] = 13;
    (*_binaryOps)[TOK_PLUS] = 12;
    (*_binaryOps)[TOK_MINUS] = 12;
    (*_binaryOps)[TOK_SHL] = 11;
    (*_binaryOps)[TOK_SHR] = 11;
    (*_binaryOps)[TOK_L] = 10;
    (*_binaryOps)[TOK_G] = 10;
    (*_binaryOps)[TOK_LE] = 10;
    (*_binaryOps)[TOK_GE] = 10;
    (*_binaryOps)[TOK_E] = 9;
    (*_binaryOps)[TOK_NE] = 9;
    (*_binaryOps)[TOK_AMP] = 8;
    (*_binaryOps)[TOK_XOR] = 7;
    (*_binaryOps)[TOK_OR] = 6;
    (*_binaryOps)[TOK_LOGICAL_AND] = 5;
    (*_binaryOps)[TOK_LOGICAL_OR] = 4;
}

Dictionaries::~Dictionaries()
{
    delete _binaryOps;
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
        case TOK_SIZEOF: return "sizeof";
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
        case TOK_E: return "==";
        case TOK_NE: return "!=";
        case TOK_XOR: return "^";
        case TOK_OR: return "|";
        case TOK_LOGICAL_AND: return "&&";
        case TOK_LOGICAL_OR: return "||";

        case TOK_QUEST: return "?:";

        case TOK_COMMA: return ",";

        case TOK_ASSIGN: return "=";
        case TOK_MUL_ASSIGN: return "*=";
        case TOK_DIV_ASSIGN: return "/=";
        case TOK_MOD_ASSIGN: return "%=";
        case TOK_ADD_ASSIGN: return "+=";
        case TOK_SUB_ASSIGN: return "-=";
        case TOK_SHL_ASSIGN: return "<<=";
        case TOK_SHR_ASSIGN: return ">>=";
        case TOK_AND_ASSIGN: return "&=";
        case TOK_XOR_ASSIGN: return "^=";
        case TOK_OR_ASSIGN: return "|=";

        default:
            throw OperationExpected();
    }
}

}
