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
    wstring _currentTest;
    ofstream _outStream;
public:
    Tester() {}
    void attachTypelessFilename(wstring& filename);
    void runFile();
    void estimateResult();
};

class FindFileError: public exception {};

void runTests(string testBlock);

}

#endif
