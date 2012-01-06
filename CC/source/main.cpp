#include <cstdio>
#include <cstdlib>
#include <windows.h>
#include <string>
#include "test.h"
#include "tokenizer.h"
#include "parser.h"
#include "expression.h"
#include "generator.h"

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
            if(argc == 2)
                printf("What block would you like to test?\n");
            else
                runTests(argv[2], argc == 4 && !strcmp(argv[3], "+o"));
        }
        else
        {
            try
            {
                Tokenizer t(argv[1]);
                Parser p(&t);
                Generator g(p.parse());
                g.generate();
                g.out();
            }
            catch(exception& e)
            {
                cout << "Error: " << e.what() << endl;
            }
        }
    }

    return 0;
}
