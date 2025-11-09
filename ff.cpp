#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
using namespace std;

map<char, vector<string>> g;
map<char, set<char>> F, FO;
char start;

set<char> first(string s)
{
    set<char> r; 
    if (s.empty() || s[0] == '#')
    {
        r.insert('#');
        return r;
    }
    if (!isupper(s[0]))
    {
        r.insert(s[0]);
        return r;
    }
    for (auto p : g[s[0]])
    {
        set<char> t = first(p);
        r.insert(t.begin(), t.end());
    }
    return r;
}

void compFirst()
{
    bool c;
    do
    {
        c = false;
        for (auto &p : g)
        {
            for (auto rhs : p.second)
            {
                int b = F[p.first].size();
                set<char> t = first(rhs);
                F[p.first].insert(t.begin(), t.end());
                if (F[p.first].size() != b)
                    c = true;
            }
        }
    } while (c);
}

void compFollow()
{
    FO[start].insert('$');
    bool c;
    do
    {
        c = false;
        for (auto &p : g)
        {
            for (auto rhs : p.second)
            {
                for (int i = 0; i < rhs.size(); i++)
                {
                    if (!isupper(rhs[i]))
                        continue;
                    string beta = (i + 1 < rhs.size()) ? rhs.substr(i + 1) : ""; 
                    set<char> fb = first(beta);
                    int b = FO[rhs[i]].size();
                    for (char f : fb)
                        if (f != '#')
                            FO[rhs[i]].insert(f);
                    if (beta.empty() || fb.count('#'))
                        FO[rhs[i]].insert(FO[p.first].begin(), FO[p.first].end());
                    if (FO[rhs[i]].size() != b)
                        c = true;
                }
            }
        }
    } while (c);
}

int main()
{
    int n;
    cout << "Enter number of productions: ";
    cin >> n;
    cout << "Enter productions (e.g., S->AB|a):\n";

    for (int i = 0; i < n; i++)
    {
        string s;
        cin >> s;
        if (i == 0)
            start = s[0];
        string rhs = s.substr(3);
        string prod = "";
        for (char c : rhs)
        {
            if (c == '|')
            {
                g[s[0]].push_back(prod);
                prod = "";
            }
            else
                prod += c;
        }
        if (!prod.empty())
            g[s[0]].push_back(prod);
    }

    compFirst();
    compFollow();

    cout << "\nFIRST sets:\n";
    for (auto &p : F)
    {
        cout << "FIRST(" << p.first << ") = { ";
        for (char c : p.second)
            cout << c << " ";
        cout << "}\n";
    }

    cout << "\nFOLLOW sets:\n";
    for (auto &p : FO)
    {
        cout << "FOLLOW(" << p.first << ") = { ";
        for (char c : p.second)
            cout << c << " ";
        cout << "}\n";
    }
    return 0;
}
