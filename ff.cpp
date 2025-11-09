#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <string>

using namespace std;

map<char, vector<string>> g;
map<char, set<char>> F, FO;
char start;

set<char> first(string s){
    set<char> r;
    if(s.empty() || s[0] == '#'){
        r.insert('#');
        return r;
    }
    
    for(int i = 0; i < s.size(); i++){
        if(!isupper(s[i])){
            r.insert(s[i]);
            return r;
        }
        set<char> firstOfSymbol = F[s[i]];
        for(char c : firstOfSymbol){
            if(c != '#'){
                r.insert(c);
            }
        }
        if(firstOfSymbol.find('#') == firstOfSymbol.end()){
            return r;
        }
        if(i == s.size() - 1){
            r.insert('#');
        }
    }
    
    return r;
}

void compFIrst(){
    bool c;
    do{
        c = false;
        for(auto &p : g){
            for(auto rhs : p.second){
                int b = F[p.first].size();
                set<char> t = first(rhs);
                F[p.first].insert(t.begin(), t.end());
                if(F[p.first].size() != b) c = true;
            }
        }

    } while(c);
}

void compFollow(){
    bool c;
    FO[start].insert('$');
    do{
        c = false;
        for(auto &p : g){
            for(auto rhs : p.second){
                for(int i=0; i<rhs.size(); i++){
                    if(!isupper(rhs[i])) continue;
                    string beta = i+1 < rhs.size() ? rhs.substr(i+1) : "";
                    set<char> fb = first(beta);
                    int b = FO[rhs[i]].size();
                    for(char f : fb){
                        if(f!='#'){
                            FO[rhs[i]].insert(f);
                        }
                        if(beta.empty() || fb.count('#')){
                            FO[rhs[i]].insert(FO[p.first].begin(), FO[p.first].end());
                        }
                        if(FO[rhs[i]].size() != b){
                            c = true;
                        }
                    }
                }
            }
        }
    } while(c);
}

int main(){
    int n;
    cin >> n;
    for(int i=0; i<n; i++){
        string s;
        cin >> s;
        if(i==0){
            start = s[0];
        }
        string rhs = s.substr(3);
        string prod = "";
        for(char c : rhs){
            if(c=='|'){
                g[s[0]].push_back(prod);
                prod = "";
            }
            else{
                prod += c;
            }
        }
        if(!prod.empty()) g[s[0]].push_back(prod);
    }

    compFIrst();
    compFollow();

    cout << "FIRST\n";
    for(auto &p : F){
        cout << "FIRST(" << p.first << ") = {";
        for(char c : p.second){
            cout << c << " ";
        }
        cout << "}\n";
    } 
    cout << "\n";
    cout << "FOLLOW\n";
    for(auto &p : FO){
        cout << "FOLLOW(" << p.first << ") = {";
        for(char c : p.second){
            cout << c << " ";
        }
        cout << "}\n";
    } 
}
