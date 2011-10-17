#include <map>
#include "token.h"
#include "operations.h"

namespace LuCCompiler
{

OperationGroups::OperationGroups()
{
    _binaryOps[TOK_ASTERISK] = 13; //*a!
    _binaryOps[TOK_DIV] = 13;
    _binaryOps[TOK_MOD] = 13;
    _binaryOps[TOK_PLUS] = 12; //+1
    _binaryOps[TOK_MINUS] = 12; //-1
    _binaryOps[TOK_SHL] = 11;
    _binaryOps[TOK_SHR] = 11;
    _binaryOps[TOK_L] = 10;
    _binaryOps[TOK_G] = 10;
    _binaryOps[TOK_LE] = 10;
    _binaryOps[TOK_GE] = 10;
    _binaryOps[TOK_EQUAL] = 9;
    _binaryOps[TOK_NOT_EQUAL] = 9;
    _binaryOps[TOK_AMP] = 8; //&readHere
    _binaryOps[TOK_XOR] = 7;
    _binaryOps[TOK_OR] = 6;
    _binaryOps[TOK_LOGICAl_AND] = 5;
    _binaryOps[TOK_LOGICAL_OR] = 4;
    //left to right

    //_unaryOps[(typename)] = 14;
    _unaryOps[TOK_ASTERISK] = 15;
    _unaryOps[TOK_AMP] = 15;
    _unaryOps[TOK_PLUS] = 15;
    _unaryOps[TOK_MINUS] = 15;
    _unaryOps[TOK_NOT] = 15;
    _unaryOps[TOK_TILDA] = 15;
    _unaryOps[TOK_SIZEOF] = 15;
    //right to left
}

OperationGroups::~OperationGroups()
{
    //_binaryOps.clear();
    //_unaryOps.clear();
}

}
