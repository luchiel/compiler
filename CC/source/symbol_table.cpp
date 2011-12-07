#include <sstream>
#include <string>
#include <assert.h>
#include <map>
#include "exception.h"
#include "symbol_table.h"
#include "symbol.h"

namespace LuCCompiler
{

void SymbolTable::addSymbol(Symbol* symbol, NameType nt, int line, int col)
{
    if(_symbols.find(make_pair(symbol->name, nt)) != _symbols.end())
        throw RedefinedSymbolException(line, col, symbol->name);

    if(symbol->name == "")
    {
        stringstream ss;
        ss << _innerIdx;
        symbol->name = ss.str();
        _innerIdx++;
    }
    _symbols[make_pair(symbol->name, nt)] = symbol;
    _ordered.push_back(symbol);
}

Symbol* SymbolTable::getSymbol(const string& name, int line, int col)
{
    Symbol* s = findSymbol(name);
    if(s == NULL)
        throw UndefinedSymbolException(line, col, name);
    else
        return s;
}

Symbol* SymbolTable::findSymbol(const string& name)
{
    map< pair<string, NameType>, Symbol*>::iterator s;
    s = _symbols.find(make_pair(name, NT_NAME));
    if(s != _symbols.end())
        return s->second;
    s = _symbols.find(make_pair(name, NT_TAG));
    if(s != _symbols.end())
        return s->second;
    return NULL;
}

Symbol* SymbolTable::findSymbol(const string& name, NameType nt)
{
    map< pair<string, NameType>, Symbol*>::iterator s;
    s = _symbols.find(make_pair(name, nt));
    if(s != _symbols.end())
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

Symbol* SymbolTableStack::findSymbol(const string& name)
{
    SymbolTable* c = _current;
    Symbol* r = c->findSymbol(name);
    while(r == NULL)
    {
        if(c == _root)
            break;
        c = c->parent;
        r = c->findSymbol(name);
    }
    return r;
}

Symbol* SymbolTableStack::findSymbol(const string& name, NameType nt)
{
    SymbolTable* c = _current;
    Symbol* r = c->findSymbol(name, nt);
    while(r == NULL)
    {
        if(c == _root)
            break;
        c = c->parent;
        r = c->findSymbol(name, nt);
    }
    return r;
}

Symbol* SymbolTableStack::getSymbol(const string& name, int line, int col)
{
    Symbol* s = findSymbol(name);
    if(s == NULL)
        throw UndefinedSymbolException(line, col, name);
    return s;
}

void SymbolTableStack::addSymbol(Symbol* symbol, NameType nt, int line, int col)
{
    _current->addSymbol(symbol, nt, line, col);
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

    primarySymbolTable->addSymbol(new SymbolType("int"), NT_NAME, 0, 0);
    primarySymbolTable->addSymbol(new SymbolType("float"), NT_NAME, 0, 0);
    primarySymbolTable->addSymbol(new SymbolType("void"), NT_NAME, 0, 0);

    return new SymbolTableStack(primarySymbolTable, primarySymbolTable);
}

}
