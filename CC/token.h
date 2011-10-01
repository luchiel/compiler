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
};

const string TOKEN_TYPE_NAME[9] =
{
    "IDENTIFICATOR",
    "INTEGER",
    "FLOAT",
    "CHARACTER",
    "STRING",
    "SEPARATOR",
    "OPERATION",
    "EOF",
    "UNKNOWN",
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
    string& asString();
};

#endif
