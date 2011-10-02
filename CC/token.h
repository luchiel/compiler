#ifndef TOKEN_H
#define TOKEN_H

#include <string>

using namespace std;

enum TokenType
{
    TOK_IDENT,
    TOK_INT,
    TOK_FLOAT,
    TOK_CHAR,
    TOK_STR,
    TOK_SEP,
    TOK_OPER,
    TOK_EOF,
    TOK_UNDEF,

    TOK_L_BRACE,
    TOK_R_BRACE,
    TOK_L_BRACKET,
    TOK_R_BRACKET,
    TOK_COMMA,
};

const string TOKEN_TYPE_NAME[14] =
{
    "IDENTIFIER",
    "INTEGER",
    "FLOAT",
    "CHARACTER",
    "STRING",
    "SEPARATOR",
    "OPERATION",
    "EOF",
    "UNKNOWN",

    "LEFT BRACE",
    "RIGHT BRACE",
    "LEFT BRACKET",
    "RIGHT BRACKET",
    "COMMA",
};

class Token
{
public:
    int line;
    int col;
    TokenType type;
    std::string text;
    void* value;

    Token(): type(TOK_UNDEF), text(""), value(NULL) {}
    string asString();
};

#endif
