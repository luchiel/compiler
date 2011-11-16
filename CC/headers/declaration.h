#ifndef DECLARATION_H
#define DECLARATION_H

#include <vector>
#include <utility>
#include "node.h"
#include "token.h"
#include "nodeWithList.h"

using namespace std;

namespace LuCCompiler
{

class Designator: public Node
{
private:
    TokenType _type; //[ or .
    Node* _sub;
public:
    Designator(TokenType type, Node* sub): _type(type), _sub(sub) {}
};

typedef NodeWithList<Designator> Designation;

typedef NodeWithList< pair<Designation*, Node*> > InitializerList;

}

#endif
