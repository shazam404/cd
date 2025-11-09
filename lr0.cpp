#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <cctype>
#include <string>
using namespace std;

map<char, vector<string>> grammar;
vector<set<string>> itemSets;
vector<map<char, int>> transitions;
set<char> allSymbols;
char startSymbol;

bool isNonTerminal(char c) { return isupper(c); }

string makeItem(char lhs, string rhs, int dotPos) {
    return string(1, lhs) + "->" + rhs.substr(0, dotPos) + "." + rhs.substr(dotPos);
}

void parseItem(const string &item, char &lhs, string &rhs, int &dotPos) {
    int arrow = item.find("->");
    lhs = item[0];
    rhs = item.substr(arrow + 2);
    dotPos = rhs.find('.');
    rhs.erase(dotPos, 1);
}

set<string> closure(set<string> items) {
    bool changed = true;
    while (changed) {
        changed = false;
        vector<string> current(items.begin(), items.end());
        for (auto &it : current) {
            char lhs; string rhs; int dotPos;
            parseItem(it, lhs, rhs, dotPos);
            if (dotPos < rhs.size() && isNonTerminal(rhs[dotPos])) {
                char nextSymbol = rhs[dotPos];
                for (auto &production : grammar[nextSymbol]) {
                    string newItem = makeItem(nextSymbol, production, 0);
                    if (!items.count(newItem)) {
                        items.insert(newItem);
                        changed = true;
                    }
                }
            }
        }
    }
    return items;
}

set<string> goTo(set<string> items, char symbol) {
    set<string> nextItems;
    for (auto &it : items) {
        char lhs; string rhs; int dotPos;
        parseItem(it, lhs, rhs, dotPos);
        if (dotPos < rhs.size() && rhs[dotPos] == symbol)
            nextItems.insert(makeItem(lhs, rhs, dotPos + 1));
    }
    return nextItems.empty() ? nextItems : closure(nextItems);
}

int findItemSetIndex(set<string> &items) {
    for (int i = 0; i < itemSets.size(); i++)
        if (itemSets[i] == items) return i;
    return -1;
}

int main() {
    int productionCount;
    cout << "Enter number of productions: ";
    cin >> productionCount;
    cout << "Enter productions (S->AB|a, use # for epsilon):\n";

    for (int i = 0; i < productionCount; i++) {
        string input;
        cin >> input;
        char lhs = input[0];
        if (i == 0) startSymbol = lhs;
        string rhs = input.substr(3);

        string current = "";
        for (char c : rhs) {
            if (c == '|') {
                grammar[lhs].push_back(current);
                current.clear();
            } else {
                current.push_back(c);
                allSymbols.insert(c);
            }
        }
        grammar[lhs].push_back(current);
        allSymbols.insert(lhs);
    }

    string start(1, startSymbol);
    string augmentedStart = "$";
    set<string> startItem = closure({ makeItem(augmentedStart[0], start, 0) });

    itemSets.push_back(startItem);
    transitions.push_back({});
    vector<char> symbolList(allSymbols.begin(), allSymbols.end());

    for (int i = 0; i < itemSets.size(); i++) {
        for (char symbol : symbolList) {
            set<string> nextSet = goTo(itemSets[i], symbol);
            if (nextSet.empty()) continue;
            int index = findItemSetIndex(nextSet);
            if (index == -1) {
                index = itemSets.size();
                itemSets.push_back(nextSet);
                transitions.push_back({});
            }
            transitions[i][symbol] = index;
        }
    }

    cout << "\nCanonical LR(0) Item Sets:\n\n";
    for (int i = 0; i < itemSets.size(); i++) {
        cout << "I" << i << ":\n";
        for (auto &it : itemSets[i])
            cout << it << "\n";
        for (auto &p : transitions[i])
            cout << "GOTO(I" << i << ", " << p.first << ") = I" << p.second << "\n";
        cout << "\n";
    }
}
