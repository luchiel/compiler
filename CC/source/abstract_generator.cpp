#include "abstract_generator.h"

namespace LuCCompiler
{

void AbstractGenerator::genIntCmp(const Command& cmpcmd)
{
    gen(cXor, rECX, rECX);
    gen(cCmp, rEAX, rEBX);
    gen(cmpcmd, rCL);
    gen(cMov, rEAX, rECX);
}

void Command::out()
{
    //
}

void Data::out()
{
    cout << name << " dd ";
    if(size == 1)
        cout << "0\n";
    else
        cout << size << " dup(0)\n";
}

void Argument::out()
{
    //
}

}
