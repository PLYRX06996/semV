%{
    #include<bits/stdc++.h>
    using namespace std;
%}

%option noyywrap

operand [a-zA-Z0-9]+
op      [+\-*/]
expr    {operand}({op}{operand})*

%%
{expr}\n {
    cout << "Valid expression identified\n";
    cout << "Input an expression: ";
}

.*\n {
    cout << "Expression is invalid\n";
    cout << "Input an expression: ";
}
%%

int main(){
    cout << "Input an expression: ";
    yylex();
    return 0;
}