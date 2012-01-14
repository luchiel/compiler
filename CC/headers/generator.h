#ifndef GENERATOR_H
#define GENERATOR_H

#include <vector>
#include <list>
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
    vector<FData*> fdataPart;
    list<Command> codePart;

    bool modified;

    void genData(const SymbolTable& t);
    void genCode(SymbolTypeFunction* f);

    void optimize();

    bool isPowerOf2(int a);
    int log2(int a);
    void performConstConditionChanges(
        list<Command>::iterator& k,
        list<Command>::iterator& j,
        list<Command>::iterator& i,
        bool condition
    );

    bool tryAddSub0(list<Command>::iterator& i);
    bool tryAddSub1(list<Command>::iterator& i);
    bool tryMov0(list<Command>::iterator& i);
    bool tryUniteLabels(list<Command>::iterator& i);
    bool tryUnitePushPop(list<Command>::iterator& i);
    bool tryMovMov(list<Command>::iterator& i);
    bool tryMovSelf(list<Command>::iterator& i);
    bool tryLiftPop(list<Command>::iterator& i);
    void swapListItems(list<Command>::iterator& i, list<Command>::iterator& j);
    bool isAddSubWithImm(Command& com, int imm = 0, bool noExactImm = true);
    bool tryMakeOpWithImm(list<Command>::iterator& i);
    bool tryRemoveUselessMovLeaXor(list<Command>::iterator& i);
    bool tryUniteAddSub(list<Command>::iterator& i);
    bool tryLeaMov(list<Command>::iterator& i);
    bool tryOtherLeaMov(list<Command>::iterator& i);
    bool tryJmpLabel(list<Command>::iterator& i);
    bool tryLeaPushIncDec(list<Command>::iterator& iterator);
    bool tryMovTest(list<Command>::iterator& i);
    bool trySetTestJmp(list<Command>::iterator& i);
    bool tryLabelJmp(list<Command>::iterator& i);
    bool tryImulWithImm(list<Command>::iterator& i);
    bool tryIdivWithImm(list<Command>::iterator& i);
    bool tryConstCondition(list<Command>::iterator& i);
    bool tryUniteDoubleMovMov(list<Command>::iterator& i);
    bool tryUniteDoublePushPop(list<Command>::iterator& i);
    bool tryLiftSubESPImm(list<Command>::iterator& i);

public:
    Generator(SymbolTable* symbols_):
        AbstractGenerator(), symbols(symbols_), main(NULL), modified(true) {}
    void generate(bool doOptimize = false);
    void out();

    virtual void gen(Command com, Argument a1, Argument a2, Argument a3);
    virtual void gen(Command com, Argument a1, Argument a2);
    virtual void gen(Command com, Argument a1);
    virtual void gen(Command com);
    virtual void genLabel(Argument* a);
    virtual string addConstant(const string& s);
    virtual string addDoubleConstant(const double& f);
};

}

#endif
