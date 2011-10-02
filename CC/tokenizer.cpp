#include <string>
#include <set>
#include <cctype>
#include "token.h"
#include "tokenizer.h"

void dd() {}

const string OPERATIONS = "+-=/*><%&^|!~.?:[]";

bool Tokenizer::tryGetLine()
{
    getline(_source, _buffer);
    _currentLine++;
    if(_source.eofbit || _source.failbit)
    {
        _current.type = TOK_EOF;
        _current.line = _currentLine;
        _current.col = 0;
        _state = IS_READ;
        return false;
    }
    else if(_source.badbit)
    {
        throw ReadError();
    }
    return true;
}

void Tokenizer::read()
{
    set<char> ops;
    for(unsigned int i = 0; i < OPERATIONS.size(); ++i)
        ops.insert(OPERATIONS[i]);

    if(_buffer.size() == 0 || _index >= _buffer.size())
    {
        if(!tryGetLine())
            return;
        _index = 0;
    }

    _current = Token();

    unsigned int cloneOfIndex = _index;
    while(_state != IS_READ)
    {
        unsigned int i = _index;

        switch(_current.type)
        {
            case TOK_UNDEF:
                switch(_buffer[i])
                {
                    case '{':   _current.type = TOK_L_BRACE;     _state = IS_READ; break;
                    case '}':   _current.type = TOK_R_BRACE;     _state = IS_READ; break;
                    case '(':   _current.type = TOK_L_BRACKET;   _state = IS_READ; break;
                    case ')':   _current.type = TOK_R_BRACKET;   _state = IS_READ; break;
                    case '"':   _current.type = TOK_STR;         _state = IS_READ; break;
                    case '\'':  _current.type = TOK_CHAR;        _state = IS_READ; break;
                    case ';':   _current.type = TOK_SEP;         _state = IS_READ; break;
                    case ',':   _current.type = TOK_COMMA;       _state = IS_READ; break;
                    default:
                        if(isalpha(_buffer[i]) || _buffer[i] == '_')
                            _current.type = TOK_IDENT;
                        else if(isspace(_buffer[i]))
                            break;
                        else if(isdigit(_buffer[i]))
                            _current.type = TOK_INT;
                        else if(_buffer[i] == '/' && i < _buffer.size() - 1)
                        {
                            _state = _buffer[i + 1] == '/' ? IS_LCOMMENT : _buffer[i + 1] == '*' ? IS_COMMENT : IS_NONE;
                            if(_state == IS_NONE)
                                _current.type = TOK_OPER;
                        }
                        else if(ops.find(_buffer[i]) != ops.end())
                            _current.type = TOK_OPER;
                };
                break;
            case TOK_IDENT:
                break;
            case TOK_INT:
                break;
            case TOK_FLOAT:
                break;
            case TOK_CHAR:
                break;
            case TOK_STR:
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
            case IS_NONE: break;
            case IS_READ:
                _current.text.assign(_buffer, _index, i - _index + 1) ;
                _current.line = _currentLine;
                _current.col = _index;
                _index = i + 1;
                break;
            case IS_LCOMMENT:
                if(!tryGetLine())
                    return;
                i = -1;
                _state = IS_NONE;
                break;
            case IS_COMMENT:
                i += 1;
                while(_state == IS_COMMENT)
                {
                    while(i < _buffer.size())
                    {
                        if(_buffer[i] == '*' && i < _buffer.size() - 1 && _buffer[i + 1] == '/')
                        {
                            i += 1;
                            _state = IS_NONE;
                            break;
                        }
                        i++;
                    }
                    if(_state == IS_COMMENT)
                    {
                        if(!tryGetLine())
                            throw UnexpectedEOFInComment();
                        i = 0;
                    }
                }
                break;
            case IS_EOL: break;
        }
        i++;
        if(i >= _buffer.size())
        {
            if(!tryGetLine())
                return;
            i = 0;
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
