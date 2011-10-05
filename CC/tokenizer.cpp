#include <string>
#include <set>
#include <cctype>
#include "token.h"
#include "tokenizer.h"

void dd() {}

const string OPERATIONS = "+-=/*><%&^|!~.?:[]";

void Tokenizer::readStr(int idx)
{
    _current.type = TOK_STR;
    _current.value.strValue = new string("");
    _current.col = idx;
    _current.line = _currentLine;

    bool isOpened = false;
    int startText = idx;

    while(idx < _buffer.size())
    {
        if(_buffer[idx] == '"')
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
        {
            if(isOpened)
                throw NewlineInConstantString();
            else
                if(!tryGetLine())
                    break;
                else
                    idx = 0;
        }
    }

    _current.text = '"' + *(_current.value.strValue) + '"';
    _index = idx;
    _state = IS_MADE;
}

void Tokenizer::readChar(int idx)
{
}

void Tokenizer::makeEOFToken()
{
    _current.text = "";
    _current.type = TOK_EOF;
    _current.line = _currentLine;
    _current.col = 0;
    //_state = IS_READ;
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

void Tokenizer::read()
{
    set<char> ops;
    for(unsigned int i = 0; i < OPERATIONS.size(); ++i)
        ops.insert(OPERATIONS[i]);
    //to the create

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
    int j = _index;
    while(_state != IS_MADE)
    {
        switch(_current.type)
        {
            case TOK_UNDEF:
                switch(_buffer[j])
                {
                    case '{':   setTypeAndReadState(TOK_L_BRACE); break;
                    case '}':   setTypeAndReadState(TOK_R_BRACE); break;
                    case '(':   setTypeAndReadState(TOK_L_BRACKET); break;
                    case ')':   setTypeAndReadState(TOK_R_BRACKET); break;
                    case '"':   readStr(j); break;
                    case '\'':  readChar(j + 1); setTypeAndReadState(TOK_CHAR); break;
                    case ';':   setTypeAndReadState(TOK_SEP); break;
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
                        else if(ops.find(_buffer[j]) != ops.end())
                            _current.type = TOK_OPER;
                };
                break;
            case TOK_IDENT:
                break;
            case TOK_INT:
                break;
            case TOK_FLOAT:
                break;
            case TOK_SEP:
                break;
            case TOK_OPER:
                break;
            case TOK_EOF:
                break;
        }
        switch(_state)
        {
            case IS_NONE:
                break;
            case IS_READ:
                _current.text.assign(_buffer, _index, j - _index + 1);
                _current.line = _currentLine;
                //works for 1-line only. How 'bout strings?
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
            case IS_EOL:
                break;
        }
        j++;
        if(j >= _buffer.size())
        {
            if(!tryGetLine())
            {
                makeEOFToken();
                return;
            }
            j = 0;
        }
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
