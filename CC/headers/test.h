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
    ofstream outStream;
public:
    Tester() {}
    void RunFile(wstring& filename);
};

class FindFileError: public exception {};

void RunTests(string testBlock);

}

#endif
