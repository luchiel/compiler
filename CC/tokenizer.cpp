#include "token.h"
#include "tokenizer.h"

Token& Tokenizer::get()
{
    if(buffer == "")
        source >> buffer;
    int i = index;
    while(i < buffer.size())
    {
        
    }
}

Token& Tokenizer::next()
{
}

Tokenizer::~Tokenizer()
{
    if(source.is_open())
        source.close();
}

void Tokenizer::bind(string filename)
{
    source.open(filename);
}
