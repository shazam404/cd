#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <algorithm>

using namespace std;

bool isNonTerm(char c){ return (c >= 'A' && c <= 'Z'); }

vector<pair<string,string>> prods;
set<char> symbols;

string makeItem(string A, string B, int dot){
    string s = A + "->";
    s += B.substr(0, dot);
    s += '.';
    s += B.substr(dot);
    return s;
}

void parseItem(string it, string &A, string &B, int &dot){
    int x = it.find("->");
    A = it.substr(0, x);
    B = it.substr(x+2);
    dot = B.find('.');
    B.erase(dot,1);
}

set<string> closure(set<string> I){
    bool changed = true;
    while(changed){
        changed = false;
        vector<string> temp(I.begin(), I.end());
        for(auto it : temp){
            string A,B; int dot;
            parseItem(it, A,B,dot);
            if(dot < (int)B.size() && isNonTerm(B[dot])){
                char X = B[dot];
                for(auto p: prods){
                    if(p.first[0]==X){
                        string item = makeItem(p.first,p.second,0);
                        if(I.insert(item).second) changed = true;
                    }
                }
            }
        }
    }
    return I;
}

set<string> goTo(set<string> I, char X){
    set<string> J;
    for(auto it : I){
        string A,B; int dot;
        parseItem(it,A,B,dot);
        if(dot<(int)B.size() && B[dot]==X)
            J.insert(makeItem(A,B,dot+1));
    }
    if(J.empty()) return {};
    return closure(J);
}

vector<string> splitByChar(string s, char c){
    vector<string> parts;
    string cur="";
    for(char ch : s){
        if(ch == c){
            parts.push_back(cur);
            cur="";
        } else cur+=ch;
    }
    parts.push_back(cur);
    return parts;
}

int main(){
    int n; cout<<"Enter number of productions: "; cin>>n;
    string s; getline(cin,s);
    cout<<"Enter productions (A=alpha | beta | ... , use # for epsilon):\n";
    for(int i=0;i<n;i++){
        getline(cin,s);
        int eq=s.find('=');
        string A=s.substr(0,eq);
        string R=s.substr(eq+1);
        vector<string> rhs = splitByChar(R,'|');
        for(string t : rhs){
            if(t=="#") t="";
            prods.push_back({A,t});
            for(char c:t) symbols.insert(c);
            symbols.insert(A[0]);
        }
    }

    string start = prods[0].first;
    string aug = start+"'"; 
    prods.insert(prods.begin(), {aug,start});
    symbols.insert(start[0]);

    set<string> I0 = closure({makeItem(aug,start,0)});
    vector<set<string>> C = {I0};
    vector<map<char,int>> trans(1);
    map<string,int> idx;

    auto key=[&](set<string> S){
        vector<string> v(S.begin(),S.end());
        sort(v.begin(),v.end());
        string k;
        for(auto &x:v) k+=x+"|";
        return k;
    };
    idx[key(I0)] = 0;

    vector<char> syms(symbols.begin(), symbols.end());
    for(int i=0;i<(int)C.size();i++){
        for(char X:syms){
            set<string> J=goTo(C[i],X);
            if(J.empty()) continue;
            string k=key(J);
            if(!idx.count(k)){
                idx[k]=C.size();
                C.push_back(J);
                trans.push_back({});
            }
            trans[i][X]=idx[k];
        }
    }

    cout<<"\nCanonical LR(0) Item Sets:\n\n";
    for(int i=0;i<(int)C.size();i++){
        cout<<"I"<<i<<":\n";
        for(auto it:C[i]) cout<<it<<"\n";
        for(auto p:trans[i]) cout<<"GOTO(I"<<i<<","<<p.first<<")=I"<<p.second<<"\n";
        cout<<"\n";
    }
}
