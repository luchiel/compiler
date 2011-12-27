#include <cstdio>
#include <iostream>
#include <sstream>
#include "symbol.h"
#include "exception.h"
#include "expression.h"
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
    if(initializer != NULL)
    {
        cout << " initialized by\n";
        vector<bool> finishedBranches;
        initializer->out(0, &finishedBranches, indent + 1);
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
    if(body != NULL && !_bodyPrinted)
    {
        _bodyPrinted = true;
        body->out(0, &finishedBranches, indent);
    }
}

bool SymbolType::operator==(Symbol& symbol)
{
    if(name == "float" && symbol.resolveAlias()->name == "int")
    {
        //the one that is ok for cast must be second
        static_cast<SymbolType*>(&symbol)->castTo = this;
        return false;
    }
    return name == symbol.name;
}

bool SymbolTypeAlias::operator==(Symbol& symbol)
{
    return *resolveAlias() == *symbol.resolveAlias();
}

bool SymbolVariable::operator==(Symbol& symbol)
{
    if(symbol.classType != CT_VAR)
        return false;
    SymbolVariable* s = static_cast<SymbolVariable*>(&symbol);
    return s->name == name && *s->type == *type;
}

bool SymbolTypePointer::operator==(Symbol& symbol)
{
    if(!symbol.isPointer())
        return false;
    SymbolTypePointer* s = static_cast<SymbolTypePointer*>(symbol.resolveAlias());
    return *s->type == *type;
}

bool SymbolTypeStruct::operator==(Symbol& symbol)
{
    if(!symbol.isStruct())
        return false;
    return *static_cast<SymbolTypeStruct*>(symbol.resolveAlias())->fields == *fields;
}

bool SymbolTypeFunction::operator==(Symbol& symbol)
{
    if(!symbol.isFunction())
        return false;
    SymbolTypeFunction* s = static_cast<SymbolTypeFunction*>(&symbol);
    return !(s->name != name || *s->type != *type || *s->args != *args);
}

int SymbolTypeAlias::size()
{
    return resolveAlias()->size();
}

int SymbolTypeArray::size()
{
    if(dynamic_cast<IntNode*>(length) == NULL)
        throw NotSupported("Array size can be only primary int expression");
    return static_cast<IntNode*>(length)->value * type->size();
}

int SymbolTypeStruct::size()
{
    if(_size != 0)
        return _size;
    int result = 0;
    for(unsigned int i = 0; i < fields->size(); i++)
        if((*fields)[i]->classType == CT_VAR)
        {
            static_cast<SymbolVariable*>((*fields)[i])->offset = result;
            result += static_cast<SymbolVariable*>((*fields)[i])->type->size();
        }
    return result;
}

void SymbolTypeFunction::localizeSymbols()
{
    for(unsigned int i = 0; i < body->_locals->size(); ++i)
        if((*body->_locals)[i]->classType == CT_VAR)
            static_cast<SymbolVariable*>((*body->_locals)[i])->varType = VT_LOCAL;
}

void SymbolTypeStruct::calculateOffsets()
{
    if(_calculated)
        return;
    size();
    _calculated = true;
}

}
