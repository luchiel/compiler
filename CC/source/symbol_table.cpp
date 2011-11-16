#include <sstream>
#include <string>
#include <assert.h>
#include <map>
#include "exception.h"
#include "symbol_table.h"
#include "symbol.h"

namespace LuCCompiler
{

void SymbolTable::addSymbol(Symbol* symbol, int line, int col, bool type)
{
    map<string, Symbol*>* table = type ? &_types : &_symbols;
    if(table->find(symbol->name) != table->end())
        throw RedefinedSymbolException(line, col, symbol->name);

    //int, float. Manual check?

    if(symbol->name == "")
    {
        stringstream ss;
        ss << _innerIdx;
        symbol->name = ss.str();
        _innerIdx++;
    }
    (*table)[symbol->name] = symbol;
    _ordered.push_back(symbol);
}

Symbol* SymbolTable::getSymbol(const string& name, int line, int col, bool type)
{
    map<string, Symbol*>* table = type ? &_types : &_symbols;
    if(table->find(name) == table->end())
        throw UndefinedSymbolException(line, col, name);
    return (*table)[name];
}

Symbol* SymbolTable::findSymbol(const string& name, bool type)
{
    map<string, Symbol*>* table = type ? &_types : &_symbols;
    map<string, Symbol*>::iterator s;
    s = table->find(name);
    if(s != table->end())
        return s->second;
    return NULL;
}

void SymbolTable::out(int indent)
{
    for(unsigned int i = 0; i < _ordered.size(); ++i)
        _ordered[i]->out(indent, false);
}

SymbolTable::~SymbolTable()
{
    //objects. How to kill?
    //cycle? Carefully delete everyone?
}

Symbol* SymbolTableStack::findSymbol(const string& name, bool type)
{
    SymbolTable* c = _current;
    Symbol* r = c->findSymbol(name, type);
    while(r == NULL)
    {
        if(c == _root)
            break;
        c = c->parent;
        r = c->findSymbol(name, type);
    }
    return r;
}

Symbol* SymbolTableStack::getSymbol(const string& name, int line, int col, bool type)
{
    Symbol* s = findSymbol(name, type);
    if(s == NULL)
        throw UndefinedSymbolException(line, col, name);
    return s;
}

void SymbolTableStack::addSymbol(Symbol* symbol, int line, int col, bool type)
{
    _current->addSymbol(symbol, line, col, type);
}

void SymbolTableStack::push(SymbolTable* t)
{
    t->parent = _current;
    _current = t;
}

void SymbolTableStack::pop()
{
    assert(_current->parent != NULL);
    _current = _current->parent;
}

void SymbolTableStack::out()
{
    _root->out(0);
}

SymbolTableStack* initPrimarySymbolTableStack()
{
    SymbolTable* primarySymbolTable = new SymbolTable();

    primarySymbolTable->addSymbol(new SymbolType("int"), 0, 0, true);
    primarySymbolTable->addSymbol(new SymbolType("float"), 0, 0, true);
    primarySymbolTable->addSymbol(new SymbolType("void"), 0, 0, true);

    return new SymbolTableStack(primarySymbolTable, primarySymbolTable);
}

}
