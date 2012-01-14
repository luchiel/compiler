#include <iostream>
#include <map>
#include <cassert>
#include "exception.h"
#include "abstract_generator.h"

namespace LuCCompiler
{

map<AsmRegister, string*> regNames;
map<AsmCommand, string*> cmdNames;

AsmCommand Command::corresponding()
{
    switch(command)
    {
        case cSetE:  return cJE;
        case cSetNE: return cJNE;
        case cSetL:  return cJL;
        case cSetG:  return cJG;
        case cSetLE: return cJLE;
        case cSetGE: return cJGE;
        case cSetZ:  return cJZ;
        case cSetNZ: return cJNZ;
        default: assert(false);
    }
}

AsmCommand Command::reverse()
{
    switch(command)
    {
        case cSetE:  return cJNE;
        case cSetNE: return cJE;
        case cSetL:  return cJGE;
        case cSetG:  return cJLE;
        case cSetLE: return cJG;
        case cSetGE: return cJL;
        case cSetZ:  return cJNZ;
        case cSetNZ: return cJZ;
        default: assert(false);
    }
}

void AbstractGenerator::genDoublePush(AsmRegister reg)
{
    gen(cSub, rESP, 8);
    gen(cMovsd, rESP + Offset(0) + swQword, reg);
}

void AbstractGenerator::genDoublePop(AsmRegister reg)
{
    gen(cMovsd, reg, rESP + Offset(0) + swQword);
    gen(cAdd, rESP, 8);
}

void AbstractGenerator::genIntCmp(const Command& cmpcmd)
{
    gen(cXor, rECX, rECX);
    gen(cCmp, rEAX, rEBX);
    gen(cmpcmd, rCL);
    gen(cMov, rEAX, rECX);
}

void AbstractGenerator::genDoubleCmp(const Command& cmpcmd)
{
    gen(cXor, rECX, rECX);
    gen(cComisd, rXMM0, rXMM1);
    gen(cmpcmd, rCL);
    gen(cMov, rEAX, rECX);
}

void AbstractGenerator::pushJumpLabels(Argument* breakA, Argument* continueA)
{
    breakStack.push_back(breakA);
    continueStack.push_back(continueA);
}

void AbstractGenerator::popJumpLabels()
{
    breakStack.pop_back();
    continueStack.pop_back();
}

void Command::out()
{
    if(command == cLabel)
    {
        args[0]->out();
        cout << ":\n";
        return;
    }
    else if(command == cRet)
    {
        cout << "\tret\n\n";
        return;
    }
    if(cmdNames.size() == 0)
    {
        cmdNames[cPush] = new string("push");
        cmdNames[cPop] = new string("pop");
        cmdNames[cAdd] = new string("add");
        cmdNames[cSub] = new string("sub");
        cmdNames[cImul] = new string("imul");
        cmdNames[cIdiv] = new string("idiv");
        cmdNames[cNeg] = new string("neg");
        cmdNames[cInc] = new string("inc");
        cmdNames[cDec] = new string("dec");
        cmdNames[cMov] = new string("mov");
        cmdNames[cLea] = new string("lea");
        cmdNames[cOr] = new string("or");
        cmdNames[cXor] = new string("xor");
        cmdNames[cAnd] = new string("and");
        cmdNames[cNot] = new string("not");
        cmdNames[cShl] = new string("shl");
        cmdNames[cShr] = new string("shr");
        cmdNames[cSar] = new string("sar");
        cmdNames[cLabel] = new string(":");
        cmdNames[cCall] = new string("call");
        //cmdNames[cProc] = new string("proc");
        //cmdNames[cEndp] = new string("endp");
        //cmdNames[cRet] = new string("ret");
        cmdNames[cCdq] = new string("cdq");
        cmdNames[cTest] = new string("test");
        cmdNames[cCmp] = new string("cmp");

        cmdNames[cSetE] = new string("sete");
        cmdNames[cSetNE] = new string("setne");
        cmdNames[cSetL] = new string("setl");
        cmdNames[cSetG] = new string("setg");
        cmdNames[cSetLE] = new string("setle");
        cmdNames[cSetGE] = new string("setge");
        cmdNames[cSetZ] = new string("setz");
        cmdNames[cSetNZ] = new string("setnz");

        cmdNames[cJE] = new string("je");
        cmdNames[cJNE] = new string("jne");
        cmdNames[cJL] = new string("jl");
        cmdNames[cJG] = new string("jg");
        cmdNames[cJLE] = new string("jle");
        cmdNames[cJGE] = new string("jge");
        cmdNames[cJZ] = new string("jz");
        cmdNames[cJNZ] = new string("jnz");
        cmdNames[cJmp] = new string("jmp");

        cmdNames[cMovsd] = new string("movsd");
        cmdNames[cCmpsd] = new string("cmpsd");
        cmdNames[cComisd] = new string("comisd");
        cmdNames[cAddsd] = new string("addsd");
        cmdNames[cSubsd] = new string("subsd");
        cmdNames[cMulsd] = new string("mulsd");
        cmdNames[cDivsd] = new string("divsd");
    }
    cout << '\t' << *cmdNames[command] << '\t';
    for(unsigned int i = 0; i < args.size(); ++i)
    {
        args[i]->out();
        if(i != args.size() - 1)
            cout << ',';
    }
    cout << '\n';
}

bool Command::operator==(Command c)
{
    bool result = command == c.command && args.size() == c.args.size();
    vector<Argument*>::iterator i = args.begin();
    vector<Argument*>::iterator j = c.args.begin();
    while(result && i != args.end())
    {
        result = **i == **j;
        ++i;
        ++j;
    }
    return result;
}

void Data::out()
{
    cout << name << " dd ";
    if(size == 1)
        cout << init.intInit << "\n";
    else
        cout << size << " dup(0)\n";
}

void RData::out()
{
    unsigned int i = 0;
    unsigned int start = 0;
    cout << name << " db ";
    while(i < value.size())
    {
        switch(value[i])
        {
            case '\n':
            case '\'':
            case '\"':
            case '\?':
            case '\\':
            case '\a':
            case '\b':
            case '\f':
            case '\r':
            case '\v':
                if(i - start > 0)
                    cout << "\"" << value.substr(start, i - start) << "\",";
                cout << int(value[i]) << ",";
                start = i + 1;
                break;
        }
        i++;
    }
    if(i - start > 0)
        cout << "\"" << value.substr(start, i - start) << "\",";
    cout << "0\n";
}

void FData::out()
{
    cout << name << " dq " << showpoint << value << "\n";
}

Argument operator+(Argument arg, Offset o)
{
    arg.offset = o.offset;
    return arg;
}

Argument operator-(Argument arg, Offset o)
{
    arg.offset = -o.offset;
    return arg;
}

Argument* AbstractGenerator::label()
{
    labelNum++;
    return new Argument("l" + itostr(labelNum));
}

void Argument::out()
{
    if(regNames.size() == 0)
    {
        regNames[rEAX] = new string("eax");
        regNames[rEBX] = new string("ebx");
        regNames[rECX] = new string("ecx");
        regNames[rEDX] = new string("edx");
        regNames[rESI] = new string("esi");
        regNames[rEDI] = new string("edi");
        regNames[rESP] = new string("esp");
        regNames[rEBP] = new string("ebp");
        regNames[rCL] = new string("cl");

        regNames[rXMM0] = new string("xmm0");
        regNames[rXMM1] = new string("xmm1");
    }

    switch(type)
    {
        case atLabel: cout << *value.sArg; break;
        case atConst: cout << value.constArg; break;
        default:
            if(offset != -1)
                cout << (size == swDword ? "dword" : "qword") << " [";
            
            if(type == atReg)
                cout << *regNames[value.regArg];
            else
            {
                if((*value.sArg) == "f_printf")
                    cout << '[' << *value.sArg << ']';
                else
                    cout << *value.sArg;
            }
            if(offset != -1)
            {
                if(offset != 0)
                    cout << (offset > 0 ? "+" : "") << offset;
                cout << ']';
            }
    }
}

Argument& Argument::operator+(SizeInWords size_)
{
    size = size_;
    return *this;
}

bool Argument::operator==(const Argument& a)
{
    return equalUpToOffset(a, *this) && offset == a.offset;
}

bool equalUpToOffset(const Argument& a, const Argument& b)
{
    if(b.type != a.type)
        return false;
    switch(b.type)
    {
        case atReg:
            return b.value.regArg == a.value.regArg ||
                   (a.value.regArg == rECX || a.value.regArg == rCL)
                && (b.value.regArg == rECX || b.value.regArg == rCL);
        case atConst: return b.value.constArg == a.value.constArg;
        default: return *b.value.sArg == *a.value.sArg;
    }
}

}
