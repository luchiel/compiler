#ifndef COMPLEX_SYMBOL_H
#define COMPLEX_SYMBOL_H

#include <vector>
#include <map>
#include <string>
#include <set>
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
    set<SymbolTypeFunction*>* callList;
    SymbolTypeFunction(SymbolType* type_, string name_):
        TypedSymbolType(type_, name_), _bodyPrinted(false),
        args(new SymbolTable()), body(NULL), callList(new set<SymbolTypeFunction*>())
        { classType = CT_FUNCTION; }
    ~SymbolTypeFunction() { delete callList; }
    virtual void out(int indent, bool noFirst = true);

    virtual bool operator==(Symbol& symbol);
    bool findCall(SymbolTypeFunction* func);
};

class SymbolTypeStruct: public SymbolType
{
private:
    int _size;
    bool _calculated;
public:
    string tag;
    SymbolTable* fields;
    SymbolTypeStruct(const string& name_, const string& tag_):
        SymbolType(name_), _size(0), _calculated(false), tag(tag_), fields(new SymbolTable())
        { classType = CT_STRUCT; }
    virtual void out(int indent, bool noFirst = true);

    virtual bool operator==(Symbol& symbol);
    virtual unsigned int size();
    void calculateOffsets();
};

}

#endif
