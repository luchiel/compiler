#include <string>
#include <sstream>
#include <cctype>
#include "../headers/token.h"
#include "../headers/tokenizer.h"

namespace LuCCompiler
{

void dd() {}

void Tokenizer::readStr(unsigned int idx)
{
    _current.type = TOK_STR_CONST;
    _current.value.strValue = new string("");
    _current.col = idx;
    _current.line = _currentLine;

    bool isOpened = false;
    unsigned int startText = idx;

    while(idx < _buffer.size())
    {
        if(_buffer[idx] == '\\')
        {
            if(idx + 1 == _buffer.size())
            {
                string tmp;
                tmp.assign(_buffer, startText + 1, idx - startText - 1);
                *(_current.value.strValue) = *(_current.value.strValue) + tmp;
                if(tryGetLine())
                    startText = -1;
                else
                    throw NewlineInConstantString();
            }
            else
            {
                idx += 2;
                if(idx == _buffer.size())
                    throw NewlineInConstantString();
            }
        }
        else if(_buffer[idx] == '"')
        {
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

    processEscapeSequencesInStrValue();
    _current.text = '"' + *(_current.value.strValue) + '"';
    _index = idx;
    _state = IS_MADE;
}

void Tokenizer::processEscapeSequencesInStrValue()
{
    string& val = *_current.value.strValue;
    unsigned int j = 0;
    for(unsigned int i = 0; i < val.size(); ++i, ++j)
    {
        //"...\" is not possible. It has \\" or it is not the end of the string
        if(val[i] == '\\')
        {
            i++;
            switch(val[i])
            {
                case 'n':   val[j] = '\n'; break;
                case 't':   val[j] = '\t'; break;
                case '\'':  val[j] = '\''; break;
                case '"':   val[j] = '\"'; break;
                case '?':   val[j] = '\?'; break;
                case '\\':  val[j] = '\\'; break;
                case 'a':   val[j] = '\a'; break;
                case 'b':   val[j] = '\b'; break;
                case 'f':   val[j] = '\f'; break;
                case 'r':   val[j] = '\r'; break;
                case 'v':   val[j] = '\v'; break;
                default:
                    throw InvalidEscapeSequence();
                // \' \" \? \\ \a \b \f \n \r \t \v \" == " \? == ?
            }
        }
        else
            val[j] = val[i];
    }
    val.resize(j);
}

void Tokenizer::readChar(unsigned int& idx)
{
    _current.col = idx;
    _current.line = _currentLine;
    _current.type = TOK_CHAR_CONST;
    _current.value.strValue = new string("");
    idx++;
    if(idx + 1 >= _buffer.size())//closing quote
        throw NewlineInConstantChar();
    if(_buffer[idx] == '\\')
    {
        idx++;
        if(idx + 1 >= _buffer.size())
            throw NewlineInConstantChar();
        if(_buffer[idx + 1] != '\'')
            throw MulticharacterConstantChar();
        *(_current.value.strValue) += '\\';
    }
    else
        if(_buffer[idx + 1] != '\'')
            throw MulticharacterConstantChar();
    *(_current.value.strValue) += _buffer[idx];

    processEscapeSequencesInStrValue();
    _current.text = '\'' + *(_current.value.strValue) + '\'';

    idx++;
    _index = idx + 1;
    _state = IS_MADE;
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
        else if(isdigit(nextSymbol))
        {
            _current.type = TOK_OCT_CONST;
            idx++;
            notation = 8;
        }
    }
    while(idx < _buffer.size() && (
        _current.type == TOK_OCT_CONST && isdigit(_buffer[idx]) ||
        _current.type == TOK_DEC_CONST && isdigit(_buffer[idx]) ||
        _current.type == TOK_HEX_CONST && isxdigit(_buffer[idx])
    ))
	{
        if(_current.type == TOK_OCT_CONST && !isOctDigit(_buffer[idx]))
            throw BadDigitInOctalConst();
        _current.value.intValue =
            _current.value.intValue * notation + getIntValue(_buffer[idx++]);
	}

    if(idx < _buffer.size() && _current.type == TOK_DEC_CONST && (
        _buffer[idx] == 'e' || _buffer[idx] == 'E' || _buffer[idx] == '.'
    ))
    {
        if(!tryReadFloatPart(idx, true))
            throw InvalidFloatingPointConstant();
    }
    else
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

bool Tokenizer::tryReadFloatPart(unsigned int& idx, bool hasIntPart)
{
    string floatPart;
    string intPart(
        hasIntPart ? _current.text.assign(_buffer, _index, idx - _index) : "");
    if(_buffer[idx] == '.')
    {
        floatPart += '.';
        idx++;
        while(idx < _buffer.size() && isdigit(_buffer[idx]))
            floatPart += _buffer[idx++];
    }
    if(!hasIntPart && floatPart.size() == 1)
    {
        idx--;
        return false;
    }
    
    if(idx < _buffer.size() && (_buffer[idx] == 'e' || _buffer[idx] == 'E'))
    {
        floatPart += _buffer[idx];
        idx++;
        if(idx == _buffer.size())
            throw InvalidFloatingPointConstant();
        if(_buffer[idx] == '+' || _buffer[idx] == '-')
        {
            floatPart += _buffer[idx];
            idx++;
            if(idx == _buffer.size())
                throw InvalidFloatingPointConstant();
        }
        while(idx < _buffer.size() && isdigit(_buffer[idx]))
            floatPart += _buffer[idx++];
        if(!isdigit(floatPart[floatPart.size() - 1]))
            throw InvalidFloatingPointConstant();
    }

    idx--;
    _current.type = TOK_FLOAT_CONST;
    floatPart = intPart + floatPart;
    stringstream ss(floatPart);
    ss >> _current.value.floatValue;
    return true;
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
        _index = 0;
    }
    while(_buffer.size() == 0);
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
            case '\'':  readChar(j); break;
            case ';':   setTypeAndReadState(TOK_SEP); break;
            case ':':   setTypeAndReadState(TOK_COLON); break;
            case ',':   setTypeAndReadState(TOK_COMMA); break;
            default:
                if(isalpha(_buffer[j]) || _buffer[j] == '_')
                    readIdentifier(j);
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
                    if(nextSymbol == '>')
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
                        TOK_ASTERISK;
                    break;
                case '&':
                    _current.type =
                        nextSymbol == '&' ? TOK_LOGICAl_AND :
                        nextSymbol == '=' ? TOK_AND_ASSIGN :
                        TOK_AMP;
                    break;
                case '?': _current.type = TOK_QUEST; break;
                case '~': _current.type = TOK_TILDA; break;
                case '.':
                    if(!tryReadFloatPart(j, false))
                        _current.type = TOK_DOT;
                    break;
                case '[': _current.type = TOK_L_SQUARE; break;
                case ']': _current.type = TOK_R_SQUARE; break;
            };
            if(_current.type == TOK_OPER)
                throw UnknownOperation();
            if(_current.type != TOK_FLOAT_CONST)
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
                            j += 2;
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
    if(!_source.good())
        throw BadFile();
}

/*void Tokenizer::bind(const wstring& filename)
{
    _source.open(filename);
    if(!_source.good())
        throw BadFile();
}*/

}
