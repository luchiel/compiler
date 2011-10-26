#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <map>
#include <string>
#include "token.h"

namespace LuCCompiler
{

class OperationGroups
{
private:
    OperationGroups();
    static OperationGroups* _instance;
    map<TokenType, int>* _binaryOps;
    //map<TokenType, int>* _unaryOps;

public:
    ~OperationGroups();

    static OperationGroups* getInstance();
    static map<TokenType, int>* binaries();
};

class OperationExpected: public exception {};

string operationName(TokenType type);

}

#endif
