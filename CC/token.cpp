#include <string>
#include "token.h"

using namespace std;

string& Token::asString()
{
    char* cresult = "";
    sprintf(cresult, "%s\t(%d, %d)\t%s\t%s\n", text, line, col, TOKEN_TYPE_NAME[type], value);
    string& sresult = string(cresult);
    return sresult;
}
