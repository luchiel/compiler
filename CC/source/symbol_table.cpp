#include <sstream>
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

SymbolTable* initPrimarySymbolTable()
{
    SymbolTable* primarySymbolTable = new SymbolTable();

    primarySymbolTable->addSymbol(new SymbolType("int"), 0, 0);
    primarySymbolTable->addSymbol(new SymbolType("float"), 0, 0);
    primarySymbolTable->addSymbol(new SymbolType("void"), 0, 0);

    return primarySymbolTable;
}

}
