#pragma once

static bool CheckName(const string& name){
    if (!regex_match(string(1, name[0]), regex("[_a-z]", regex_constants::icase)))
        return false;

    regex re("[_a-z0-9]", regex_constants::icase);
    for (char c : name){
        if (!regex_match(string(1, c), re))
            return false;
    }
    
    return true;
}

namespace Syntax{
    unordered_map<string, string> Definitions{
        {"__DATE__", "new Date().toString().match(/\\w+ \\d+ \\d+/)[0]"},
        {"__TIME__", "new Date().toString().match(/\\d+:\\d+:\\d+/)[0]"},
        {"__FILE__", "new Error().stack.match(/\\/([\\w.]+):\\d+/)[1]"},
        {"__LINE__", "new Error().stack.match(/:(\\d+):/)[1]"},
    };

    struct {
        const string Keyword = "#define";

        constexpr void Callback(const vector<string>& args) {
            if (args.size()<2) 
                Error("No macro name given in "+args[0]+" directive");
            if (!CheckName(args[1])) 
                Error("Macro name must be identifier");

            Definitions[args[1]] = args.size()<3 ? "" : args[2];
        }
    } Define;
}