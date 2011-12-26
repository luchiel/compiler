#ifndef COMPLEX_SYMBOL_H
#define COMPLEX_SYMBOL_H

#include <vector>
#include <map>
#include <string>
#include "symbol.h"
#include "symbol_table.h"
#include "statement.h"

using namespace std;

namespace LuCCompiler
{

class SymbolTypeFunction: public TypedSymbolType
{
private:
    bool _bodyPrinted;
public:
    SymbolTable* args;
    CompoundStatement* body;
    SymbolTypeFunction(SymbolType* type_, string name_):
        TypedSymbolType(type_, name_), _bodyPrinted(false), args(new SymbolTable()), body(NULL)
        { classType = CT_FUNCTION; }
    virtual void out(int indent, bool noFirst = true);

    virtual bool operator==(Symbol& symbol);
};

class SymbolTypeStruct: public SymbolType
{
private:
    int _size;
public:
    string tag;
    SymbolTable* fields;
    SymbolTypeStruct(const string& name_, const string& tag_):
        SymbolType(name_), _size(0), tag(tag_), fields(new SymbolTable())
        { classType = CT_STRUCT; }
    virtual void out(int indent, bool noFirst = true);

    virtual bool operator==(Symbol& symbol);
    virtual int size();
};

}

#endif
