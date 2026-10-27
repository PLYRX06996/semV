%{
#include <stdio.h>
#include <stdlib.h>
int yylex();
void yyerror(const char *s);
%}
%token N
%left '+' '-'
%left '*' '/'
%right '^'
%%
S: E '\n' { printf("\n"); exit(0); }
 ;
E: E '+' E { printf("+ "); }
 | E '-' E { printf("- "); }
 | E '*' E { printf("* "); }
 | E '/' E { printf("/ "); }
 | E '^' E { printf("^ "); }
 | '(' E ')' 
 | N       { printf("%d ", $1); }
 ;
%%
void yyerror(const char *s) {
    printf("Invalid expression\n");
}
int main() {
    yyparse();
    return 0;
}