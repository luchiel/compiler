#include <cstdio>
#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include "../headers/test.h"

using namespace std;

namespace LuCCompiler
{

const string TEST_DIR = "tests/";

const string OUTPUT = "output.txt";

void RunTests(string testBlock)
{
    // there will be some selection between tests, some input parameter etc
    // log to the text file? arg!

    Tester t;

    string path(TEST_DIR + testBlock + '/');
    LPWSTR wpath = (LPWSTR)malloc((path.size() + 1) * sizeof(WCHAR));
    MultiByteToWideChar(CP_ACP, 0, path.c_str(), path.size() + 1, wpath, path.size() + 1);

    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;

    hFind = FindFirstFile(wpath, &FindFileData);
    do
    {
        if(hFind == INVALID_HANDLE_VALUE)
        {
            //error
            //printf("FindFirstFile failed (%d)\n", GetLastError());
            return;
        }
        else
        {
            t.RunFile(path + FindFileData.cFileName);
        }
    }
    while(FindNextFile(hFind, &FindFileData) || GetLastError() != ERROR_NO_MORE_FILES);
    FindClose(hFind);*/
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

}
