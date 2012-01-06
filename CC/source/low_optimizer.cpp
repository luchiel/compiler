#include "generator.h"

using namespace std;

namespace LuCCompiler
{

bool Generator::tryAddSub0(list<Command>::iterator& i)
{
    if
    (
        (i->command == cAdd || i->command == cSub) &&
        (i->args[1]->type == atConst || i->args[1]->type == atConstF) &&
        i->args[1]->value.constArg == 0
    )
    {
        list<Command>::iterator j(i);
        i++;
        codePart.erase(j);
        //codePart.remove(j); //removes all alike -_-
        return true;
    }
    return false;
}

}
