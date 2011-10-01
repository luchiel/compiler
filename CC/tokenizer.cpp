#include <string>
#include "token.h"
#include "tokenizer.h"

void Tokenizer::read()
{
    if(buffer.size() == 0)
        getline(source, buffer);
    int i = index;
    while(i < buffer.size())
    {
        
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
