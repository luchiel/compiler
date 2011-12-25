#include <string>
#include <sstream>
#include "exception.h"

namespace LuCCompiler
{

string itostr(const int i)
{
    stringstream ss;
    ss << i;
    return ss.str();
}

string LuCCException::text() const throw()
{
    stringstream ss;
    ss << _line << ":" << _col << " " << _error;
    return ss.str();
}

string TokenizerException::text() const throw()
{
    string e("TokenError:");
	e += LuCCException::text();
    return e;
}

string ParserException::text() const throw()
{
    string e("ParseError:");
    e += LuCCException::text();
    return e;
}

string UndefinedSymbolException::text() const throw()
{
    string e("UndefinedSymbol:");
    e += LuCCException::text();
    return e;
}

string RedefinedSymbolException::text() const throw()
{
    string e("RedefinedSymbol:");
    e += LuCCException::text();
    return e;
}

}
