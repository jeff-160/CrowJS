#pragma once

struct JSTranspiler{
    string Code, CurrentFile, CurrentCode;
    vector<string> Lines;
    int CurrentLine = 0;

    char LastQuote = NULL;
    bool InString = false, InComment = false;

    unordered_map<string, string> Definitions{
        {"__DATE__", "new Date().toString().match(/\\w+ \\d+ \\d+/)[0]"},
        {"__TIME__", "new Date().toString().match(/\\d+:\\d+:\\d+/)[0]"},
        {"__FILE__", "new Error().stack.match(/\\/([\\w.]+):\\d+/)[1]"},
        {"__LINE__", "new Error().stack.match(/:(\\d+):/)[1]"},
    };

    string Transpile();
};

JSTranspiler Transpiler;