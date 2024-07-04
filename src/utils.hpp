#pragma once

inline string Replace(const string& s, const string& c, const string& d){
    return regex_replace(s, regex(c), d);
}

void Error(const string& message, bool debug=true){
    if (debug)
        cout << Transpiler.CurrentFile+":"+to_string(Transpiler.CurrentLine)+": "+message+"\n\t"+Transpiler.CurrentCode;

    else
        system(string("powershell -command Write-Host "+Replace(message, "#", "`#")+" -ForegroundColor 'red'").c_str());
    exit(0);
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
        if (t.size())
            v.push_back(t);

    return v;
}

vector<string> GetArgs(const string& s, size_t t){
    #define ADD string tb = Trim(b); if (tb.size()) { v.push_back(tb); t--; }

    vector<string> v;
    string b;

    for (char c : s){
        if (c==' ' && t){
            ADD;
            b = "";
            continue;
        }
        b+=c;
    }
    ADD;
    
    #undef ADD
    return v;
}