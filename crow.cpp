#include "src/include.hpp"

static pair<string, vector<string>> ReadFile(const string& path){
    ifstream file(path);

    auto rev = [](string s) -> string {
        reverse(s.begin(), s.end());
        return s;
    };

    if (!file.is_open())
        Error("Invalid file path", false);
    if (rev(path).find(rev(".js"))!=0)
        Error("Input file lacks .js extension", false);

    string line, contents;
    vector<string> lines;
    while (getline(file, line)){
        contents+=line+"\n";
        lines.push_back(line);
    }

    file.close();
    return {contents, lines};
}

static void WriteFile(const string& path, const string& contents){
    ofstream file(path);
    file << contents;
    file.close();
}

int main(int argc, char* argv[]){
    if (argc<2)
        Error("No input file", false);

    filesystem::path path(argv[1]);
    string abspath = filesystem::absolute(path).string();
    Transpiler.CurrentFile = path.filename().string();

    auto start = high_resolution_clock::now();
    
    tie(Transpiler.Code, Transpiler.Lines) = ReadFile(abspath);
    WriteFile(abspath, Transpiler.Transpile());
 
    cout << "Transpiled to "+Transpiler.CurrentFile << endl 
        << "Process exited after " << (float)(duration_cast<microseconds>(high_resolution_clock::now()-start).count())/1e6 << " seconds";
}