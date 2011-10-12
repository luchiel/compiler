#include <string>
#include "../headers/token.h"

using namespace std;

namespace LuCCompiler
{

void Token::outputAsString(ostream& outStream)
{
    outStream << "(" << line << ", " << col + 1 << ")";
    outStream << "\t\t" << TOKEN_TYPE_NAME[type];
    if(!text.empty())
        outStream << "\t\t" << text;

    if(type == TOK_DEC_CONST || type == TOK_OCT_CONST || type == TOK_HEX_CONST)
        outStream << ", " << value.intValue;
    else if(type == TOK_STR_CONST || type == TOK_CHAR_CONST)
        outStream << ", " << value.strValue;
    else if(type == TOK_FLOAT_CONST)
        outStream << ", " << value.floatValue;
    outStream << endl;
}

}
