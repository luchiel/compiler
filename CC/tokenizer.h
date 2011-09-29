#include <cstdio>
#include <iostream>
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

public:
    TOKEN_TYPE getType() { return current.type; }
    string& getText() { return current.text; }
    void* getValue() { return current.value; }

    Token& get();
    Token& next();

    Tokenizer(): current(Token()), buffer("") {}
    ~Tokenizer();

    void bind(string filename);
};
