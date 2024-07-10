#pragma once
#define REGQUOTES regex("["+Syntax::Quotes+"]")
#define ADDARG(OUT) string tb = Trim(t); \
                tb.size() ? (!decl || Syntax::NameType(tb)==1 || tb==Syntax::VArg ? v.push_back(tb) : Error("Macro parameter must be identifier")) : \
                OUT ? (void)0 : Error("Macro function parameter cannot be empty"); \
                t = ""

namespace Syntax{
    const regex Name("[_a-z0-9]+", regex_constants::icase);
    const regex FuncName("([_a-z0-9]+)\\((.*)\\)", regex_constants::icase);

    static char NameType(const string& name){
        if (!regex_match(string(1, name[0]), regex("[_a-z]", regex_constants::icase)))
            return 0;

        return regex_match(name, Name) ? 1 : regex_match(name, FuncName) ? 2 : 0;
    }

    const string Quotes = "`'\"";
    const string Comment = "//";
    const string MComment[2] = {"/*", "*/"};
    const string Interpolate[2] = {"${", "}"};
    const string VArg = "...";
    const string VMacro = "__VA_ARGS__";

    struct {
        const string Keyword = "#define";

        void Callback(const vector<string>& args) {
            if (args.size()<2) 
                Error("No macro name given in "+args[0]+" directive");

            char type = NameType(args[1]);
            if (!type)
                Error("Macro name must be identifier");

            string name = args[1];
            vector<string> params;

            if (type==2){
                smatch m;
                regex_search(name, m, FuncName);

                params = GetFuncArgs(m[2], true);
                name = string(m[1])+"\\(([^)]*)\\)";
            }
            
            Transpiler.Definitions[name] = Macro(args.size()<3 ? "" : args[2], type==2, params);
        }
    } Define;
}

vector<string> GetFuncArgs(const string& s, bool decl){
    vector<string> v;
    string t = "";
    char lastquote = NULL;
    bool instring = false;

    for (size_t i=0;i<s.size();i++){
        if (regex_match(string(1, s[i]), REGQUOTES) && (lastquote==NULL || s[i]==lastquote)){
            lastquote = lastquote==NULL ? s[i] : NULL;
            instring = !instring;
        }

        if (s[i]==',' && !instring){
            ADDARG(0);
            continue;
        }
        
        t+=s[i];
    }
    ADDARG(1);

    bool e = false;
    for (string a : v){
        if (e)
            Error(a==Syntax::VArg ? "Macro function can only have a single variadic argument list" : "Variadic argument list must be last parameter in macro function");
        if (a==Syntax::VArg)
            e = true;
    }

    return v;
}