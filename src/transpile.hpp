#pragma once

#define RESET Transpiler.InString = false; Transpiler.LastQuote = NULL

static string ReplaceInstances(string line, string macro, string value){
    regex quotes("[`'\"]");

    size_t pos = 0;

    while (pos<line.size()){
        if (regex_match(string(1, line[pos]), quotes) && (Transpiler.LastQuote==NULL || Transpiler.LastQuote==line[pos])){
            Transpiler.InString = !Transpiler.InString;

            if (Transpiler.InString){
                Transpiler.LastQuote = line[pos];

                size_t nextquote = line.find(Transpiler.LastQuote, pos+1);
                if (nextquote==string::npos)
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
            for (char c : "\"'`"){
                size_t p = line.find(c, pos);
                if (p!=string::npos && p<nextmacro)
                    goto end;
            }

            if (nextmacro!=string::npos){
                line.replace(nextmacro, macro.size(), value);
                pos=nextmacro+value.size();
            }
        }

        end:
        pos++;
    }

    if (Transpiler.LastQuote!='`'){
        RESET;
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