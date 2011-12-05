#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include "parser.h"
#include "declaration.h"
#include "token.h"
#include "symbol.h"
#include "symbol_table.h"
#include "complex_symbol.h"

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
        if(tokenType() == TOK_R_BRACE)
            return node;
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
        case TOK_INT:
            _tokens->next();
            return static_cast<SymbolType*>(getSymbol("int", true));
        case TOK_FLOAT:
            _tokens->next();
            return static_cast<SymbolType*>(getSymbol("float", true));
        case TOK_VOID:
            _tokens->next();
            return static_cast<SymbolType*>(getSymbol("void", true));
        case TOK_STRUCT:
            _tokens->next();
            return parseStructSpecifier();
        case TOK_IDENT:
            Symbol* tmp;
            tmp = findSymbol(_tokens->get().text, true);
            if(tmp != NULL)
                _tokens->next();
            return static_cast<SymbolType*>(tmp);
        default:
            return NULL;
    }
}

SymbolTypeStruct* Parser::parseStructSpecifier()
{
    string name("");
    SymbolTypeStruct* node = NULL;
    bool found = true;
    if(tokenType() == TOK_IDENT)
    {
        name = _tokens->get().text;
        _tokens->next();
        node = static_cast<SymbolTypeStruct*>(findSymbol(name, true));
    }
    if(node == NULL)
    {
        node = new SymbolTypeStruct(name);
        addSymbol(node, true);
        found = false;
    }
    if(tokenType() == TOK_L_BRACE)
    {
        _tokens->next();
        if(found && node->fields->size() != 0)
            throw makeException("redefinition of struct " + name);
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
    safeAddSymbol(type, true);
    addSymbol(new SymbolVariable(type, _varName));
    _varName = "";
    while(tokenType() == TOK_COMMA)
    {
        _tokens->next();
        type = parseDeclarator(type);
        safeAddSymbol(type, true);
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
        p = new SymbolTypePointer(type, "");
        type = p;
    }
    return type;
}

SymbolType* Parser::parseDeclarator(SymbolType* type, DecKind isAbstract)
{
    SymbolType* left = parsePointer(type);
    SymbolType* center = NULL;
    SymbolType* right = left;

    if(tokenType() == TOK_IDENT)
    {
        if(isAbstract == D_ABSTRACT)
            throw makeException("abstract declarator expected");
        if(_varName != "")
            throw makeException("unexpected identifier");
        _varName = _tokens->get().text;
        _tokens->next();
    }
    else if(tokenType() == TOK_L_BRACKET)
    {
        _tokens->next();
        center = parseDeclarator(new SymbolUnknownType());
        consumeTokenOfType(TOK_R_BRACKET, "')' expected");
    }
    while(tokenType() == TOK_L_SQUARE || tokenType() == TOK_L_BRACKET)
    {
        if(tokenType() == TOK_L_BRACKET)
        {
            _tokens->next();
            SymbolTypeFunction* function = new SymbolTypeFunction(new SymbolUnknownType(), "");
            _symbols->push(function->args);
            if(tokenType() != TOK_R_BRACKET)
            {
                string tempVarName(_varName);
                _varName = "";
                parseParameterDeclaration();
                while(tokenType() == TOK_COMMA)
                {
                    _tokens->next();
                    parseParameterDeclaration();
                }
                _varName = tempVarName;
            }
            _symbols->pop();
            consumeTokenOfType(TOK_R_BRACKET, "')' expected");
            if(right != left)
                static_cast<TypedSymbolType*>(right->getUndefined())->type = function;
            else
                right = function;
        }
        else
        {
            _tokens->next();
            SymbolTypeArray* array = new SymbolTypeArray(new SymbolUnknownType(), "");
            if(tokenType() != TOK_R_SQUARE)
                array->length = parseAssignmentExpression();
            consumeTokenOfType(TOK_R_SQUARE, "']' expected");
            if(right != left)
                static_cast<TypedSymbolType*>(right->getUndefined())->type = array;
            else
                right = array;
        }
    }
    if(right != left)
        static_cast<TypedSymbolType*>(right->getUndefined())->type = left;

    if(_varName == "" && isAbstract == D_NOT_ABSTRACT)
        throw makeException("non-abstract declarator expected");

    if(center != NULL)
    {
        static_cast<TypedSymbolType*>(center->getUndefined())->type = right;
        return center;
    }

    return right;
}

void Parser::parseParameterDeclaration()
{
    SymbolType* type = parseTypeSpecifier();
    if(type == NULL)
        return;
    type = parseDeclarator(type, D_BOTH);
    safeAddSymbol(type, true);
    //what if function?
    addSymbol(new SymbolVariable(type, _varName));
    _varName = "";
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
    safeAddSymbol(type, true);
    //function declaration? not variable...
    addSymbol(new SymbolVariable(type, _varName, initializer));
    _varName = "";
}

bool Parser::parseDeclaration(bool definitionAllowed)
{
    DecKind canBeAbstract = D_NOT_ABSTRACT;
    if(tokenType() == TOK_STRUCT)
        canBeAbstract = D_BOTH;
    SymbolType* initial = parseTypeSpecifier();
    if(initial == NULL)
        return false;
    SymbolType* type = parseDeclarator(initial, canBeAbstract);
    if(_varName == "") //if non-abstract here, then except earlier
    {
        safeAddSymbol(type, true);
        consumeTokenOfType(TOK_SEP, "';' expected");
        return true;
    }
    Node* initializer = parseInitializerPart();

    if(initializer == NULL && tokenType() == TOK_L_BRACE)
    {
        if(!definitionAllowed)
            throw makeException("unexpected function definition");
        //is function?
        SymbolTypeFunction* function = static_cast<SymbolTypeFunction*>(type);
        function->name = _varName;
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

SymbolType* Parser::parseTypeName()
{
    SymbolType* type = parseTypeSpecifier();
    if(type == NULL)
        return NULL;
    type = parseDeclarator(type, D_ABSTRACT);
    safeAddSymbol(type, true);
    return type;
}

}
