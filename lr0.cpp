#include <bits/stdc++.h>
using namespace std;

map<char, vector<string>> grammar;   
vector<set<string>> itemSets;        
vector<map<char, int>> transitions;  
set<char> symbols;                  
char startSymbol;

bool isNonTerminal(char c) { return isupper(c); }

string makeItem(char lhs, string rhs, int dot) {
    return string(1, lhs) + "->" + rhs.substr(0, dot) + "." + rhs.substr(dot);
}

void parseItem(const string &item, char &lhs, string &rhs, int &dot) {
    lhs = item[0];
    rhs = item.substr(3);
    dot = rhs.find('.');
    rhs.erase(dot, 1);
}

set<string> closure(set<string> items) {
    bool changed = true;
    while (changed) {
        changed = false;
        for (auto it : vector<string>(items.begin(), items.end())) {
            char lhs; string rhs; int dot;
            parseItem(it, lhs, rhs, dot);
            if (dot < rhs.size() && isNonTerminal(rhs[dot])) {
                for (auto &prod : grammar[rhs[dot]]) {
                    string newItem = makeItem(rhs[dot], prod, 0);
                    if (!items.count(newItem)) items.insert(newItem), changed = true;
                }
            }
        }
    }
    return items;
}

set<string> goTo(set<string> items, char sym) {
    set<string> next;
    for (auto it : items) {
        char lhs; string rhs; int dot;
        parseItem(it, lhs, rhs, dot);
        if (dot < rhs.size() && rhs[dot] == sym)
            next.insert(makeItem(lhs, rhs, dot + 1));
    }
    return next.empty() ? next : closure(next);
}

int findItemSetIndex(set<string> &items) {
    for (int i = 0; i < itemSets.size(); i++)
        if (itemSets[i] == items) return i;
    return -1;
}

int main() {
    int n; cout << "Enter number of productions: "; cin >> n;
    cout << "Enter productions (S->AB|a):\n";

    for (int i = 0; i < n; i++) {
        string s; cin >> s; char lhs = s[0];
        if (i == 0) startSymbol = lhs;
        string rhs = s.substr(3), cur = "";
        for (char c : rhs) {
            if (c == '|') grammar[lhs].push_back(cur), cur = "";
            else cur += c, symbols.insert(c);
        }
        grammar[lhs].push_back(cur);
        symbols.insert(lhs);
    }

    set<string> start = closure({ makeItem('$', string(1, startSymbol), 0) });
    itemSets.push_back(start); transitions.push_back({});

    vector<char> symList(symbols.begin(), symbols.end());
    for (int i = 0; i < itemSets.size(); i++) {
        for (char sym : symList) {
            set<string> next = goTo(itemSets[i], sym);
            if (next.empty()) continue;
            int idx = findItemSetIndex(next);
            if (idx == -1) {
                idx = itemSets.size();
                itemSets.push_back(next);
                transitions.push_back({});
            }
            transitions[i][sym] = idx;
        }
    }

    cout << "\nCanonical LR(0) Item Sets:\n\n";
    for (int i = 0; i < itemSets.size(); i++) {
        cout << "I" << i << ":\n";
        for (auto &it : itemSets[i]) cout << "  " << it << "\n";
        for (auto &p : transitions[i])
            cout << "  GOTO(I" << i << ", " << p.first << ") = I" << p.second << "\n";
        cout << "\n";
    }
}
