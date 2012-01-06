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
    ofstream _errStream;
    int testsCount;
    int testsPassed;
    streambuf *backup, *ebackup;
    void redirectStreams(string& testBlock);
    void restoreStreams();

public:
    bool optimized;
    Tester(bool opt_ = false): testsCount(0), testsPassed(0), optimized(opt_) {}
    void attachTypelessFilename(wstring filename);
    void runFile(string& testBlock);
    void estimateResult();
    void outputGlobalResult();
};

class FindFileError: public exception {};

void runTests(string testBlock, bool tagOptimized);

}

#endif
