#include <iostream>
#include "generator.h"
#include "symbol.h"
#include "complex_symbol.h"
#include "exception.h"
#include "symbol_table.h"
#include "declaration.h"

namespace LuCCompiler
{

void Generator::genCode(SymbolTypeFunction* f)
{
    if(f->name == "printf")
        return;
    if(f->body == NULL)
        throw NoFunctionDefinition(f->name);
    if(main == NULL && f->name == "main")
    {
        main = f;
        return;
    }
    else if(f->name == "main")
    {
        genReturnLabel();
        genLabel(new Argument(f->name, atLabel));
        f->body->gen(*this);
        genLabel(&returnLabel());
        return;
    }
    currentParamSize = f->args->offset();

    genReturnLabel();

    gen(cLabel, "f_" + f->name);
    gen(cPush, rEBP);
    gen(cMov, rEBP, rESP);

    gen(cSub, rEBP, 4);
    f->body->gen(*this);
    genLabel(&returnLabel());
    gen(cAdd, rEBP, 4);

    gen(cMov, rESP, rEBP);
    gen(cPop, rEBP);
    gen(cRet);

    popReturnLabel();
}

void Generator::genData(const SymbolTable& t)
{
    for(unsigned int j = 0; j < t.size(); ++j)
    {
        if(t[j]->classType != CT_VAR)
        {
            if(t[j]->isFunction())
                genCode(static_cast<SymbolTypeFunction*>(t[j]));
            continue;
        }
        SymbolVariable* var = static_cast<SymbolVariable*>(t[j]);
        Data* d = new Data("v_" + var->name, var->type->size());
        dataPart.push_back(d);

        if(var->initializer != NULL)
        {
            if(static_cast<ENode*>(var->initializer)->isIntConst())
                d->init.intInit = static_cast<IntNode*>(var->initializer)->value;
            else if(static_cast<ENode*>(var->initializer)->isDoubleConst())
            {
                d->initType = dtDouble;
                d->init.doubleInit = static_cast<DoubleNode*>(var->initializer)->value;
            }
        }
        //elses currently not supported
    }
}

void Generator::out()
{
    cout <<
        "format PE console\n"
        "entry main\n\n"
        "include '\\fasm\\include\\win32a.inc'\n";

    cout << "\nsection '.data' data readable writeable\n";
    for(unsigned int i = 0; i < dataPart.size(); ++i)
        dataPart[i]->out();
    for(unsigned int i = 0; i < rdataPart.size(); ++i)
        rdataPart[i]->out();
    for(unsigned int i = 0; i < fdataPart.size(); ++i)
        fdataPart[i]->out();
    cout << "d_zero dq 0.0\n";
    cout << "d_one dq 1.0\n";
    cout << "d_minus_one dq -1.0\n";

    cout << "\nsection '.code' code executable\n";
    for(list<Command>::iterator j = codePart.begin(); j != codePart.end(); ++j)
        j->out();

    cout <<
        "stdcall [ExitProcess],0\n\n"
        "section '.idata' import data readable\n\n"
        "library kernel32,'kernel32.dll', msvcrt,'msvcrt.dll'\n"
        "import msvcrt, f_printf,'printf'\n"
        "import kernel32, ExitProcess,'ExitProcess'" << endl;
}

void Generator::generate(bool doOptimize)
{
    genData(*symbols);
    if(main == NULL)
        throw NoFunctionDefinition("main");
    genCode(main);
    if(doOptimize)
        optimize();
}

void Generator::gen(Command com, Argument a1, Argument a2, Argument a3)
{
    com.args.push_back(new Argument(a1));
    com.args.push_back(new Argument(a2));
    com.args.push_back(new Argument(a3));
    codePart.push_back(com);
};

void Generator::gen(Command com, Argument a1, Argument a2)
{
    com.args.push_back(new Argument(a1));
    com.args.push_back(new Argument(a2));
    codePart.push_back(com);
}

void Generator::gen(Command com, Argument a1)
{
    com.args.push_back(new Argument(a1));
    codePart.push_back(com);
}

void Generator::gen(Command com)
{
    codePart.push_back(com);
}

void Generator::genLabel(Argument* a)
{
    Command* com = new Command(cLabel);
    com->args.push_back(a);
    codePart.push_back(*com);
}

string Generator::addConstant(const string& s)
{
    RData* r = new RData("c_" + itostr(labelNum++), s);
    rdataPart.push_back(r);
    return r->name;
}

string Generator::addDoubleConstant(const double& f)
{
    FData* fd = new FData("d_" + itostr(labelNum++), f);
    fdataPart.push_back(fd);
    return fd->name;
}

void Generator::optimize()
{
    while(modified)
    {
        modified = false;
        list<Command>::iterator i = codePart.begin();
        while(i != codePart.end())
        {
            bool optimized =
                (i == codePart.begin() ?
                    false :
                       tryUniteLabels(i)
                    || tryUnitePushPop(i)
                    || tryMovMov(i)
                    || tryLiftPop(i)
                    || tryMakeOpWithImm(i)
                    || tryUniteAddSub(i)
                    || tryLeaMov(i)
                    || tryLeaPushIncDec(i)
                    || tryMovTest(i)
                    || trySetTestJmp(i)
                    || tryLabelJmp(i)
                    || tryOtherLeaMov(i)
                    //|| tryIdivWithImm(i)
                    || tryConstCondition(i)
                    || tryUniteDoublePushPop(i)
                )
                || tryAddSub0(i)
                || tryAddSub1(i)
                || tryMovSelf(i)
                || tryRemoveUselessMovLeaXor(i)
                || tryJmpLabel(i)
                || tryImulWithImm(i)
                ;
            if(!optimized)
                i++;
            modified = modified || optimized;
        }
    }
    list<Command>::iterator i = codePart.begin();
    while(i != codePart.end())
    {
        tryMov0(i);
        if(i != codePart.begin() && tryLiftSubESPImm(i))
            tryUniteAddSub(i);
        i++;
    }
}

}
