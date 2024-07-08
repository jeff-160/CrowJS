#pragma once
#define ISENTER(STR) pos==line.find(STR, pos)
#define ISQUOTE(STR) regex_match(STR, REGQUOTES) 
#define ISINTERPOLATE(STR) STR==Syntax::Interpolate[0]
#define PAIR Transpiler.StringStack, Transpiler.InComment


bool JSTranspiler::InString(){
    return !this->StringStack.empty() && ISQUOTE(this->StringStack.back());
}

static void RemoveString(){
    for (int i=Transpiler.StringStack.size()-1;i>=0;i--){
        if (ISQUOTE(Transpiler.StringStack[i]))
            return (void)Transpiler.StringStack.erase(Transpiler.StringStack.begin()+i);
    }
}

bool JSTranspiler::InInterpolate(){
    return !this->StringStack.empty() && ISINTERPOLATE(this->StringStack.back());
}

static void RemoveInterpolate(){
    for (int i=Transpiler.StringStack.size()-1;i>=0;i--){
        if (ISINTERPOLATE(Transpiler.StringStack[i]))
            return (void)Transpiler.StringStack.erase(Transpiler.StringStack.begin()+i);
    }
}

string JSTranspiler::LastQuote(){
    for (int i=this->StringStack.size()-1;i>=0;i--){
        if (ISQUOTE(this->StringStack[i]))
            return this->StringStack[i];
    }
    return "";
}

static inline bool CheckBefore(const string& s, const string& f, size_t p, size_t c){
    size_t i = s.find(f, p);
    return (i!=string::npos && i<c);
}

static constexpr bool IsEscape(const string& s, int p){
    int t = 0;

    while (p>=0){
        if (s[p]=='\\')
            t++;
        else
            break;
        p--;
    }

    return t%2;
}

static inline bool IsIdentifier(const string& s, int a, int b){
    auto m = [s](int i) -> bool { return !regex_match(string(1, s[i]), Syntax::Name); };

    return (!a || m(a-1))+(b>s.size()-2 || m(b+1))==2;
}

static string ReplaceMacro(string s, const unordered_map<string, Syntax::Macro>& def);

static string ReplaceFunction(const Syntax::Macro& macro, const string& params){
    unordered_map<string, Syntax::Macro> copydef = Transpiler.Definitions;

    vector<string> args = GetFuncArgs(params);
    size_t nargs = macro.Params.size();
    if (args.size()!=nargs)
        Error("Macro function expected "+to_string(nargs)+" arguments, got "+to_string(args.size()));
    
    for (size_t i=0;i<nargs;i++)
        copydef[macro.Params[i]] = args[i];

    return ReplaceMacro(macro.Value, copydef);
}

static pair<bool, string> ReplaceInstances(string line, const string& macro, const Syntax::Macro& repr, bool exclude){
    bool c = false;

    size_t pos = 0;
    while (pos<line.size()){
        if (!Transpiler.InString()){
            if (ISENTER(Syntax::Comment))
                break;

            for (size_t i=0;i<=1;i++){
                if (ISENTER(Syntax::MComment[i]))
                    Transpiler.InComment = !i;
            }
        }

        for (size_t i=0;i<=1;i++){
            if (ISENTER(Syntax::Interpolate[i]) && Transpiler.LastQuote()=="`"){
                i ? (Transpiler.InString() ? (void)0 : RemoveInterpolate()) : Transpiler.StringStack.push_back(Syntax::Interpolate[0]);
            }
        }

        if (
            !Transpiler.InComment &&
            ISQUOTE(string(1, line[pos])) && 
            !IsEscape(line, pos-1)
        ) Transpiler.InString() ? RemoveString() : Transpiler.StringStack.push_back(string(1, line[pos]));

        if (!Transpiler.InString() && !Transpiler.InComment){
            smatch m;
            string sstr = line.substr(pos);
            bool found = regex_search(sstr, m, regex(macro));
            size_t nextmacro = pos+m.position();

            if (pos==nextmacro && IsIdentifier(line, nextmacro, nextmacro+string(m[0]).size()-1)){
                for (char c : Syntax::Quotes){
                    if (CheckBefore(line, string(1, c), pos, nextmacro))
                        goto inc;
                }

                if (exclude)
                    Error("Recursive macro is not allowed");
                c = true;
                
                string value = repr.IsFunction ? ReplaceFunction(repr, m[1]) : repr.Value;
                line.replace(nextmacro, string(m[0]).size(), value);
                pos = nextmacro+value.size()-1;
            }
        }

        inc:
            pos++;
    }

    if (Transpiler.LastQuote()!="`" && Transpiler.InString())
        Transpiler.StringStack.pop_back();

    return {c, line};
}

static string ReplaceMacro(string s, const unordered_map<string, Syntax::Macro>& def){
    bool b = true;
    unordered_map<string, bool> m;
    pair<vector<string>, bool> ss, is, ls; 
        ss = is = ls = {PAIR};

    while (b){
        b = false;

        ls = {PAIR};
        for (auto [name, repr] : def){
            tie(PAIR) = ls;
            
            bool c;
            tie(c, s) = ReplaceInstances(s, name, repr, m[name]);

            if (c)
                m[name] = b = true;

            is = {PAIR};
        }
        tie(PAIR) = ss;
    }
    tie(PAIR) = is;

    return s;
}

string JSTranspiler::Transpile(){
    vector<string> result;
    bool preend = false;

    for (size_t i=0;i<this->Lines.size();){
        this->CurrentCode = this->Lines[i];
        this->CurrentLine++;

        string tline = Trim(this->CurrentCode);

        if (!tline.rfind("#")){
            vector<string> args = Split(tline, ' ');
            
            if (args[0]==Syntax::Define.Keyword){
                while (i<this->Lines.size()-1 && RTrim(this->Lines[i]).back()=='\\'){
                    i++;
                    tline.pop_back();
                    tline+=RTrim(this->Lines[i]);
                }

                if (preend)
                    Error("Macro should be defined at top of file");

                Syntax::Define.Callback(GetArgs(tline, 2));
            }
            else
                preend || (!tline.rfind("#!") && this->CurrentLine==1) ? 
                    result.push_back(this->CurrentCode) : Error("#define should be used to declare a macro");
        }
        else{
            if (!tline.empty() && tline.rfind(Syntax::Comment) && tline.rfind(Syntax::MComment[0]))
                preend = true;
            result.push_back(ReplaceMacro(this->CurrentCode, Transpiler.Definitions));
        }

        i++;
    }

    return Join(result, "\n");
}