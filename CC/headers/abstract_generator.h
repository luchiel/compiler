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
    cAdd, cSub, cImul, cIdiv, cNeg, cInc, cDec, cCdq,
    cMov, cLea, 
    cOr, cXor, cAnd, cNot, cShl, cShr,
    cCmp, cTest,
    cJE, cJNE, cJL, cJG, cJLE, cJGE, cJZ, cJNZ,
    cSetE, cSetNE, cSetL, cSetG, cSetLE, cSetGE, cSetZ, cSetNZ,
    cLabel, cCall, cProc, cEndp, cRet
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

class RData
{
public:
    string name;
    string value;
    RData(const string& name_, const string& value_): name(name_), value(value_) {}
    void out();
};

class Argument
{
public:
    ArgType type;
    int offset;
    union
    {
        int constArg;
        AsmRegister regArg;
        string* sArg;
    }
    value;
    Argument(AsmRegister r_): type(atReg), offset(-1) { value.regArg = r_; }
    Argument(ArgType type_): type(type_), offset(-1) {}
    Argument(int v_): type(atConst), offset(-1) { value.constArg = v_; }
    Argument(string v_, ArgType type_): type(type_), offset(-1)
        { value.sArg = new string(v_); }
    Argument(string v_): offset(-1)
    {
        value.sArg = new string(v_);
        type = v_[0] == 'l' ? atLabel : atMem; //!f -> offset
    }
    Argument(const Argument& a): type(a.type), offset(a.offset), value(a.value) {}

    virtual void out();
};

class Offset
{
public:
    int offset;
    Offset(const int& o): offset(o) {}
    friend Argument operator+(Argument arg, Offset o);
    friend Argument operator-(Argument arg, Offset o);
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
    int currentParamSize;

    AbstractGenerator(): labelNum(0), currentParamSize(0) {}

    virtual void gen(Command com, Argument a1, Argument a2, Argument a3) {}
    virtual void gen(Command com, Argument a1, Argument a2) {}
    virtual void gen(Command com, Argument a1) {}
    virtual void gen(Command com) {}
    virtual void genLabel(Argument* a) {}
    virtual string addConstant(const string& s) { return ""; }

    void genIntCmp(const Command& cmpcmd);
    Argument label();
};

}

#endif
