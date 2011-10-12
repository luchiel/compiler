#include <cstdio>
#include <windows.h>

#include <direct.h>
#include <cstdlib>

#include <iostream>
#include <string>
#include <fstream>
#include "../headers/test.h"
#include "../headers/tokenizer.h"

using namespace std;

namespace LuCCompiler
{

const string TEST_DIR = "\\tests\\";

void RunTests(string testBlock)
{
    Tester t;

    //path to tests
    string path(TEST_DIR + testBlock + '\\');
    LPWSTR wpath = (LPWSTR)malloc((path.size() + 1) * sizeof(WCHAR));
    MultiByteToWideChar(CP_ACP, 0, path.c_str(), path.size() + 1, wpath, path.size() + 1);

    //current path
    wchar_t wdir[FILENAME_MAX];
    _wgetcwd(wdir, MAX_PATH);
    wstring ws(wdir);
    ws.append(wpath);
    ws.append(L"*.in");

    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;

    hFind = FindFirstFile(ws.c_str(), &FindFileData);
    do
    {
        if(hFind != INVALID_HANDLE_VALUE)
        {
            wstring filename(FindFileData.cFileName);
            //t.RunFile(/*wpath, */);
            //cmp .in.log with .out
        }
        else
            throw FindFileError();
    }
    while(FindNextFile(hFind, &FindFileData) || GetLastError() != ERROR_NO_MORE_FILES);
    FindClose(hFind);
}

void Tester::RunFile(std::string filename)
{
    streambuf *file, *backup;
    outStream.open(string(filename + ".log").c_str());

    //redirect stdout to outputfile
    backup = cout.rdbuf();
    file = outStream.rdbuf();
    cout.rdbuf(file);

    Tokenizer t;
    t.bind(filename);

    printf("(line, col)\tType\t\tText, Value\n");

    while(t.get().type != TOK_EOF)
    {
        t.next().outputAsString(cout);
    }

    //restore stdout
    cout.rdbuf(backup);
    outStream.close();
}

}
