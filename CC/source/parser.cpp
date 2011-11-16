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
        case TOK_INT:   return static_cast<SymbolType*>(getSymbol("int"));
        case TOK_FLOAT: return static_cast<SymbolType*>(getSymbol("float"));
        case TOK_VOID:  return static_cast<SymbolType*>(getSymbol("void"));
        default:        return parseStructSpecifier();
    }
}

SymbolTypeStruct* Parser::parseStructSpecifier()
{
    if(tokenType() != TOK_STRUCT)
        return NULL;
    _tokens->next();

    string name("");
    if(tokenType() == TOK_IDENT)
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

    //check is_pointer_to_function
    type = parseDeclarator(type);
    safeAddSymbol(type);
    addSymbol(new SymbolVariable(type, _varName));
    _varName = "";
    while(tokenType() == TOK_COMMA)
    {
        _tokens->next();
        type = parseDeclarator(type);
        safeAddSymbol(type);
        addSymbol(new SymbolVariable(type, _varName));
        _varName = "";
    }
    consumeTokenOfType(TOK_SEP, "';' expected");
    return true;
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
        _varName = _tokens->get().text;
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
            SymbolTypeFunction* function = new SymbolTypeFunction(type, "");
            _symbols->push(function->locals);
            if(tokenType() != TOK_R_BRACKET)
            {
                //parseParameterList();
            }
            _symbols->pop();
            consumeTokenOfType(TOK_R_BRACKET, "')' expected");
            type = function;
        }
    }
    if(_varName == "")
        throw makeException("non-abstract declarator expected");

    return type;
}

Node* Parser::parseInitializerPart()
{
    Node* initializer = NULL;
    if(tokenType() == TOK_ASSIGN)
    {
        _tokens->next();
        initializer = parseInitializer();
        nullException(initializer, "initializer expected");
    }
    return initializer;
}

void Parser::addTypeAndInitializedVariable(SymbolType* type, Node* initializer)
{
    safeAddSymbol(type);
    //function declaration? not variable...
    addSymbol(new SymbolVariable(type, _varName, initializer));
    _varName = "";
}

bool Parser::parseDeclaration(bool definitionAllowed)
{
    SymbolType* initial = parseTypeSpecifier();
    if(initial == NULL)
        return false;
    SymbolType* type = parseDeclarator(initial);
    Node* initializer = parseInitializerPart();

    if(initializer == NULL && tokenType() != TOK_COMMA && tokenType() == TOK_L_BRACE)
    {
        if(!definitionAllowed)
            throw makeException("unexpected function definition");
        SymbolTypeFunction* function = new SymbolTypeFunction(type, _varName);
        _varName = "";
        function->body = parseCompoundStatement();
        addSymbol(function);
        return true;
    }

    addTypeAndInitializedVariable(type, initializer);
    while(tokenType() == TOK_COMMA)
    {
        _tokens->next();
        type = parseDeclarator(initial);
        initializer = parseInitializerPart();
        addTypeAndInitializedVariable(type, initializer);
    }
    consumeTokenOfType(TOK_SEP, "';' expected");
    return true;
}

void Parser::parseTranslationUnit()
{
    while(parseDeclaration(true));
}

}
