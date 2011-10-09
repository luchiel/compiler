#include <cstdio>
#include <cstdlib>
#include <string>
#include "headers/test.h"
#include "headers/tokenizer.h"

void Tokenize()
{
    Tokenizer t;
    t.bind("first.c");

    printf("(line, col)\tType\t\tText, Value\n");

    while(t.get().type != TOK_EOF)
    {
        t.next().outputAsString(cout);
    }
}

int main(int argc, char *argv[])
{
    if(argc == 1)
    {
        printf("%s\n", "No args found. You should enter some filename or --test to test the application");
        Tokenize();
    }
    else if(argc > 1)
    {
        if(strcmp(argv[1], "--test"))
            RunTests();
        else
        {
            Tokenizer t;
            t.bind(argv[1]);

            printf("(line, col)\tType\t\tText, Value\n");

            while(t.get().type != TOK_EOF)
            {
                t.next().outputAsString(cout);
            }
        }
    }
    return 0;
}
