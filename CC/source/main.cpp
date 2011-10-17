#include <cstdio>
#include <cstdlib>
#include <windows.h>
#include <string>
#include "test.h"
#include "tokenizer.h"

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
                printf("Do something\n");
            else
                runTests(argv[2]);
        }
        else
        {
            Tokenizer t(argv[1]);

            printf("(line, col)\t\tType\t\tText, Value\n");

            while(t.get().type != TOK_EOF)
            {
                try
                {
                    t.next().outputAsString(cout);
                }
                catch(exception& e)
                {
                    cout << "Error: " << e.what() << endl;
                    break;
                }
            }
        }
    }

    return 0;
}
