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
    //cCmp, cTest,
    //cJE, cJNE, cJL, cJG, cJLE, cLGE, cJZ, cJNZ,
    //cSetE, cSetNE, cSetL, cSetG, cSetLE, cSetGE, cSetZ, cSetNZ,
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

void Data::out()
{
    cout << name << " dd ";
    if(size == 1)
        cout << "0\n";
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

Argument AbstractGenerator::label()
{
    labelNum++;
    return Argument(itostr(labelNum));
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
            }
            if(offset != -1) cout << '[';
            cout << *regNames[value.regArg];
            if(offset != -1) cout << '+' << offset << ']';
            return;
        case atLabel:
            cout << *value.sArg;
            return;
        case atConst:
            cout << value.constArg;
            break;
        case atMem:
            if((*value.sArg)[0] != 'f' && *value.sArg != "crt_printf")
                cout << "offset ";
            cout << *value.sArg;
            break;
    }
}

}
