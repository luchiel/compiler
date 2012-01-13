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

bool Generator::tryMov0(list<Command>::iterator& i)
{
    if
    (
           i->command != cMov
        || i->args[1]->type != atConst
        || i->args[1]->value.constArg != 0
        || i->args[0]->type != atReg
        || i->args[0]->offset != -1
    )
        return false;
    i->command = cXor;
    delete i->args[1];
    i->args[1] = new Argument(*i->args[0]);
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
        if
        (
            i->args[0]->type == atReg && i->args[0]->offset != -1 &&
            (
                j->args[1]->type == atMem ||
                j->args[1]->type == atReg && j->args[1]->offset != -1
            )
        )
            return false;
        delete i->args[1];
        i->args[1] = new Argument(*j->args[1]);
        return true;
    }
    return false;
}

bool Generator::tryMakeOpWithImm(list<Command>::iterator& i)
{
    list<Command>::iterator j(i); j--;
    if(j->command != cMov)
        return false;
    switch(i->command)
    {
        case cAdd:
        case cSub:
        case cImul:
        case cOr:
        case cAnd:
        case cXor:
            if(j->args[1]->type == atConst && *j->args[0] == *i->args[1])
            {
                delete i->args[1];
                i->args[1] = new Argument(*j->args[1]);
                return true;
            }
            if(i->command == cImul || !equalUpToOffset(*j->args[0], *i->args[0]))
                return false;
            if
            (
                i->args[0]->offset != -1 &&
                j->args[0]->offset == -1 &&
                j->args[1]->offset == -1 &&
                j->args[1]->type != atMem
            )
            {
                //mov eax,ebp
                //add dword ptr [eax],5
                int iOffset = i->args[0]->offset;
                delete i->args[0];
                i->args[0] = new Argument(*j->args[1]);
                i->args[0]->offset = iOffset;
                return true;
            }
            return false;
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
                if
                (
                       k > 0
                    || j->command != cPop && j->command != cMov && j->command != cLea
                    || i->args[0]->offset != j->args[k]->offset
                )
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
    list<Command>::iterator j(i); j--;
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
    list<Command>::iterator j(i); j--;
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

bool Generator::tryOtherLeaMov(list<Command>::iterator& i)
{
    if
    (
           i->command != cMov
        || i->args[0]->offset != 0
        || i->args[1]->type != atConst
            && (i->args[1]->type != atReg || i->args[1]->offset != -1)
        || equalUpToOffset(*i->args[0], *i->args[1])
    )
        return false;

    list<Command>::iterator j(i); j--;
    if
    (
           j->command != cLea
        || j->args[0]->offset != -1
        || !equalUpToOffset(*i->args[0], *j->args[0])
        || j->args[1]->offset == -1
    )
        return false;

    delete i->args[0];
    i->args[0] = new Argument(*j->args[1]);
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

void Generator::performConstConditionChanges(
    list<Command>::iterator& k,
    list<Command>::iterator& j,
    list<Command>::iterator& i,
    bool condition
)
{
    if(condition)
    {
        codePart.erase(k);
        codePart.erase(j);
        i->command = cJmp;
    }
    else
    {
        codePart.erase(k);
        codePart.erase(j);
        list<Command>::iterator l(i);
        i++;
        codePart.erase(l);
    }
}

bool Generator::tryConstCondition(list<Command>::iterator& i)
{
    //mov eax,1
    //test    eax,eax
    //jxx  l2
    list<Command>::iterator j(i); j--;
    if(j->command != cTest || j == codePart.begin() || *j->args[0] != *j->args[1])
        return false;
    list<Command>::iterator k(j); k--;
    if(k->command != cMov || k->args[1]->type != atConst || *k->args[0] != *j->args[0])
        return false;
    int value = k->args[1]->value.constArg;

    switch(i->command)
    {
        case cJZ:
        case cJE:
            performConstConditionChanges(k, j, i, value == 0); break;
        case cJNZ:
        case cJNE:
            performConstConditionChanges(k, j, i, value != 0); break;
        case cJLE: performConstConditionChanges(k, j, i, value <= 0); break;
        case cJL:  performConstConditionChanges(k, j, i, value < 0); break;
        case cJGE: performConstConditionChanges(k, j, i, value >= 0); break;
        case cJG:  performConstConditionChanges(k, j, i, value > 0); break;
        default:
            return false;
    }

    return true;
}

bool Generator::tryLabelJmp(list<Command>::iterator& i)
{
    if(i->command != cJmp)
        return false;
    list<Command>::iterator j(i); j--;
    if(j->command != cLabel)
        return false;
    for(list<Command>::iterator k = codePart.begin(); k != codePart.end(); ++k)
        if
        (
            k->args.size() > 0 && k->args[0]->type == atLabel &&
            *k->args[0]->value.sArg == *j->args[0]->value.sArg && k != j
        )
            k->args[0]->value.sArg = i->args[0]->value.sArg;
    delete j->args[0]->value.sArg;
    codePart.erase(j);
    j = i;
    i++;
    codePart.erase(j);
    return true;
}

bool Generator::isPowerOf2(int a)
{
    return a != 0 && (a & (a - 1)) == 0;
}

int Generator::log2(int a)
{
    int result = 1;
    while(a > 2)
    {
        a /= 2;
        result++;
    }
    return result;
}

bool Generator::tryImulWithImm(list<Command>::iterator& i)
{
    if(i->command != cImul || i->args[1]->type != atConst)
        return false;
    int value = i->args[1]->value.constArg;
    if(value == 0)
    {
        delete i->args[1];
        i->command = cXor;
        i->args[1] = new Argument(i->args[0]->value.regArg);
    }
    if(value == 1)
    {
        list<Command>::iterator j(i);
        i++;
        codePart.erase(j);
    }
    else if(isPowerOf2(value))
    {
        delete i->args[1];
        i->command = cShl;
        i->args[1] = new Argument(log2(value));
    }
    else
        return false;
    return true;
}

bool Generator::tryIdivWithImm(list<Command>::iterator& i)
{
    //disabled because of % operation
    if(i->command != cIdiv)
        return false;
    list<Command>::iterator j(i);
    int k = 0;
    while(j != codePart.begin() && k++ < 3)
        j--;
    if
    (
           k < 3
        || j->command != cMov
        || j->args[1]->type != atConst
        || !isPowerOf2(j->args[1]->value.constArg)
        || *j->args[0] != *i->args[0]
    )
        return false;
    delete i->args[0];
    i->command = cSar;
    i->args[0] = new Argument(rEAX);
    i->args.push_back(new Argument(log2(j->args[1]->value.constArg)));

    j++;
    list<Command>::iterator l(j); l++;
    codePart.erase(j);
    codePart.erase(l);

    return true;
}

}
