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
    virtual void out(int indent, bool noFirst = true);
};

class SymbolType: public Symbol
{
public:
    SymbolType(string name_): Symbol(name_) {}
    virtual void out(int indent, bool noFirst = true);
};

class SymbolVariable: public Symbol
{
protected:
    Node* _initializer;
public:
    SymbolType* type;
    SymbolVariable(SymbolType* type_, string name_):
        Symbol(name_), _initializer(NULL), type(type_) {}
    SymbolVariable(SymbolType* type_, string name_, Node* initializer_):
        Symbol(name_), _initializer(initializer_), type(type_) {}
    virtual void out(int indent, bool noFirst = true);
};

class SymbolTypeArray: public SymbolType
{
public:
    SymbolType* elementType;
    Node* length;
    SymbolTypeArray(SymbolType* type_, string name_):
        SymbolType(name_), elementType(type_), length(NULL) {}
    virtual void out(int indent, bool noFirst = true);
};

class SymbolTypePointer: public SymbolType
{
public:
    SymbolType* type;
    SymbolTypePointer(string name_): SymbolType(name_), type(NULL) {}
    virtual void out(int indent, bool noFirst = true);
};

}

#endif
