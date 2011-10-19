#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <map>
#include <string>
#include "token.h"

namespace LuCCompiler
{

class OperationGroups
{
//private:
    //OperationGroups();
    //static OperationGroups* _instance;

public:
    map<TokenType, int>* _binaryOps;
    //map<TokenType, int>* _unaryOps;

    ~OperationGroups();

    //till problem found
    OperationGroups();
    /*
    static OperationGroups* getInstance()
    {
        if(_instance == NULL)
            _instance = new OperationGroups();
        return _instance;
    }
    */
};

class OperationExpected: public exception {};

string operationName(TokenType type);

//OperationGroups* OperationGroups::_instance = NULL;

}

#endif
