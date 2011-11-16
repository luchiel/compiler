#ifndef PARSER_H
#define PARSER_H

#include "iostream"
#include "declaration.h"
#include "expression.h"
#include "tokenizer.h"
#include "exception.h"
#include "symbol_table.h"

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

public:
    void parse();
    void parseExpr();

    Parser(Tokenizer* tokens): _varName("");

    Node* parsePrimaryExpression();
    Node* parsePostfixExpression();
    Node* parseUnaryExpression();
    Node* parseBinaryExpression(int priority);
    Node* parseCastExpression();
    Node* parseConditionalExpression();
    Node* parseAssignmentExpression();
    Node* parseExpression();

    Node* parseDeclaration();

    Node* parseBlockItem();
    Node* parseCompoundStatement();
    Node* parseSelectionStatement();
    Node* parseJumpStatement();
    Node* parseExpressionStatement();
    Node* parseIterationStatement();
    Node* parseStatement();

    Designator* parseDesignator();
    Designation* parseDesignation();

    Node* parseInitializer();
    InitializerList* parseInitializerList();
    //Node* parseTranslationUnit();
    //Node* parseExternalDeclaration();

    //Node* parseIdentifierList();

    ParserException makeException(const string& e);
    void nullException(void* pointerToCheck, const string& e);
    void consumeTokenOfType(TokenType type, const string& except);

    void out();

    Symbol* getSymbol(const string& name);
    void addSymbol(Symbol* symbol);
};

}

#endif
