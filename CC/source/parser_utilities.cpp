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

Symbol* Parser::getSymbol(const string& name)
{
    return _symbols->getSymbol(name, _tokens->get().line, _tokens->get().col + 1);
}

void Parser::addSymbol(Symbol* symbol, NameType nt)
{
    _symbols->addSymbol(symbol, nt, _tokens->get().line, _tokens->get().col + 1);
}

Symbol* Parser::findSymbol(const string& name)
{
    return _symbols->findSymbol(name);
}

Symbol* Parser::findSymbol(const string& name, NameType nt)
{
    return _symbols->findSymbol(name, nt);
}

void Parser::safeAddSymbol(Symbol* symbol, NameType nt)
{
    if(!findSymbol(symbol->name, nt))
        addSymbol(symbol, nt);
}

void Parser::out()
{
    if(_root != NULL)
    {
        vector<bool> finishedBranches;
        _root->out(0, &finishedBranches);
    }
    else
        _symbols->out();
}

Parser::Parser(Tokenizer* tokens): _varName("")
{
    _tokens = tokens;
    _symbols = initPrimarySymbolTableStack();
    _root = NULL;
}

void Parser::parse()
{
    _mode = PM_SYMBOLS;
    parseTranslationUnit();
}

void Parser::parseStat()
{
    _mode = PM_NO_SYMBOLS;
    _root = parseStatement();
}

void Parser::parseExpr()
{
    _mode = PM_NO_SYMBOLS;
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
