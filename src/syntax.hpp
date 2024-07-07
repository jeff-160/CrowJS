#pragma once

namespace Syntax{
    const regex Name("[_a-z0-9]+", regex_constants::icase);
    const regex FuncName("([_a-z0-9]+)\\((.*)\\)", regex_constants::icase);

    static int NameType(const string& name){
        if (!regex_match(string(1, name[0]), regex("[_a-z]", regex_constants::icase)))
            return 0;

        return regex_match(name, Name) ? 1 : regex_match(name, FuncName) ? 2 : 0;
    }

    const string Quotes = "`'\"";
    const string Comment = "//";
    const string MComment[2] = {"/*", "*/"};
    const string Interpolate[2] = {"${", "}"};

    struct {
        const string Keyword = "#define";

        void Callback(const vector<string>& args) {
            if (args.size()<2) 
                Error("No macro name given in "+args[0]+" directive");

            int type = NameType(args[1]);
            if (!type)
                Error("Macro name must be identifier");

            string name = args[1];
            unordered_map<string, string> params;

            if (type==2){
                smatch m;
                regex_search(name, m, FuncName);

                for (auto i: GetFuncArgs(m[2]))
                    params[i] = "";
                    
                name = string(m[1])+"\\(([^)]*)\\)";
            }
            
            Transpiler.Definitions[name] = Macro(args.size()<3 ? "" : args[2], type==2, params);
        }
    } Define;
}