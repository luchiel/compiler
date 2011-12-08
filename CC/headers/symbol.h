#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>
#include "node.h"

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
    bool setName(int name_);
    virtual Symbol* resolveAlias();

    bool operator!=(Symbol& symbol) { return !(*this == symbol); }
    virtual bool operator==(Symbol& symbol) { return name == symbol.name; }
};

class SymbolType: public Symbol
{
public:
    SymbolType(string name_): Symbol(name_) {}
    virtual void out(int indent, bool noFirst = true);
    virtual SymbolType* getUndefined() { return NULL; }
    virtual bool isUndefined() { return false; }
};

class SymbolTypeAlias: public SymbolType
{
public:
    SymbolType* alias;
    SymbolTypeAlias(SymbolType* alias_, string name_): SymbolType(name_), alias(alias_) {}
    virtual void out(int indent, bool noFirst = true);
    virtual Symbol* resolveAlias();

    virtual bool operator==(Symbol& symbol);
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

    virtual bool operator==(Symbol& symbol);
};

class SymbolTypePointer: public TypedSymbolType
{
public:
    SymbolTypePointer(SymbolType* type_, string name_): TypedSymbolType(type_, name_) {}
    virtual void out(int indent, bool noFirst = true);

    virtual bool operator==(Symbol& symbol);
};

class SymbolTypeArray: public SymbolTypePointer
{
public:
    Node* length;
    SymbolTypeArray(SymbolType* type_, string name_):
        SymbolTypePointer(type_, name_), length(NULL) {}
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
