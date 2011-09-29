#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum TOKEN_TYPE
{
    TOK_IDENT,
    TOK_INT,
    TOK_FLOAT,
    TOK_CHAR,
    TOK_SEP,
    TOK_OPER,
    TOK_EOF,
    TOK_UNDEF,
};

class Token
{
public:
    int line;
    int col;
    TOKEN_TYPE type;
    std::string text;
    void* value;

    Token(): type(TOK_UNDEF), text(""), value(NULL) {}
};

#endif
