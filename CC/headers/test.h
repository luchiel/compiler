#ifndef TEST_H
#define TEST_H

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

namespace LuCCompiler
{

class Tester
{
private:
    string _currentTest;
    ofstream _outStream;
    int testsCount;
    int testsPassed;

public:
    Tester(): testsCount(0), testsPassed(0) {}
    void attachTypelessFilename(wstring filename);
    void runFile(string& testBlock);
    void estimateResult();
    void outputGlobalResult();
};

class FindFileError: public exception {};

void runTests(string testBlock);

}

#endif
