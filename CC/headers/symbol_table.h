#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <vector>
#include <map>
#include <string>
#include "symbol.h"
#include "statement.h"

using namespace std;

namespace LuCCompiler
{

class SymbolTable
{
protected:
    map<string, Symbol*> _symbols;
    map<string, Symbol*> _types;
    vector<Symbol*> _ordered;
    int _innerIdx;

public:
    SymbolTable* parent;
    Symbol* getSymbol(const string& name, int line, int col, bool type = false);
    void addSymbol(Symbol* symbol, int line, int col, bool type = false);
    Symbol* findSymbol(const string& name, bool type = false);

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
    Symbol* getSymbol(const string& name, int line, int col, bool type = false);
    void addSymbol(Symbol* symbol, int line, int col, bool type = false);
    Symbol* findSymbol(const string& name, bool type = false);

    void push(SymbolTable* t);
    void pop();
    void out();

    SymbolTableStack(SymbolTable* root, SymbolTable* current): _root(root), _current(current) {}
    ~SymbolTableStack() { delete _root; }
};

SymbolTableStack* initPrimarySymbolTableStack();

//int -> float
//[] -> *

class SymbolTypeFunction: public SymbolType
{
public:
    SymbolType* type;
    SymbolTable* locals;
    CompoundStatement* body;
    SymbolTypeFunction(SymbolType* type_, string name_):
        SymbolType(name_), type(type_), locals(new SymbolTable()), body(NULL) {}
    virtual void out(int indent, bool noFirst = true);
};

class SymbolTypeStruct: public SymbolType
{
public:
    SymbolTable* fields;
    SymbolTypeStruct(const string& name_): SymbolType(name_), fields(new SymbolTable()) {}
    virtual void out(int indent, bool noFirst = true);
};

}

#endif
