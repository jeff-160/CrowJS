#pragma once
#define RESET Transpiler.InString = false; Transpiler.LastQuote = NULL
#define TUP Transpiler.InString, Transpiler.LastQuote


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

static pair<bool, string> ReplaceInstances(string line, const string& macro, const string& value, bool exclude){
    bool c = false;
    regex quotes("["+Syntax::Quotes+"]");

    size_t pos = 0;
    while (pos<line.size()){
        if (regex_match(string(1, line[pos]), quotes) && (Transpiler.LastQuote==NULL || Transpiler.LastQuote==line[pos])){
            Transpiler.InString = !Transpiler.InString;

            if (Transpiler.InString){
                Transpiler.LastQuote = line[pos];

                size_t nextquote = line.find(Transpiler.LastQuote, pos+1);
                if (nextquote==string::npos || IsEscape(line, nextquote-1))
                    break;
                
                pos = nextquote+1;
                RESET;
                continue;
            }
            else{
                RESET;
            }
        }

        if (!Transpiler.InString){
            size_t nextmacro = line.find(macro, pos);
            for (char c : Syntax::Quotes){
                if (CheckBefore(line, string(1, c), pos, nextmacro))
                    goto inc;
            }

            if (CheckBefore(line, "//", pos, nextmacro))
                goto end;

            if (nextmacro!=string::npos){
                if (exclude)
                    Error("Recursive macro is not allowed");
                c = true;
                line.replace(nextmacro, macro.size(), value);
                pos=nextmacro+value.size();
            }
        }

        inc:
            pos++;
    }

    if (Transpiler.LastQuote!='`'){
        RESET;
    }

    end:
        return {c, line};
}

string JSTranspiler::ReplaceMacro(string s){
    bool b = true;
    unordered_map<string, bool> m;
    tuple<bool, char> ss, is, ls; 
        ss = is = ls = {TUP};

    while (b){
        b = false;

        ls = {TUP};
        for (auto [name, value] : Syntax::Definitions){
            tie(TUP) = ls;
            
            bool c;
            tie(c, s) = ReplaceInstances(s, name, value, m[name]);

            if (c)
                m[name] = b = true;

            is = {TUP};
        }
        tie(TUP) = ss;
    }
    tie(TUP) = is;

    return s;
}

string JSTranspiler::Transpile(){
    vector<string> result;
    bool preend = false;

    for (string line : this->Lines){
        this->CurrentCode = line;
        this->CurrentLine++;

        string tline = Trim(line);

        if (!tline.rfind("#", 0)){
            vector<string> args = Split(tline, ' ');
            
            if (args[0]==Syntax::Define.Keyword){
                if (preend)
                    Error("Macro should be defined at top of file");

                Syntax::Define.Callback(GetArgs(tline, 2));
            }
            else
                preend ? result.push_back(line) : Error("#define should be used to declare a macro");
        }
        else{
            if (line.size())
                preend = true;
            result.push_back(this->ReplaceMacro(line));
        }
    }

    return accumulate(next(result.begin()), result.end(), result[0], [](string a, string b) {return a+"\n"+b;});
}