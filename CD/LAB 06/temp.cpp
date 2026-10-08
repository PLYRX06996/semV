#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <stack>
#include <iomanip>
#include <algorithm>

using namespace std;
const string EPSILON = "eps";
const vector<string> vars = {"E", "E'", "T", "T'", "F"};
const set<string> tokens = {"id", "+", "*", "(", ")", "$"};

map<string, vector<vector<string>>> cfg = {
    {"E", {{"T", "E'"}}},
    {"E'", {{"+", "T", "E'"}, {EPSILON}}},
    {"T", {{"F", "T'"}}},
    {"T'", {{"*", "F", "T'"}, {EPSILON}}},
    {"F", {{"(", "E", ")"}, {"id"}}}};

map<string, set<string>> firstSet;
map<string, set<string>> followSet;
map<string, map<string, vector<string>>> predictiveTable;

set<string> findSeqFirst(const vector<string> &rhs)
{
    set<string> seqFirst;
    bool hasEpsilon = true;

    for (const string &symbol : rhs)
    {
        if (tokens.count(symbol) || symbol == EPSILON)
        {
            seqFirst.insert(symbol);
            hasEpsilon = false;
            break;
        }

        bool symbolDerivesEps = false;
        for (const string &f : firstSet[symbol])
        {
            if (f == EPSILON)
            {
                symbolDerivesEps = true;
            }
            else
            {
                seqFirst.insert(f);
            }
        }

        if (!symbolDerivesEps)
        {
            hasEpsilon = false;
            break;
        }
    }

    if (hasEpsilon)
    {
        seqFirst.insert(EPSILON);
    }
    return seqFirst;
}

void computeFirst()
{
    bool isModified;
    do
    {
        isModified = false;
        for (const string &lhs : vars)
        {
            for (const vector<string> &rhs : cfg[lhs])
            {
                bool ruleHasEps = true;

                for (const string &symbol : rhs)
                {
                    if (tokens.count(symbol) || symbol == EPSILON)
                    {
                        if (firstSet[lhs].insert(symbol).second)
                            isModified = true;
                        ruleHasEps = false;
                        break;
                    }
                    else
                    {
                        bool localEps = false;
                        for (const string &item : firstSet[symbol])
                        {
                            if (item != EPSILON)
                            {
                                if (firstSet[lhs].insert(item).second)
                                    isModified = true;
                            }
                            else
                            {
                                localEps = true;
                            }
                        }
                        if (!localEps)
                        {
                            ruleHasEps = false;
                            break;
                        }
                    }
                }

                if (ruleHasEps)
                {
                    if (firstSet[lhs].insert(EPSILON).second)
                        isModified = true;
                }
            }
        }
    } while (isModified);
}

void computeFollow()
{
    followSet["E"].insert("$");
    bool isModified;

    do
    {
        isModified = false;
        for (const string &lhs : vars)
        {
            for (const auto &rhs : cfg[lhs])
            {
                for (size_t i = 0; i < rhs.size(); ++i)
                {
                    string current = rhs[i];

                    if (tokens.count(current) || current == EPSILON)
                        continue;

                    bool nextHasEps = true;
                    for (size_t j = i + 1; j < rhs.size(); ++j)
                    {
                        string nextSym = rhs[j];

                        if (tokens.count(nextSym))
                        {
                            if (followSet[current].insert(nextSym).second)
                                isModified = true;
                            nextHasEps = false;
                            break;
                        }
                        else
                        {
                            bool epsFound = false;
                            for (const string &f : firstSet[nextSym])
                            {
                                if (f != EPSILON)
                                {
                                    if (followSet[current].insert(f).second)
                                        isModified = true;
                                }
                                else
                                {
                                    epsFound = true;
                                }
                            }
                            if (!epsFound)
                            {
                                nextHasEps = false;
                                break;
                            }
                        }
                    }

                    if (nextHasEps)
                    {
                        for (const string &fol : followSet[lhs])
                        {
                            if (followSet[current].insert(fol).second)
                                isModified = true;
                        }
                    }
                }
            }
        }
    } while (isModified);
}

void constructTable()
{
    for (const string &lhs : vars)
    {
        for (const vector<string> &rhs : cfg[lhs])
        {
            set<string> rhsFirst = findSeqFirst(rhs);

            for (const string &terminal : rhsFirst)
            {
                if (terminal != EPSILON)
                {
                    predictiveTable[lhs][terminal] = rhs;
                }
            }

            if (rhsFirst.count(EPSILON))
            {
                for (const string &terminal : followSet[lhs])
                {
                    predictiveTable[lhs][terminal] = rhs;
                }
            }
        }
    }
}

string formatRule(const string &left, const vector<string> &right)
{
    string res = left + " -> ";
    for (const string &s : right)
        res += s + " ";
    return res;
}

void printSets()
{
    cout << "--- FIRST SETS ---\n";
    for (const string &v : vars)
    {
        cout << "FIRST(" << setw(2) << v << ") = { ";
        for (const string &val : firstSet[v])
            cout << val << " ";
        cout << "}\n";
    }

    cout << "\n--- FOLLOW SETS ---\n";
    for (const string &v : vars)
    {
        cout << "FOLLOW(" << setw(2) << v << ") = { ";
        for (const string &val : followSet[v])
            cout << val << " ";
        cout << "}\n";
    }
}

void printTable()
{
    cout << "\n--- PREDICTIVE PARSING TABLE ---\n";
    vector<string> columns = {"id", "+", "*", "(", ")", "$"};

    cout << left << setw(8) << "VAR";
    for (const string &col : columns)
        cout << setw(18) << col;
    cout << "\n"
         << string(110, '=') << "\n";

    for (const string &lhs : vars)
    {
        cout << left << setw(8) << lhs;
        for (const string &col : columns)
        {
            if (predictiveTable[lhs].count(col))
            {
                cout << setw(18) << formatRule(lhs, predictiveTable[lhs][col]);
            }
            else
            {
                cout << setw(18) << "-";
            }
        }
        cout << "\n";
    }
}

bool verifyLL1()
{
    for (const string &lhs : vars)
    {
        const auto &rules = cfg[lhs];
        for (size_t i = 0; i < rules.size(); ++i)
        {
            set<string> f1 = findSeqFirst(rules[i]);
            for (size_t j = i + 1; j < rules.size(); ++j)
            {
                set<string> f2 = findSeqFirst(rules[j]);

                // Intersection check
                for (const string &item : f1)
                {
                    if (item != EPSILON && f2.count(item))
                        return false;
                }

                if (f1.count(EPSILON))
                {
                    for (const string &item : f2)
                    {
                        if (followSet[lhs].count(item))
                            return false;
                    }
                }
                if (f2.count(EPSILON))
                {
                    for (const string &item : f1)
                    {
                        if (followSet[lhs].count(item))
                            return false;
                    }
                }
            }
        }
    }
    return true;
}

void simulateParse(const vector<string> &buffer)
{
    cout << "\n--- LL(1) PARSE TRACE ---\n";
    stack<string> pStack;
    pStack.push("$");
    pStack.push("E");

    size_t pointer = 0;
    cout << left << setw(30) << "STACK" << setw(30) << "BUFFER" << "ACTION\n";
    cout << string(80, '-') << "\n";

    while (!pStack.empty())
    {
        stack<string> copyStack = pStack;
        vector<string> stackItems;
        while (!copyStack.empty())
        {
            stackItems.push_back(copyStack.top());
            copyStack.pop();
        }

        string sStr = "";
        for (int i = stackItems.size() - 1; i >= 0; --i)
            sStr += stackItems[i] + " ";
        string bStr = "";
        for (size_t i = pointer; i < buffer.size(); ++i)
            bStr += buffer[i] + " ";

        string topElem = pStack.top();
        string lookahead = buffer[pointer];

        if (topElem == lookahead)
        {
            cout << left << setw(30) << sStr << setw(30) << bStr << "Match '" << lookahead << "'\n";
            pStack.pop();
            pointer++;

            if (topElem == "$")
            {
                cout << "\n-> SUCCESS: Input string is valid!\n";
                return;
            }
        }
        else if (tokens.count(topElem) || topElem == "$")
        {
            cout << left << setw(30) << sStr << setw(30) << bStr << "Error: Unexpected terminal\n";
            cout << "\n-> FAILURE: String rejected.\n";
            return;
        }
        else
        {
            if (predictiveTable[topElem].count(lookahead))
            {
                vector<string> rule = predictiveTable[topElem][lookahead];
                pStack.pop();

                cout << left << setw(30) << sStr << setw(30) << bStr << formatRule(topElem, rule) << "\n";

                if (!(rule.size() == 1 && rule[0] == EPSILON))
                {
                    for (int i = rule.size() - 1; i >= 0; --i)
                    {
                        pStack.push(rule[i]);
                    }
                }
            }
            else
            {
                cout << left << setw(30) << sStr << setw(30) << bStr << "Error: Blank table entry\n";
                cout << "\n-> FAILURE: String rejected.\n";
                return;
            }
        }
    }
}

int main()
{
    computeFirst();
    computeFollow();
    printSets();
    constructTable();
    printTable();

    vector<string> inputString = {"id", "+", "id", "*", "id", "$"};
    if (verifyLL1())
    {
        simulateParse(inputString);
    }
    else
    {
        cout << "\nGrammar is not LL(1) compliant!\n";
    }

    return 0;
}