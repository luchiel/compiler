#include <string>
#include "../headers/token.h"

using namespace std;

void Token::outputAsString(ostream& outStream)
{
    printf(
        "(%d, %d)\t\t%s\t\t%s",
        line, col + 1,
        TOKEN_TYPE_NAME[type].c_str(),
        text.c_str());//value);
    if(type == TOK_DEC_CONST || type == TOK_OCT_CONST || type == TOK_HEX_CONST)
        printf(", %d\n", value.intValue);
    else if(type == TOK_STR || type == TOK_CHAR)
        printf(", %s\n", *(value.strValue));
    else if(type == TOK_FLOAT)
        printf(", %f\n", value.floatValue);
    else
        printf("\n");
}
