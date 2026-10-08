%{
#include <iostream>
using namespace std;
%}

%option noyywrap

%%
[ \t\n]+    
"//".*      
"/*"([^*]|\*+[^*/])*\*+"/" 

"Integer"|"Float"|"String"|"Begin"|"End"|"read"|"write"|"for"|"to"|"do"|"begin"|"endfor"|"if"|"then"|"elseif"|"else"|"endif" { 
    cout << "keyword: " << yytext << endl; 
}

[0-9]+\.[0-9]+ { 
    cout << "float_literal: " << yytext << endl; 
}

[0-9]+ { 
    cout << "integer_literal: " << yytext << endl; 
}

\"[^\"]*\" { 
    cout << "string_literal: " << yytext << endl; 
}

[a-zA-Z_][a-zA-Z0-9_]* { 
    cout << "identifier: " << yytext << endl; 
}

":=" { 
    cout << "assign_opr: " << yytext << endl; 
}

">="|"<="|"=="|"!="|">"|"<" { 
    cout << "relational_opr: " << yytext << endl; 
}

"+"|"-"|"*"|"/" { 
    cout << "arithmetic_opr: " << yytext << endl; 
}

";"|","|"["|"]"|"("|")" { 
    cout << "punctuation: " << yytext << endl; 
}

. { 
    cout << "unidentified_token: " << yytext << endl; 
}
%%

int main() {
    cout << "Enter code for Q2 (Press Ctrl+D to stop):" << endl;
    yylex();
    return 0;
}