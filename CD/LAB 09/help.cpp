#include <bits/stdc++.h> // Includes all standard libraries (vectors, maps, sets, strings, etc.)
using namespace std;

// Structure to represent a Grammar Production rule
struct P { 
    string l;         // LHS (Left-Hand Side) non-terminal of the production (e.g., "E")
    vector<string> r; // RHS (Right-Hand Side) symbols stored as a list (e.g., {"E", "OR", "T"})
};

// Structure to represent a single LR(1) Item: [A -> alpha . beta, a]
struct I {
    int p;      // Index of the production in the global grammar vector 'G'
    int d;      // Position of the dot '.' (0 means dot is at the beginning of RHS)
    string la;  // The lookahead terminal symbol (e.g., "$", "OR")
    
    // Custom comparison operator required to store Items inside a std::set
    bool operator<(const I& o) const {
        if (p != o.p) return p < o.p;   // Sort primarily by production index
        if (d != o.d) return d < o.d;   // Then sort by dot position
        return la < o.la;               // Finally sort by lookahead symbol
    }
    
    // Equality operator to check if two LR(1) items are exactly the same
    bool operator==(const I& o) const {
        return p == o.p && d == o.d && la == o.la;
    }
};

// Typedef to make declaring Sets of Items (which represent Parser States) easier
typedef set<I> IS;

// Global Data Structures to hold the parser's state and tables
vector<P> G;                                  // Stores all grammar productions (0 is augmented start)
vector<string> NT;                            // List of all Non-Terminals
vector<string> T;                             // List of all Terminals
map<string, set<string>> F;                   // Stores the FIRST set for every symbol
vector<IS> CC;                                // The Canonical Collection of states (I0, I1, I2, ...)
map<pair<int, string>, int> GT;               // Raw transitions: GT[{state, symbol}] = next_state
struct Act { string t; int v; };              // Action entry: type ("S"hift, "R"educe, "A"ccept), v = state/prod #
map<pair<int, string>, Act> AT;               // ACTION Table: AT[{state, terminal}] = Action
map<pair<int, string>, int> GoT;              // GOTO Table: GoT[{state, non-terminal}] = next_state
bool cfl = false;                             // Flag set to true if any conflicts are found (means NOT LR(1))
vector<string> conf;                          // Stores descriptions of any Shift/Reduce or Reduce/Reduce conflicts
vector<string> test_in, test_inv;             // Vectors to hold the valid and invalid test strings

// Helper function to check if a given string symbol is a Terminal
bool isT(string x) {
    return find(T.begin(), T.end(), x) != T.end();
}

// Function to prepare the grammar, augment it, and extract terminals/non-terminals
void prepG(vector<P> inG) {
    // 1. Augment the grammar by adding a new start symbol (e.g., E' -> E) at index 0
    G.push_back({inG[0].l + "'", {inG[0].l}});
    
    // 2. Copy the rest of the user-provided grammar into our global 'G'
    for (auto& p : inG) G.push_back(p);
    
    // 3. Extract all unique Non-Terminals (everything that appears on the LHS of a rule)
    set<string> nts;
    for (auto& p : G) nts.insert(p.l);
    for (auto& n : nts) NT.push_back(n); // Store in global NT vector
    
    // 4. Extract all Terminals (symbols on RHS that are NOT in the Non-Terminal set)
    set<string> ts;
    for (auto& p : G) {
        for (auto& s : p.r) {
            if (nts.find(s) == nts.end()) ts.insert(s);
        }
    }
    for (auto& t : ts) T.push_back(t); // Store in global T vector
    T.push_back("$");                  // Manually add the End-Of-Input marker as a terminal
}

// Function to compute FIRST sets for all symbols (needed for lookaheads in CLOSURE)
void cFst() {
    // Rule 1: For any terminal 't', FIRST(t) is just {t}
    for (auto& t : T) F[t].insert(t);
    
    bool ch = true; // Flag to track if sets are still updating
    while (ch) {
        ch = false;
        // Iterate through all productions to propagate FIRST sets bottom-up
        for (auto& p : G) {
            int sz = F[p.l].size(); // Track initial size of LHS's FIRST set
            if (p.r.empty()) continue; // Skip empty (epsilon) RHS for this specific grammar
            
            // Rule 2: Add FIRST(RHS[0]) into FIRST(LHS)
            for (auto& f : F[p.r[0]]) {
                F[p.l].insert(f);
            }
            
            // If the size increased, a new terminal was added, so we must loop again
            if (F[p.l].size() > sz) ch = true;
        }
    }
}

// Helper to get the FIRST set of a sequence of symbols (beta) appended with a lookahead (a)
// Used when calculating lookaheads for new items in the CLOSURE operation
set<string> getF(vector<string> s, string la) {
    set<string> res;
    // If beta is empty, FIRST(beta a) is just the lookahead 'a'
    if (s.empty()) { 
        res.insert(la); 
        return res; 
    }
    // Otherwise, FIRST(beta a) is FIRST(beta[0]) (assuming no epsilon productions here)
    for (auto& f : F[s[0]]) res.insert(f);
    return res;
}

// Function to compute the CLOSURE of a set of LR(1) items
// If [A -> alpha . B beta, a] is in I, then for each production B -> gamma,
// add [B -> . gamma, b] to I, where b is in FIRST(beta a)
IS cls(IS I_set) {
    IS J = I_set; // Start with the core items
    bool ch = true; // Track changes
    
    while (ch) {
        ch = false;
        IS tmp = J; // Use a temp copy to safely iterate while adding new items to J
        
        for (auto& i : tmp) {
            // Check if the dot is NOT at the end of the RHS
            if (i.d < G[i.p].r.size()) {
                string B = G[i.p].r[i.d]; // The symbol immediately after the dot
                
                // If the symbol after the dot is a Non-Terminal, we must expand it
                if (!isT(B)) {
                    vector<string> beta;
                    // Extract 'beta' (everything after symbol B on the RHS)
                    for (int k = i.d + 1; k < G[i.p].r.size(); ++k) {
                        beta.push_back(G[i.p].r[k]);
                    }
                    
                    // Calculate the new lookaheads: b = FIRST(beta + original_lookahead)
                    set<string> f_beta = getF(beta, i.la);
                    
                    // Find all productions where LHS is 'B'
                    for (int k = 0; k < G.size(); ++k) {
                        if (G[k].l == B) {
                            // Create a new item for each possible lookahead 'b'
                            for (auto& b : f_beta) {
                                // .second is true if item was successfully inserted (meaning it's new)
                                if (J.insert({k, 0, b}).second) ch = true;
                            }
                        }
                    }
                }
            }
        }
    }
    return J; // Return the fully closed set of items
}

// Function to compute GOTO(I, X)
// Moves the dot one position past the symbol 'X' for all applicable items in set 'I_set'
IS go_op(IS I_set, string X) {
    IS J; // Set to hold the core of the new state
    for (auto& i : I_set) {
        // If dot is not at the end AND the symbol after the dot matches 'X'
        if (i.d < G[i.p].r.size() && G[i.p].r[i.d] == X) {
            // Add the item to J, moving the dot forward by 1 (i.d + 1)
            J.insert({i.p, i.d + 1, i.la});
        }
    }
    // Return the CLOSURE of the new core items
    return cls(J);
}

// Function to build the Canonical Collection (CC) of all LR(1) item sets (States)
void bCC() {
    IS I0;
    // State 0 core: Augmented production (index 0), dot at 0, lookahead "$"
    I0.insert({0, 0, "$"});
    CC.push_back(cls(I0)); // Expand State 0 and add to CC
    
    bool ch = true;
    while (ch) {
        ch = false;
        // Iterate over all currently existing states
        for (int i = 0; i < CC.size(); ++i) {
            // Create a list of all grammar symbols (Terminals + Non-Terminals)
            vector<string> syms = T;
            syms.insert(syms.end(), NT.begin(), NT.end());
            
            // Try transitioning from the current state 'i' using every possible symbol 'X'
            for (auto& X : syms) {
                IS nxt = go_op(CC[i], X); // Compute the destination state
                
                // If the transition actually goes somewhere (destination state is not empty)
                if (!nxt.empty()) {
                    int idx = -1;
                    // Check if this destination state already exists in our collection
                    for (int j = 0; j < CC.size(); ++j) {
                        if (CC[j] == nxt) { idx = j; break; }
                    }
                    
                    // If it is a completely new state, add it to the collection
                    if (idx == -1) {
                        CC.push_back(nxt);
                        idx = CC.size() - 1; // Index of the newly added state
                        ch = true;           // Keep looping since we found a new state
                    }
                    // Record the raw transition map: State i + Symbol X -> State idx
                    GT[{i, X}] = idx;
                }
            }
        }
    }
}

// Function to construct the ACTION and GOTO parsing tables from the Canonical Collection
void bTab() {
    // Loop over every State (i)
    for (int i = 0; i < CC.size(); ++i) {
        // Loop over every Item (it) inside State (i)
        for (auto& it : CC[i]) {
            
            // REDUCE condition: The dot is at the very end of the RHS
            if (it.d == G[it.p].r.size()) {
                // ACCEPT condition: We are reducing the Augmented Start production with lookahead "$"
                if (it.p == 0 && it.la == "$") {
                    AT[{i, "$"}] = {"A", 0}; // Action = Accept
                } else {
                    // Regular REDUCE: Reduce by production 'it.p' on lookahead 'it.la'
                    
                    // Conflict Checking: If a rule already exists in this table cell
                    if (AT.count({i, it.la})) {
                        if (AT[{i, it.la}].t == "S") {
                            // A Shift already exists -> Shift/Reduce conflict
                            conf.push_back("Shift/Reduce conflict in State " + to_string(i) + " on terminal " + it.la);
                            cfl = true;
                        } else if (AT[{i, it.la}].t == "R" && AT[{i, it.la}].v != it.p) {
                            // Another Reduce already exists -> Reduce/Reduce conflict
                            conf.push_back("Reduce/Reduce conflict in State " + to_string(i) + " on terminal " + it.la);
                            cfl = true;
                        }
                    }
                    // Write the Reduce action to the ACTION table
                    AT[{i, it.la}] = {"R", it.p};
                }
            } 
            // SHIFT condition: The dot is before a Terminal symbol
            else {
                string a = G[it.p].r[it.d]; // The symbol right after the dot
                // If it is a Terminal AND we have a valid transition calculated earlier
                if (isT(a) && GT.count({i, a})) {
                    
                    // Conflict Checking: If a Reduce already exists in this table cell
                    if (AT.count({i, a})) {
                        if (AT[{i, a}].t == "R") {
                            conf.push_back("Shift/Reduce conflict in State " + to_string(i) + " on terminal " + a);
                            cfl = true;
                        }
                    }
                    // Write the Shift action to the ACTION table (v = destination state)
                    AT[{i, a}] = {"S", GT[{i, a}]};
                }
            }
        }
        
        // GOTO Table setup: For every Non-Terminal, if a transition exists from state 'i'
        for (auto& A : NT) {
            if (GT.count({i, A})) {
                GoT[{i, A}] = GT[{i, A}]; // Copy the raw transition into the official GOTO table
            }
        }
    }
}

// Helper function to turn a raw input string into a list of exact terminal tokens
vector<string> tok(string in) {
    vector<string> tk;
    string c = "";
    for (char x : in) {
        if (isspace(x)) continue; // Ignore spaces
        
        // Parentheses are single-character tokens
        if (x == '(' || x == ')') {
            tk.push_back(string(1, x));
        } 
        // Build multi-character keywords (id, OR, AND, NOT)
        else {
            c += x;
            if (c == "id" || c == "OR" || c == "AND" || c == "NOT") { 
                tk.push_back(c); 
                c = ""; // Reset buffer after identifying a keyword
            }
        }
    }
    tk.push_back("$"); // Append EOF token at the end
    return tk;
}

// Function to simulate the LR(1) parsing algorithm on a given tokenized string
void prs(string in) {
    cout << "\nTrace: " << in << "\n";
    vector<string> tk = tok(in); // Get the tokens
    vector<int> st = {0};        // Stack initialized with Start State 0
    vector<string> sy = {"$"};   // Symbol stack (mostly for visualization here)
    int ip = 0;                  // Input Pointer tracking our position in the token list
    
    // The main predictive parsing loop
    while (true) {
        int s = st.back();       // Current state is always top of the stack
        string a = tk[ip];       // Current input lookahead token
        
        // String formatting to display Stack and Input nicely in the console
        string s_str = "", in_str = "";
        for (int x : st) s_str += to_string(x) + " ";
        for (int i = ip; i < tk.size(); i++) in_str += tk[i] + " ";
        cout << left << setw(25) << s_str.substr(0, 24) << setw(25) << in_str.substr(0, 24);

        // Look up the action in our ACTION table for [CurrentState, CurrentLookahead]
        if (AT.count({s, a})) {
            Act ac = AT[{s, a}]; // Retrieve the action struct
            
            // Execution: SHIFT
            if (ac.t == "S") {
                st.push_back(ac.v);   // Push new state to stack
                sy.push_back(a);      // Push symbol to stack
                ip++;                 // Advance input pointer to read next token
                cout << "Shift " << ac.v << "\n";
            } 
            // Execution: REDUCE
            else if (ac.t == "R") {
                // Pop the RHS length off the stack (for both state and symbol stacks)
                for (int i = 0; i < G[ac.v].r.size(); ++i) { 
                    st.pop_back(); 
                    sy.pop_back(); 
                }
                
                // Lookup the GOTO table using the revealed state and LHS of the production
                st.push_back(GoT[{st.back(), G[ac.v].l}]);
                sy.push_back(G[ac.v].l); // Push the LHS symbol
                
                // Print the reduction trace
                cout << "Reduce " << G[ac.v].l << " -> ";
                for(auto r : G[ac.v].r) cout << r << " ";
                cout << "\n";
            } 
            // Execution: ACCEPT
            else if (ac.t == "A") {
                cout << "ACCEPTED\n"; 
                break; // Parsing succeeds, break the loop
            }
        } 
        // If the table cell is empty, the input violates the grammar
        else {
            cout << "REJECTED\n"; 
            break; // Parsing fails, break the loop
        }
    }
}

// Master function to execute all steps sequentially and print outputs mapping to lab requirements
void dispAll() {
    cFst(); // Calc FIRST sets needed for CLOSURE
    bCC();  // Generate Canonical Collection of LR(1) Item Sets
    bTab(); // Build the Action/Goto tables and check for conflicts
    
    // Req 1: Display Augmented Grammar
    cout << "1. Augmented Grammar:\n";
    for (int i = 0; i < G.size(); ++i) {
        cout << i << ": " << G[i].l << " -> ";
        for (auto& x : G[i].r) cout << x << " ";
        cout << "\n";
    }
    
    // Req 2, 3, 4, 5: Display States (CLOSURE and GOTO logic are naturally verified by accurate states)
    cout << "\n2, 3, 4, 5. LR(1) Sets with Lookaheads:\n";
    for (int i = 0; i < CC.size(); ++i) {
        cout << "I" << i << ":\n";
        for (auto& it : CC[i]) {
            cout << "  " << G[it.p].l << " -> ";
            for (int k = 0; k < G[it.p].r.size(); ++k) {
                if (k == it.d) cout << ". "; // Print dot at correct location
                cout << G[it.p].r[k] << " ";
            }
            if (it.d == G[it.p].r.size()) cout << ". "; // Print dot at end if fully traversed
            cout << ", " << it.la << "\n";              // Print the lookahead
        }
    }
    
    // Req 6: Print out the final built ACTION and GOTO tables
    cout << "\n6. ACTION/GOTO Table Constructed:\n";
    cout << string(100, '-') << "\n";
    cout << left << setw(8) << "State";
    for (auto& t : T) cout << setw(8) << t; // Print Terminal Headers (ACTION)
    cout << " | ";
    for (auto& nt : NT) {
        if (nt != G[0].l) cout << setw(6) << nt; // Print Non-Terminal Headers (GOTO)
    }
    cout << "\n" << string(100, '-') << "\n";
    
    // Fill the Table rows
    for (int i = 0; i < CC.size(); ++i) {
        cout << left << setw(8) << i;
        for (auto& t : T) {
            string val = "";
            if (AT.count({i, t})) {
                if (AT[{i, t}].t == "A") val = "acc";
                else val = AT[{i, t}].t + to_string(AT[{i, t}].v); // e.g., "S4" or "R2"
            }
            cout << setw(8) << val;
        }
        cout << " | ";
        for (auto& nt : NT) {
            if (nt == G[0].l) continue;
            string val = "";
            if (GoT.count({i, nt})) val = to_string(GoT[{i, nt}]); // e.g., "7"
            cout << setw(6) << val;
        }
        cout << "\n";
    }
    
    // Req 7: Output any mathematical conflicts found during table construction
    cout << "\n7. Conflicts:\n";
    if (conf.empty()) {
        cout << "None detected.\n";
    } else {
        for (const auto& c : conf) cout << c << "\n";
    }
    
    // Req 8: State whether the grammar is valid LR(1) based on presence of conflicts
    cout << "\n8. Grammar is " << (cfl ? "NOT LR(1)" : "LR(1)") << "\n";
    
    // Req 9, 10: Run the simulated parser over the provided test cases to print traces
    cout << "\n9, 10. LR(1) Parsing traces:\n";
    for (auto& in : test_in) prs(in);
    for (auto& in : test_inv) prs(in);
}

// Entry point of the program
int main() {
    // Hardcode the Boolean Logic Grammar provided in the lab instructions
    vector<P> inG = {
        {"E", {"E", "OR", "T"}},
        {"E", {"T"}},
        {"T", {"T", "AND", "F"}},
        {"T", {"F"}},
        {"F", {"NOT", "F"}},
        {"F", {"(", "E", ")"}},
        {"F", {"id"}}
    };
    
    // Target test cases from instructions
    test_in = {"id OR id AND id", "NOT id OR id", "(id OR id) AND id", "NOT (id AND id)"};
    test_inv = {"id OR AND id", "NOT OR id", "(id OR id", "id AND (id OR)"};
    
    prepG(inG);  // Initialize grammar, find symbols
    dispAll();   // Run the entire LR(1) algorithm pipeline
    
    return 0;
}