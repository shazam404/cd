#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <cctype>
#include <string>
using namespace std;

map<char, vector<string>> grammar;
map<char, set<char>> FIRST, FOLLOW;
map<pair<char, char>, string> table;
set<char> nonTerminals, terminals;
char startSymbol;

bool isTerminal(char c) {
    return !isupper(c) && c != '#';
}

set<char> firstOf(string s) {
    set<char> res;
    if (s.empty()) {
        res.insert('#');
        return res;
    }
    char ch = s[0];
    if(ch == '#') {
        res.insert('#');
        return res;
    }
    if (isTerminal(ch)) {
        res.insert(ch);
    } else {
        for (auto prod : grammar[ch]) {
            set<char> temp = firstOf(prod);
            res.insert(temp.begin(), temp.end());
        }
    }
    return res;
}

void computeFirst() {
    bool changed;
    do {
        changed = false;
        for (auto &p : grammar) {
            char nt = p.first;
            for (auto rhs : p.second) {
                set<char> temp = firstOf(rhs);
                int before = FIRST[nt].size();
                FIRST[nt].insert(temp.begin(), temp.end());
                if (FIRST[nt].size() != before) changed = true;
            }
        }
    } while (changed);
}

void computeFollow() {
    FOLLOW[startSymbol].insert('$');
    bool changed;
    do {
        changed = false;
        for (auto &p : grammar) {
            char A = p.first;
            for (auto rhs : p.second) {
                for (int i = 0; i < rhs.size(); ++i) {
                    char B = rhs[i];
                    if (!isupper(B)) continue;
                    string beta = "";
                    if (i + 1 < rhs.size()) beta = rhs.substr(i + 1);
                    set<char> firstBeta = firstOf(beta);
                    int before = FOLLOW[B].size();
                    for (char f : firstBeta)
                        if (f != '#')
                            FOLLOW[B].insert(f);
                    if (beta.empty() || firstBeta.count('#'))
                        FOLLOW[B].insert(FOLLOW[A].begin(), FOLLOW[A].end());
                    if (FOLLOW[B].size() != before) changed = true;
                }
            }
        }
    } while (changed);
}

void buildTable() {
    for (auto &p : grammar) {
        char A = p.first;
        for (auto rhs : p.second) {
            set<char> f = firstOf(rhs);
            for (char a : f)
                if (a != '#')
                    table[{A, a}] = rhs;
            if (f.count('#'))
                for (char b : FOLLOW[A])
                    table[{A, b}] = rhs;
        }
    }
}

int main() {
    int n;
    cout << "Enter number of productions: ";
    cin >> n;
    cout << "Enter productions (e.g., S->AB|a):\n";

    for (int i = 0; i < n; i++) {
        string input;
        cin >> input;
        char lhs = input[0];
        if (i == 0) startSymbol = lhs;
        nonTerminals.insert(lhs);

        string rhs = "";
        for (int j = 3; j < input.size(); j++) rhs += input[j];
        string prod = "";
        for (int j = 0; j < rhs.size(); j++) {
            if (rhs[j] == '|') {
                grammar[lhs].push_back(prod);
                prod = "";
            } else prod += rhs[j];
        }
        if (!prod.empty()) grammar[lhs].push_back(prod);

        for (char c : rhs)
            if (isTerminal(c) && c != '|' && c != '#')
                terminals.insert(c);
    }

    computeFirst();
    computeFollow();
    buildTable();

    cout << "\nFIRST sets:\n";
    for (auto &p : FIRST) {
        cout << "FIRST(" << p.first << ") = { ";
        for (char c : p.second) cout << c << " ";
        cout << "}\n";
    }

    cout << "\nFOLLOW sets:\n";
    for (auto &p : FOLLOW) {
        cout << "FOLLOW(" << p.first << ") = { ";
        for (char c : p.second) cout << c << " ";
        cout << "}\n";
    }
    return 0;
}
