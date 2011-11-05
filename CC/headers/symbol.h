#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>

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
    int length;
    SymbolType* elementType;
    SymbolTypeArray(string name_): SymbolType(name_), elementType(NULL) {}
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
