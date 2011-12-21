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
    enum DecKind { D_ABSTRACT, D_NOT_ABSTRACT, D_BOTH };
    enum ParseMode { PM_SYMBOLS, PM_NO_SYMBOLS };

    Node* _root;
    Tokenizer* _tokens;
    SymbolTableStack* _symbols;
    string _varName;
    ParseMode _mode;
    SymbolType* _int;
    SymbolType* _float;
    Symbol* _NULL;

    TokenType tokenType() { return _tokens->get().type; }

    Symbol* getSymbol(const string& name); //tags not seen
    Symbol* findSymbol(const string& name);
    Symbol* findSymbol(const string& name, NameType nt);
    void addSymbol(Symbol* symbol);
    void addTag(SymbolTypeStruct* symbol);
    void safeAddSymbol(Symbol* symbol);
    bool isArithmetic(SymbolType& type);
    bool isInt(SymbolType& type);
    bool isFloat(SymbolType& type);
    void checkArgumentsArithmetic(BinaryNode* node);
    void performImplicitCast(BinaryNode* node);

    ENode* parsePrimaryExpression();
    ENode* parsePostfixExpression();
    ENode* parseUnaryExpression();
    ENode* parseBinaryExpression(int priority);
    ENode* parseCastExpression();
    ENode* parseConditionalExpression();
    ENode* parseAssignmentExpression();
    ENode* parseExpression();

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
    void addParsedSymbols(SymbolType* type, Node* initializer, bool isTypedef);
    void parseParameterDeclaration();
    void parseTranslationUnit();

    ParserException makeException(const string& e);
    void nullException(void* pointerToCheck, const string& e);
    void consumeTokenOfType(TokenType type, const string& except);

public:
    void parse();
    void parseStat();
    void parseExpr();

    Parser(Tokenizer* tokens);

    void out();
};

string itostr(const int i);

}

#endif
