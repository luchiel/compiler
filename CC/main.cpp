#include <cstdio>
#include <cstdlib>
#include <windows.h>
#include <string>
#include "headers/test.h"
#include "headers/tokenizer.h"

using namespace LuCCompiler;

/*void Tokenize()
{
    Tokenizer t;
    t.bind("first.c");

    printf("(line, col)\tType\t\tText, Value\n");

    while(t.get().type != TOK_EOF)
    {
        t.next().outputAsString(cout);
    }
}*/

int main(int argc, char *argv[])
{
    if(argc == 1)
    {
        printf("%s\n", "No args found. You should enter some filename or --test to test the application");
        //Tokenize();
    }
    else if(argc > 1)
    {
        if(!strcmp(argv[1], "--test"))
        {
            if(argc == 1)
                printf("%s\n", "Do something");
            else
                RunTests(string(argv[2]));
        }
        else
        {
            Tokenizer t;
            t.bind(argv[1]);    //string?

            printf("(line, col)\tType\t\tText, Value\n");

            while(t.get().type != TOK_EOF)
            {
                t.next().outputAsString(cout);
            }
        }
    }
    return 0;
}
