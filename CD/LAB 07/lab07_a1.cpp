#include <bits/stdc++.h>
using namespace std;

set<string> nT = {"S", "A", "B", "C"};
vector<string> prods = {
    "S -> A B C", "S -> S A",
    "A -> a b", "A -> a",
    "B -> b c", "B -> b",
    "C -> c d", "C -> c"
};

map<string, set<string>> lastS, precS;
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

void computeLast()
{
    bool changed = true;
    while (changed)
    {
        changed = false;
        for (const auto &prod : prods)
        {
            string lhs = prod.substr(0, prod.find(" ->"));
            vector<string> rhs = splitRHS(prod);
            int prevSize = lastS[lhs].size();

            string lastSym = rhs.back();
            if (!nT.count(lastSym))
            {
                lastS[lhs].insert(lastSym);
            }
            else
            {
                for (const string &f : lastS[lastSym])
                {
                    if (f != "eps")
                        lastS[lhs].insert(f);
                }
            }
            if ((int)lastS[lhs].size() > prevSize)
                changed = true;
        }
    }
}

void computePrecede()
{
    precS["S"].insert("$");
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

                int prevSize = precS[rhs[i]].size();
                if (i > 0)
                {
                    string prevSym = rhs[i - 1];
                    if (!nT.count(prevSym))
                    {
                        precS[rhs[i]].insert(prevSym);
                    }
                    else
                    {
                        for (const string &f : lastS[prevSym])
                        {
                            if (f != "eps")
                                precS[rhs[i]].insert(f);
                        }
                    }
                }
                else
                {
                    for (const string &f : precS[lhs])
                    {
                        precS[rhs[i]].insert(f);
                    }
                }
                if ((int)precS[rhs[i]].size() > prevSize)
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
        string lastSym = rhs.back();

        set<string> lastOfRHS;
        if (!nT.count(lastSym))
        {
            lastOfRHS.insert(lastSym);
        }
        else
        {
            lastOfRHS = lastS[lastSym];
        }

        for (const string &term : lastOfRHS)
        {
            if (term != "eps")
                PT[lhs][term] = prod;
        }
        if (lastOfRHS.count("eps"))
        {
            for (const string &term : precS[lhs])
            {
                PT[lhs][term] = prod;
            }
        }
    }
}

void printSets()
{
    cout << "LAST sets:\n";
    for (const string nt : {"S", "A", "B", "C"})
    {
        cout << "LAST(" << nt << ") = { ";
        for (auto it = lastS[nt].begin(); it != lastS[nt].end(); ++it)
        {
            cout << *it << (next(it) != lastS[nt].end() ? ", " : "");
        }
        cout << " }\n";
    }

    cout << "\nPRECEDE sets:\n";
    for (const string nt : {"S", "A", "B", "C"})
    {
        cout << "PRECEDE(" << nt << ") = { ";
        for (auto it = precS[nt].begin(); it != precS[nt].end(); ++it)
        {
            cout << *it << (next(it) != precS[nt].end() ? ", " : "");
        }
        cout << " }\n";
    }
    cout << "\n";
}

void printTable()
{
    vector<string> terms = {"a", "b", "c", "d", "$"};
    vector<string> nonTermOrder = {"S", "A", "B", "C"};

    cout << "RR(1) Parsing Table:\n";
    cout << string(80, '-') << "\n";
    cout << "NT\t";
    for (const string &t : terms)
        cout << t << "\t\t";
    cout << string(80, '-') << "\n";

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
                cout << "\t\t";
            }
        }
        cout << "\n";
    }
    cout << string(80, '-') << "\n\n";
}

bool parseInputString(const string &inpStr)
{
    vector<string> inputTokens = {"$"};
    for (char c : inpStr)
    {
        inputTokens.push_back(string(1, c));
    }

    vector<string> stack = {"$", "S"};
    int cursor = (int)inputTokens.size() - 1;

    cout << "Input String: \"" << inpStr << "\" (with marker: ";
    for (const string &s : inputTokens) cout << s << " ";
    cout << ")\n";

    cout << left << setw(30) << "STACK" << setw(30) << "INPUT" << "ACTION\n";
    cout << string(80, '-') << "\n";

    while (!stack.empty())
    {
        string top = stack.back();
        string curr = (cursor >= 0) ? inputTokens[cursor] : "$";

        string stackStr = "", inputStr = "";
        for (const string &s : stack)
            stackStr += s + " ";
        for (int i = 0; i <= cursor; i++)
            inputStr += inputTokens[i] + " ";

        cout << left << setw(30) << stackStr << setw(30) << inputStr;

        if (top == curr)
        {
            if (top == "$")
            {
                cout << "Accept\n";
                cout << string(80, '-') << "\n";
                cout << inpStr << " -> ACCEPTED\n\n";
                return true;
            }
            cout << "Match " << curr << "\n";
            stack.pop_back();
            cursor--;
        }
        else if (nT.count(top) && PT[top].count(curr))
        {
            string prod = PT[top][curr];
            cout << "Output " << prod << "\n";
            stack.pop_back();
            vector<string> rhs = splitRHS(prod);

            for (size_t i = 0; i < rhs.size(); i++)
            {
                stack.push_back(rhs[i]);
            }
        }
        else
        {
            cout << "Error\n";
            cout << string(80, '-') << "\n";
            cout << inpStr << " -> REJECTED\n\n";
            return false;
        }
    }

    cout << string(80, '-') << "\n";
    cout << inpStr << " -> REJECTED\n\n";
    return false;
}

int main()
{
    computeLast();
    computePrecede();
    buildPT();

    printSets();
    printTable();

    vector<string> testInputs = {
        "abcd",
        "abc",
        "ababcd",
        "ababc",
        "abbcd"
    };

    for (const string &s : testInputs)
    {
        parseInputString(s);
    }

    return 0;
}
