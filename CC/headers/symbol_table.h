#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <vector>
#include <map>
#include <string>
#include "symbol.h"

using namespace std;

namespace LuCCompiler
{

enum NameType { NT_TAG, NT_NAME };

class SymbolTable
{
protected:
    map< pair<string, NameType>, Symbol*> _symbols;
    vector<Symbol*> _ordered;

public:
    SymbolTable* parent;
    bool addSymbol(Symbol* symbol, int line, int col, int name);
    bool addTag(Symbol* symbol, int line, int col, int name);
    Symbol* getSymbol(const string& name, int line, int col);
    Symbol* findSymbol(const string& name);
    Symbol* findSymbol(const string& name, NameType nt);

    unsigned int size() const { return _ordered.size(); }
    void out(int indent);

    SymbolTable(): parent(NULL) {}
    ~SymbolTable();

    Symbol* operator[](const int i) const;
    friend bool operator==(const SymbolTable& t1, const SymbolTable& t2);
    friend bool operator!=(const SymbolTable& t1, const SymbolTable& t2) { return !(t1 == t2); }
};

class SymbolTableStack
{
private:
    SymbolTable* _current;
    int _innerIdx;

public:
    SymbolTable* _root;

    void addSymbol(Symbol* symbol, int line, int col);
    void addTag(Symbol* symbol, int line, int col);
    Symbol* getSymbol(const string& name, int line, int col);
    Symbol* findSymbol(const string& name);
    Symbol* findSymbol(const string& name, NameType nt);

    void push(SymbolTable* t);
    void pop();
    void out();

    SymbolTableStack(SymbolTable* root, SymbolTable* current):
        _current(current), _innerIdx(0), _root(root) {}
    ~SymbolTableStack() { delete _root; }
};

SymbolTableStack* initPrimarySymbolTableStack();

//int -> float
//[] -> *

}

#endif
