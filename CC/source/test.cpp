#include <cstdio>
#include <windows.h>

#include <direct.h>
#include <cstdlib>

#include <iostream>
#include <string>
#include <fstream>
#include "test.h"
#include "tokenizer.h"
#include "parser.h"
#include "exception.h"
#include "generator.h"

using namespace std;

namespace LuCCompiler
{

const string TEST_DIR = "\\tests\\";
const string TEST_ALL[] = {
    "tokenizer", "expressions", "statements",
    "declarations", "generator"
};
const int TEST_ALL_SIZE = 5;

void runTests(string testBlock, bool tagNotOptimized)
{
    system("remove_test_exe_files.bat");
    int i = 0;
    while(i < TEST_ALL_SIZE)
    {
        Tester t(!tagNotOptimized);

        string currentTest(testBlock == "all" ? TEST_ALL[i] : testBlock);
        //path to tests
        string path(TEST_DIR + currentTest + '\\');
        LPWSTR wpath = (LPWSTR)malloc((path.size() + 1) * sizeof(WCHAR));
        MultiByteToWideChar(CP_ACP, 0, path.c_str(), path.size() + 1, wpath, path.size() + 1);

        //current path
        wchar_t wdir[FILENAME_MAX];
        _wgetcwd(wdir, MAX_PATH);
        wstring wfullpath(wdir);
        wfullpath.append(wpath);

        wstring tmp(wfullpath);
        tmp.append(L"*.in");

        WIN32_FIND_DATAW FindFileData;
        HANDLE hFind;

        hFind = FindFirstFileW(tmp.c_str(), &FindFileData);
        do
        {
            if(hFind != INVALID_HANDLE_VALUE)
            {
                wstring filename(FindFileData.cFileName);
                t.attachTypelessFilename(wfullpath + filename);
                t.runFile(currentTest);
                t.estimateResult();
            }
            else
                throw FindFileError();
        }
        while(FindNextFileW(hFind, &FindFileData) || GetLastError() != ERROR_NO_MORE_FILES);
        FindClose(hFind);

        t.outputGlobalResult();
        if(i == 0 && testBlock != "all")
            break;
        i++;
        cout << endl;
    }
}

void Tester::attachTypelessFilename(wstring filename)
{
    char ctmp[MAX_PATH];
    wstring wtmp = filename.substr(0, filename.size() - 3);
    int l = WideCharToMultiByte(CP_ACP, 0, wtmp.c_str(), wtmp.size(), ctmp, 0, NULL, NULL);
    WideCharToMultiByte(CP_ACP, 0, wtmp.c_str(), wtmp.size(), ctmp, l, NULL, NULL);
    _currentTest.clear();
    _currentTest.append(ctmp, l);
}

void Tester::redirectStreams(string& testBlock)
{
    _outStream.open(
        string(_currentTest + (testBlock == "generator" ? ".asm" : ".log")).c_str());
    _errStream.open((string(_currentTest + ".log")).c_str());
    if(!_outStream.good() || !_errStream.good())
        throw BadFile();

    backup = cout.rdbuf();
    ebackup = cerr.rdbuf();
    cout.rdbuf(_outStream.rdbuf());
    cerr.rdbuf(_errStream.rdbuf());
}

void Tester::restoreStreams()
{
    cout.rdbuf(backup);
    cerr.rdbuf(ebackup);
    _outStream.close();
    _errStream.close();
}

void Tester::runFile(string& testBlock)
{
    redirectStreams(testBlock);

    Tokenizer t(_currentTest + ".in");
    try
    {
        if(testBlock == "tokenizer")
        {
            cout << "(line, col)\t\tType\t\tText, Value\n";
            do
            {
                t.next().outputAsString(cout);
            }
            while(t.get().type != TOK_EOF);
        }
        else if(testBlock == "generator")
        {
            Parser p(&t, optimized);
            Generator g(p.parse(), optimized);
            g.generate();
            g.out();

            restoreStreams();
            string redirect("1>2>" + _currentTest + ".log");
            string exeFile(_currentTest + ".exe ");
            system(("fasm " + _currentTest + ".asm").c_str());
            if(ifstream(exeFile.c_str()) != NULL)
                system((exeFile + redirect).c_str());
            testsCount++;
            return;
        }
        else
        {
            Parser p(&t);
            if(testBlock == "expressions")
                p.parseExpr();
            else if(testBlock == "statements")
                p.parseStat();
            else
                p.parse();
            p.out();
        }
    }
    catch(LuCCException& e)
    {
        cerr << "Error: " << e.text() << endl;
    }

    restoreStreams();

    testsCount++;
}

void Tester::estimateResult()
{
    ifstream outFile, logFile;
    outFile.open(string(_currentTest + ".out").c_str());
    logFile.open(string(_currentTest + ".log").c_str());

    int line = 0, result = -1;
    string oS, lS;
    while(!outFile.eof() && !logFile.eof())
    {
        line++;
        getline(outFile, oS);
        getline(logFile, lS);
        if(oS != lS)
        {
            result = line;
            break;
        }
    }

    if(!outFile.eof() || !logFile.eof())
        result = line + 1;

    if(result == -1)
    {
        cout << "[OK] Test " << _currentTest.c_str() << ".in" << endl;
        testsPassed++;
    }
    else
        cout << "[FAIL] Test " << _currentTest.c_str() << ".in:\n\tmismatch in line " << line << "." << endl;

    outFile.close();
    logFile.close();
}

void Tester::outputGlobalResult()
{
    cout << "_______________________________________" << endl;
    cout << "Result: "<< testsPassed << '/' << testsCount << " passed" << endl;
}

}
