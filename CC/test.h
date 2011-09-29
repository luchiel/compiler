#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>

class Tester
{
private:
    std::ofstream outStream;
public:
    Tester() {}
    void RunFile(std::string filename);

    //class FILE_NOT_FOUND: public std::exception {};
};

void RunTests();