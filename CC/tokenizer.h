#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <cstdio>
#include <string>
#include <fstream>
#include "token.h"

using namespace std;

class Tokenizer
{
private:
    Token current;
    string buffer;
    ifstream source;
    int index;

    void read();

public:
    TokenType getType() { return current.type; }
    string& getText() { return current.text; }
    void* getValue() { return current.value; }

    Token& get();
    Token& next();

    Tokenizer(): current(Token()), buffer("") {}
    ~Tokenizer();

    void bind(const string& filename);
};

#endif
