#ifndef ABSTRACT_GENERATOR_H
#define ABSTRACT_GENERATOR_H

#include <vector>
#include <string>

using namespace std;

namespace LuCCompiler
{

enum Register { rEAX, rEBX, rECX, rEDX, rESI, rEDI, rESP, rEBP, rCL };
enum AsmCommand
{
    cPush, cPop,
    cAdd, cSub, cImul, cIdiv,
    cMov, cLea, 
    cOr, cXor, cAnd, cNot, cShl, cShr,
    cCmp, cTest,
    cJE, cJNE, cJL, cJG, cJLE, cLGE,
    cSetE, cSetNE, cSetL, cSetG, cSetLE, cSetGE,
};

enum ArgType { atReg, atMem, atConst, atLabel };

class Data
{
public:
    string name;
    int size;
    Data(const string& name_, const int size_): name(name_), size(size_) {}
    void out();
};

class Argument
{
public:
    ArgType type;
    union
    {
        Register regArg;
        void* memArg;
        void* constArg;
        int labelArg;
    }
    value;
    Argument() {}
    Argument(Register r_): type(atReg) { value.regArg = r_; }
    Argument(ArgType type_, int v_): type(type_)
    {
        switch(type)
        {
            atLabel: value.labelArg = v_; break;
            atConst: value.constArg = v_; break;
        }
    }
    void out();
};

class Command
{
public:
    AsmCommand command;
    vector<Argument*> args;
    Command(const AsmCommand& c_): command(c_) {}
    void out();
};

class AbstractGenerator
{
protected:
    int labelNum;

public:
    AbstractGenerator() {}

    virtual void gen(const Command& com, Argument a1, Argument a2, Argument a3) {}
    virtual void gen(const Command& com, Argument a1, Argument a2) {}
    virtual void gen(const Command& com, Argument a1) {}

    void genIntCmp(const Command& cmpcmd);
    void genLabel(const Argument& a) {}
    Argument label() { labelNum++; return new Argument(atLabel, labelNum); }
};

}

#endif
