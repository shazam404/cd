#include <bits/stdc++.h>
using namespace std;

struct Prod { string lhs, rhs; };
struct Item { int p, d; };
struct State { vector<Item> items; int id; };

vector<Prod> P;
vector<State> S;
map<pair<int,char>,int> trans;
map<pair<int,char>,string> table;

bool operator==(const Item& a, const Item& b) { return a.p==b.p && a.d==b.d; }

void closure(State& s) {
    for(int i=0; i<s.items.size(); i++) {
        Item it = s.items[i];
        if(it.d < P[it.p].rhs.size() && isupper(P[it.p].rhs[it.d])) {
            char nt = P[it.p].rhs[it.d];
            for(int j=0; j<P.size(); j++) {
                if(P[j].lhs[0] == nt) {
                    Item n = {j, 0};
                    if(find(s.items.begin(), s.items.end(), n) == s.items.end())
                        s.items.push_back(n);
                }
            }
        }
    }
}

State go(State& s, char x) {
    State n; n.items.clear();
    for(auto it : s.items) {
        if(it.d < P[it.p].rhs.size() && P[it.p].rhs[it.d] == x)
            n.items.push_back({it.p, it.d+1});
    }
    if(!n.items.empty()) closure(n);
    return n;
}

bool same(State& a, State& b) {
    if(a.items.size() != b.items.size()) return false;
    for(auto it : a.items)
        if(find(b.items.begin(), b.items.end(), it) == b.items.end())
            return false;
    return true;
}

int findState(State& s) {
    for(int i=0; i<S.size(); i++)
        if(same(S[i], s)) return i;
    return -1;
}

void buildLR0() {
    State s0; s0.items = {{0,0}}; s0.id = 0;
    closure(s0);
    S.push_back(s0);
    
    set<char> syms;
    for(auto p : P)
        for(char c : p.rhs)
            if(c != '#') syms.insert(c);
    
    for(int i=0; i<S.size(); i++) {
        for(char x : syms) {
            State n = go(S[i], x);
            if(!n.items.empty()) {
                int idx = findState(n);
                if(idx == -1) {
                    n.id = S.size();
                    S.push_back(n);
                    trans[{i,x}] = n.id;
                } else {
                    trans[{i,x}] = idx;
                }
            }
        }
    }
}

void buildTable() {
    set<char> terms, nonterms;
    for(auto p : P) {
        if(p.lhs != "S'") nonterms.insert(p.lhs[0]);
        for(char c : p.rhs)
            if(c!='#') (isupper(c) ? nonterms : terms).insert(c);
    }
    terms.insert('$');
    
    for(int i=0; i<S.size(); i++) {
        for(auto it : S[i].items) {
            if(it.d == P[it.p].rhs.size()) {
                if(it.p == 0) table[{i,'$'}] = "acc";
                else for(char t : terms) table[{i,t}] = "r"+to_string(it.p);
            }
        }
        for(auto [k,v] : trans) {
            if(k.first == i) {
                if(isupper(k.second)) table[{i,k.second}] = to_string(v);
                else table[{i,k.second}] = "s"+to_string(v);
            }
        }
    }
    
    cout << "\nLR(0) TABLE:\n";
    cout << "State | ACTION                    | GOTO\n";
    cout << "      |";
    for(char t : terms) cout << " " << t << "    ";
    cout << "|";
    for(char nt : nonterms) cout << " " << nt << " ";
    cout << "\n" << string(60,'-') << "\n";
    
    for(int i=0; i<S.size(); i++) {
        cout << setw(5) << i << " |";
        for(char t : terms) {
            string e = table[{i,t}];
            cout << setw(6) << (e.empty()?"":e);
        }
        cout << "|";
        for(char nt : nonterms) {
            string e = table[{i,nt}];
            cout << setw(3) << (e.empty()?"":e);
        }
        cout << "\n";
    }
}

int main() {
    int n; cin >> n; cin.ignore();
    
    for(int i=0; i<n; i++) {
        string line, lhs, rhs;
        getline(cin, line);
        int pos = line.find('=');
        lhs = line.substr(0,pos); rhs = line.substr(pos+1);
        lhs.erase(remove(lhs.begin(),lhs.end(),' '),lhs.end());
        rhs.erase(remove(rhs.begin(),rhs.end(),' '),rhs.end());
        
        if(i==0) P.push_back({"S'", string(1,lhs[0])});
        
        stringstream ss(rhs);
        string alt;
        while(getline(ss, alt, '|'))
            P.push_back({lhs, alt});
    }
    
    cout << "\nAugmented Grammar:\n";
    for(int i=0; i<P.size(); i++)
        cout << i << ". " << P[i].lhs << " -> " << P[i].rhs << "\n";
    
    buildLR0();
    
    cout << "\nCanonical Collection:\n";
    for(auto s : S) {
        cout << "I" << s.id << ":\n";
        for(auto it : s.items) {
            cout << "  " << P[it.p].lhs << " -> ";
            for(int i=0; i<P[it.p].rhs.size(); i++) {
                if(i==it.d) cout << ".";
                cout << P[it.p].rhs[i];
            }
            if(it.d==P[it.p].rhs.size()) cout << ".";
            cout << "\n";
        }
        for(auto [k,v] : trans)
            if(k.first == s.id)
                cout << "  GOTO(I" << s.id << "," << k.second << ") = I" << v << "\n";
        cout << "\n";
    }
    
    buildTable();
    return 0;
}
