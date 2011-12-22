#include <iostream>
#include "generator.h"
#include "symbol.h"
#include "complex_symbol.h"
#include "exception.h"
#include "symbol_table.h"
#include "declaration.h"

namespace LuCCompiler
{

void Generator::generateCode(SymbolTypeFunction* f)
{
    if(f->body == NULL)
        throw NoFunctionDefinition(f->name);
    if(main == NULL && f->name == "main")
    {
        main = f;
        return;
    }
    //gen code
}

void Generator::generateData(const SymbolTable& t, const string& prefix)
{
    for(unsigned int j = 0; j < t.size(); ++j)
    {
        if(t[j]->classType != CT_VAR)
        {
            if(t[j]->isFunction())
                generateCode(static_cast<SymbolTypeFunction*>(t[j]));
            continue;
        }
        SymbolVariable* var = static_cast<SymbolVariable*>(t[j]);
        Data* d = new Data(prefix + var->name, var->type->size());
        dataPart.push_back(d);

        if(dynamic_cast<InitializerList*>(var->initializer) == NULL)
        {/*do initialize*/}//else currently not supported
    }
}

void Command::out()
{
    //
}

void Data::out()
{
    cout << name << " dd ";
    if(size == 1)
        cout << "0\n";
    else
        cout << size << " dup(0)\n";
}

void Generator::out()
{
    cout << ".586\n.model flat, stdcall\nExitProcess proto :DWORD\n";
    cout << ".data\n";
    for(unsigned int i = 0; i < dataPart.size(); ++i)
        dataPart[i]->out();
    cout << ".code\n";
    for(unsigned int i = 0; i < codePart.size(); ++i)
        codePart[i]->out();
}

void Generator::generate()
{
    generateData(*symbols);
    if(main == NULL)
        throw NoFunctionDefinition("main");
    generateCode(main);
}

}
