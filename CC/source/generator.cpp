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
        genLabel(new Argument(f->name, atLabel));
        f->body->gen(*this);
        return;
    }
    currentParamSize = f->args->offset();

    genReturnLabel();

    gen(cProc, "f_" + f->name);
    gen(cPush, rEBP);
    gen(cMov, rEBP, rESP);

    gen(cSub, rEBP, 4);
    f->body->gen(*this);
    genLabel(&returnLabel());
    gen(cAdd, rEBP, 4);

    gen(cMov, rESP, rEBP);
    gen(cPop, rEBP);
    gen(cRet);
    gen(cEndp, "f_" + f->name);

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

        if(var->initializer != NULL && static_cast<ENode*>(var->initializer)->isIntConst())
            d->init.intInit = static_cast<IntNode*>(var->initializer)->value;
        //elses currently not supported
    }
}

void Generator::out()
{
    cout <<
        ".686\n.xmm\n.model flat, stdcall\n"
        "include \\masm32\\include\\msvcrt.inc\n"
        "include \\masm32\\include\\kernel32.inc\n"
        "includelib \\masm32\\lib\\kernel32.lib\n"
        "includelib \\masm32\\lib\\msvcrt.lib\n\n"

        "ExitProcess proto :DWORD\n";

    cout << "\n.data\n";
    for(unsigned int i = 0; i < dataPart.size(); ++i)
        dataPart[i]->out();
    for(unsigned int i = 0; i < rdataPart.size(); ++i)
        rdataPart[i]->out();
    cout << "\n.code\n";
    for(list<Command>::iterator j = codePart.begin(); j != codePart.end(); ++j)
        j->out();

    cout << "call ExitProcess\n\nend main" << endl;
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

void Generator::optimize()
{
    while(modified)
    {
        modified = false;
        list<Command>::iterator i = codePart.begin();
        while(i != codePart.end())
        {
            bool tryOptimize =
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
                )
                || tryAddSub0(i)
                || tryAddSub1(i)
                || tryMovSelf(i)
                || tryRemoveUselessMovLea(i)
                || tryJmpLabel(i)
                || tryImulWithImm(i)
                ;
            if(!tryOptimize)
                i++;
            modified = modified || tryOptimize;
        }
    }
    list<Command>::iterator i = codePart.begin();
    while(i != codePart.end())
    {
        tryMov0(i);
        i++;
    }
}

}
