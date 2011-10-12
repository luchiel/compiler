#include <string>
#include "../headers/token.h"

using namespace std;

namespace LuCCompiler
{

void Token::outputAsString()
{
    printf(
        "(%d, %d)\t\t%s\t\t%s",
        line, col + 1,
        TOKEN_TYPE_NAME[type].c_str(),
        text.c_str()
    );
    if(type == TOK_DEC_CONST || type == TOK_OCT_CONST || type == TOK_HEX_CONST)
        printf(", %d\n", value.intValue);
    else if(type == TOK_STR_CONST || type == TOK_CHAR_CONST)
        printf(", %s\n", *(value.strValue));
    else if(type == TOK_FLOAT_CONST)
        printf(", %f\n", value.floatValue);
    else
        printf("\n");
}

}
