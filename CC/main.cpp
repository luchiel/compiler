#include <cstdio>
#include <cstdlib>
#include <windows.h>
#include <string>
#include "headers/test.h"
#include "headers/tokenizer.h"

using namespace LuCCompiler;

int main(int argc, char *argv[])
{
    if(argc == 1)
    {
        printf("%s\n", "No args found. You should enter some filename or --test to test the application");
    }
    else if(argc > 1)
    {
        if(!strcmp(argv[1], "--test"))
        {
            if(argc == 1)
                printf("%s\n", "Do something");
            else
                runTests(string(argv[2]));
        }
        else
        {
            Tokenizer t;
            t.bind(argv[1]);    //string?

            printf("(line, col)\t\tType\t\tText, Value\n");

            while(t.get().type != TOK_EOF)
            {
                t.next().outputAsString(cout);
            }
        }
    }
/*
\' \" \? \\
\a \b \f \n \r \t \v
\" == "
\? == ?

//and ignore that weird slash behavior - not in STD

+ignore hex/octal? ignore hex float

+add test error: line 1, 1 do not look alike or smth
*/

    return 0;
}
