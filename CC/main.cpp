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
        printf(
            "No args found. You should enter some filename "
            "or --test <test_option> to test the application\n"
        );
    }
    else if(argc > 1)
    {
        if(!strcmp(argv[1], "--test"))
        {
            if(argc == 1)
                printf("%s\n", "Do something");
            else
                runTests(argv[2]);
        }
        else
        {
            Tokenizer t;
            t.bind(argv[1]);

            printf("(line, col)\t\tType\t\tText, Value\n");

            while(t.get().type != TOK_EOF)
            {
                try
                {
                    t.next().outputAsString(cout);
                }
                catch(exception& e)
                {
                    cout << "Exception caught: " << e.what() << endl;
                    break;
                }
            }
        }
    }
/*
+ignore hex/octal? ignore hex float

+add test error: line 1, 1 do not look alike or smth
*/

    return 0;
}
