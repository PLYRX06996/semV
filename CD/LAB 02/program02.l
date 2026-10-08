%{
    #include<bits/stdc++.h>
    using namespace std;
%}

%option noyywrap

%%
[ \t\n]+

[-+]?[0-9]+ {
    cout << yytext << "   --> INTEGER" << endl;
}

[-+]?([0-9]+\.[0-9]*|\.[0-9]+) {
    cout << yytext << "   --> FLOAT" << endl;
}

[-+]?([0-9]+\.[0-9]*|\.[0-9]+|[0-9]+)[eE][-+]?[0-9]+ {
    cout << yytext << "   --> SCIENTIFIC_NOTATION" << endl;
}

[-+]?([0-9]+\.[0-9]*|\.[0-9]+|[0-9]+)[eE][-+]?[^ \t\n]* {
    cout << yytext << "   --> INVALID NUMBER" << endl;
}

[-+]?[0-9]*\.[0-9]*\.[0-9\.]* {
    cout << yytext << "   --> INVALID NUMBER" << endl;
}
%%

int main()
{
    cout << "Enter the numbers: " << endl;
    yylex();
    return 0;
}