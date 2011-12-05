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
public:
    SymbolTable* args;
    CompoundStatement* body;
    SymbolTypeFunction(SymbolType* type_, string name_):
        TypedSymbolType(type_, name_), args(new SymbolTable()), body(NULL) {}
    virtual void out(int indent, bool noFirst = true);
};

class SymbolTypeStruct: public SymbolType
{
public:
    SymbolTable* fields;
    SymbolTypeStruct(const string& name_): SymbolType(name_), fields(new SymbolTable()) {}
    virtual void out(int indent, bool noFirst = true);
};

}

#endif
