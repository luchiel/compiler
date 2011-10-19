#include <string>
#include <sstream>
#include "exception.h"

namespace LuCCompiler
{

const char* LuCCException::what() const throw()
{
    stringstream ss;
    ss << _line << ":" << _col << " " << _error;
    return ss.str().c_str();
}

const char* TokenizerException::what() const throw()
{
    string e("TokenError:");
    e += LuCCException::what();
    return e.c_str();
}

const char* ParserException::what() const throw()
{
    string e("ParseError:");
    e += LuCCException::what();
    return e.c_str();
}

}
