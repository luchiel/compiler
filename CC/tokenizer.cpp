#include "token.h"
#include "tokenizer.h"

Token& Tokenizer::get()
{
    if(buffer.size() == 0)
        source >> buffer;
    int i = index;
    while(i < buffer.size())
    {
        
    }
    return current;
}

Token& Tokenizer::next()
{
    return current;
}

Tokenizer::~Tokenizer()
{
    if(source.is_open())
        source.close();
}

void Tokenizer::bind(string filename)
{
    source.open(filename.c_str());
}
