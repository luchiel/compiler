#include <cstdio>
#include <iostream>
#include <sstream>
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

Symbol* Symbol::resolveAlias()
{
    return this;
}

bool Symbol::setName(int name_)
{
    if(name == "")
    {
        stringstream ss;
        ss << name_;
        name = ss.str();
        return true;
    }
    return false;
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

void SymbolTypeAlias::out(int indent, bool noFirst)
{
    SymbolType::out(indent, noFirst);
    Symbol::out(indent + 1, noFirst);
    cout << "alias to type <" << alias->name << ">\n";
}

Symbol* SymbolTypeAlias::resolveAlias()
{
    return alias->resolveAlias();
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
    Symbol::out(indent + 1);
    cout << "tag " << tag << endl;
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

bool SymbolTypeAlias::operator==(Symbol& symbol)
{
    return *resolveAlias() == *symbol.resolveAlias();
}

bool SymbolVariable::operator==(Symbol& symbol)
{
    SymbolVariable* s = dynamic_cast<SymbolVariable*>(&symbol);
    if(s == NULL || s->name != name || *s->type->resolveAlias() != *type->resolveAlias())
        return false;
    return true;
}

bool SymbolTypePointer::operator==(Symbol& symbol)
{
    SymbolTypePointer* s = dynamic_cast<SymbolTypePointer*>(symbol.resolveAlias());
    if(s == NULL || *s->type->resolveAlias() != *type->resolveAlias())
        return false;
    return true;
}

bool SymbolTypeStruct::operator==(Symbol& symbol)
{
    SymbolTypeStruct* s = dynamic_cast<SymbolTypeStruct*>(symbol.resolveAlias());
    if(s == NULL || *s->fields != *fields)
        return false;
    return true;
}

bool SymbolTypeFunction::operator==(Symbol& symbol)
{
    SymbolTypeFunction* s = dynamic_cast<SymbolTypeFunction*>(&symbol);
    if(
        s == NULL || s->name != name ||
        *s->type->resolveAlias() != *type->resolveAlias() ||
        *s->args != *args
    )
        return false;
    return true;
}

}
