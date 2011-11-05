#ifndef DECLARATION_H
#define DECLARATION_H

#include <vector>
#include <utility>
#include "node.h"
#include "token.h"
#include "nodeWithList.h"

using namespace std;

namespace LuCCompiler
{

class Designator: public Node
{
private:
    TokenType _type; //[ or .
    Node* _sub;
public:
    Designator(TokenType type, Node* sub): _type(type), _sub(sub) {}
};

typedef NodeWithList<Designator> Designation;

typedef NodeWithList< pair<Designation*, Node*> > InitializerList;

/*
class TranslationUnit = NodeWithList<ExternalDeclaration>;

class ExternalDeclaration: public Node
{
public:
    ExternalDeclaration() {}
};

class FunctionDefinition: public ExternalDeclaration
{
private:
    DeclarationSpecifiers* _specifiers;
    Declarator* _dec; //rename later
    DeclarationList* _decList; //rename later
    CompoundStatement* _definition;

public:
    FunctionDefinition(
        DeclarationSpecifiers* decSpec,
        Declarator* dec,
        DeclarationList* decList,
        CompoundStatement* stat
    ): _specifiers(decSpec), _dec(dec), _decList(decList), _definition(stat) { /*to table? }
};

class Declaration: public ExternalDeclaration
{
private:
    DeclarationSpecifiers* _specifiers;
    InitDeclaratorList* _init;
public:
    Declaration(DeclarationSpecifiers* decSpec, InitDeclaratorList* init):
        _specifiers(decSpec), _init(init) { /*to table? }
};

class InitDeclaratorList: public NodeWithList
{
public:
    InitDeclaratorList(): NodeWithList() {}
};

class DirectDeclarator: public Node
{};

class Declarator: public DirectDeclarator
{
protected:
    Pointer* _pointer;
public:
    Declarator(DirectDeclarator* dd, Pointer* p): DirectDeclarator(dd), _pointer(p) {}
};

class InitDeclarator: public Declarator
{
protected:
    Initializer* _init;
public:
    InitDeclarator(Declarator* dec,  Initiaizer* init): Declarator(dec), _init(init) {}
};

class IdentifierList: public NodeWithList
{
public:
    IdentifierList(): NodeWithList() {}
};*/

}

#endif
