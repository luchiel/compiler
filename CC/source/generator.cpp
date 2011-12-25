#include <iostream>
#include "generator.h"
#include "symbol.h"
#include "complex_symbol.h"
#include "exception.h"
#include "symbol_table.h"
#include "declaration.h"

namespace LuCCompiler
{

void Generator::genCode(SymbolTypeFunction* f)
{
    if(f->body == NULL)
        throw NoFunctionDefinition(f->name);
    if(main == NULL && f->name == "main")
    {
        main = f;
        return;
    }
    else if(f->name == "main")
    {
        //
        return;
    }
    //gen();
    //gen code
}

void Generator::genData(const SymbolTable& t, const string& prefix)
{
    for(unsigned int j = 0; j < t.size(); ++j)
    {
        if(t[j]->classType != CT_VAR)
        {
            if(t[j]->isFunction())
                genCode(static_cast<SymbolTypeFunction*>(t[j]));
            continue;
        }
        SymbolVariable* var = static_cast<SymbolVariable*>(t[j]);
        Data* d = new Data(prefix + var->name, var->type->size());
        dataPart.push_back(d);

        if(dynamic_cast<InitializerList*>(var->initializer) == NULL)
        {/*do initialize*/}//else currently not supported
    }
}

void Generator::out()
{
    cout << ".586\n.model flat, stdcall\n\nExitProcess proto :DWORD\n\n";
    cout << ".data\n";
    for(unsigned int i = 0; i < dataPart.size(); ++i)
        dataPart[i]->out();
    cout << "\n.code\n";
    for(unsigned int i = 0; i < codePart.size(); ++i)
        codePart[i]->out();
}

void Generator::generate()
{
    genData(*symbols);
    if(main == NULL)
        throw NoFunctionDefinition("main");
    genCode(main);
}

}
