#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <iomanip>
#include <sstream>

using namespace std;

// An LR(0) item is represented as a pair: (production_index, dot_position)
using Item = pair<int, int>;
using State = set<Item>;

// Augmented grammar productions: LHS -> RHS tokens
vector<pair<string, vector<string>>> prods = {
    {"S'", {"S"}},
    {"S",  {"id", "=", "E"}},
    {"S",  {"E"}},
    {"E",  {"*", "E"}},
    {"E",  {"id"}}
};

// Terminals and non-terminals for table columns
vector<string> terms = {"id", "=", "*", "$"};
vector<string> nonterms = {"S", "E"};
vector<string> symbols = {"S", "E", "id", "=", "*"};

// Canonical collection of LR(0) states and GOTO transitions
vector<State> states;
map<pair<int, string>, int> trans;

// FIRST and FOLLOW sets for SLR(1)
map<string, set<string>> firstSet, followSet;

// Parsing tables for LR(0) and SLR(1): state -> symbol -> set of actions
map<int, map<string, set<string>>> lrTable, slrTable;

// Computes the closure of a set of LR(0) items
State closure(State I) {
    bool changed = true;
    while (changed) {
        changed = false;
        for (auto it : I) {
            int p = it.first, dot = it.second;
            if (dot < (int)prods[p].second.size()) {
                string B = prods[p].second[dot];
                for (size_t i = 0; i < prods.size(); i++) {
                    if (prods[i].first == B) {
                        if (!I.count({i, 0})) {
                            I.insert({i, 0});
                            changed = true;
                        }
                    }
                }
            }
        }
    }
    return I;
}

// Computes the GOTO set for a state on grammar symbol X
State goTo(const State &I, const string &X) {
    State J;
    for (auto it : I) {
        int p = it.first, dot = it.second;
        if (dot < (int)prods[p].second.size() && prods[p].second[dot] == X) {
            J.insert({p, dot + 1});
        }
    }
    return closure(J);
}

// Generates the canonical collection of LR(0) states
void findStates() {
    states.push_back(closure({{0, 0}}));
    for (size_t i = 0; i < states.size(); i++) {
        for (const string &sym : symbols) {
            State nxt = goTo(states[i], sym);
            if (nxt.empty()) continue;
            int dest = -1;
            for (size_t j = 0; j < states.size(); j++) {
                if (states[j] == nxt) {
                    dest = j;
                    break;
                }
            }
            if (dest == -1) {
                states.push_back(nxt);
                dest = states.size() - 1;
            }
            trans[{i, sym}] = dest;
        }
    }
}

// Computes FIRST and FOLLOW sets required for SLR(1)
void findFirstFollow() {
    for (const string &t : terms) firstSet[t].insert(t);

    bool changed = true;
    while (changed) {
        changed = false;
        for (auto &p : prods) {
            string A = p.first;
            string X = p.second[0];
            size_t oldSize = firstSet[A].size();
            for (const string &f : firstSet[X]) firstSet[A].insert(f);
            if (firstSet[A].size() > oldSize) changed = true;
        }
    }

    followSet["S'"].insert("$");
    followSet["S"].insert("$");
    changed = true;
    while (changed) {
        changed = false;
        for (auto &p : prods) {
            string A = p.first;
            auto &rhs = p.second;
            for (size_t i = 0; i < rhs.size(); i++) {
                string B = rhs[i];
                if (B != "S" && B != "E") continue;
                size_t oldSize = followSet[B].size();
                if (i + 1 < rhs.size()) {
                    string beta = rhs[i + 1];
                    for (const string &f : firstSet[beta]) followSet[B].insert(f);
                } else {
                    for (const string &f : followSet[A]) followSet[B].insert(f);
                }
                if (followSet[B].size() > oldSize) changed = true;
            }
        }
    }
}

// Constructs both LR(0) and SLR(1) ACTION/GOTO tables
void buildTables() {
    for (size_t i = 0; i < states.size(); i++) {
        for (auto it : states[i]) {
            int p = it.first, dot = it.second;
            auto &rhs = prods[p].second;
            if (dot < (int)rhs.size()) {
                string a = rhs[dot];
                if (trans.count({i, a}) && (a == "id" || a == "=" || a == "*")) {
                    string act = "s" + to_string(trans[{i, a}]);
                    lrTable[i][a].insert(act);
                    slrTable[i][a].insert(act);
                }
            } else {
                if (p == 0) {
                    lrTable[i]["$"].insert("acc");
                    slrTable[i]["$"].insert("acc");
                } else {
                    string act = "r" + to_string(p);
                    for (const string &t : terms) lrTable[i][t].insert(act);
                    for (const string &t : followSet[prods[p].first]) slrTable[i][t].insert(act);
                }
            }
        }
    }
}

// Formats an LR(0) item as a string with the dot
string itemStr(const Item &it) {
    int p = it.first, dot = it.second;
    string s = prods[p].first + " -> ";
    for (size_t i = 0; i <= prods[p].second.size(); i++) {
        if ((int)i == dot) s += ". ";
        if (i < prods[p].second.size()) s += prods[p].second[i] + " ";
    }
    if (!s.empty() && s.back() == ' ') s.pop_back();
    return s;
}

// 1. Prints the numbered augmented grammar
void printGrammar() {
    cout << "1. Augmented Grammar:\n";
    for (size_t i = 0; i < prods.size(); i++) {
        cout << "(" << i << ") " << prods[i].first << " -> ";
        for (const string &s : prods[i].second) cout << s << " ";
        cout << "\n";
    }
    cout << "\n";
}

// 2. Prints the complete set of LR(0) items and state transitions
void printStates() {
    cout << "2. LR(0) Items and States (Total: " << states.size() << "):\n";
    for (size_t i = 0; i < states.size(); i++) {
        cout << "State I" << i << ":\n";
        for (auto it : states[i]) cout << "  " << itemStr(it) << "\n";
        cout << "  Transitions: ";
        bool first = true;
        for (const string &s : symbols) {
            if (trans.count({i, s})) {
                if (!first) cout << ", ";
                cout << s << "->I" << trans[{i, s}];
                first = false;
            }
        }
        if (first) cout << "none";
        cout << "\n\n";
    }
}

// 3 & 6. Prints an ACTION / GOTO parsing table
void printTable(const string &title, const map<int, map<string, set<string>>> &table) {
    cout << title << ":\n";
    cout << string(68, '-') << "\n";
    cout << left << setw(8) << "State" << "| ";
    for (const string &t : terms) cout << setw(9) << t;
    cout << "| ";
    for (const string &nt : nonterms) cout << setw(8) << nt;
    cout << "\n" << string(68, '-') << "\n";

    for (size_t i = 0; i < states.size(); i++) {
        cout << left << setw(8) << i << "| ";
        for (const string &t : terms) {
            string cell = "";
            if (table.count(i) && table.at(i).count(t)) {
                for (const auto &act : table.at(i).at(t)) cell += (cell.empty() ? "" : "/") + act;
            }
            cout << setw(9) << cell;
        }
        cout << "| ";
        for (const string &nt : nonterms) {
            string cell = "";
            if (trans.count({i, nt})) cell = to_string(trans[{i, nt}]);
            cout << setw(8) << cell;
        }
        cout << "\n";
    }
    cout << string(68, '-') << "\n\n";
}

// 4. Detects and reports any conflicts in LR(0) table
void printConflicts() {
    cout << "4. LR(0) Conflicts:\n";
    int count = 0;
    for (size_t i = 0; i < states.size(); i++) {
        for (const string &t : terms) {
            if (lrTable.count(i) && lrTable[i].count(t) && lrTable[i][t].size() > 1) {
                count++;
                cout << "  Conflict in State I" << i << " on terminal '" << t << "': { ";
                for (const auto &act : lrTable[i][t]) cout << act << " ";
                cout << "}\n";
            }
        }
    }
    if (count == 0) cout << "  No conflicts.\n";
    else cout << "  Total conflicts: " << count << " (Grammar is NOT LR(0))\n";
    cout << "\n";
}

// 5. Prints FIRST and FOLLOW sets
void printFirstFollowSets() {
    cout << "5. FIRST and FOLLOW Sets:\n";
    for (string nt : {"S'", "S", "E"}) {
        cout << "  FIRST(" << nt << ") = { ";
        for (const string &s : firstSet[nt]) cout << s << " ";
        cout << "}\n";
    }
    cout << "\n";
    for (string nt : {"S'", "S", "E"}) {
        cout << "  FOLLOW(" << nt << ") = { ";
        for (const string &s : followSet[nt]) cout << s << " ";
        cout << "}\n";
    }
    cout << "\n";
}

// 7. Explains how SLR(1) resolves the LR(0) conflict
void printResolution() {
    cout << "7. Conflict Resolution in SLR(1):\n";
    cout << "  In State I3, LR(0) had conflict on '=' with actions s5 and r4.\n";
    cout << "  SLR(1) places r4 (E -> id) only in FOLLOW(E) = { $ }.\n";
    cout << "  Since '=' is not in FOLLOW(E), r4 is removed from '='.\n";
    cout << "  Only shift s5 remains, resolving the conflict. Grammar is SLR(1).\n\n";
}

// Splits input string into grammar tokens: "id", "=", "*"
vector<string> tokenize(const string &str) {
    vector<string> toks;
    size_t i = 0;
    while (i < str.size()) {
        if (isspace(str[i])) { i++; continue; }
        if (i + 1 < str.size() && str.substr(i, 2) == "id") {
            toks.push_back("id");
            i += 2;
        } else if (str[i] == '=' || str[i] == '*') {
            toks.push_back(string(1, str[i]));
            i++;
        } else {
            toks.push_back(string(1, str[i]));
            i++;
        }
    }
    return toks;
}

// 8. Executes the shift-reduce parsing simulation
void parse(const string &str, const map<int, map<string, set<string>>> &table, const string &mode) {
    vector<string> toks = tokenize(str);
    toks.push_back("$");

    cout << "Parsing \"" << str << "\" using " << mode << ":\n";
    cout << string(68, '-') << "\n";
    cout << left << setw(6) << "Step" << setw(26) << "Stack" << setw(18) << "Input" << "Action\n";
    cout << string(68, '-') << "\n";

    vector<int> stateStack = {0};
    vector<string> symStack;
    size_t ip = 0;
    int step = 1;

    while (true) {
        int s = stateStack.back();
        string a = (ip < toks.size()) ? toks[ip] : "$";

        // Build stack display string
        string stackStr = "";
        for (size_t k = 0; k < symStack.size(); k++) {
            stackStr += to_string(stateStack[k]) + " " + symStack[k] + " ";
        }
        stackStr += to_string(stateStack.back());

        // Build remaining input display string
        string inStr = "";
        for (size_t k = ip; k < toks.size(); k++) inStr += toks[k] + " ";

        cout << left << setw(6) << step++ << setw(26) << stackStr << setw(18) << inStr;

        if (!table.count(s) || !table.at(s).count(a) || table.at(s).at(a).empty()) {
            cout << "Error\n" << string(68, '-') << "\n";
            cout << "Result: REJECTED\n\n";
            return;
        }

        if (table.at(s).at(a).size() > 1) {
            cout << "Conflict (Parser Halted)\n" << string(68, '-') << "\n";
            cout << "Result: HALTED due to conflict\n\n";
            return;
        }

        string act = *table.at(s).at(a).begin();
        if (act[0] == 's') {
            int nxt = stoi(act.substr(1));
            cout << "Shift " << nxt << "\n";
            stateStack.push_back(nxt);
            symStack.push_back(a);
            ip++;
        } else if (act[0] == 'r') {
            int p = stoi(act.substr(1));
            cout << "Reduce " << p << " (" << prods[p].first << " -> ";
            for (const string &x : prods[p].second) cout << x << " ";
            cout << ")\n";

            for (size_t k = 0; k < prods[p].second.size(); k++) {
                stateStack.pop_back();
                symStack.pop_back();
            }
            int top = stateStack.back();
            if (!trans.count({top, prods[p].first})) {
                cout << "GOTO Error\n" << string(68, '-') << "\n";
                cout << "Result: REJECTED\n\n";
                return;
            }
            stateStack.push_back(trans[{top, prods[p].first}]);
            symStack.push_back(prods[p].first);
        } else if (act == "acc") {
            cout << "Accept\n" << string(68, '-') << "\n";
            cout << "Result: ACCEPTED\n\n";
            return;
        }
    }
}

int main() {
    findStates();
    findFirstFollow();
    buildTables();

    printGrammar();
    printStates();
    printTable("3. LR(0) Parsing Table", lrTable);
    printConflicts();
    printFirstFollowSets();
    printTable("6. SLR(1) Parsing Table", slrTable);
    printResolution();

    cout << "8. Parser Implementation & Execution:\n\n";
    vector<string> tests = {"id = id", "* id", "id = * id", "id =", "* = id"};
    for (const string &t : tests) {
        parse(t, lrTable, "LR(0)");
        parse(t, slrTable, "SLR(1)");
    }
    return 0;
}
