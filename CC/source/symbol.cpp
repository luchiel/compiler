#include <cstdio>
#include <iostream>
#include "symbol.h"
#include "symbol_table.h"

using namespace std;

namespace LuCCompiler
{

void Symbol::out(int indent)
{
    while(indent-- > 0)
        cout << '\t';
}

void SymbolType::out(int indent)
{
    if(name != "")
    {
        Symbol::out(indent);
        cout << "type <" << name << ">\n";
    }
}

void SymbolVariable::out(int indent)
{
    Symbol::out(indent);
    cout << "variable " << name << " of type <" << type->name << ">";
    if(_initializer != NULL)
    {
        cout << " initialized by\n";
        vector<bool> finishedBranches;
        _initializer->out(0, &finishedBranches, indent + 1);
    }
    else
        cout << "\n";
}

void SymbolTypeArray::out(int indent)
{
    SymbolType::out(indent++);
    Symbol::out(indent);
    cout << "array [" << endl;
    vector<bool> finishedBranches;
    if(length != NULL)
        length->out(0, &finishedBranches, indent + 1);
    Symbol::out(indent);
    cout << "] of ";
    if(elementType->name != "")
        cout << elementType->name << endl;
    else
        elementType->out(-1);
}

void SymbolTypeStruct::out(int indent)
{
    SymbolType::out(indent++);
    Symbol::out(indent);
    cout << "struct " << name << endl;
    fields->out(indent + 1);
}

void SymbolTypePointer::out(int indent)
{
    SymbolType::out(indent++);
    Symbol::out(indent);
    cout << "pointer to ";
    if(type->name != "")
        cout << type->name << endl;
    else
        type->out(-1);
}

void SymbolTypeFunction::out(int indent)
{
    SymbolType::out(indent++);
    Symbol::out(indent);
    cout << "function " << name << " of type " << type->name << endl;
    locals->out(indent + 1);
    vector<bool> finishedBranches;
    body->out(0, &finishedBranches, indent + 1);
}

}
