#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <map>
#include <string>
#include "token.h"

namespace LuCCompiler
{

class Dictionaries
{
private:
    Dictionaries();
    static Dictionaries* _instance;
    map<TokenType, int>* _binaryOps;

public:
    ~Dictionaries();

    static Dictionaries* getInstance();
    static map<TokenType, int>* binaries();
};

class OperationExpected: public exception {};

string operationName(TokenType type);

}

#endif
