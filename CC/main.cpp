#include <cstdio>
#include <cstdlib>
#include <string>
#include "test.h"
#include "tokenizer.h"

int main(int argc, char *argv[])
{
    if(argc == 1)
        printf("%s\n", "No args found. You should enter some filename or --test to test the application");
    else if(argc > 1)
    {
        if(argv[1] == "--test")
            RunTests();
    }
    return 0;
}
