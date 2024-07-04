#pragma once

struct JSTranspiler{
    string Code, CurrentFile, CurrentCode;
    vector<string> Lines;
    int CurrentLine = 0;

    char LastQuote = NULL;
    bool InString = false, InComment = false;

    string Transpile();
    string ReplaceMacro(string s);
};

JSTranspiler Transpiler;