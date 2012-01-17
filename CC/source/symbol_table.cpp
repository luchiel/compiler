#include <string>
#include <assert.h>
#include <map>
#include "exception.h"
#include "expression.h"
#include "symbol_table.h"
#include "symbol.h"
#include "complex_symbol.h"

namespace LuCCompiler
{

Symbol* SymbolTable::operator[](const int i) const
{
    return _ordered[i];
}

bool operator==(const SymbolTable& t1, const SymbolTable& t2)
{
    if(t1._ordered.size() != t2._ordered.size())
        return false;
    for(unsigned int i = 0; i < t1._ordered.size(); ++i)
    {
        if(*t1._ordered[i] != *t2._ordered[i])
            return false;
    }
    return true;
}

bool SymbolTable::addSymbol(Symbol* symbol, int line, int col, int name)
{
    if(_symbols.find(make_pair(symbol->name, NT_NAME)) != _symbols.end())
        throw RedefinedSymbolException(line, col, symbol->name);

    bool r = symbol->setName(name);
    _symbols[make_pair(symbol->name, NT_NAME)] = symbol;
    _ordered.push_back(symbol);
    return r;
}

bool SymbolTable::addTag(Symbol* symbol, int line, int col, int name)
{
    SymbolTypeStruct* s = static_cast<SymbolTypeStruct*>(symbol);
    if(_symbols.find(make_pair(s->tag, NT_TAG)) != _symbols.end())
        throw RedefinedSymbolException(line, col, s->tag);

    bool r = symbol->setName(name);
    _symbols[make_pair(s->tag, NT_TAG)] = symbol;
    _ordered.push_back(symbol);
    return r;
}

Symbol* SymbolTable::getSymbol(const string& name, int line, int col)
{
    Symbol* s = findSymbol(name, NT_NAME);
    if(s == NULL)
        throw UndefinedSymbolException(line, col, name);
    return s;
}

Symbol* SymbolTable::findSymbol(const string& name)
{
    map< pair<string, NameType>, Symbol*>::iterator s;
    s = _symbols.find(make_pair(name, NT_NAME));
    if(s != _symbols.end())
        return s->second;
    s = _symbols.find(make_pair(name, NT_TAG));
    if(s != _symbols.end())
        return s->second;
    return NULL;
}

Symbol* SymbolTable::findSymbol(const string& name, NameType nt)
{
    map< pair<string, NameType>, Symbol*>::iterator s;
    s = _symbols.find(make_pair(name, nt));
    if(s != _symbols.end())
        return s->second;
    return NULL;
}

void SymbolTable::out(int indent)
{
    for(unsigned int i = 0; i < _ordered.size(); ++i)
        _ordered[i]->out(indent, false);
}

SymbolTable::~SymbolTable()
{
    _symbols.clear();
    for(unsigned int i = 0; i < _ordered.size(); ++i)
        delete _ordered[i];
    _ordered.clear();
}

unsigned int SymbolTable::offset()
{
    if(parent == NULL)
        return 0;
    if(_offset == 0)
    {
        _offset = parent->offset();
        bool isFuncLocals = false;
        for(unsigned int i = 0; i < parent->size() && !isFuncLocals; ++i)
            if((*parent)[i]->classType == CT_VAR)
                if(static_cast<SymbolVariable*>((*parent)[i])->varType == VT_PARAM)
                    isFuncLocals = true;

        for(unsigned int i = 0; i < size(); ++i)
            if(_ordered[i]->classType == CT_VAR)
            {
                _ordered[i]->offset = _offset;
                if(isFuncLocals)
                    _ordered[i]->offset -= parent->offset();
                _offset += static_cast<SymbolVariable*>(_ordered[i])->type->size();
            }
    }
    return _offset;
}

void SymbolTable::genInitLocals(AbstractGenerator& g)
{
    for(unsigned int i = 0; i < size(); ++i)
        if((*this)[i]->classType == CT_VAR)
        {
            SymbolVariable* var = static_cast<SymbolVariable*>((*this)[i]);
            if(dynamic_cast<ENode*>(var->initializer) != NULL)
            {
                if(static_cast<ENode*>(var->initializer)->isDoubleConst())
                {
                    string s = g.addDoubleConstant(
                        static_cast<DoubleNode*>(var->initializer)->value
                    );
                    g.gen(cMovsd, rXMM0, s + Offset(0) + swQword);
                    g.gen(cMovsd, rEBP + Offset(-(var->offset + 2) * 4) + swQword, rXMM0);
                }
                else if(static_cast<ENode*>(var->initializer)->isIntConst())
                    g.gen(
                        cMov, rEBP + Offset(-(var->offset + 1) * 4),
                        static_cast<IntNode*>(var->initializer)->value
                    );
                else if(static_cast<ENode*>(var->initializer)->isNULL())
                    g.gen(cMov, rEBP + Offset(-(var->offset + 1) * 4), 0);
                //else wait for init node
            }
            //else not supported
        }
}


void SymbolTable::erase(vector<Symbol*>::iterator& i)
{
    _symbols.erase(_symbols.find(make_pair((*i)->name, NT_NAME)));
    i = _ordered.erase(i);
    i--;
}

Symbol* SymbolTableStack::findSymbol(const string& name)
{
    SymbolTable* c = _current;
    Symbol* r = c->findSymbol(name);
    while(r == NULL)
    {
        if(c == _root)
            break;
        c = c->parent;
        r = c->findSymbol(name);
    }
    return r;
}

Symbol* SymbolTableStack::findSymbol(const string& name, NameType nt)
{
    SymbolTable* c = _current;
    Symbol* r = c->findSymbol(name, nt);
    while(r == NULL)
    {
        if(c == _root)
            break;
        c = c->parent;
        r = c->findSymbol(name, nt);
    }
    return r;
}

Symbol* SymbolTableStack::getSymbol(const string& name, int line, int col)
{
    Symbol* s = findSymbol(name);
    if(s == NULL)
        throw UndefinedSymbolException(line, col, name);
    return s;
}

void SymbolTableStack::addSymbol(Symbol* symbol, int line, int col)
{
    if(_current->addSymbol(symbol, line, col, _innerIdx))
        _innerIdx++;
}

void SymbolTableStack::addTag(Symbol* symbol, int line, int col)
{
    if(_current->addTag(symbol, line, col, _innerIdx))
        _innerIdx++;
}

void SymbolTableStack::push(SymbolTable* t)
{
    t->parent = _current;
    _current = t;
}

void SymbolTableStack::pop()
{
    assert(_current->parent != NULL);
    _current = _current->parent;
}

void SymbolTableStack::out()
{
    _root->out(0);
}

SymbolTableStack* initPrimarySymbolTableStack()
{
    SymbolTable* primarySymbolTable = new SymbolTable();

    SymbolType* int_ = new SymbolType("int");
    SymbolType* void_ = new SymbolType("void");
    SymbolType* void_ptr = new SymbolTypePointer(void_, "void*");

    primarySymbolTable->addSymbol(int_, 0, 0, 0);
    primarySymbolTable->addSymbol(new SymbolType("double"), 0, 0, 0);
    primarySymbolTable->addSymbol(void_, 0, 0, 0);

    primarySymbolTable->addSymbol(new SymbolTypePointer(int_, "int*"), 0, 0, 0);
    primarySymbolTable->addSymbol(void_ptr, 0, 0, 0);
    IntNode* zero = new IntNode(0);
    zero->expType = static_cast<SymbolType*>(primarySymbolTable->getSymbol("int", 0, 0));
    primarySymbolTable->addSymbol(new SymbolVariable(void_ptr, "NULL", zero, VT_GLOBAL), 0, 0, 0);

    primarySymbolTable->addSymbol(new SymbolTypeFunction(int_, "printf"), 0, 0, 0);

    return new SymbolTableStack(primarySymbolTable, primarySymbolTable);
}

}
