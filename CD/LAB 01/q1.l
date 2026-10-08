%{
#include <bits/stdc++.h>
using namespace std;
%}

%option noyywrap 

%%
[ \t\n]+    

"Prog"|"Integer"|"Begin"|"read"|"if"|"then"|"else"|"endif"|"while"|"do"|"endwhile"|"write"|"end" { 
    cout << "keyword: " << yytext << endl; 
}

[a-zA-Z_][a-zA-Z0-9_]* { 
    cout << "identifier: " << yytext << endl; 
}

[0-9]+ { 
    cout << "Integer: " << yytext << endl; 
}

":=" { 
    cout << "assign_opr: " << yytext << endl; 
}

"<"|"+"|"*" { 
    cout << "opr: " << yytext << endl; 
}

";"|"," { 
    cout << "punctuation: " << yytext << endl; 
}

. { 
    cout << "unidentified_token: " << yytext << endl; 
}
%%

int main() {
    cout << "Enter code for Q1 (Press Ctrl+D to stop):" << endl;
    yylex();
    return 0;
}