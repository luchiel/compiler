#include <cstdio>
#include <iostream>
#include "symbol.h"
#include "complex_symbol.h"

using namespace std;

namespace LuCCompiler
{

void Symbol::out(int indent, bool noFirst)
{
    while(indent-- > 0)
        cout << '\t';
}

void SymbolType::out(int indent, bool noFirst)
{
    if(name != "")
    {
        if(!noFirst)
            Symbol::out(indent);
        cout << "type <" << name << ">\n";
    }
}

void SymbolVariable::out(int indent, bool noFirst)
{
    if(!noFirst)
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

void SymbolTypeArray::out(int indent, bool noFirst)
{
    SymbolType::out(indent, noFirst);
    if(!noFirst)
        Symbol::out(indent + 1);
    cout << "array [" << endl;
    vector<bool> finishedBranches;
    if(length != NULL)
        length->out(0, &finishedBranches, indent + 2);
    Symbol::out(indent + 1);
    cout << "] of ";
    if(type->name != "")
        cout << "<" << type->name << ">\n";
    else
        type->out(indent);
}

void SymbolTypeStruct::out(int indent, bool noFirst)
{
    SymbolType::out(indent++, noFirst);
    if(!noFirst)
        Symbol::out(indent);
    cout << "struct " << name << endl;
    fields->out(indent + 1);
}

void SymbolTypePointer::out(int indent, bool noFirst)
{
    SymbolType::out(indent, noFirst);
    if(!noFirst)
        Symbol::out(indent + 1);
    cout << "pointer to ";
    if(type->name != "")
        cout << "<" << type->name << ">\n";
    else
        type->out(indent + 1);
}

void SymbolTypeFunction::out(int indent, bool noFirst)
{
    if(!noFirst)
        Symbol::out(indent);
    else
        SymbolType::out(indent++, noFirst);
    cout << "function " << name << "\n";
    indent++;
    Symbol::out(indent);
    cout << "returns ";
    type->out(indent);
    Symbol::out(indent);
    cout << "arguments\n";
    if(args->size() != 0)
        args->out(indent + 1);
    else
    {
        Symbol::out(indent + 1);
        cout << "<no args>\n";
    }
    vector<bool> finishedBranches;
    if(body != NULL)
        body->out(0, &finishedBranches, indent);
}

}
