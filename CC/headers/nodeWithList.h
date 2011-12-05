#ifndef NODE_WITH_LIST_H
#define NODE_WITH_LIST_H

#include <vector>
#include "node.h"

using namespace std;

namespace LuCCompiler
{

template<typename T> class NodeWithList: public Node
{
protected:
    vector<T*> _list;
public:
    NodeWithList() {}
    void add(T* node) { _list.push_back(node); };
};

}

#endif
