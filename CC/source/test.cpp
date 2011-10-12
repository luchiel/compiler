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

void runTests(string testBlock)
{
    Tester t;

    //path to tests
    string path(TEST_DIR + testBlock + '\\');
    LPWSTR wpath = (LPWSTR)malloc((path.size() + 1) * sizeof(WCHAR));
    MultiByteToWideChar(CP_ACP, 0, path.c_str(), path.size() + 1, wpath, path.size() + 1);

    //current path
    wchar_t wdir[FILENAME_MAX];
    _wgetcwd(wdir, MAX_PATH);
    wstring wfullpath(wdir);
    wfullpath.append(wpath);

    wstring tmp(wfullpath);
    tmp.append(L"*.in");

    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;

    hFind = FindFirstFile(tmp.c_str(), &FindFileData);
    do
    {
        if(hFind != INVALID_HANDLE_VALUE)
        {
            wstring filename(FindFileData.cFileName);
            t.attachTypelessFilename(wfullpath + filename);
            t.runFile();
            t.estimateResult();
        }
        else
            throw FindFileError();
    }
    while(FindNextFile(hFind, &FindFileData) || GetLastError() != ERROR_NO_MORE_FILES);
    FindClose(hFind);
}

void Tester::attachTypelessFilename(wstring& filename)
{
    _currentTest = filename.substr(0, filename.size() - 3);
}

void Tester::runFile()
{
    streambuf *backup;
    _outStream.open(_currentTest + L".log");
    if(!_outStream.good())
        throw BadFile();

    //redirect cout to outputfile
    backup = cout.rdbuf();
    cout.rdbuf(_outStream.rdbuf());

    Tokenizer t;
    t.bind(_currentTest + L".in");

    cout << "(line, col)\tType\t\tText, Value\n";

    while(t.get().type != TOK_EOF)
    {
        t.next().outputAsString(cout);
    }

    //restore stdout
    cout.rdbuf(backup);
    _outStream.close();
}

void Tester::estimateResult()
{
    ifstream outFile, logFile;
    outFile.open(_currentTest + L".out");
    logFile.open(_currentTest + L".log");

    outFile.close();
    logFile.close();
}

}
