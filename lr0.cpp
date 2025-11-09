#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
using namespace std;

vector<pair<string, string>> g; 
set<char> S; 

string make(string A, string B, int d) {
    return A + "->" + B.substr(0, d) + "." + B.substr(d);
}

void parse(string it, string &A, string &B, int &d) {
    int x = it.find("->");
    A = it.substr(0, x);
    B = it.substr(x + 2);
    d = B.find('.');
    B.erase(d, 1);
}

set<string> closure(set<string> I) {
    bool c;
    do {
        c = false;
        vector<string> v(I.begin(), I.end());
        for (auto it : v) {
            string A, B;
            int d;
            parse(it, A, B, d);
            if (d < (int)B.size() && isupper(B[d])) {
                for (auto p : g) {
                    if (p.first[0] == B[d]) {
                        if (I.insert(make(p.first, p.second, 0)).second)
                            c = true;
                    }
                }
            }
        }
    } while (c);
    return I;
}

set<string> goTo(set<string> I, char X) {
    set<string> J;
    for (auto it : I) {
        string A, B;
        int d;
        parse(it, A, B, d);
        if (d < (int)B.size() && B[d] == X)
            J.insert(make(A, B, d + 1));
    }
    return J.empty() ? set<string>{} : closure(J);
}

int main() {
    int n;
    cout << "Enter number of productions: ";
    cin >> n;
    string s;
    getline(cin, s);
    cout << "Enter productions (A=alpha|beta|..., use # for epsilon):\n";
    for (int i = 0; i < n; i++) {
        getline(cin, s);
        int eq = s.find('=');
        string lhs = s.substr(0, eq), rhs = s.substr(eq + 1), prod = "";
        for (char c : rhs) {
            if (c == '|') {
                g.push_back({lhs, prod == "#" ? "" : prod});
                for (char x : prod) S.insert(x);
                prod = "";
            } else prod += c;
        }
        g.push_back({lhs, prod == "#" ? "" : prod});
        for (char x : prod) S.insert(x);
        S.insert(lhs[0]);
    }

    string start = g[0].first, aug = start + "'";
    g.insert(g.begin(), {aug, start});
    S.insert(start[0]);

    set<string> I0 = closure({make(aug, start, 0)});
    vector<set<string>> C = {I0};
    vector<map<char, int>> T(1);
    map<string, int> idx;

    auto key = [](set<string> s) {
        string k;
        for (auto &x : s) k += x + "|";
        return k;
    };
    idx[key(I0)] = 0;

    for (int i = 0; i < (int)C.size(); i++) {
        for (char X : S) {
            set<string> J = goTo(C[i], X);
            if (J.empty()) continue;
            string k = key(J);
            if (!idx.count(k)) {
                idx[k] = C.size();
                C.push_back(J);
                T.push_back({});
            }
            T[i][X] = idx[k];
        }
    }

    cout << "\nCanonical LR(0) Item Sets:\n";
    for (int i = 0; i < (int)C.size(); i++) {
        cout << "I" << i << ":\n";
        for (auto it : C[i]) cout << it << "\n";
        for (auto p : T[i]) cout << "GOTO(I" << i << "," << p.first << ")=I" << p.second << "\n";
        cout << "\n";
    }
}
