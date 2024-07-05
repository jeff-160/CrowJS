#pragma once
#define RESET Transpiler.InString = false; Transpiler.LastQuote = NULL;
#define TUP Transpiler.InString, Transpiler.LastQuote, Transpiler.InComment, Transpiler.InInterpolate


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

static void CheckIn(const string& line, size_t pos, const string (&syn)[2], bool& state){
    for (size_t i=0;i<=1;i++){
        if (pos==line.find(syn[i], pos))
            state = !i;
    }
}

static pair<bool, string> ReplaceInstances(string line, const string& macro, const string& value, bool exclude){
    bool c = false;
    regex quotes("["+Syntax::Quotes+"]");

    size_t pos = 0;
    while (pos<line.size()){
        if (!Transpiler.InString)
            CheckIn(line, pos, Syntax::MComment, Transpiler.InComment);

        if (Transpiler.LastQuote=='`')
            CheckIn(line, pos, Syntax::Interpolate, Transpiler.InInterpolate);

        if (!Transpiler.InComment && regex_match(string(1, line[pos]), quotes) && (Transpiler.LastQuote==NULL || Transpiler.LastQuote==line[pos])){
            Transpiler.InString = !Transpiler.InString;

            if (Transpiler.InString){
                Transpiler.LastQuote = line[pos];

                // size_t nextquote = line.find(Transpiler.LastQuote, pos+1);
                // if (nextquote==string::npos || IsEscape(line, nextquote-1))
                //     break;
                
                // pos = nextquote+1;
                // RESET;
                // continue;
            }
            else{
                RESET;
            }
        }

        if ((!Transpiler.InString || Transpiler.InInterpolate) && !Transpiler.InComment){
            size_t nextmacro = line.find(macro, pos);
            for (char c : Syntax::Quotes){
                if (CheckBefore(line, string(1, c), pos, nextmacro))
                    goto inc;
            }

            if (Transpiler.InInterpolate && CheckBefore(line, Syntax::Interpolate[1], pos, nextmacro))
                goto inc;

            if (CheckBefore(line, Syntax::Comment, pos, nextmacro))
                goto end;

            if (nextmacro!=string::npos){
                if (exclude)
                    Error("Recursive macro is not allowed");
                c = true;
                line.replace(nextmacro, macro.size(), value);
                pos = nextmacro+value.size()-1;
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

static string ReplaceMacro(string s){
    bool b = true;
    unordered_map<string, bool> m;
    tuple<bool, char, bool, bool> ss, is, ls; 
        ss = is = ls = {TUP};

    while (b){
        b = false;

        ls = {TUP};
        for (auto [name, value] : Transpiler.Definitions){
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
            if (tline.size() && tline.rfind(Syntax::Comment) && tline.rfind(Syntax::MComment[0]))
                preend = true;
            result.push_back(ReplaceMacro(this->CurrentCode));
        }

        i++;
    }

    return accumulate(next(result.begin()), result.end(), result[0], [](string a, string b) {return a+"\n"+b;});
}