#include <cstdio>
#include <cstdlib>
#include <string>
#include "test.h"
#include "tokenizer.h"

void Tokenize()
{
    Tokenizer t;
    t.bind("first.c");

    printf("Text\t(line, col)\tType\tValue\n");

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

            printf("Text\t(line, col)\tType\tValue\n");

            while(t.get().type != TOK_EOF)
            {
                t.next().outputAsString(cout);
            }
        }
    }
    return 0;
}
