#include <iostream>
#include <map>
#include "exception.h"
#include "abstract_generator.h"

namespace LuCCompiler
{

map<AsmRegister, string*> regNames;

void AbstractGenerator::genIntCmp(const Command& cmpcmd)
{
    gen(cXor, rECX, rECX);
    gen(cCmp, rEAX, rEBX);
    gen(cmpcmd, rCL);
    gen(cMov, rEAX, rECX);
}

void Command::out()
{
    if(command == cCol)
    {
        args[0]->out();
        cout << ":\n";
        return;
    }
}

void Data::out()
{
    cout << name << " dd ";
    if(size == 1)
        cout << "0\n";
    else
        cout << size << " dup(0)\n";
}

Argument& Argument::operator+(const int& arg)
{
    offset = arg;
    return *this;
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
            cout << *regNames[value.regArg];
            return;
        case atLabel:
            cout << *value.labelArg;
            return;
        case atConst:
            //
            break;
    }
}

}
