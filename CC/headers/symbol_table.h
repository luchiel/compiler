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
    void out(int indent);

    SymbolTable(parent_ = NULL): _innerIdx(0), parent(parent_) {}
    ~SymbolTable();
};

SymbolTable PrimarySymbolTable();

PrimarySymbolTable.addSymbol(new SymbolType("int"));
PrimarySymbolTable.addSymbol(new SymbolType("float"));

//int -> float
//[] -> *

}

#endif
