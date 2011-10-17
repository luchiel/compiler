#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <cstdio>
#include <string>
#include <fstream>
#include <set>
#include <map>
#include "token.h"

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

    Tokenizer(const string& filename);
    ~Tokenizer();

    void bind(const string& filename);
    //void bind(const wstring& filename);

    class UnexpectedEOFInComment: public exception
    {
        virtual const char* what() const throw() { return "Unexpected EOF in comment"; }
    };
    class NewlineInConstantString: public exception
    {
        virtual const char* what() const throw() { return "Newline in constant string"; }
    };
    class NewlineInConstantChar: public exception
    {
        virtual const char* what() const throw() { return "Newline in constant char"; }
    };
    class MulticharacterConstantChar: public exception
    {
        virtual const char* what() const throw() { return "Multicharacter constant char"; }
    };
    class InvalidFloatingPointConstant: public exception
    {
        virtual const char* what() const throw() { return "Invalid floating point constant"; }
    };
    class UnknownOperation: public exception
    {
        virtual const char* what() const throw() { return "Inknown operation"; }
    };
    class BadDigitInOctalConst: public exception
    {
        virtual const char* what() const throw() { return "Bad digit in octal const"; }
    };
    class InvalidEscapeSequence: public exception
    {
        virtual const char* what() const throw() { return "Invalid escape sequence"; }
    };
};

class BadFile: public std::exception {};

}

#endif
