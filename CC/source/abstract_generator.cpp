#include <iostream>
#include <map>
#include "exception.h"
#include "abstract_generator.h"

namespace LuCCompiler
{

map<AsmRegister, string*> regNames;
map<AsmCommand, string*> cmdNames;

void AbstractGenerator::genIntCmp(const Command& cmpcmd)
{
    gen(cXor, rECX, rECX);
    gen(cCmp, rEAX, rEBX);
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
    else if(command == cProc || command == cEndp)
    {
        if(command == cProc) cout << '\n';
        args[0]->out();
        cout << (command == cProc ? " proc\n" : " endp\n");
        if(command == cEndp) cout << '\n';
        return;
    }
    //map not required. Just case with word setters?
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
        cmdNames[cLabel] = new string(":");
        cmdNames[cCall] = new string("call");
        cmdNames[cProc] = new string("proc");
        cmdNames[cEndp] = new string("endp");
        cmdNames[cRet] = new string("ret");
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

        cmdNames[cMovss] = new string("movss");
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
    cout << name << " db \"" << value << "\",0\n";
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
    switch(type)
    {
        case atReg:
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
            }
            if(offset != -1) cout << "dword ptr [";
            cout << *regNames[value.regArg];
            if(offset != -1)
            {
                if(offset != 0)
                    cout << (offset > 0 ? "+" : "") << offset;
                cout << ']';
            }
            return;
        case atLabel:
            cout << *value.sArg;
            return;
        case atConst:
            cout << value.constArg;
            break;
        case atConstF:
            cout << value.constArgF;
            break;
        case atMem:
            if((*value.sArg)[0] != 'f' && *value.sArg != "crt_printf")
                cout << "offset ";
            cout << *value.sArg;
            break;
    }
}

bool Argument::operator==(const Argument& a)
{
    if(type != a.type || offset != a.offset)
        return false;
    switch(type)
    {
        case atReg: return value.regArg == a.value.regArg;
        case atConst: return value.constArg == a.value.constArg;
        case atConstF: return value.constArgF == a.value.constArgF;
        default: return *value.sArg == *a.value.sArg;
    }
}

}
