#ifndef DECLARATION_H
#define DECLARATION_H

#include <vector>
#include "node.h"

using namespace std;

namespace LuCCompiler
{

class IdentifierList: public Node
{
public:
    vector<IdentNode*>* _list;
    IdentifierList() { _list = new vector<IdentNode*>; }
    ~IdentifierList() { delete _list; }
};

}

#endif
