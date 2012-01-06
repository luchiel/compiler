#ifndef ABSTRACT_GENERATOR_H
#define ABSTRACT_GENERATOR_H

#include <vector>
#include <string>

using namespace std;

namespace LuCCompiler
{

enum AsmRegister
{
    rEAX, rEBX, rECX, rEDX, rESI, rEDI, rESP, rEBP, rCL,
    rXMM0, rXMM1, rXMM2, rXMM3,
};

enum AsmCommand
{
    cPush, cPop,
    cAdd, cSub, cImul, cIdiv, cNeg, cInc, cDec, cCdq,
    cMov, cLea, 
    cOr, cXor, cAnd, cNot, cShl, cShr,
    cCmp, cTest,
    cJE, cJNE, cJL, cJG, cJLE, cJGE, cJZ, cJNZ, cJmp,
    cSetE, cSetNE, cSetL, cSetG, cSetLE, cSetGE, cSetZ, cSetNZ,
    cLabel, cCall, cProc, cEndp, cRet,

    cMovss, cAddss, cSubss, cMulss, cDivss, cCmpss, //cComiss
    cCvtsi2ss, cCvtss2si,
};

enum ArgType { atReg, atMem, atConst, atLabel, atConstF };

class Data
{
public:
    string name;
    int size;
    union
    {
        int intInit;
        float floatInit;
    }
    init;
    Data(const string& name_, const int size_): name(name_), size(size_) { init.intInit = 0; }
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
        float constArgF;
        AsmRegister regArg;
        string* sArg;
    }
    value;
    Argument(AsmRegister r_): type(atReg), offset(-1) { value.regArg = r_; }
    Argument(ArgType type_): type(type_), offset(-1) {}
    Argument(int v_): type(atConst), offset(-1) { value.constArg = v_; }
    Argument(unsigned int v_): type(atConst), offset(-1) { value.constArg = v_; }
    Argument(float v_): type(atConstF), offset(-1) { value.constArgF = v_; }

    Argument(string v_, ArgType type_): type(type_), offset(-1)
        { value.sArg = new string(v_); }
    Argument(string v_): offset(-1)
    {
        value.sArg = new string(v_);
        type = v_[0] == 'l' ? atLabel : atMem; //!f -> offset
    }
    Argument(const Argument& a): type(a.type), offset(a.offset), value(a.value) {}

    virtual void out();
    bool operator==(const Argument& a);
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
    bool operator==(Command c);
};

class AbstractGenerator
{
protected:
    int labelNum;
    vector<Argument*> breakStack;
    vector<Argument*> continueStack;
    vector<Argument*> returnStack;

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
    Argument* label();

    void pushJumpLabels(Argument* breakA, Argument* continueA);
    void popJumpLabels();

    void genReturnLabel() { returnStack.push_back(label()); }
    void popReturnLabel() { returnStack.pop_back(); }

    Argument breakLabel() { return *breakStack[breakStack.size() - 1]; }
    Argument continueLabel() { return *continueStack[continueStack.size() - 1]; }
    Argument returnLabel() { return *returnStack[returnStack.size() - 1]; }
};

}

#endif
