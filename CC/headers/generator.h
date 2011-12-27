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
    vector<RData*> rdataPart;
    vector<Command> codePart;
    void genData(const SymbolTable& t);
    void genCode(SymbolTypeFunction* f);

public:
    Generator(SymbolTable* symbols_):
        AbstractGenerator(), symbols(symbols_), main(NULL) {}
    void generate();
    void out();

    virtual void gen(Command com, Argument a1, Argument a2, Argument a3);
    virtual void gen(Command com, Argument a1, Argument a2);
    virtual void gen(Command com, Argument a1);
    virtual void gen(Command com);
    virtual void genLabel(Argument* a);
    virtual string addConstant(const string& s);
};

}

#endif
