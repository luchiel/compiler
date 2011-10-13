#ifndef TOKEN_H
#define TOKEN_H

#include <string>

using namespace std;

namespace LuCCompiler
{

enum TokenType
{
    TOK_IDENT,
    TOK_OCT_CONST,
    TOK_DEC_CONST,
    TOK_HEX_CONST,
    TOK_FLOAT_CONST,
    TOK_CHAR_CONST,
    TOK_STR_CONST,
    TOK_SEP,
    TOK_EOF,
    TOK_UNDEF,

    TOK_L_BRACE,
    TOK_R_BRACE,
    TOK_L_BRACKET,
    TOK_R_BRACKET,
    TOK_COMMA,
    TOK_COLON,

    TOK_OPER,
    TOK_ARROW,
    TOK_QUEST,
    TOK_EQUAL,
    TOK_NOT_EQUAL,
    TOK_LOGICAl_AND,
    TOK_OR,
    TOK_LOGICAL_OR,
    TOK_NOT,
    TOK_XOR,
    TOK_PLUS,
    TOK_INC,
    TOK_ASTERISK,
    TOK_MINUS,
    TOK_DEC,
    TOK_DIV,
    TOK_MOD,
    TOK_SHL,
    TOK_SHR,
    TOK_ASSIGN,
    TOK_ADD_ASSIGN,
    TOK_SUB_ASSIGN,
    TOK_MUL_ASSIGN,
    TOK_DIV_ASSIGN,
    TOK_MOD_ASSIGN,
    TOK_SHL_ASSIGN,
    TOK_SHR_ASSIGN,
    TOK_AND_ASSIGN,
    TOK_OR_ASSIGN,
    TOK_XOR_ASSIGN,
    TOK_G,
    TOK_L,
    TOK_LE,
    TOK_GE,
    TOK_AMP,
    TOK_DOT,
    TOK_TILDA,
    TOK_L_SQUARE,
    TOK_R_SQUARE,
    TOK_SIZEOF,

    TOK_IF,
    TOK_ELSE,
    TOK_FOR,
    TOK_DO,
    TOK_WHILE,
    TOK_STRUCT,
    TOK_BREAK,
    TOK_CONTINUE,
    TOK_RETURN,

    TOK_INT,
    TOK_FLOAT,
    TOK_VOID,
};

const string TOKEN_TYPE_NAME[] =
{
    "IDENTIFIER",
    "OCTAL CONST",
    "DECIMAL CONST",
    "HEXADECIMAL CONST",
    "FLOAT CONST",
    "CHARACTER",
    "STRING",
    "SEPARATOR",
    "EOF",
    "UNKNOWN",

    "LEFT BRACE",
    "RIGHT BRACE",
    "LEFT BRACKET",
    "RIGHT BRACKET",
    "COMMA",
    "COLON",

    "OPERATION ",
    "OPERATION ->",
    "OPERATION ?",
    "OPERATION ==",
    "OPERATION !=",
    "OPERATION &&",
    "OPERATION |",
    "OPERATION ||",
    "OPERATION !",
    "OPERATION ^",
    "OPERATION +",
    "OPERATION ++",
    "OPERATION *",
    "OPERATION -",
    "OPERATION --",
    "OPERATION /",
    "OPERATION %",
    "OPERATION <<",
    "OPERATION >>",
    "OPERATION =",
    "OPERATION +=",
    "OPERATION -=",
    "OPERATION *=",
    "OPERATION /=",
    "OPERATION %=",
    "OPERATION <<=",
    "OPERATION >>=",
    "OPERATION &=",
    "OPERATION |=",
    "OPERATION ^=",
    "OPERATION >",
    "OPERATION <",
    "OPERATION <=",
    "OPERATION >=",
    "OPERATION &",
    "OPERATION .",
    "OPERATION ~",
    "OPERATION [",
    "OPERATION ]",
    "OPERATION sizeof",

    "if KEYWORD",
    "else KEYWORD",
    "for KEYWORD",
    "do KEYWORD",
    "while KEYWORD",
    "struct KEYWORD",
    "break KEYWORD",
    "continue KEYWORD",
    "return KEYWORD",

    "int KEYWORD",
    "float KEYWORD",
    "void KEYWORD",
};

class Token
{
public:
    TokenType type;
    std::string text;

    int line;
    int col;

    union
    {
        double floatValue;
        int intValue;
        string* strValue;
    }
    value;

    Token(): type(TOK_UNDEF), text(""), line(0), col(0) {}
    void outputAsString(ostream& outStream);
};

}

#endif
