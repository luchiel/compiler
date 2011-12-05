#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <cstdio>
#include <string>
#include <fstream>
#include <set>
#include <map>
#include "token.h"
#include "exception.h"

using namespace std;

namespace LuCCompiler
{

class Tokenizer
{
private:
    enum InnerState
    {
        IS_NONE,
        IS_READ,
        IS_MADE,
        IS_LCOMMENT,
        IS_COMMENT,
    };

    set<char> operations;
    map<string, TokenType> keywords;

    InnerState _state;
    Token _current;
    Token _prev;
    Token _next;
    string _buffer;

    unsigned int _index;
    unsigned int _currentLine;

    ifstream _source;

    void read();
    bool tryGetLine();
    void checkKeywords();

    void readStr(unsigned int idx);
    void readChar(unsigned int& idx);

    int getIntValue(char cval);
    bool isOctDigit(char cval);
    void readInt(unsigned int& idx);
    void readIdentifier(unsigned int& idx);
    bool tryReadFloatPart(unsigned int& idx, bool hasIntPart);

    void setTypeAndReadState(TokenType tt) { _current.type = tt; _state = IS_READ; }
    void makeEOFToken();

    void processEscapeSequencesInStrValue();

    char trySymbol(unsigned int pos);

public:
    TokenType getType() { return _current.type; }
    string& getText() { return _current.text; }

    Token& get();
    Token& next();
    void lookForward();
    void rollBack();

    Tokenizer(const string& filename);
    ~Tokenizer();

    void bind(const string& filename);

    TokenizerException makeException(int col, const string& e);
};

class BadFile: public std::exception {};

}

#endif
