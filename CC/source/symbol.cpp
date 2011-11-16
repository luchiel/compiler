#include <iostream>
#include "symbol.h"
#include "symbol_table.h"

using namespace std;

namespace LuCCompiler
{

void Symbol::out(int indent)
{
    for(int i = indent; i > 0; --i)
        cout << "\t";
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
        _initializer->out(0, &finishedBranches);
    }
    else
        cout << "\n";
}

void SymbolTypeArray::out(int indent)
{
    SymbolType::out(indent);
    Symbol::out(indent + 1);
    cout << "array [" << endl;
    vector<bool> finishedBranches;
    length->out(0, &finishedBranches);
    Symbol::out(indent + 1);
    cout << "] of\n";
    elementType->out(indent + 1);
}

void SymbolTypeStruct::out(int indent)
{
    SymbolType::out(indent);
    Symbol::out(indent + 1);
    cout << "struct " << name << endl;
    fields->out(indent + 1);
}

void SymbolTypePointer::out(int indent)
{
    SymbolType::out(indent);
    Symbol::out(indent + 1);
    cout << "pointer to\n";
    type->out(indent + 1);
}

void SymbolTypeFunction::out(int indent)
{
    SymbolType::out(indent);
    Symbol::out(indent + 1);
    cout << "function " << name << " of type " << type->name << endl;
    locals->out(indent + 1);
    vector<bool> finishedBranches;
    body->out(0, &finishedBranches);
}

}
