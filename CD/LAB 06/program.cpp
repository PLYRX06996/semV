#include <bits/stdc++.h>
using namespace std;

set<string> nT = {"E", "E'", "T", "T'", "F"};
vector<string> prods = {
    "E -> T E'", "E' -> + T E'", "E' -> eps",
    "T -> F T'", "T' -> * F T'", "T' -> eps",
    "F -> ( E )", "F -> id"};

map<string, set<string>> fiS, foS;
map<string, map<string, string>> PT;

vector<string> splitRHS(const string &prod)
{
    vector<string> tokens;
    string word;
    stringstream ss(prod.substr(prod.find("->") + 3));
    while (ss >> word)
        tokens.push_back(word);
    return tokens;
}

void computeFirst()
{
    bool changed = true;
    while (changed)
    {
        changed = false;
        for (const auto &prod : prods)
        {
            string lhs = prod.substr(0, prod.find(" ->"));
            vector<string> rhs = splitRHS(prod);
            int prevSize = fiS[lhs].size();

            if (!nT.count(rhs[0]))
            {
                fiS[lhs].insert(rhs[0]);
            }
            else
            {
                for (const string &f : fiS[rhs[0]])
                {
                    fiS[lhs].insert(f);
                }
            }
            if (fiS[lhs].size() > prevSize)
                changed = true;
        }
    }
}

void computeFollow()
{
    foS["E"].insert("$");
    bool changed = true;
    while (changed)
    {
        changed = false;
        for (const auto &prod : prods)
        {
            string lhs = prod.substr(0, prod.find(" ->"));
            vector<string> rhs = splitRHS(prod);

            for (size_t i = 0; i < rhs.size(); i++)
            {
                if (!nT.count(rhs[i]))
                    continue;

                int prevSize = foS[rhs[i]].size();
                if (i + 1 < rhs.size())
                {
                    string nextSym = rhs[i + 1];
                    if (!nT.count(nextSym))
                    {
                        foS[rhs[i]].insert(nextSym);
                    }
                    else
                    {
                        for (const string &f : fiS[nextSym])
                        {
                            if (f != "eps")
                                foS[rhs[i]].insert(f);
                        }
                        if (fiS[nextSym].count("eps"))
                        {
                            for (const string &f : foS[lhs])
                            {
                                foS[rhs[i]].insert(f);
                            }
                        }
                    }
                }
                else
                {
                    for (const string &f : foS[lhs])
                    {
                        foS[rhs[i]].insert(f);
                    }
                }
                if (foS[rhs[i]].size() > prevSize)
                    changed = true;
            }
        }
    }
}

void buildPT()
{
    for (const auto &prod : prods)
    {
        string lhs = prod.substr(0, prod.find(" ->"));
        vector<string> rhs = splitRHS(prod);
        string firstSym = rhs[0];

        set<string> firstOfRHS;
        if (!nT.count(firstSym))
        {
            firstOfRHS.insert(firstSym);
        }
        else
        {
            firstOfRHS = fiS[firstSym];
        }

        for (const string &term : firstOfRHS)
        {
            if (term != "eps")
                PT[lhs][term] = prod;
        }
        if (firstOfRHS.count("eps"))
        {
            for (const string &term : foS[lhs])
            {
                PT[lhs][term] = prod;
            }
        }
    }
}

void parseInputString(vector<string> inputTokens)
{
    vector<string> stack = {"$", "E"};
    int cursor = 0;

    cout << left << setw(30) << "STACK" << setw(30) << "INPUT" << "ACTION\n";
    cout << string(80, '-') << "\n";

    while (!stack.empty())
    {
        string top = stack.back();
        string curr = inputTokens[cursor];

        string stackStr = "", inputStr = "";
        for (const string &s : stack)
            stackStr += s + " ";
        for (size_t i = cursor; i < inputTokens.size(); i++)
            inputStr += inputTokens[i] + " ";
        cout << left << setw(30) << stackStr << setw(30) << inputStr;

        if (top == curr)
        {
            if (top == "$")
            {
                cout << "Accept\n";
                break;
            }
            cout << "Match " << curr << "\n";
            stack.pop_back();
            cursor++;
        }
        else if (nT.count(top) && PT[top].count(curr))
        {
            string prod = PT[top][curr];
            cout << "Output " << prod << "\n";
            stack.pop_back();
            vector<string> rhs = splitRHS(prod);

            if (rhs[0] != "eps")
            {
                for (int i = rhs.size() - 1; i >= 0; i--)
                {
                    stack.push_back(rhs[i]);
                }
            }
        }
        else
        {
            cout << "Error\n";
            break;
        }
    }
}

void printSets()
{
    cout << "FIRST Sets:\n";
    for (const string &nt : {"E", "E'", "T", "T'", "F"})
    {
        cout << "FIRST(" << nt << ") = { ";
        for (auto it = fiS[nt].begin(); it != fiS[nt].end(); ++it)
        {
            cout << *it << (next(it) != fiS[nt].end() ? ", " : "");
        }
        cout << " }\n";
    }

    cout << "\nFOLLOW Sets:\n";
    for (const string &nt : {"E", "E'", "T", "T'", "F"})
    {
        cout << "FOLLOW(" << nt << ") = { ";
        for (auto it = foS[nt].begin(); it != foS[nt].end(); ++it)
        {
            cout << *it << (next(it) != foS[nt].end() ? ", " : "");
        }
        cout << " }\n";
    }
    cout << "\n";
}

void printTable()
{
    vector<string> terms = {"id", "+", "*", "(", ")", "$"};
    vector<string> nonTermOrder = {"E", "E'", "T", "T'", "F"};

    cout << "LL(1) Parsing Table:\n";
    cout << "-------------\n";

    // printing headers for the cols
    cout << "NT\t";
    for (const string &t : terms)
        cout << t << "\t\t";
    cout << "\n----------------\n";

    // printing rows
    for (const string &nt : nonTermOrder)
    {
        cout << nt << "\t";
        for (const string &t : terms)
        {
            if (PT[nt].count(t))
            {
                string prod = PT[nt][t];
                cout << prod << (prod.length() > 6 ? "\t" : "\t\t");
            }
            else
            {
                cout << "\t";
            }
        }
        cout << "\n";
    }
    cout << "---------------------\n";
}

int main()
{
    computeFirst();
    computeFollow();
    buildPT();

    vector<string> input = {"id", "+", "id", "*", "id", "$"};

    printSets();
    printTable();
    parseInputString(input);
    return 0;
}