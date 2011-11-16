#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include "parser.h"
#include "declaration.h"
#include "token.h"
#include "symbol.h"
#include "symbol_table.h"

using namespace std;

namespace LuCCompiler
{

Designator* Parser::parseDesignator()
{
    if(tokenType() == TOK_L_SQUARE)
    {
        _tokens->next();
        Node* sub = parseConditionalExpression();
        nullException(sub, "expression expected");
        consumeTokenOfType(TOK_R_SQUARE, "']' expected");
        return new Designator(TOK_L_SQUARE, sub);
    }
    else if(tokenType() == TOK_DOT)
    {
        _tokens->next();
        if(tokenType() != TOK_IDENT)
            throw makeException("identifier expected");
        Node* sub = new IdentNode(_tokens->get().text);
        _tokens->next();
        return new Designator(TOK_DOT, sub);
    }
    return NULL;
}

Designation* Parser::parseDesignation()
{
    Designator* d = parseDesignator();
    if(d == NULL)
        return NULL;
    Designation* node = new Designation();
    while(d != NULL)
    {
        node->add(d);
        d = parseDesignator();
    }
    consumeTokenOfType(TOK_ASSIGN, "'=' expected");
    return node;
}

Node* Parser::parseInitializer()
{
    if(tokenType() == TOK_L_BRACE)
    {
        _tokens->next();
        InitializerList* node = parseInitializerList();
        if(tokenType() == TOK_COMMA)
            _tokens->next();
        consumeTokenOfType(TOK_R_BRACE, "'}' expected");
        return node;
    }
    else
        return parseAssignmentExpression();
}

InitializerList* Parser::parseInitializerList()
{
    Designation* d = parseDesignation();
    Node* i = parseInitializer();
    if(i == NULL)
    {
        if(d != NULL)
            throw makeException("initializer expected");
        else
            return NULL;
    }
    InitializerList* node = new InitializerList();
    node->add(new pair<Designation*, Node*>(d, i));
    while(tokenType() == TOK_COMMA)
    {
        _tokens->next();
        d = parseDesignation();
        i = parseInitializer();
        nullException(i, "initializer expected");
        node->add(new pair<Designation*, Node*>(d, i));
    }
    return node;
}

SymbolType* Parser::parseTypeSpecifier()
{
    switch(tokenType())
    {
        case TOK_INT:   return getSymbol("int");
        case TOK_FLOAT: return getSymbol("float");
        case TOK_VOID:  return getSymbol("void");
        default:        return parseStructSpecifier();
    }
}

SymbolTypeStruct* Parser::parseStructSpecifier()
{
    if(tokenType() != TOK_STRUCT)
        return NULL;
    _tokens->next();

    string name("");
    it(tokenType() == TOK_IDENT)
    {
        name = _tokens->get().text;
        _tokens->next();
    }
    SymbolTypeStruct* node = new SymbolTypeStruct(name);
    addSymbol(node);
    if(tokenType() == TOK_L_BRACE)
    {
        _tokens->next();
        _symbols->push(node->fields);

        bool added = true;
        while(added)
            added = parseStructDeclaration();
        if(node->fields->size() == 0)
            throw makeException("declaration expected");

        consumeTokenOfType(TOK_R_BRACE, "'}' expected");
        _symbols->pop();
    }
    else if(name == "")
        throw makeException("identifier or declaration expected");
    return node;
}

bool Parser::parseStructDeclaration()
{
    SymbolType* type = parseTypeSpecifier();
    if(type == NULL)
        return false;

    type = parseDeclarator(type);
    nullException(type, "declarator expected");
    addSymbol(type);
    addSymbol(new SymbolVariable(type, _varName));
    while(tokenType() == TOK_COMMA)
    {
        _tokens->next();
        type = parseDeclarator(type);
        nullException(d, "declarator expected");
        addSymbol(type);
        addSymbol(new SymbolVariable(type, _varName));
    }

    consumeTokenOfType(TOK_SEP, "';' expected");
}

SymbolType* Parser::parsePointer(SymbolType* type)
{
    SymbolTypePointer* p = NULL;
    while(tokenType() == TOK_ASTERISK)
    {
        _tokens->next();
        p = new SymbolTypePointer("");
        p->type = type;
        type = p;
    }
    return type;
}

SymbolType* Parser::parseDeclarator(SymbolType* type)
{
    SymbolType** initial = &type;
    type = parsePointer(type);

    if(tokenType() == TOK_IDENT)
    {
        if(_varName != "")
            throw makeException("unexpected identifier");
        _varName = _tokens->get().name;
        _tokens->next();
    }
    else if(tokenType() == TOK_L_BRACKET)
    {
        _tokens->next();
        SymbolType* tmp = parseDeclarator(*initial);
        *initial = tmp;
        consumeTokenOfType(TOK_R_BRACKET, "')' expected");
    }
    while(tokenType() == TOK_L_SQUARE || tokenType() == TOK_L_BRACKET)
    {
        if(tokenType() == TOK_L_SQUARE)
        {
            _tokens->next();
            SymbolTypeArray* array = new SymbolTypeArray(type, "");
            if(tokenType() != TOK_R_SQUARE)
                array->length = parseAssignmentExpression();
            consumeTokenOfType(TOK_R_SQUARE, "']' expected");
            type = array;
        }
        else
        {
            _tokens->next();
            SymbolFunction* function = new SymbolFunction("");
            _symbols->push(function->locals);
            if(tokenType() != TOK_R_BRACKET)
            {
                if(tokenType() == TOK_IDENT)
                {
                    addSymbol(SymbolVariable(NULL, _tokens->get().name));
                    _tokens->next();
                    while(tokenType() == TOK_COMMA)
                    {
                        _tokens->next();
                        if(tokenType() != TOK_IDENT)
                            throw makeException("identifier expected");
                        addSymbol(SymbolVariable(NULL, _tokens->get().name));
                        _tokens->next();
                    }
                }
                else
                {
                    parseParameterList();
                }
            }
            _symbols->pop();
            consumeTokenOfType(TOK_R_BRACKET, "')' expected");
            type = function;
        }
    }
    //_varName has name :3
    return type;
}

/*void Parser::parseParameterList()
{
    //
    //
    while(tokenType() == TOK_COMMA)
    {
        _tokens->next();
    }

parameter_declaration = declaration_specifiers [declarator | abstract_declarator] ;

abstract_declarator = pointer | [pointer] direct_abstract_declarator ;

direct_abstract_declarator =
    '(' abstract_declarator ')' |
    [direct_abstract_declarator] '[' [assignment_expression] ']' |
    [direct_abstract_declarator] '(' [parameter_list] ')' ;

}

void Parser::parseDeclarationSpecifiers()
{
    declaration_specifiers = type_specifier [declaration_specifiers];
}
*/

/*
Node* Parser::parseInitDeclarator()
{
    Declarator* dec = parseDeclarator();
    if(dec == NULL)
        return NULL;
    if(tokenType() == TOK_ASSIGN)
    {
        _tokens->next();
        return InitDeclarator(dec, parseInitializer()); //exception if null
    }
    return dec;
}

Node* Parser::parseParameterList()
{
    //parameter_declaration {',' parameter_list} ;
    tmp = parseParameterDeclaration();
    if(tmp == NULL)
        return NULL;
    node->list->push_back(tmp);
    while(tokenType() == TOK_COMMA)
    {
        _tokens->next();
        tmp = parseParameterDeclaration();
        if(tmp == NULL)
            throw makeException("something's wrong");
        node->list->push_back(tmp);
    }
}

Node* Parser::parseParameterDeclaration()
{
    //declaration_specifiers [declarator | abstract_declarator] ;
    node = parseDeclarationSpecifiers();
    tmp = parseDeclarator();
    if(tmp == NULL)
        tmp = parseAbstractDeclarator();
    //attach
    return node;
}

Node* Parser::parseTypeName()
{
    //type_specifier [abstract_declarator] ;
    node = parseTypeSpecifier();
    tmp = parseAbstractDeclarator();
    //attach
    return node;
}

Node* Parser::parseAbstractDeclarator()
{
    //pointer | [pointer] direct_abstract_declarator ;
    tmp = parsePointer();
    node = parseDirectAbstractDeclarator();
    //do
    return node;
}

Node* Parser::parseDirectAbstractDeclarator()
{
    //'(' abstract_declarator ')' |
    //[direct_abstract_declarator] '[' ['*' | assignment_expression] ']' |
    //[direct_abstract_declarator] '(' [parameter_list] ')' ;
    if(tokenType() == TOK_L_BRACKET)
    {
        _tokens->next();
        node = parseAbstractDeclarator();
        consumeTokenOfType(TOK_R_BRACKET, "')' expected");
    }
    node = parseDirectAbstractDeclarator(); //and if none?
    if(tokenType() == TOK_L_SQUARE)
    {
        _tokens->next();
        if(tokenType() == TOK_ASTERISK)
        {}
        else
        {
            tmp = parseAssignmentExpression();
            //if(tmp != NULL)
                //attach
        }
        consumeTokenOfType(TOK_R_SQUARE, "']' expected");
    }
    else if(tokenType() == TOK_L_BRACKET)
    {
        _tokens->next();
        tmp = parseParameterList();
        //if(tmp != NULL)
            //attach
        consumeTokenOfType(TOK_R_BRACKET, "')' expected");
    }
    else
    {
        return NULL;
        //or throw
    }
}

TranslationUnit* Parser::parseTranslationUnit()
{
    TranslationUnit* node = new TranslationUnit();
    ExternalDeclaration* tmp = parseExternalDeclaration();
    if(tmp == NULL)
        return NULL;
    while(tmp != NULL)
    {
        node->add(tmp);
        tmp = parseExternalDeclaration();
    }
}

ExternalDeclaration* Parser::parseExternalDeclaration()
{
    ExternalDeclaration* node = parseFunctionDefinition();
    if(node != NULL)
        return node;
    return parseDeclaration();
}

FunctionDefinition* Parser::parseFunctionDefinition()
{
    DeclarationSpecifiers* decSpec = parseDeclarationSpecifiers();
    Declarator* dec = parseDeclarator();
    DeclarationList* decList = parseDeclarationList(); //can be null
    CompoundStatement* stat = parseCompoundStatement();
    if(stat == NULL)
        return NULL;
    if(decSpec == NULL || dec == NULL)
        throw makeException("bad declaration"); //brief
    return new FunctionDefinition(decSpec, dec, decList, stat);
}*/

Node* Parser::parseDeclaration()
{
    //DeclarationSpecifiers* decSpec = parseDeclarationSpecifiers();
    //if(decSpec == NULL)
    //    return NULL; //required by BlockItem
    //InitDeclaratorList* init = parseInitDeclaratorList();
    //consumeTokenOfType(TOK_SEP, "';' expected");
    //return new Declaration(decSpec, init);
    return NULL;
}
/*
InitDeclaratorList* Parser::parseInitDeclaratorList()
{
    InitDeclarator* dec = parseInitDeclarator();
    if(dec == NULL)
        return NULL;
    InitDeclaratorList* node = new InitDeclaratorList();
    while(tokenType() == TOK_COMMA)
    {
        _tokens->next();
        node->add(dec);
        dec = parseInitDeclarator();
        if(dec == NULL)
            throw makeException("init expected");
    }
    return node;
}

Node* Parser::parseDeclarationList()
{
    tmp = parseDeclaration();
    if(tmp == NULL)
        return NULL;
    while(tmp != NULL)
    {
        node->list->push_back(tmp);
        tmp = parseDeclaration();
    }
    return node;
}

Node* Parser::parseIdentifierList()
{
    if(tokenType() != TOK_IDENT)
        return NULL;
    IdentifierList* node = new IdentifierList();
    node->add(new IdentNode(_tokens->get().text));
    _tokens->next();
    while(tokenType() == TOK_COMMA)
    {
        _tokens->next();
        if(tokenType() != TOK_IDENT)
            throw makeException("identifier expected");
        node->add(new IdentNode(_tokens->get().text));
        _tokens->next();
    }
    return node;
}

Node* Parser::parseArgumentExpressionList()
{
    tmp = parseAssignmentExpression();
    if(tmp == NULL)
        return NULL;
    while(tokenType() == TOK_COMMA)
    {
        _tokens->next();
        node->list->push_back(tmp);
        tmp = parseAssignmentExpression();
        if(tmp == NULL)
            throw makeException("expression expected");
    }
    return node;
}*/

}
