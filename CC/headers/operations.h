#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <map>
#include "token.h"

namespace LuCCompiler
{

class OperationGroups
{
private:
    OperationGroups();
    OperationGroups* _instance;

public:
    map<TokenType, int> _binaryOps;
    map<TokenType, int> _unaryOps;

    ~OperationGroups();

    static OperationGroups* getInstance()
    {
        if(_instance == NULL)
            _instance = new OperaionGroups;
        return _instance;
    }
};

OperationGroups* OperationGroups::_instance = NULL;

}

#endif
