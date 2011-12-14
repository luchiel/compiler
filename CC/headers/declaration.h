#ifndef DECLARATION_H
#define DECLARATION_H

#include <vector>
#include <utility>
#include "expression.h"
#include "token.h"
#include "nodeWithList.h"

using namespace std;

namespace LuCCompiler
{

class Designator: public ENode
{
public:
    TokenType type; //[ or .
    ENode* sub;
    Designator(TokenType type_, ENode* sub_): type(type_), sub(sub_) {}
};

typedef NodeWithList<Designator> DesignationT;

class Designation: public DesignationT
{
public:
    Designation(): DesignationT() {}
    virtual void out(unsigned int depth, vector<bool>* branches, int level = 0);
};

typedef NodeWithList< pair<Designation*, Node*> > InitializerListT;

class InitializerList: public InitializerListT
{
public:
    InitializerList(): InitializerListT() {}
    virtual void out(unsigned int depth, vector<bool>* branches, int level = 0);
};

}

#endif
