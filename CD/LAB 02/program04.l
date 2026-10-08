%{
    #include <iostream>
    using namespace std;

    int num_keywords = 0, num_identifiers = 0, num_int_const = 0, num_float_const = 0;
    int num_arith_op = 0, num_rel_op = 0, num_log_op = 0, num_assign_op = 0;
    int num_punct = 0, num_invalid = 0, total_tokens = 0;
%}

%option noyywrap

%%
[ \t\n]+    

"program"|"main"|"int"|"float"|"if"|"else"|"print" { 
    cout << yytext << "\t\tKEYWORD\t\t101\n"; 
    num_keywords++; 
    total_tokens++; 
}

[a-zA-Z_][a-zA-Z0-9_]* { 
    cout << yytext << "\t\tIDENTIFIER\t103\n"; 
    num_identifiers++; 
    total_tokens++; 
}

[0-9]+\.[0-9]+ { 
    cout << yytext << "\t\tFLOAT_CONST\t201\n"; 
    num_float_const++; 
    total_tokens++; 
}

[0-9]+ { 
    cout << yytext << "\t\tINT_CONST\t202\n"; 
    num_int_const++; 
    total_tokens++; 
}

"+"|"-"|"*"|"/" { 
    cout << yytext << "\t\tARITH_OP\t301\n"; 
    num_arith_op++; 
    total_tokens++; 
}

">="|"<="|"=="|"!="|">"|"<" { 
    cout << yytext << "\t\tREL_OP\t\t302\n"; 
    num_rel_op++; 
    total_tokens++; 
}

"&&"|"||"|"!" { 
    cout << yytext << "\t\tLOGICAL_OP\t303\n"; 
    num_log_op++; 
    total_tokens++; 
}

"=" { 
    cout << yytext << "\t\tASSIGN_OP\t304\n"; 
    num_assign_op++; 
    total_tokens++; 
}

"("|")"|"{"|"}"|";"|"," { 
    cout << yytext << "\t\tPUNCTUATION\t401\n"; 
    num_punct++; 
    total_tokens++; 
}

. { 
    cout << yytext << "\t\tINVALID\t\t999\n"; 
    num_invalid++; 
    total_tokens++; 
}
%%

int main() {
    cout << "Lexeme\t\tToken\t\tReturn Value\n";
    cout << "------------------------------------------------------\n";
    
    yylex();

    cout << "\nNumber of Keywords: " << num_keywords << endl;
    cout << "Number of Identifiers: " << num_identifiers << endl;
    cout << "Number of Integer Constants: " << num_int_const << endl;
    cout << "Number of Floating Constants: " << num_float_const << endl;
    cout << "Number of Arithmetic Operators: " << num_arith_op << endl;
    cout << "Number of Relational Operators: " << num_rel_op << endl;
    cout << "Number of Logical Operators: " << num_log_op << endl;
    cout << "Number of Assignment Operators: " << num_assign_op << endl;
    cout << "Number of Punctuations: " << num_punct << endl;
    cout << "Number of Invalid Tokens: " << num_invalid << endl;
    cout << "Total Tokens: " << total_tokens << endl;

    return 0;
}