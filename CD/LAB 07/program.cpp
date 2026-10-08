#include <bits/stdc++.h>
using namespace std;

// Context-Free Grammar for RR(1) parsing:
// S -> A B C | S A
// A -> a b | a
// B -> b c | b
// C -> c d | c
set<string> nT = {"S", "A", "B", "C"};
vector<string> prods = {
    "S -> A B C", "S -> S A",
    "A -> a b", "A -> a",
    "B -> b c", "B -> b",
    "C -> c d", "C -> c"
};

// Data structures for LAST and PRECEDE sets, and RR(1) Parsing Table
map<string, set<string>> lastS, precS;
map<string, map<string, string>> PT;

// Splits the right-hand side of a production rule into tokens
vector<string> splitRHS(const string &prod)
{
    vector<string> tokens;
    string word;
    stringstream ss(prod.substr(prod.find("->") + 3));
    while (ss >> word)
        tokens.push_back(word);
    return tokens;
}

// Computes the LAST set for every non-terminal by processing the grammar from right to left.
// Analogous to FIRST in LL(1), LAST(A) is the set of terminals that can appear at the end of a production of A.
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

            // Right-to-left processing: inspect the rightmost symbol
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

// Computes the PRECEDE set for every non-terminal.
// Analogous to FOLLOW in LL(1), PRECEDE(A) is the set of terminals that can appear immediately before A in any sentential form.
// $ is the beginning-of-input marker.
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
                // The symbol immediately preceding rhs[i] is rhs[i - 1]
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
                    // If at the start of RHS (i == 0), whatever precedes lhs precedes rhs[0]
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

// Constructs the RR(1) Parsing Table internally to guide the right-to-left parser
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

// Parses the input string from right to left using the RR(1) algorithm and $ as beginning-of-input marker
bool parseInputString(const string &rawInput)
{
    // Beginning-of-input marker is '$'
    vector<string> inputTokens = {"$"};
    for (char c : rawInput)
    {
        inputTokens.push_back(string(1, c));
    }

    // Stack is initialized with beginning marker '$' and start symbol 'S'
    vector<string> stack = {"$", "S"};
    // Cursor starts at the rightmost input symbol
    int cursor = (int)inputTokens.size() - 1;

    cout << "Input String: \"" << rawInput << "\" (with marker: ";
    for (const string &s : inputTokens) cout << s << " ";
    cout << ")\n";

    cout << left << setw(30) << "STACK" << setw(30) << "INPUT" << "ACTION\n";
    cout << string(80, '-') << "\n";

    while (!stack.empty())
    {
        string top = stack.back();
        string curr = (cursor >= 0) ? inputTokens[cursor] : "$";

        // Format current stack and remaining input (from beginning $ up to cursor)
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
                cout << rawInput << " -> ACCEPTED\n\n";
                return true;
            }
            cout << "Match " << curr << "\n";
            stack.pop_back();
            cursor--; // Move input cursor leftwards
        }
        else if (nT.count(top) && PT[top].count(curr))
        {
            string prod = PT[top][curr];
            cout << "Output " << prod << "\n";
            stack.pop_back();
            vector<string> rhs = splitRHS(prod);

            // In RR(1), push RHS symbols in left-to-right order so the rightmost symbol is on top
            for (size_t i = 0; i < rhs.size(); i++)
            {
                stack.push_back(rhs[i]);
            }
        }
        else
        {
            cout << "Error\n";
            cout << string(80, '-') << "\n";
            cout << rawInput << " -> REJECTED\n\n";
            return false;
        }
    }

    cout << string(80, '-') << "\n";
    cout << rawInput << " -> REJECTED\n\n";
    return false;
}

// Displays the computed LAST and PRECEDE sets matching the lab manual specification
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

// Optional: Displays the RR(1) Parsing Table (Not required by Lab 07 manual, but available for inspection)
void printTable()
{
    vector<string> terms = {"a", "b", "c", "d", "$"};
    vector<string> nonTermOrder = {"S", "A", "B", "C"};

    cout << "RR(1) Parsing Table:\n";
    cout << "--------------------------------------------------------\n";
    cout << "NT\t";
    for (const string &t : terms)
        cout << t << "\t\t";
    cout << "\n--------------------------------------------------------\n";

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
                cout << "--\t\t";
            }
        }
        cout << "\n";
    }
    cout << "--------------------------------------------------------\n\n";
}

int main()
{
    // Step 1: Compute LAST sets
    computeLast();

    // Step 2: Compute PRECEDE sets
    computePrecede();

    // Step 3: Build RR(1) table internally
    buildPT();

    // Step 4: Display LAST and PRECEDE sets
    printSets();

    // Step 5: Test inputs specified in the lab manual
    vector<string> testInputs = {
        "abcd",
        "abc",
        "ababcd",
        "ababc",
        "abbcd"
    };

    cout << "Testing Inputs from Lab Manual:\n";
    cout << "================================================================================\n\n";

    for (const string &s : testInputs)
    {
        parseInputString(s);
    }

    return 0;
}
