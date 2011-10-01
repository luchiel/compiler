#include <string>
#include "token.h"
#include "tokenizer.h"

void dd() {}

void Tokenizer::read()
{
    if(buffer.size() == 0)
        getline(source, buffer);

    current = Token();

    unsigned int i = index;
    while(i < buffer.size())
    {
        switch(current.type)
        {
            case TOK_UNDEF:
                if(buffer[i] >= 'A' && buffer[i] <= 'Z' || buffer[i] >= 'a' && buffer[i] <= 'z' || buffer[i] == '_')
                    current.type = TOK_IDENT;
                else if(buffer[i] == '"')
                    dd;
                else if(buffer[i] == '\'')
                    current.type == TOK_CHAR;
                else if(buffer[i] == '\t' || buffer[i] == ' ')
                    break;
                else if(buffer[i] == '/' && i < buffer.size() - 1)
                    if(buffer[i + 1] == '/')
                        dd;
                    else if(buffer[i + 1] == '*')
                        dd;
                    dd;
                break;

            default: break;

        /*
            TOK_IDENT,
            TOK_INT,
            TOK_FLOAT,
            TOK_CHAR,
            TOK_STR,
            TOK_SEP,
            TOK_OPER,
            TOK_EOF,
            TOK_UNDEF,
        */
        }
    }
}

Token& Tokenizer::get()
{
    return current;
}

Token& Tokenizer::next()
{
    read();
    return current;
}

Tokenizer::~Tokenizer()
{
    if(source.is_open())
        source.close();
}

void Tokenizer::bind(const string& filename)
{
    source.open(filename.c_str());
}
