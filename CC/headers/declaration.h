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
public:
    TokenType type; //[ or .
    Node* sub;
    Designator(TokenType type_, Node* sub_): type(type_), sub(sub_) {}
};

typedef NodeWithList<Designator> DesignationT;

class Designation: public DesignationT
{
public:
    Designation(): DesignationT() {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

typedef NodeWithList< pair<Designation*, Node*> > InitializerListT;

class InitializerList: public InitializerListT
{
public:
    InitializerList(): InitializerListT() {}
    virtual void out(unsigned int depth, vector<bool>* branches, int indent = 0);
};

}

#endif
