#pragma once

struct JSTranspiler{
    string Code, CurrentFile, CurrentCode;
    vector<string> Lines;
    int CurrentLine = 0;

    string Transpile();
    string ReplaceMacros(string s);
};

JSTranspiler Transpiler;