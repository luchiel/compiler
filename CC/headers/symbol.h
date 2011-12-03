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
    virtual SymbolType* getUndefined() { return NULL; }
    virtual bool isUndefined() { return false; }
};

class TypedSymbolType: public SymbolType
{
public:
    SymbolType* type;
    TypedSymbolType(SymbolType* type_, string name_): SymbolType(name_), type(type_) {}
    virtual SymbolType* getUndefined()
    {
        if(type->isUndefined())
            return this;
        return type->getUndefined();
    }
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

class SymbolTypeArray: public TypedSymbolType
{
public:
    Node* length;
    SymbolTypeArray(SymbolType* type_, string name_):
        TypedSymbolType(type_, name_), length(NULL) {}
    virtual void out(int indent, bool noFirst = true);
};

class SymbolTypePointer: public TypedSymbolType
{
public:
    SymbolTypePointer(SymbolType* type_, string name_): TypedSymbolType(type_, name_) {}
    virtual void out(int indent, bool noFirst = true);
};

class SymbolUnknownType: public SymbolType
{
public:
    SymbolUnknownType(): SymbolType("") {}
    virtual bool isUndefined() { return true; }
};

}

#endif
