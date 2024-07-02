#pragma once

static string ReplaceInstances(string line, string macro, string value){
    char cquote;
    regex quotes = regex("['\"`]");

    for (size_t pos=0;pos<line.size();){
        size_t nextquote = string::npos;
        for (size_t i=pos;i<line.size();i++){
            if (regex_match(string(1, line[i]), quotes)){
                nextquote = i;
                cquote = line[i];

                if (cquote=='`')
                    Transpiler.InString = true;
                break;
            }
        }

        size_t nextmacro = line.find(macro, pos);
        if (nextmacro==string::npos)
            break;

        if (nextquote!=string::npos && nextquote<nextmacro){
            nextquote = line.find(cquote, nextquote+1);
            
            if (nextquote==string::npos)
                break;
            pos = nextquote+1;
            Transpiler.InString = false;
            continue;
        }

        if (!Transpiler.InString)
            line.replace(nextmacro, macro.size(), value);
            pos+=value.size()+1;
    }

    return line;
}

string JSTranspiler::ReplaceMacro(string s){
    for (auto [name, value] : Syntax::Definitions){
        s = ReplaceInstances(s, name, value);
    }
    return s;
}

string JSTranspiler::Transpile(){
    vector<string> result;
    bool preend = false;

    for (string line : this->Lines){
        this->CurrentCode = line;
        this->CurrentLine++;

        string tline = Trim(line);

        if (tline.rfind("#", 0)==0){
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