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

void SymbolVariable::out(int indent)
{
    Symbol::out(indent);
    cout << "variable " << name << " of type " << type->name << endl;
}

void SymbolTypeArray::out(int indent)
{
    Symbol::out(indent);
    cout << "array [" << length << "] of type " << elementType->name << endl;
}

void SymbolTypeStruct::out(int indent)
{
    Symbol::out(indent);
    cout << "struct " << name << endl;
    fields->out(indent + 1);
}

void SymbolTypePointer::out(int indent)
{
    Symbol::out(indent);
    cout << "pointer to type " << type->name << endl;
}

void SymbolFunction::out(int indent)
{
    Symbol::out(indent);
    cout << "function " << name << endl;
    locals->out(indent + 1);
}

}
