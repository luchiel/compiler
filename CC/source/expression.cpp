#include <stdio.h>
#include <iostream>
#include <vector>
#include "expression.h"
#include "operations.h"

using namespace std;

namespace LuCCompiler
{

void IdentNode::out(unsigned int depth, vector<bool>* branches, int level)
{
    printIndent(depth, branches, level);
    cout << (depth == 0 ? "" : "+-") << "{" << _name << "}" << endl;
}

void StringNode::out(unsigned int depth, vector<bool>* branches, int level)
{
    printIndent(depth, branches, level);
    cout << (depth == 0 ? "" : "+-") << "{string " << _value << "}" << endl;
}

void CharNode::out(unsigned int depth, vector<bool>* branches, int level)
{
    printIndent(depth, branches, level);
    cout << (depth == 0 ? "" : "+-") << "{char " << _value << "}" << endl;
}

void IntNode::out(unsigned int depth, vector<bool>* branches, int level)
{
    printIndent(depth, branches, level);
    cout << (depth == 0 ? "" : "+-") << "{int " << _value << "}" << endl;
}

void FloatNode::out(unsigned int depth, vector<bool>* branches, int level)
{
    printIndent(depth, branches, level);
    cout << (depth == 0 ? "" : "+-") << "{float " << _value << "}" << endl;
}

void PostfixNode::out(unsigned int depth, vector<bool>* branches, int level)
{
    string x(_type == TOK_INC || _type == TOK_DEC ? "x" : "");
    makeNodeTop(depth, branches, x + operationName(_type), level);
    setBranch(depth, branches);

    printIndentBeforeNode(depth, branches, level);

    if(_tail == NULL)
        (*branches)[depth] = true;

    _only->out(depth + 1, branches, level);

    if(_tail == NULL)
        return;

    printNodeWithIndent(depth, branches, true, _tail, level);
}

void UnaryNode::out(unsigned int depth, vector<bool>* branches, int level)
{
    string x(_type == TOK_INC || _type == TOK_DEC ? "x" : "");
    makeNodeTop(depth, branches, operationName(_type) + x, level);
    printNodeWithIndent(depth, branches, true, _only, level);
}

void BinaryNode::out(unsigned int depth, vector<bool>* branches, int level)
{
    makeNodeTop(depth, branches, operationName(_type), level);
    printNodeWithIndent(depth, branches, false, _left, level);
    printNodeWithIndent(depth, branches, true, _right, level);
}

void TernaryNode::out(unsigned int depth, vector<bool>* branches, int level)
{
    makeNodeTop(depth, branches, operationName(_type), level);

    printNodeWithIndent(depth, branches, false, _if, level);
    printNodeWithIndent(depth, branches, false, _then, level);
    printNodeWithIndent(depth, branches, true, _else, level);
}

void CastNode::out(unsigned int depth, vector<bool>* branches, int level)
{
    makeNodeTop(depth, branches, "cast", level);

    printIndentBeforeNode(depth, branches, level);
    printIndent(depth + 1, branches, level);
    cout << "+-<" << type->name << ">\n";
    printNodeWithIndent(depth, branches, true, element, level);
}

void SizeofNode::out(unsigned int depth, vector<bool>* branches, int level)
{
    makeNodeTop(depth, branches, operationName(_type), level);
    if(_only != NULL)
        printNodeWithIndent(depth, branches, true, _only, level);
    else
    {
        printIndentBeforeNode(depth, branches, level);
        printIndent(depth + 1, branches, level);
        cout << "+-<" << _symbolType->name << ">\n";
    }
}

}
