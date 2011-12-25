#ifndef GENERATOR_H
#define GENERATOR_H

#include <vector>
#include <string>
#include "symbol.h"
#include "complex_symbol.h"
#include "symbol_table.h"
#include "abstract_generator.h"

using namespace std;

namespace LuCCompiler
{

class Generator: public AbstractGenerator
{
private:
    SymbolTable* symbols;
    SymbolTypeFunction* main;
    vector<Data*> dataPart;
    vector<Command*> codePart;
    void genData(const SymbolTable& t, const string& prefix = "");
    void genCode(SymbolTypeFunction* f);

public:
    Generator(SymbolTable* symbols_):
        AbstractGenerator(), labelNum(0), symbols(symbols_), main(NULL) {}
    void generate();
    void out();

    virtual void gen(const Command& com, Argument a1, Argument a2, Argument a3) {}
    virtual void gen(const Command& com, Argument a1, Argument a2) {}
    virtual void gen(const Command& com, Argument a1) {}
};

}

#endif
