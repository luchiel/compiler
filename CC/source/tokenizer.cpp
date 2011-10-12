#include <string>
#include <cctype>
#include "../headers/token.h"
#include "../headers/tokenizer.h"

namespace LuCCompiler
{

void dd() {}

//const string a = "dsdg\
//                 dsfdf";

void Tokenizer::readStr(unsigned int idx)
{
    _current.type = TOK_STR_CONST;
    _current.value.strValue = new string("");
    _current.col = idx;
    _current.line = _currentLine;

    bool isOpened = false;
    unsigned int startText = idx;
    //int backslashes = 0;

    while(idx < _buffer.size())
    {
        //if(_buffer[idx] == '\\')
        //{
        //    backslashes = backslashes == 1 ? 0 : 1;
        //}
        if(_buffer[idx] == '"')// && backslashes == 0)
        {
            //forgot about \"
            isOpened = !isOpened;
            if(!isOpened)
            {
                string tmp;
                tmp.assign(_buffer, startText + 1, idx - startText - 1);
                *(_current.value.strValue) = *(_current.value.strValue) + tmp;
            }
            else
                startText = idx;
        }
        else if(!isOpened && !isspace(_buffer[idx]))
            break;
        idx++;
        if(idx == _buffer.size())
            if(isOpened)
                throw NewlineInConstantString();
            else if(tryGetLine())
                idx = 0;
            else
                break;
    }

    _current.text = '"' + *(_current.value.strValue) + '"';
    _index = idx;
    _state = IS_MADE;
}

void Tokenizer::readChar(unsigned int idx)
{
}

bool Tokenizer::isOctDigit(char cval)
{
    return '0' <= cval && cval <= '7';
}

int Tokenizer::getIntValue(char cval)
{
    if(cval >= 'a' && cval <= 'f')
        return cval - 'a' + 0xA;
    if(cval >= 'A' && cval <= 'F')
        return cval - 'A' + 0xA;
    return cval - '0';
}

//then tests & test unit
//exceptions too
//then string & char or char & string
//floats
//look at hex and oct float syntax

void Tokenizer::readInt(unsigned int& idx)
{
    _state = IS_READ;
    _current.value.intValue = 0;
    _current.type = TOK_DEC_CONST;
    int notation = 10;
    if(_buffer[idx] == '0')
    {
        char nextSymbol = trySymbol(idx + 1);
        if(nextSymbol == 'x' || nextSymbol == 'X')
        {
            _current.type = TOK_HEX_CONST;
            idx += 2;
            notation = 16;
        }
        else if(!isdigit(nextSymbol))
        {
//            idx++;
            return;
        }
        else
        {
            _current.type = TOK_OCT_CONST;
            idx++;
            notation = 8;
        }
    }
    while(idx < _buffer.size() && (
        _current.type == TOK_OCT_CONST && isOctDigit(_buffer[idx]) ||
        _current.type == TOK_DEC_CONST && isdigit(_buffer[idx]) ||
        _current.type == TOK_HEX_CONST && isxdigit(_buffer[idx])
    ))
        _current.value.intValue =
            _current.value.intValue * notation + getIntValue(_buffer[idx++]);

    if(idx < _buffer.size() && _current.type == TOK_DEC_CONST && (
        _buffer[idx] == 'e' || _buffer[idx] == 'E' || _buffer[idx] == '.'
    ))
    {
        idx++;
        _current.type = TOK_FLOAT_CONST;
        //rules here
    }

    idx--;
}

void Tokenizer::readIdentifier(unsigned int& idx)
{
    //no need to check first symbol is not a digit
    //it was checked earlier
    _current.type = TOK_IDENT;
    while(
        idx < _buffer.size() && (
        isalpha(_buffer[idx]) || _buffer[idx] == '_' || isdigit(_buffer[idx])
    ))
        idx++;
    idx--;
    _state = IS_READ;
}

void Tokenizer::makeEOFToken()
{
    _current.text = "";
    _current.type = TOK_EOF;
    _current.line = _currentLine;
    _current.col = _index;
    //_state = IS_MADE;
}

bool Tokenizer::tryGetLine()
{
    do
    {
        if(_source.eof())
            return false;
        getline(_source, _buffer);
        _currentLine++;
    }
    while(_buffer.size() == 0);
    _index = 0;
    return true;
}

char Tokenizer::trySymbol(unsigned int pos)
{
    //returns ' ' if EOL
    return pos < _buffer.size() ? _buffer[pos] : ' ';
}

Tokenizer::Tokenizer(): _state(IS_NONE), _current(Token()), _buffer(""), _index(0), _currentLine(0)
{
    const string OPERATIONS = "+-=/*><%&^|!~.?[]";

    for(unsigned int i = 0; i < OPERATIONS.size(); ++i)
        operations.insert(OPERATIONS[i]);

    keywords["sizeof"] = TOK_SIZEOF;
    keywords["int"] = TOK_INT;
    keywords["float"] = TOK_FLOAT;
    keywords["void"] = TOK_VOID;

    keywords["if"] = TOK_IF;
    keywords["else"] = TOK_ELSE;
    keywords["for"] = TOK_FOR;
    keywords["do"] = TOK_DO;
    keywords["while"] = TOK_WHILE;
    keywords["struct"] = TOK_STRUCT;
    keywords["break"] = TOK_BREAK;
    keywords["continue"] = TOK_CONTINUE;
    keywords["return"] = TOK_RETURN;
}

void Tokenizer::checkKeywords()
{
    if(keywords.find(_current.text) != keywords.end())
        _current.type = keywords[_current.text];
}

void Tokenizer::read()
{
    if(_buffer.size() == 0 || _index >= _buffer.size())
    {
        if(!tryGetLine())
        {
            makeEOFToken();
            return;
        }
        //_index = 0;
    }

    _current = Token();

    unsigned int cloneOfIndex = _index;
    unsigned int j = _index;
    while(_state != IS_MADE)
    {
        if(j >= _buffer.size())
        {
            if(!tryGetLine())
            {
                makeEOFToken();
                return;
            }
            j = 0;
        }
        switch(_buffer[j])
        {
            case '{':   setTypeAndReadState(TOK_L_BRACE); break;
            case '}':   setTypeAndReadState(TOK_R_BRACE); break;
            case '(':   setTypeAndReadState(TOK_L_BRACKET); break;
            case ')':   setTypeAndReadState(TOK_R_BRACKET); break;
            case '"':   readStr(j); break;
            case '\'':  readChar(j + 1); setTypeAndReadState(TOK_CHAR_CONST); break;
            case ';':   setTypeAndReadState(TOK_SEP); break;
            case ':':   setTypeAndReadState(TOK_COLON); break;
            case ',':   setTypeAndReadState(TOK_COMMA); break;
            default:
                if(isalpha(_buffer[j]) || _buffer[j] == '_')
                    readIdentifier(j);
                //else if(isspace(_buffer[j]))
                //    break;
                else if(isdigit(_buffer[j]))
                    readInt(j);
                else if(_buffer[j] == '/' && j < _buffer.size() - 1)
                {
                    _state = _buffer[j + 1] == '/' ? IS_LCOMMENT : _buffer[j + 1] == '*' ? IS_COMMENT : IS_NONE;
                    if(_state == IS_NONE)
                        _current.type = TOK_OPER;
                }
                else if(operations.find(_buffer[j]) != operations.end())
                    _current.type = TOK_OPER;
        };
        if(_current.type == TOK_OPER)
        {
            char nextSymbol = trySymbol(j + 1);
            switch(_buffer[j])
            {
                case '-':
                    switch(nextSymbol)
                    {
                        case '-': _current.type = TOK_DEC; break;
                        case '=': _current.type = TOK_SUB_ASSIGN; break;
                        case '>': _current.type = TOK_ARROW; break;
                        default: _current.type = TOK_MINUS; break;
                    };
                    break;
                case '+':
                    _current.type =
                        nextSymbol == '+' ? TOK_INC :
                        nextSymbol == '=' ? TOK_ADD_ASSIGN :
                        TOK_PLUS;
                    break;
                case '=':
                    _current.type = nextSymbol == '=' ? TOK_EQUAL : TOK_ASSIGN;
                    break;
                case '!':
                    _current.type =
                        trySymbol(j + 1) == '=' ?
                        TOK_NOT_EQUAL : TOK_NOT;
                    break;
                case '|':
                    _current.type =
                        nextSymbol == '||' ? TOK_LOGICAL_OR :
                        nextSymbol == '=' ? TOK_OR_ASSIGN :
                        TOK_OR;
                    break;
                case '^':
                    _current.type = nextSymbol == '=' ? TOK_XOR_ASSIGN : TOK_XOR;
                    break;
                case '<':
                    if(nextSymbol == '<')
                        if(trySymbol(j + 2) == '=')
                            _current.type = TOK_SHL_ASSIGN;
                        else
                            _current.type = TOK_SHL;
                    else if(nextSymbol == '=')
                        _current.type = TOK_LE;
                    else
                        _current.type = TOK_L;
                    break;
                case '>':
                    if(nextSymbol == '<')
                        if(trySymbol(j + 2) == '=')
                            _current.type = TOK_SHR_ASSIGN;
                        else
                            _current.type = TOK_SHR;
                    else if(nextSymbol == '=')
                        _current.type = TOK_GE;
                    else
                        _current.type = TOK_G;
                    break;
                case '%':
                    _current.type =
                        nextSymbol == '=' ? TOK_MOD_ASSIGN :
                        TOK_MOD;
                    break;
                case '/':
                    _current.type =
                        nextSymbol == '=' ? TOK_DIV_ASSIGN :
                        TOK_DIV;
                    break;
                case '*':
                    _current.type =
                        nextSymbol == '=' ? TOK_MUL_ASSIGN :
                        TOK_STAR;
                    break;
                case '&':
                    _current.type =
                        nextSymbol == '&' ? TOK_LOGICAl_AND :
                        nextSymbol == '=' ? TOK_AND_ASSIGN :
                        TOK_AMP;
                    break;
                case '?': _current.type = TOK_QUEST; break;
                case '~': _current.type = TOK_TILDA; break;
                case '.': _current.type = TOK_DOT; break;
                case '[': _current.type = TOK_L_SQUARE; break;
                case ']': _current.type = TOK_R_SQUARE; break;
            };
            if(_current.type == TOK_OPER)
                dd; //no such token. Error. Error.

            j +=
                TOKEN_TYPE_NAME[_current.type].size() -
                TOKEN_TYPE_NAME[TOK_OPER].size() - 1;
            _state = IS_READ;
        }

        switch(_state)
        {
            case IS_NONE:
                _index++;
                break;
            case IS_READ:
                _current.text.assign(_buffer, _index, j - _index + 1);
                if(_current.type == TOK_IDENT)
                    checkKeywords();
                _current.line = _currentLine;
                _current.col = _index;
                _index = j + 1;
                _state = IS_MADE;
                break;
            case IS_LCOMMENT:
                if(!tryGetLine())
                {
                    makeEOFToken();
                    return;
                }
                j = -1;
                _state = IS_NONE;
                break;
            case IS_COMMENT:
                j += 1;
                while(_state == IS_COMMENT)
                {
                    while(j < _buffer.size())
                    {
                        if(_buffer[j] == '*' && j < _buffer.size() - 1 && _buffer[j + 1] == '/')
                        {
                            j += 2; // sure? 2, not 1?
                            _state = IS_NONE;
                            break;
                        }
                        j++;
                    }
                    if(_state == IS_COMMENT)
                    {
                        if(!tryGetLine())
                            throw UnexpectedEOFInComment();
                        j = 0;
                    }
                }
                break;
        };
        j++;
    }
    _state = IS_NONE;
}

Token& Tokenizer::get()
{
    return _current;
}

Token& Tokenizer::next()
{
    read();
    return _current;
}

Tokenizer::~Tokenizer()
{
    if(_source.is_open())
        _source.close();
}

void Tokenizer::bind(const string& filename)
{
    _source.open(filename.c_str());
}

}
