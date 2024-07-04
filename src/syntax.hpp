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
    const string Quotes = "`'\"";
    const string Comments[2] = {"/*", "*/"};
    const string Interpolate[2] = {"${", "}"};

    struct {
        const string Keyword = "#define";

        void Callback(const vector<string>& args) {
            if (args.size()<2) 
                Error("No macro name given in "+args[0]+" directive");
            if (!CheckName(args[1])) 
                Error("Macro name must be identifier");

            Transpiler.Definitions[args[1]] = args.size()<3 ? "" : args[2];
        }
    } Define;
}