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

    t.outputGlobalResult();
}

void Tester::attachTypelessFilename(wstring& filename)
{
    char ctmp[MAX_PATH];
    wstring wtmp = filename.substr(0, filename.size() - 3);
    int l = WideCharToMultiByte(CP_ACP, 0, wtmp.c_str(), wtmp.size(), ctmp, 0, NULL, NULL);
    WideCharToMultiByte(CP_ACP, 0, wtmp.c_str(), wtmp.size(), ctmp, l, NULL, NULL);
    _currentTest.clear();
    _currentTest.append(ctmp, l);
}

void Tester::runFile()
{
    streambuf *backup;
    _outStream.open(_currentTest + ".log");
    if(!_outStream.good())
        throw BadFile();

    //redirect cout to outputfile
    backup = cout.rdbuf();
    cout.rdbuf(_outStream.rdbuf());

    Tokenizer t;
    t.bind(_currentTest + ".in");

    cout << "(line, col)\t\tType\t\tText, Value\n";

    while(t.get().type != TOK_EOF)
    {
        t.next().outputAsString(cout);
    }

    //restore stdout
    cout.rdbuf(backup);
    _outStream.close();

    testsCount++;
}

void Tester::estimateResult()
{
    ifstream outFile, logFile;
    outFile.open(_currentTest + ".out");
    logFile.open(_currentTest + ".log");

    bool result = true;
    string oS, lS;
    while(!outFile.eof() && !logFile.eof())
    {
        getline(outFile, oS);
        getline(logFile, lS);
        if(oS != lS)
        {
            result = false;
            break;
        }
    }

    if(!outFile.eof() || !logFile.eof())
        result = false;

    if(result)
    {
        cout << "[OK] Test " << _currentTest.c_str() << ".in" << endl;
        testsPassed++;
    }
    else
        cout << "[FAIL] Test " << _currentTest.c_str() << ".in" << endl;

    outFile.close();
    logFile.close();
}

void Tester::outputGlobalResult()
{
    cout << "_______________________________________" << endl;
    cout << "Result: "<< testsPassed << '/' << testsCount << " passed" << endl;
}

}
