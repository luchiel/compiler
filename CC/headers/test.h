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
    std::ofstream outStream;
public:
    Tester() {}
    void RunFile(std::string filename);

    //class FILE_NOT_FOUND: public std::exception {};
};

void RunTests(string testBlock);

}

#endif
