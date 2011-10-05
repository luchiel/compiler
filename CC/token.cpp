#include <string>
#include "token.h"

using namespace std;

void Token::outputAsString(ostream& outStream)
{
    printf("%s\t(%d, %d)\t%s\t%s\n", text.c_str(), line, col + 1, TOKEN_TYPE_NAME[type].c_str(), "");//value);
}
