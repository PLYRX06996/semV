#include <bits/stdc++.h>
using namespace std;

struct P { string l; vector<string> r; };
struct I {
    int p, d; string la;
    bool operator<(const I& o) const {
        if (p != o.p) return p < o.p;
        if (d != o.d) return d < o.d;
        return la < o.la;
    }
    bool operator==(const I& o) const {
        return p == o.p && d == o.d && la == o.la;
    }
};

typedef set<I> IS;

vector<P> G;
vector<string> NT;
vector<string> T;
map<string, set<string>> F;
vector<IS> CC;
map<pair<int, string>, int> GT;
struct Act { string t; int v; };
map<pair<int, string>, Act> AT;
map<pair<int, string>, int> GoT;
bool cfl = false;
vector<string> conf;
vector<string> test_in, test_inv;

bool isT(string x) {
    return find(T.begin(), T.end(), x) != T.end();
}

void prepG(vector<P> inG) {
    G.push_back({inG[0].l + "'", {inG[0].l}});
    for (auto& p : inG) G.push_back(p);
    
    set<string> nts;
    for (auto& p : G) nts.insert(p.l);
    for (auto& n : nts) NT.push_back(n);
    
    set<string> ts;
    for (auto& p : G) {
        for (auto& s : p.r) if (nts.find(s) == nts.end()) ts.insert(s);
    }
    for (auto& t : ts) T.push_back(t);
    T.push_back("$");
}

void cFst() {
    for (auto& t : T) F[t].insert(t);
    bool ch = true;
    while (ch) {
        ch = false;
        for (auto& p : G) {
            int sz = F[p.l].size();
            if (p.r.empty()) continue;
            for (auto& f : F[p.r[0]]) F[p.l].insert(f);
            if (F[p.l].size() > sz) ch = true;
        }
    }
}

set<string> getF(vector<string> s, string la) {
    set<string> res;
    if (s.empty()) { res.insert(la); return res; }
    for (auto& f : F[s[0]]) res.insert(f);
    return res;
}

IS cls(IS I_set) {
    IS J = I_set;
    bool ch = true;
    while (ch) {
        ch = false;
        IS tmp = J;
        for (auto& i : tmp) {
            if (i.d < G[i.p].r.size()) {
                string B = G[i.p].r[i.d];
                if (!isT(B)) {
                    vector<string> beta;
                    for (int k = i.d + 1; k < G[i.p].r.size(); ++k) beta.push_back(G[i.p].r[k]);
                    set<string> f_beta = getF(beta, i.la);
                    for (int k = 0; k < G.size(); ++k) {
                        if (G[k].l == B) {
                            for (auto& b : f_beta) {
                                if (J.insert({k, 0, b}).second) ch = true;
                            }
                        }
                    }
                }
            }
        }
    }
    return J;
}

IS go_op(IS I_set, string X) {
    IS J;
    for (auto& i : I_set) {
        if (i.d < G[i.p].r.size() && G[i.p].r[i.d] == X) J.insert({i.p, i.d + 1, i.la});
    }
    return cls(J);
}

void bCC() {
    IS I0;
    I0.insert({0, 0, "$"});
    CC.push_back(cls(I0));
    
    bool ch = true;
    while (ch) {
        ch = false;
        for (int i = 0; i < CC.size(); ++i) {
            vector<string> syms = T;
            syms.insert(syms.end(), NT.begin(), NT.end());
            for (auto& X : syms) {
                IS nxt = go_op(CC[i], X);
                if (!nxt.empty()) {
                    int idx = -1;
                    for (int j = 0; j < CC.size(); ++j) {
                        if (CC[j] == nxt) { idx = j; break; }
                    }
                    if (idx == -1) {
                        CC.push_back(nxt);
                        idx = CC.size() - 1;
                        ch = true;
                    }
                    GT[{i, X}] = idx;
                }
            }
        }
    }
}

void bTab() {
    for (int i = 0; i < CC.size(); ++i) {
        for (auto& it : CC[i]) {
            if (it.d == G[it.p].r.size()) {
                if (it.p == 0 && it.la == "$") {
                    AT[{i, "$"}] = {"A", 0};
                } else {
                    if (AT.count({i, it.la})) {
                        if (AT[{i, it.la}].t == "S") {
                            conf.push_back("Shift/Reduce conflict in State " + to_string(i) + " on terminal " + it.la);
                            cfl = true;
                        } else if (AT[{i, it.la}].t == "R" && AT[{i, it.la}].v != it.p) {
                            conf.push_back("Reduce/Reduce conflict in State " + to_string(i) + " on terminal " + it.la);
                            cfl = true;
                        }
                    }
                    AT[{i, it.la}] = {"R", it.p};
                }
            } else {
                string a = G[it.p].r[it.d];
                if (isT(a) && GT.count({i, a})) {
                    if (AT.count({i, a})) {
                        if (AT[{i, a}].t == "R") {
                            conf.push_back("Shift/Reduce conflict in State " + to_string(i) + " on terminal " + a);
                            cfl = true;
                        }
                    }
                    AT[{i, a}] = {"S", GT[{i, a}]};
                }
            }
        }
        for (auto& A : NT) if (GT.count({i, A})) GoT[{i, A}] = GT[{i, A}];
    }
}

vector<string> tok(string in) {
    vector<string> tk;
    string c = "";
    for (char x : in) {
        if (isspace(x)) continue;
        if (x == '(' || x == ')') tk.push_back(string(1, x));
        else {
            c += x;
            if (c == "id" || c == "OR" || c == "AND" || c == "NOT") { tk.push_back(c); c = ""; }
        }
    }
    tk.push_back("$");
    return tk;
}

void prs(string in) {
    cout << "\nTrace: " << in << "\n";
    vector<string> tk = tok(in);
    vector<int> st = {0};
    vector<string> sy = {"$"};
    int ip = 0;
    
    while (true) {
        int s = st.back();
        string a = tk[ip];
        string s_str = "", in_str = "";
        for (int x : st) s_str += to_string(x) + " ";
        for (int i = ip; i < tk.size(); i++) in_str += tk[i] + " ";
        
        cout << left << setw(25) << s_str.substr(0, 24) << setw(25) << in_str.substr(0, 24);

        if (AT.count({s, a})) {
            Act ac = AT[{s, a}];
            if (ac.t == "S") {
                st.push_back(ac.v); sy.push_back(a); ip++;
                cout << "Shift " << ac.v << "\n";
            } else if (ac.t == "R") {
                for (int i = 0; i < G[ac.v].r.size(); ++i) { st.pop_back(); sy.pop_back(); }
                st.push_back(GoT[{st.back(), G[ac.v].l}]);
                sy.push_back(G[ac.v].l);
                cout << "Reduce " << G[ac.v].l << " -> ";
                for(auto r : G[ac.v].r) cout << r << " ";
                cout << "\n";
            } else if (ac.t == "A") {
                cout << "ACCEPTED\n"; break;
            }
        } else {
            cout << "REJECTED\n"; break;
        }
    }
}

void dispAll() {
    cFst();
    bCC();
    bTab();
    
    cout << "1. Augmented Grammar:\n";
    for (int i = 0; i < G.size(); ++i) {
        cout << i << ": " << G[i].l << " -> ";
        for (auto& x : G[i].r) cout << x << " ";
        cout << "\n";
    }
    
    cout << "\n2, 3, 4, 5. LR(1) Sets with Lookaheads:\n";
    for (int i = 0; i < CC.size(); ++i) {
        cout << "I" << i << ":\n";
        for (auto& it : CC[i]) {
            cout << "  " << G[it.p].l << " -> ";
            for (int k = 0; k < G[it.p].r.size(); ++k) {
                if (k == it.d) cout << ". ";
                cout << G[it.p].r[k] << " ";
            }
            if (it.d == G[it.p].r.size()) cout << ". ";
            cout << ", " << it.la << "\n";
        }
    }
    
    cout << "\n6. ACTION/GOTO Table Constructed:\n";
    cout << string(100, '-') << "\n";
    cout << left << setw(8) << "State";
    for (auto& t : T) cout << setw(8) << t;
    cout << " | ";
    for (auto& nt : NT) {
        if (nt != G[0].l) cout << setw(6) << nt;
    }
    cout << "\n" << string(100, '-') << "\n";
    
    for (int i = 0; i < CC.size(); ++i) {
        cout << left << setw(8) << i;
        for (auto& t : T) {
            string val = "";
            if (AT.count({i, t})) {
                if (AT[{i, t}].t == "A") val = "acc";
                else val = AT[{i, t}].t + to_string(AT[{i, t}].v);
            }
            cout << setw(8) << val;
        }
        cout << " | ";
        for (auto& nt : NT) {
            if (nt == G[0].l) continue;
            string val = "";
            if (GoT.count({i, nt})) val = to_string(GoT[{i, nt}]);
            cout << setw(6) << val;
        }
        cout << "\n";
    }
    
    cout << "\n7. Conflicts:\n";
    if (conf.empty()) {
        cout << "None detected.\n";
    } else {
        for (const auto& c : conf) cout << c << "\n";
    }
    
    cout << "\n8. Grammar is " << (cfl ? "NOT LR(1)" : "LR(1)") << "\n";
    
    cout << "\n9, 10. LR(1) Parsing traces:\n";
    for (auto& in : test_in) prs(in);
    for (auto& in : test_inv) prs(in);
}

int main() {
    vector<P> inG = {
        {"E", {"E", "OR", "T"}},
        {"E", {"T"}},
        {"T", {"T", "AND", "F"}},
        {"T", {"F"}},
        {"F", {"NOT", "F"}},
        {"F", {"(", "E", ")"}},
        {"F", {"id"}}
    };
    test_in = {"id OR id AND id", "NOT id OR id", "(id OR id) AND id", "NOT (id AND id)"};
    test_inv = {"id OR AND id", "NOT OR id", "(id OR id", "id AND (id OR)"};
    
    prepG(inG);
    dispAll();
    
    return 0;
}