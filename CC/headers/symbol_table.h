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
    Symbol* findSymbol(const string& name);

    int size() { return _ordered.size(); }
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
    Symbol* findSymbol(const string& name);

    void push(SymbolTable* t);
    void pop();
    void out();

    SymbolTableStack(SymbolTable* root, SymbolTable* current): _root(root), _current(current) {}
    ~SymbolTableStack() { delete _root; }
};

SymbolTableStack* initPrimarySymbolTableStack();

//int -> float
//[] -> *

}

#endif
