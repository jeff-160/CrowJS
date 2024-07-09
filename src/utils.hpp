#pragma once

inline string Replace(const string& s, const string& c, const string& d){
    return regex_replace(s, regex(c), d);
}

void Error(const string& message, bool debug=true){
    cout << (debug ? 
        Transpiler.CurrentFile+":"+to_string(Transpiler.CurrentLine)+": "+message+"\n\t"+Transpiler.CurrentCode :
        "CrowJS: "+message
    );
    exit(0);
}

inline string Join(const vector<string>& v, const string& delim){
    return accumulate(v.begin(), v.end(), string(), [delim](string &r, const string &s) { return r.empty() ? s : r+delim+s; });
}

inline string Trim(string s){
    s.erase(s.begin(), find_if(s.begin(), s.end(), [](int c) {return !isspace(c);}));
    return s;
}

vector<string> Split(const string& s, char d){
    stringstream ss(s);
    vector<string> v;
    string t;

    while (getline(ss, t, d))
        if (!t.empty())
            v.push_back(t);

    return v;
}

#define ADD string tb = Trim(b); if (!tb.empty()) { v.push_back(tb); t--; }
vector<string> GetArgs(const string& s, int t){
    vector<string> v;
    string b;
    bool br = false;

    for (char c : s){
        br = c=='(' ? true : c==')' ? false : br;

        if (c==' ' && !br && t){
            ADD;
            b = "";
            continue;
        }
        b+=c;
    }
    ADD;
    
    return v;
}

vector<string> GetFuncArgs(const string& s);