#include <string>
#include <cctype>
#include "../headers/token.h"
#include "../headers/tokenizer.h"

void dd() {}

//const string a = "dsdg\
//                 dsfdf";

void Tokenizer::readStr(unsigned int idx)
{
    _current.type = TOK_STR;
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

void Tokenizer::makeEOFToken()
{
    _current.text = "";
    _current.type = TOK_EOF;
    _current.line = _currentLine;
    _current.col = 0;
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
        _index = 0;
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
            case '\'':  readChar(j + 1); setTypeAndReadState(TOK_CHAR); break;
            case ';':   setTypeAndReadState(TOK_SEP); break;
            case ':':   setTypeAndReadState(TOK_COLON); break;
            case ',':   setTypeAndReadState(TOK_COMMA); break;
            default:
                if(isalpha(_buffer[j]) || _buffer[j] == '_')
                    _current.type = TOK_IDENT;
                else if(isspace(_buffer[j]))
                    break;
                else if(isdigit(_buffer[j]))
                    _current.type = TOK_INT;
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
            char next = trySymbol(j + 1);
            switch(_buffer[j])
            {
                case '-':
                    switch(next)
                    {
                        case '-': _current.type = TOK_DEC; break;
                        case '=': _current.type = TOK_SUB_ASSIGN; break;
                        case '>': _current.type = TOK_ARROW; break;
                        default: _current.type = TOK_MINUS; break;
                    };
                    break;
                case '+':
                    _current.type =
                        next == '+' ? TOK_INC :
                        next == '=' ? TOK_ADD_ASSIGN :
                        TOK_PLUS;
                    break;
                case '=':
                    _current.type = next == '=' ? TOK_EQUAL : TOK_ASSIGN;
                    break;
                case '!':
                    _current.type =
                        trySymbol(j + 1) == '=' ?
                        TOK_NOT_EQUAL : TOK_NOT;
                    break;
                case '|':
                    _current.type =
                        next == '||' ? TOK_LOGICAL_OR :
                        next == '=' ? TOK_OR_ASSIGN :
                        TOK_OR;
                    break;
                case '^':
                    _current.type = next == '=' ? TOK_XOR_ASSIGN : TOK_XOR;
                    break;
                case '<':
                    if(next == '<')
                        if(trySymbol(j + 2) == '=')
                            _current.type = TOK_SHL_ASSIGN;
                        else
                            _current.type = TOK_SHL;
                    else if(next == '=')
                        _current.type = TOK_LE;
                    else
                        _current.type = TOK_L;
                    break;
                case '>':
                    if(next == '<')
                        if(trySymbol(j + 2) == '=')
                            _current.type = TOK_SHR_ASSIGN;
                        else
                            _current.type = TOK_SHR;
                    else if(next == '=')
                        _current.type = TOK_GE;
                    else
                        _current.type = TOK_G;
                    break;
                case '%':
                    _current.type =
                        next == '=' ? TOK_MOD_ASSIGN :
                        TOK_MOD;
                    break;
                case '/':
                    _current.type =
                        next == '=' ? TOK_DIV_ASSIGN :
                        TOK_DIV;
                    break;
                case '*':
                    _current.type =
                        next == '=' ? TOK_MUL_ASSIGN :
                        TOK_STAR;
                    break;
                case '&':
                    _current.type =
                        next == '&' ? TOK_LOGICAl_AND :
                        next == '=' ? TOK_AND_ASSIGN :
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
            case IS_READ:
                _current.text.assign(_buffer, _index, j - _index + 1);
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