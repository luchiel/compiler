#include "generator.h"

using namespace std;

namespace LuCCompiler
{

bool Generator::isAddSubWithImm(Command& com, int imm, bool noExactImm)
{
    return
           (com.command == cAdd || com.command == cSub)
        && (com.args[1]->type == atConst)
        && (noExactImm || com.args[1]->value.constArg == imm);
}

bool Generator::tryUniteAddSub(list<Command>::iterator& i)
{
    if(!isAddSubWithImm(*i))
        return false;
    list<Command>::iterator j(i);
    j--;
    if(!isAddSubWithImm(*j))
        return false;
    if(*i->args[0] != *j->args[0])
        return false;
    int iVal = i->args[1]->value.constArg;
    int jVal = j->args[1]->value.constArg;
    int sum = 0;
    sum += i->command == cAdd ? iVal : -iVal;
    sum += j->command == cAdd ? jVal : -jVal;
    codePart.erase(j);
    i->command = sum > 0 ? cAdd : cSub;
    i->args[1]->value.constArg = sum > 0 ? sum : -sum;
    return true;
}

bool Generator::tryAddSub0(list<Command>::iterator& i)
{
    if(isAddSubWithImm(*i, 0, false))
    {
        list<Command>::iterator j(i);
        i++;
        codePart.erase(j);
        return true;
    }
    return false;
}

bool Generator::tryAddSub1(list<Command>::iterator& i)
{
    if(isAddSubWithImm(*i, 1, false))
    {
        i->args.pop_back();
        i->command = i->command == cAdd ? cInc : cDec;
        return true;
    }
    return false;
}

bool Generator::tryMakeOpWithImm(list<Command>::iterator& i)
{
    list<Command>::iterator j(i);
    j--;
    if(j->command != cMov || j->args[1]->type != atConst)
        return false;
    switch(i->command)
    {
        case cAdd:
        case cSub:
        case cImul:
        case cOr:
        case cAnd:
        case cXor:
            if(*j->args[0] != *i->args[1])
                return false;
            delete i->args[1];
            i->args[1] = new Argument(*j->args[1]);
            return true;
        default:
            return false;
    }
}

bool Generator::tryRemoveUselessMovLea(list<Command>::iterator& i)
{
    if(i->command != cMov && i->command != cLea || i->args[0]->offset != -1)
        return false;
    list<Command>::iterator j(i);
    j++;
    bool found = false;
    for(; j != codePart.end() && !found; ++j)
        for(int k = j->args.size() - 1; k >= 0; --k)
            if(equalUpToOffset(*i->args[0], *j->args[k]))
            {
                if(k > 0)
                    return false;
                if(j->command != cPop && j->command != cMov && j->command != cLea)
                    return false;
                if(i->args[0]->offset != j->args[k]->offset)
                    return false;
                found = true;
                break;
            }
            else if
            (
                j->command == cIdiv &&
                i->args[0]->type == atReg &&
                i->args[0]->value.regArg == rEAX
            )
                return false;
    j = i;
    i++;
    codePart.erase(j);
    return true;
}

bool Generator::tryUniteLabels(list<Command>::iterator& i)
{
    list<Command>::iterator j(i);
    j--; //control begin outside
    if(i->command != cLabel || j->command != cLabel || *j->args[0]->value.sArg == "main")
        return false;
    for(list<Command>::iterator k = codePart.begin(); k != codePart.end(); ++k)
    {
        if
        (
            k->args.size() > 0 && k->args[0]->type == atLabel &&
            *k->args[0]->value.sArg == *j->args[0]->value.sArg && k != j
        )
            k->args[0]->value.sArg = i->args[0]->value.sArg;
    }
    delete j->args[0]->value.sArg;
    codePart.erase(j);
    i++;
    return true;
}

bool Generator::tryUnitePushPop(list<Command>::iterator& i)
{
    list<Command>::iterator j(i);
    j--;
    if(j->command != cPush && i->command != cPush)
        return false;
    if(j->command != cPop && i->command != cPop)
        return false;

    if(*i->args[0] == *j->args[0])
    {
        list<Command>::iterator k(i);
        i++;
        codePart.erase(j);
        codePart.erase(k);
    }
    else if(j->command == cPush && i->command == cPop)
    {
        i->command = cMov;
        i->args.push_back(j->args[0]);
        codePart.erase(j);
    }
    else
        return false;
    return true;
}

bool Generator::tryMovMov(list<Command>::iterator& i)
{
    list<Command>::iterator j(i);
    j--;
    if(i->command != cMov || j->command != cMov)
        return false;
    if(*i->args[1] == *j->args[0])
    //mov eax,1; mov ebx,eax -> mov eax,1; mov ebx,1
    {
        delete i->args[1];
        i->args[1] = new Argument(*j->args[1]);
        return true;
    }
    return false;
}

bool Generator::tryMovSelf(list<Command>::iterator& i)
{
    if(i->command == cMov && *i->args[0] == *i->args[1])
    {
        list<Command>::iterator j(i);
        i++;
        codePart.erase(j);
        return true;
    }
    return false;
}

void Generator::swapListItems(list<Command>::iterator& i, list<Command>::iterator& j)
{
    list<Command>::iterator k(i);
    k++;
    codePart.insert(k, *(new Command(*j)));
    codePart.erase(j);
}

bool Generator::tryLiftPop(list<Command>::iterator& i)
{
    if(i->command != cPop)
        return false;
    list<Command>::iterator j(i);
    j--;
    switch(j->command)
    {
        case cCdq:
            swapListItems(i, j);
            break;
        //case cIdiv:
        //    break; just not done
        case cInc:
        case cDec:
        case cNeg:
        case cAdd:
        case cSub:
        case cImul:
        case cOr:
        case cXor:
        case cAnd:
        case cNot:
        case cMov:
        case cLea:
        case cShl:
        case cShr:
            if(*i->args[0] == *j->args[0])
                codePart.erase(j);
            else
            {
                if(i->args[0]->offset != -1)
                    return false;
                for(vector<Argument*>::iterator k = j->args.begin(); k != j->args.end(); ++k)
                    if
                    (
                          (*k)->offset != -1
                        || **k == *i->args[0]
                        || (*k)->type == atReg && (*k)->value.regArg == rESP
                    )
                        return false;
                swapListItems(i, j);
            }
            break;
        default:
            //cLabel, cProc, cRet, cCall, cEndp, cPop, cPush,
            //cJE, cJNE, cJL, cJG, cJLE, cJGE, cJZ, cJNZ, cJmp,
            //to upper 1: pop may be unexpected, complicated
            //to lower 1: pop does not change flags, so probably allowed
            //cSetE, cSetNE, cSetL, cSetG, cSetLE, cSetGE, cSetZ, cSetNZ,
            //cTest, cCmp //always followed by jmp/set
            return false;
    }
    return true;
}

bool Generator::tryLeaMov(list<Command>::iterator& i)
{
    if(i->command != cMov || i->args[1]->offset == -1)
        return false;
    list<Command>::iterator j(i);
    j--;
    if
    (
           j->command != cLea
        || !equalUpToOffset(*j->args[0], *i->args[1])
        || j->args[0]->offset != -1
        || j->args[1]->offset == -1
    )
        return false;
    int iOffset = i->args[1]->offset;
    delete i->args[1];
    i->args[1] = new Argument(*j->args[1]);
    i->args[1]->offset += iOffset;
    if(*i->args[0] == *j->args[0])
        codePart.erase(j);
    return true;
}

bool Generator::tryJmpLabel(list<Command>::iterator& i)
{
    if(i->command != cJmp)
        return false;
    list<Command>::iterator j(i);
    j++;
    if(j == codePart.end() || j->command == cLabel)
        return false;
    while(j != codePart.end() && j->command != cLabel)
    {
        list<Command>::iterator k(j);
        j++;
        codePart.erase(k);
    }
    return true;
}

bool Generator::tryLeaPushIncDec(list<Command>::iterator& i)
{
    if(i->command != cPush && i->command != cInc && i->command != cDec)
        return false;
    list<Command>::iterator j(i);
    j--;
    if
    (
           j->command != cLea
        || j->args[0]->offset != -1
        || !equalUpToOffset(*j->args[0], *i->args[0])
        || i->args[0]->offset != -1 && i->args[0]->offset != 0
        || i->args[0]->offset == -1 && j->args[1]->offset != 0
    )
        return false;

    bool removeOffset = i->args[0]->offset == -1;
    delete i->args[0];
    i->args[0] = new Argument(*j->args[1]);
    if(removeOffset)
        i->args[0]->offset = -1;

    return true;
}

bool Generator::tryMovTest(list<Command>::iterator& i)
{
    if(i->command != cTest)
        return false;
    list<Command>::iterator j(i);
    j--;
    if(j->command != cMov || j->args[1]->type != atReg || *i->args[0] != *j->args[0])
        return false;
    delete i->args[0];
    delete i->args[1];
    i->args[0] = new Argument(*j->args[1]);
    i->args[1] = new Argument(*j->args[1]);
    return true;
}

bool Generator::trySetTestJmp(list<Command>::iterator& i)
{
    if(i->command != cJZ && i->command != cJNZ)
        return false;
    list<Command>::iterator j(i); j--;
    if(j->command != cTest || j == codePart.begin())
        return false;
    list<Command>::iterator k(j); k--;
    switch(k->command)
    {
        case cSetE:
        case cSetNE:
        case cSetL:
        case cSetG:
        case cSetLE:
        case cSetGE:
        case cSetZ:
        case cSetNZ:
            i->command = i->command == cJNZ ? k->corresponding() : k->reverse();
            codePart.erase(j);
            codePart.erase(k);
            return true;
        default:
            return false;
    }
}

}
