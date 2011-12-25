#ifndef ABSTRACT_GENERATOR_H
#define ABSTRACT_GENERATOR_H

#include <vector>
#include <string>

using namespace std;

namespace LuCCompiler
{

enum AsmRegister { rEAX, rEBX, rECX, rEDX, rESI, rEDI, rESP, rEBP, rCL };
enum AsmCommand
{
    cPush, cPop,
    cAdd, cSub, cImul, cIdiv, cNeg, cInc, cDec,
    cMov, cLea, 
    cOr, cXor, cAnd, cNot, cShl, cShr,
    cCmp, cTest,
    cJE, cJNE, cJL, cJG, cJLE, cLGE, cJZ, cJNZ,
    cSetE, cSetNE, cSetL, cSetG, cSetLE, cSetGE, cSetZ, cSetNZ,
    cCol,
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
    int offset;
    union
    {
        void* memArg;
        int constArg;
        AsmRegister regArg;
        string* labelArg;
    }
    value;
    Argument(ArgType type_): type(type_) {}
    Argument(int v_) { value.constArg = v_; }
    Argument(AsmRegister r_): type(atReg), offset(-1) { value.regArg = r_; }
    Argument(string v_): type(atLabel), offset(-1) { value.labelArg = new string(v_); }
    Argument(const Argument& a): type(a.type), offset(a.offset), value(a.value) {}

    virtual void out();
    Argument& operator+(const int& arg);
};

class Command
{
public:
    AsmCommand command;
    vector<Argument*> args;
    Command(AsmCommand c_): command(c_) {}
    void out();
};

class AbstractGenerator
{
protected:
    int labelNum;

public:
    AbstractGenerator(): labelNum(0) {}

    virtual void gen(Command com, Argument a1, Argument a2, Argument a3) {}
    virtual void gen(Command com, Argument a1, Argument a2) {}
    virtual void gen(Command com, Argument a1) {}
    virtual void genLabel(Argument* a) {}

    void genIntCmp(const Command& cmpcmd);
    Argument label();
};

}

#endif
