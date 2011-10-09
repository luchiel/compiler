#include <string>
#include "token.h"

using namespace std;

void Token::outputAsString(ostream& outStream)
{
    printf("(%d, %d)\t\t%s\t\t%s, %s\n", line, col + 1, TOKEN_TYPE_NAME[type].c_str(), text.c_str(), "");//value);
}
