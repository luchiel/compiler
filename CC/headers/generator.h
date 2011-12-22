#ifndef GENERATOR_H
#define GENERATOR_H

#include <vector>
#include <string>
#include "symbol.h"
#include "complex_symbol.h"
#include "symbol_table.h"

using namespace std;

namespace LuCCompiler
{

class Data
{
public:
    string name;
    int size;
    Data(const string& name_, const int size_): name(name_), size(size_) {}
    void out();
};

//class Argument {};

//class Registry: Argument {};
//class Constant: Argument {};

class Command
{
public:
    string command;
    //vector<Argument&> args;
    Command(const string& c_): command(c_) {}
    void out();
};

class Generator
{
private:
    SymbolTable* symbols;
    SymbolTypeFunction* main;
    vector<Data*> dataPart;
    vector<Command*> codePart;
    void generateData(const SymbolTable& t, const string& prefix = "");
    void generateCode(SymbolTypeFunction* f);
public:
    Generator(SymbolTable* symbols_): symbols(symbols_), main(NULL) {}
    void generate();
    void out();
};

}

#endif
