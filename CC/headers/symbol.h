#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>
#include "expression.h"

using namespace std;

namespace LuCCompiler
{

class Symbol
{
public:
    string name;
    Symbol(string name_): name(name_) {}
    virtual ~Symbol() {}
    virtual void out(int indent);
};

class SymbolType: public Symbol
{
public:
    SymbolType(string name_): Symbol(name_) {}
};

class SymbolVariable: public Symbol
{
public:
    SymbolType* type;
    SymbolVariable(SymbolType* type_, string name_): Symbol(name_), type(type_) {}
    virtual void out(int indent);
};

class SymbolTypeArray: public SymbolType
{
public:
    SymbolType* elementType;
    AssignmentNode* length;
    SymbolTypeArray(SymbolType* type_, string name_):
        SymbolType(name_), elementType(type_), length(NULL) {}
    virtual void out(int indent);
};

class SymbolTypePointer: public SymbolType
{
public:
    SymbolType* type;
    SymbolTypePointer(string name_): SymbolType(name_), type(NULL) {}
    virtual void out(int indent);
};

}

#endif
