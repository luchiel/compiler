#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>
#include <sstream>

using namespace std;

namespace LuCCompiler
{

class LuCCException: public exception
{
private:
    int _line;
    int _col;
    string _error;

public:
    LuCCException(const int line, const int col, const string& error) throw():
        exception(), _line(line), _col(col), _error(error) {}
    ~LuCCException() throw() {}

    virtual string text() const throw();
};

class TokenizerException: public LuCCException
{
public:
    TokenizerException(const int line, const int col, const string& error) throw():
        LuCCException(line, col, error) {}

    virtual string text() const throw();
};

class ParserException: public LuCCException
{
public:
    ParserException(const int line, const int col, const string& error):
        LuCCException(line, col, error) {}

    virtual string text() const throw();
};

class RedefinedSymbolException: public LuCCException
{
public:
    RedefinedSymbolException(const int line, const int col, const string& error):
        LuCCException(line, col, error) {}

    virtual string text() const throw();
};

class UndefinedSymbolException: public LuCCException
{
public:
    UndefinedSymbolException(const int line, const int col, const string& error):
        LuCCException(line, col, error) {}

    virtual string text() const throw();
};

}

#endif
