#ifndef PARSER_H
#define PARSER_H

#include "iostream"
#include "declaration.h"
#include "expression.h"
#include "tokenizer.h"
#include "exception.h"
#include "symbol.h"
#include "symbol_table.h"
#include "complex_symbol.h"

namespace LuCCompiler
{

class Parser
{
private:
    Node* _root;
    Tokenizer* _tokens;
    SymbolTableStack* _symbols;
    string _varName;

    TokenType tokenType() { return _tokens->get().type; }

    Symbol* getSymbol(const string& name, bool type = false);
    Symbol* findSymbol(const string& name, bool type = false);
    void addSymbol(Symbol* symbol, bool type = false);
    void safeAddSymbol(Symbol* symbol, bool type = false);

    enum DecKind { D_ABSTRACT, D_NOT_ABSTRACT, D_BOTH };

public:
    void parse();
    void parseStat();
    void parseExpr();

    Parser(Tokenizer* tokens);

    Node* parsePrimaryExpression();
    Node* parsePostfixExpression();
    Node* parseUnaryExpression();
    Node* parseBinaryExpression(int priority);
    Node* parseCastExpression();
    Node* parseConditionalExpression();
    Node* parseAssignmentExpression();
    Node* parseExpression();

    Node* parseBlockItem();
    CompoundStatement* parseCompoundStatement();
    Node* parseSelectionStatement();
    Node* parseJumpStatement();
    Node* parseExpressionStatement();
    Node* parseIterationStatement();
    Node* parseStatement();

    Designator* parseDesignator();
    Designation* parseDesignation();

    Node* parseInitializer();
    InitializerList* parseInitializerList();

    SymbolType* parseTypeSpecifier();
    SymbolType* parseTypeName();
    SymbolTypeStruct* parseStructSpecifier();
    bool parseStructDeclaration();
    SymbolType* parsePointer(SymbolType* type);
    SymbolType* parseDeclarator(SymbolType* type, DecKind isAbstract = D_NOT_ABSTRACT);
    bool parseDeclaration(bool definitionAllowed);
    Node* parseInitializerPart();
    void addTypeAndInitializedVariable(SymbolType* type, Node* initializer);
    void parseParameterDeclaration();
    void parseTranslationUnit();

    ParserException makeException(const string& e);
    void nullException(void* pointerToCheck, const string& e);
    void consumeTokenOfType(TokenType type, const string& except);

    void out();
};

}

#endif
