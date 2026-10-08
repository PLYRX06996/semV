%{
#include <bits/stdc++.h>
using namespace std;
%}

%option noyywrap

%%
[ \t\n]+
[0-9]+\.[0-9]+ {
    cout << yytext << " is a floating-point number" << endl;
}
[0-9]+ {
    cout << yytext << " is not a floating number" << endl;
}
%%

int main() {
    cout << "Enter the number: " << endl;
    yylex();
    return 0;
}