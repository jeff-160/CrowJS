#pragma once

inline string Replace(string s, string c, string d){
    return regex_replace(s, regex(c), d);
}

inline void Error(string message, bool debug=true){
    if (debug)
        cout << Transpiler.CurrentFile+":"+to_string(Transpiler.CurrentLine)+": "+message+"\n\t"+Transpiler.CurrentCode;

    else{
        message = Replace(message, "#", "`#");
        system(string("powershell -command Write-Host "+message+" -ForegroundColor 'red'").c_str());
    }
    exit(0);
}

inline string Trim(string s){
    s.erase(s.begin(), find_if(s.begin(), s.end(), [](int c) {return !isspace(c);}));
    return s;
}

inline vector<string> Split(string s, char d){
    stringstream ss(s);
    vector<string> v;
    string t;

    while (getline(ss, t, d))
        if (t.size())
            v.push_back(t);

    return v;
}

vector<string> GetArgs(string s, size_t t){
    vector<string> v;
    string b;

    for (char c : s){
        if (c==' ' && t){
            v.push_back(Trim(b));
            t--;
            b = "";
            continue;
        }
        b+=c;
    }
    if (b.size())
        v.push_back(b);

    return v;
}