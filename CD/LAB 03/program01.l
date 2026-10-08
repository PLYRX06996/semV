%{
#include <bits/stdc++.h>
using namespace std;

bool space_pending = false;
bool last_was_punctuation = false;
int word_count = 0;
int whitespace_sequence_replaced = 0;
%}

%option c++
%option noyywrap

%%
^[ \t]*\n+ {

}
[ \t]+   {
    whitespace_sequence_replaced++;
    space_pending = true;
}
[.,;:!?] {
    space_pending = false;
    last_was_punctuation = true;
    cout << yytext ;
}
\n+  {
    if(last_was_punctuation) {
        last_was_punctuation = false;
        space_pending = false;
        cout << "\n";
    } else {
        space_pending = true;
    }
}
[a-zA-Z0-9]+ {
    if(space_pending) {
        cout << " ";
        space_pending = false;
    }
    word_count++;
    cout << yytext ;
}
.        {
    if(space_pending) {
        cout << " ";
        space_pending = false;
    }
    cout << yytext ;
}

%%