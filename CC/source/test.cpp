#include <cstdio>
#include <iostream>
#include <string>
#include <fstream>
#include "../headers/test.h"

using namespace std;

const string TEST_DIR = "tests/";

const string OUTPUT = "output.txt";

const int TOKENIZER_TESTS_NUM = 1;
const string TOKENIZER_TESTS[1] = { "empty.c" };
// indexes, but way to directory!

void RunTests()
{
    // there will be some selection between tests, some input parameter etc
    // log to the text file? arg!
    Tester t;
    for(int i = 0; i < TOKENIZER_TESTS_NUM; ++i)
        t.RunFile(TEST_DIR + TOKENIZER_TESTS[i]);
}

void Tester::RunFile(std::string filename)
{
    streambuf *file, *backup;
    outStream.open(OUTPUT.c_str());

    backup = cout.rdbuf();     // back up cout's streambuf
    file = outStream.rdbuf();   // get file's streambuf
    cout.rdbuf(file);         // assign streambuf to cout

    cout << "This is written to the file";
  
    cout.rdbuf(backup);        // restore cout's original streambuf

    outStream.close();
}
