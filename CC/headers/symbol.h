#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>
#include "node.h"

using namespace std;

namespace LuCCompiler
{

enum ClassType
{
    CT_UNKNOWN,
    CT_BASE,
    CT_VAR,
    CT_ARRAY,
    CT_POINTER,
    CT_ALIAS,
    CT_STRUCT,
    CT_FUNCTION,
};

class Symbol
{
public:
    ClassType classType;
    string name;
    Symbol(string name_): classType(CT_UNKNOWN), name(name_) {}
    virtual ~Symbol() {}
    virtual void out(int indent, bool noFirst = true);
    bool setName(int name_);
    virtual Symbol* resolveAlias();

    bool operator!=(Symbol& symbol) { return !(*this == symbol); }
    bool isFunction() { return resolveAlias()->classType == CT_FUNCTION; }
    bool isStruct() { return resolveAlias()->classType == CT_STRUCT; }
    bool isPointer()
    {
        return
            resolveAlias()->classType == CT_POINTER ||
            resolveAlias()->classType == CT_ARRAY;
    }
    virtual bool operator==(Symbol& symbol) { return true; }
};

class SymbolType: public Symbol
{
public:
    SymbolType* castTo;
    SymbolType(string name_): Symbol(name_), castTo(NULL) { classType = CT_BASE; }
    virtual void out(int indent, bool noFirst = true);
    virtual SymbolType* getUndefined() { return NULL; }
    virtual bool isUndefined() { return false; }
    virtual bool operator==(Symbol& symbol);
};

class SymbolTypeAlias: public SymbolType
{
public:
    SymbolType* alias;
    SymbolTypeAlias(SymbolType* alias_, string name_):
        SymbolType(name_), alias(alias_) { classType = CT_ALIAS; }
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
        Symbol(name_), _initializer(NULL), type(type_) { classType = CT_VAR; }
    SymbolVariable(SymbolType* type_, string name_, Node* initializer_):
        Symbol(name_), _initializer(initializer_), type(type_) { classType = CT_VAR; }
    virtual void out(int indent, bool noFirst = true);

    virtual bool operator==(Symbol& symbol);
};

class SymbolTypePointer: public TypedSymbolType
{
public:
    SymbolTypePointer(SymbolType* type_, string name_):
        TypedSymbolType(type_, name_) { classType = CT_POINTER; }
    virtual void out(int indent, bool noFirst = true);

    virtual bool operator==(Symbol& symbol);
};

class SymbolTypeArray: public SymbolTypePointer
{
public:
    Node* length;
    SymbolTypeArray(SymbolType* type_, string name_):
        SymbolTypePointer(type_, name_), length(NULL) { classType = CT_ARRAY; }
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
