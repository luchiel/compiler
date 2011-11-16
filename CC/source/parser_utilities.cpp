#include <vector>
#include <string>
#include "parser.h"
#include "exception.h"
#include "tokenizer.h"
#include "token.h"
#include "symbol.h"
#include "symbol_table.h"

using namespace std;

namespace LuCCompiler
{

Symbol* Parser::getSymbol(const string& name, bool type)
{
    return _symbols->getSymbol(name, _tokens->get().line, _tokens->get().col + 1, type);
}

void Parser::addSymbol(Symbol* symbol, bool type)
{
    _symbols->addSymbol(symbol, _tokens->get().line, _tokens->get().col + 1, type);
}

Symbol* Parser::findSymbol(const string& name, bool type)
{
    return _symbols->findSymbol(name, type);
}

void Parser::safeAddSymbol(Symbol* symbol, bool type)
{
    if(!findSymbol(symbol->name, type))
        addSymbol(symbol, type);
}

void Parser::out()
{
    if(_root != NULL)
    {
        vector<bool> finishedBranches;
        _root->out(0, &finishedBranches);
    }
    else
    {
        _symbols->out();
    }
}

Parser::Parser(Tokenizer* tokens): _varName("")
{
    _tokens = tokens;
    _symbols = initPrimarySymbolTableStack();
    _root = NULL;
}

void Parser::parse()
{
    parseTranslationUnit();
}

void Parser::parseStat()
{
    _root = parseStatement();
}

void Parser::parseExpr()
{
    _root = parseExpression();
}

void Parser::consumeTokenOfType(TokenType type, const string& except)
{
    if(tokenType() != type)
        throw makeException(except);
    _tokens->next();
}

ParserException Parser::makeException(const string& e)
{
    return ParserException(_tokens->get().line, _tokens->get().col + 1, e);
}

void Parser::nullException(void* pointerToCheck, const string& e)
{
    if(pointerToCheck == NULL)
        throw makeException(e);
}

}
