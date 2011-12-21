#include <cstdio>
#include <iostream>
#include <vector>
#include "token.h"
#include "expression.h"
#include "declaration.h"

using namespace std;

namespace LuCCompiler
{

void Designation::out(unsigned int depth, vector<bool>* branches, int level)
{
    for(unsigned int i = 0; i < _list.size(); ++i)
    {
        Node::out(depth, branches, level);
        if(_list[i]->type == TOK_DOT)
            cout << "." << static_cast<IdentNode*>(_list[i]->sub)->name << "\n";
        else
        {
            cout << "[\n";
            _list[i]->sub->out(depth, branches, level + 1);
            Node::out(depth, branches, level);
            cout << "]\n";
        }
    }
}

void InitializerList::out(unsigned int depth, vector<bool>* branches, int level)
{
    Node::out(depth, branches, level);
    cout << "{\n";
    for(unsigned int i = 0; i < _list.size(); ++i)
    {
        if(_list[i]->first != NULL)
        {
            _list[i]->first->out(depth, branches, level + 1);
            Node::out(depth, branches, level + 1);
            cout << "=\n";
        }
        _list[i]->second->out(depth, branches, level + 1);
        Node::out(depth, branches, level + 1);
        cout << ",\n";
    }
    Node::out(depth, branches, level);
    cout << "}\n";
}

}
