#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <vector>
#include <map>
#include <string>
#include "symbol.h"

using namespace std;

namespace LuCCompiler
{

class SymbolTable
{
protected:
    map<string, Symbol*> _symbols;
    vector<Symbol*> _ordered;
    int _innerIdx;

public:
    SymbolTable* parent;
    Symbol* getSymbol(const string& name, int line, int col);
    void addSymbol(Symbol* symbol, int line, int col);
    void size() { return _symbols.size(); }
    void out(int indent);

    SymbolTable(): _innerIdx(0), parent(NULL) {}
    ~SymbolTable();
};

class SymbolTableStack
{
private:
    SymbolTable* _root;
    SymbolTable* _current;
public:
    Symbol* getSymbol(const string& name, int line, int col);
    void addSymbol(Symbol* symbol, int line, int col);
    void push(SymbolTable* t);
    void pop();

    SymbolTableStack(SymbolTable* root, SymbolTable* current): _root(root), _current(current) {}
    ~SymbolTableStack() { delete _root; }
};

SymbolTableStack* initPrimarySymbolTableStack();

//int -> float
//[] -> *

class SymbolFunction: public Symbol
{
public:
    SymbolTable* locals;
    SymbolFunction(string name_): Symbol(name_), locals(new SymbolTable()) {}
    virtual void out(int indent);
};

class SymbolTypeStruct: public SymbolType
{
public:
    SymbolTable* fields;
    SymbolTypeStruct(const string& name_): SymbolType(name_), fields(new SymbolTable()) {}
    virtual void out(int indent);
};

}

#endif
