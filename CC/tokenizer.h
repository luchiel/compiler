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
    enum InnerState
    {
        IS_NONE,
        IS_READ,
        IS_LCOMMENT,
        IS_COMMENT,
        IS_EOL,
    };

    InnerState _state;
    Token _current;
    string _buffer;

    int _index;
    int _currentLine;

    ifstream _source;

    void read();
    bool tryGetLine();

public:
    TokenType getType() { return _current.type; }
    string& getText() { return _current.text; }
    void* getValue() { return _current.value; }

    Token& get();
    Token& next();

    Tokenizer(): _state(IS_NONE), _current(Token()), _buffer(""), _index(0), _currentLine(0) {}
    ~Tokenizer();

    void bind(const string& filename);

    class ReadError: public std::exception {};
    class UnexpectedEOFInComment: public std::exception {};
};

#endif
