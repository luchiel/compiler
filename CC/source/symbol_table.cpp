#include <sstream>
#include <assert.h>
#include "exception.h"
#include "symbol_table.h"
#include "symbol.h"

namespace LuCCompiler
{

void SymbolTable::addSymbol(Symbol* symbol, int line, int col)
{
    if(_symbols.find(symbol->name) != _symbols.end())
        throw RedefinedSymbolException(line, col, symbol->name);

    //int, float. Manual check?

    if(symbol->name == "")
    {
        stringstream ss;
        ss << _innerIdx;
        symbol->name = ss.str();
        _innerIdx++;
    }
    _symbols[symbol->name] = symbol;
    _ordered.push_back(symbol);
}

Symbol* SymbolTable::getSymbol(const string& name, int line, int col)
{
    if(_symbols.find(name) == _symbols.end())
        throw UndefinedSymbolException(line, col, name);
    return _symbols[name];
}

Symbol* SymbolTable::findSymbol(const string& name)
{
    map<string, Symbol*>::iterator s;
    s = _symbols.find(name);
    if(s != _symbols.end())
        return s->second;
    return NULL;
}

void SymbolTable::out(int indent)
{
    for(unsigned int i = 0; i < _ordered.size(); ++i)
        _ordered[i]->out(indent);
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

Symbol* SymbolTableStack::getSymbol(const string& name, int line, int col)
{
    return _current->getSymbol(name, line, col);
}

void SymbolTableStack::addSymbol(Symbol* symbol, int line, int col)
{
    _current->addSymbol(symbol, line, col);
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

SymbolTableStack* initPrimarySymbolTableStack()
{
    SymbolTable* primarySymbolTable = new SymbolTable();

    primarySymbolTable->addSymbol(new SymbolType("int"), 0, 0);
    primarySymbolTable->addSymbol(new SymbolType("float"), 0, 0);
    primarySymbolTable->addSymbol(new SymbolType("void"), 0, 0);

    return new SymbolTableStack(primarySymbolTable, primarySymbolTable);
}

}
