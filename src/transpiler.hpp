#pragma once

namespace Syntax {
    struct Macro {
        string Value;
        bool IsFunction;
        vector<string> Params;

        Macro() {}
        Macro(string v, bool i=false, vector<string> p={}) : Value(v), IsFunction(i), Params(p) {}
    };
}

struct JSTranspiler{
    string Code, CurrentFile, CurrentCode;
    vector<string> Lines;
    size_t CurrentLine = 0;

    bool InComment = false, InRegex = false;
    vector<string> StringStack;

    unordered_map<string, Syntax::Macro> Definitions{
        {"__DATE__", Syntax::Macro("new Date().toString().match(/\\w+ \\d+ \\d+/)[0]")},
        {"__TIME__", Syntax::Macro("new Date().toString().match(/\\d+:\\d+:\\d+/)[0]")},
        {"__FILE__", Syntax::Macro("new Error().stack.match(/\\/([\\w.]+):\\d+/)[1]")},
        {"__LINE__", Syntax::Macro("new Error().stack.match(/:(\\d+):/)[1]")},
        {"__FUNCTION__", Syntax::Macro("(typeof arguments=='undefined' ? null : arguments?.callee?.name)")}
    };

    bool InString();
    bool InInterpolate();
    string LastQuote();
    string Transpile();
};

JSTranspiler Transpiler;